// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/merged_sstable_reader.h"

#include <map>
#include <set>

#include "toft/base/scoped_ptr.h"
#include "toft/base/stl_util.h"
#include "toft/base/string/format.h"
#include "toft/base/string/number.h"
#include "toft/storage/sharding/sharding.h"
#include "toft/storage/sstable/reader/on_disk_sstable_reader.h"
#include "toft/storage/sstable/sstable.h"
#include "toft/storage/sstable/sstable_reader.h"

#include "thirdparty/glog/logging.h"

DEFINE_bool(tolerate_sstable_open_failure, false, "whether to tolerate sstable open failure");
DEFINE_bool(ignore_sstable_setid, false, "If true, ignore sstable's setid");

// GLOBAL_NOLINT(readability/casting)

namespace toft {

// One sstable set means a set of sstables that have the same set id.
class SSTableReaderSet {
    TOFT_DECLARE_UNCOPYABLE(SSTableReaderSet);

public:
    SSTableReaderSet(const std::string &set_id, const std::string &sharding_policy, int num_shard)
                    : set_id_(set_id),
                      sharding_policy_(sharding_policy),
                      num_shard_(num_shard) {
        if (!sharding_policy_.empty()) {
            sharding_man_.reset(TOFT_CREATE_SHARDING_POLICY(sharding_policy));
            CHECK(sharding_man_.get())
            << "Can get inst of sharding_polity named:" << sharding_policy;
            sharding_man_->SetShardingNumber(num_shard_);
        }
    }

    ~SSTableReaderSet() {}

    bool Lookup(const std::string &key, std::string *value) {
        if (!sharding_man_.get()) {
            LOG(WARNING) << "invalid sharding policy: " << sharding_policy_
            << ", the performance may suffer! (table #: "
            << tables_.size() << ")";
            bool exist = false;
            value->clear();
            std::string tmp_value;
            for (std::map<int, SSTableReader*>::const_iterator it = tables_.begin();
                            it != tables_.end(); ++it) {
                if (it->second->Lookup(key, &tmp_value) &&
                                (value->empty() || *value > tmp_value)) {
                    *value = tmp_value;
                    exist = true;
                }
            }
            return exist;
        } else {
            std::map<int, SSTableReader*>::iterator it = tables_.find(sharding_man_->Shard(key));
            if (it == tables_.end()) return false;
            return it->second->Lookup(key, value);
        }
        return false;
    }

    bool AddSSTableReader(SSTableReader *sstable,
                    const std::string &set_id,
                    const std::string &sharding_policy,
                    int num_shard,
                    int index) {
        CHECK(set_id == set_id_) << "wrong set id: " << set_id;
        if (!set_id.empty()) {
            if (sharding_policy != sharding_policy_) {
                LOG(WARNING) << "sharding policy mismatch!";
                return false;
            }
            if (num_shard != num_shard_) {
                LOG(WARNING) << "sharding number mismatch!";
                return false;
            }
            if (tables_.find(index) != tables_.end()) {
                LOG(WARNING) << "index " << index << " is already in!";
                return false;
            }
            if (index < 0 || index >= num_shard) {
                LOG(WARNING) << "index out of range: " << index << "[0, " << num_shard
                << "]";
                return false;
            }
            tables_[index] = sstable;
        } else {
            tables_[tables_.size()] = sstable;
        }
        return true;
    }

private:
    const std::string set_id_;
    const std::string sharding_policy_;
    int num_shard_;
    std::map<int, SSTableReader *> tables_;
    toft::scoped_ptr<ShardingPolicy> sharding_man_;
};

class MergedSSTableReader::Impl {
    TOFT_DECLARE_UNCOPYABLE(Impl);

public:
    Impl() {
    }
    ~Impl() {
        Reset();
    }

    void Reset() {
        DeleteElements(&tables_);
        DeleteValues(&sets_);
    }

    std::map<std::string, SSTableReaderSet*> sets_;
    std::vector<SSTableReader*> tables_;
};

struct IteratorComp {
    inline bool operator()(const SSTableReader::Iterator *iter1,
                           const SSTableReader::Iterator *iter2) const {
        if (iter1->key() < iter2->key())
            return true;
        if (iter1->key() == iter2->key())
            return iter1->value() < iter2->value();
        return false;
    }
};

class MergedIterator : public SSTableReader::Iterator {
    TOFT_DECLARE_UNCOPYABLE(MergedIterator);

public:
    MergedIterator(MergedSSTableReader::Impl *sstable, const std::string &key)
                    : sstable_(sstable) {
        SeekKey(key);
        if (valid_) {
            LoadItem();
        }
    }
    ~MergedIterator() {
        DeleteElements(&iter_queue_);
    }

    virtual void Next() {
        valid_ = !iter_queue_.empty();
        if (valid_) {
            LoadItem();
        }
    }
    virtual void SeekKey(const std::string &key);

private:
    void LoadItem();

    MergedSSTableReader::Impl *sstable_;
    std::multiset<SSTableReader::Iterator*, IteratorComp> iter_queue_;
};

void MergedIterator::SeekKey(const std::string &key) {
    valid_ = false;
    std::vector<SSTableReader*>::iterator iter = sstable_->tables_.begin();
    for (; iter != sstable_->tables_.end(); ++iter) {
        SSTableReader::Iterator *it = (*iter)->Seek(key);
        if (it->Valid()) {
            iter_queue_.insert(it);
            valid_ = true;
        } else {
            delete it;
        }
    }
}

void MergedIterator::LoadItem() {
    std::multiset<SSTableReader::Iterator*>::iterator iter = iter_queue_.begin();
    SSTableReader::Iterator *it = *iter;
    key_ = it->key();
    value_ = it->value();
    it->Next();
    iter_queue_.erase(iter);
    if (it->Valid()) {
        iter_queue_.insert(it);
    } else {
        delete it;
    }
}

MergedSSTableReader::MergedSSTableReader()
                : impl_(new Impl) {
}

MergedSSTableReader::~MergedSSTableReader() {
}

bool MergedSSTableReader::Open(const std::vector<std::string> &paths,
                               ReadMode type,
                               bool ignore_bad_files) {
    impl_->Reset();
    for (size_t i = 0; i < paths.size(); ++i) {
        LOG(INFO)<< "path:" << paths[i];
        if (!LoadSSTableReader(paths[i], type) && !ignore_bad_files) {
            return false;
        }
    }
    VLOG(2) << "loaded " << impl_->tables_.size() << " sstables.";
    return !impl_->tables_.empty();
}

bool MergedSSTableReader::Open(const std::vector<std::string> &paths) {
    return Open(paths, ON_DISK, FLAGS_tolerate_sstable_open_failure);
}

void MergedSSTableReader::GetPaths(std::vector<std::string> *paths) const {
    paths->clear();
    paths->reserve(impl_->tables_.size());
    for (size_t i = 0; i < impl_->tables_.size(); ++i) {
        paths->push_back(impl_->tables_[i]->GetPath());
    }
}

bool MergedSSTableReader::LoadSSTableReader(const std::string &path, ReadMode type) {
    SSTableReader *sstable = SSTableReader::Open(path, type);
    if (sstable == NULL) {
        LOG(ERROR)<< "Failed to open sstable:" << path;
        return false;
    }
    if (sstable->EntryCount() == 0) {
        LOG(WARNING)<< "sstable " << path << " is empty.";
        delete sstable;
        return false;
    }

    // now fill it into proper sstable set.
    std::string set_id = sstable->GetMetaData(kSSTableSetID);
    if (set_id.empty()) {
        LOG(WARNING)<< "sstable with empty set id";
    }
    std::string policy = sstable->GetMetaData(kShardPolicy);
    std::string str_num_shard = sstable->GetMetaData(kShardTotalNum);
    std::string str_shard_id = sstable->GetMetaData(kShardID);
    VLOG(2) << "set_id: " << set_id << ", policy: " << policy << ", num_shard: "
            << str_num_shard << ", shard_id: " << str_shard_id;

    int num_shard = 0;
    if (!StringToNumber(str_num_shard, &num_shard)) {
        if (!set_id.empty()) {
            LOG(WARNING)<< "bad num shard: " << str_num_shard << ", path: "
            << sstable->GetPath();
            delete sstable;
            return false;
        }
    }
    int shard_id = -1;
    if (!StringToNumber(str_shard_id, &shard_id)) {
        if (!set_id.empty()) {
            LOG(WARNING)<< "bad shard id: " << str_shard_id;
            delete sstable;
            return false;
        }
    }

    std::map<std::string, SSTableReaderSet*>::iterator it = impl_->sets_.find(set_id);
    if (it == impl_->sets_.end()) {
        if (set_id.empty()) {
            // Ignore sharding policy for sstable files that has no set id
            if (FLAGS_ignore_sstable_setid) {
                impl_->sets_[""] = new SSTableReaderSet("", policy, num_shard);
            } else {
                impl_->sets_[""] = new SSTableReaderSet("", "", 0);
            }
        } else {
            impl_->sets_[set_id] = new SSTableReaderSet(set_id, policy, num_shard);
        }
        it = impl_->sets_.find(set_id);
    }

    impl_->tables_.push_back(sstable);
    return it->second->AddSSTableReader(sstable, set_id, policy, num_shard, shard_id);
}

int MergedSSTableReader::EntryCount() const {
    int count = 0;
    std::vector<SSTableReader *>::const_iterator iter = impl_->tables_.begin();
    for (; iter != impl_->tables_.end(); iter++) {
        count += (*iter)->EntryCount();
    }
    return count;
}

SSTableReader::Iterator* MergedSSTableReader::Seek(const std::string &key) {
    return new MergedIterator(impl_.get(), key);
}

const std::string MergedSSTableReader::GetMetaData(const std::string &key) const {
    std::vector<SSTableReader*>::const_iterator iter = impl_->tables_.begin();
    for (; iter != impl_->tables_.end(); iter++) {
        if ((*iter)->GetMetaData(key) != "") {
            return (*iter)->GetMetaData(key);
        }
    }
    return std::string();
}

void MergedSSTableReader::IterateMetaData(
    toft::Closure<bool(const std::string &, const std::string &)> *callback) const {
    std::vector<SSTableReader*>::iterator iter = impl_->tables_.begin();
    for (; iter != impl_->tables_.end(); iter++) {
        (*iter)->IterateMetaData(callback);
    }
}

bool MergedSSTableReader::Lookup(const std::string &key, std::string *value) {
    VLOG(1) << "Lookup " << key << ", set num: " << impl_->sets_.size();
    for (std::map<std::string, SSTableReaderSet*>::iterator it = impl_->sets_.begin();
                    it != impl_->sets_.end(); ++it) {
        if (it->second->Lookup(key, value))
            return true;
    }
    return false;
}

}  // namespace toft
