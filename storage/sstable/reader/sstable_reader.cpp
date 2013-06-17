// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: Ye Shunping <yeshunping@gmail.com>

#include "toft/storage/sstable/sstable.h"

#include <stdio.h>
#include <string.h>

#include <string>
#include <vector>

#include "toft/storage/file/file.h"
#include "toft/storage/sstable/reader/in_memory_sstable_reader.h"
#include "toft/storage/sstable/reader/on_disk_sstable_reader.h"
#include "toft/storage/sstable/reader/sstable_reader_impl.h"

namespace toft {

SSTableReader::SSTableReader()
                : impl_(new Impl()) {
}

SSTableReader::~SSTableReader() {
}

SSTableReader *SSTableReader::Open(const std::string &path, ReadMode type) {
    toft::scoped_ptr<SSTableReader> ptr;
    switch (type) {
    case ON_DISK:
        ptr.reset(new OnDiskSSTableReader);
        break;
    case IN_MEMORY:
        ptr.reset(new InMemorySSTableReader);
        break;
    default:
        DCHECK(false) << "invalid sstable type: " << type;
    }
    if (ptr.get()) {
        if (!ptr->impl_->LoadFile(path)) {
            return NULL;
        }
        ptr->Init();
    } else {
        CHECK(false) << "fail to new sstable";
    }
    return ptr.release();
}

//  static
bool SSTableReader::GetMetaData(const std::string &path,
                                const std::string &key,
                                std::string *value) {
    toft::scoped_ptr<File> file(File::Open(path, "r"));
    if (!file.get()) {
        LOG(ERROR)<< "open sstable failed: " << path;
        return false;
    }

    hfile::FileInfo file_info;
    hfile::FileTrailer file_trailer;
    if (!Impl::LoadFileInfo(file.get(), NULL, &file_info, &file_trailer))
        return false;

    *value = Impl::FindValue(key, file_info.meta_items);
    return !value->empty();
}

//  Static
bool SSTableReader::GetEntryCount(const std::string &path, int *count) {
    toft::scoped_ptr<File> file(File::Open(path, "r"));
    if (!file.get()) {
        LOG(ERROR)<< "open sstable failed: " << path;
        return false;
    }

    hfile::FileTrailer file_trailer;
    if (!Impl::LoadFileInfo(file.get(), NULL, NULL, &file_trailer))
        return false;

    *count = file_trailer.entry_count();
    return true;
}

//  Not static, get meta data in meta data blocks
const std::string SSTableReader::GetMetaData(const std::string &key) const {
    return impl_->GetMetaData(key);
}

void SSTableReader::IterateMetaData(
        toft::Closure<bool(const std::string&, const std::string&)> *callback) const {
    impl_->IterMetaData(callback);
}

int SSTableReader::EntryCount() const {
    return impl_->EntryCount();
}

SSTableReader::Iterator *SSTableReader::NewIterator() {
    return Seek("");
}

std::string SSTableReader::GetPath() const {
    return impl_->path_;
}

bool SSTableReader::Lookup(const std::string &key, std::string *value) {
    toft::scoped_ptr<Iterator> iter(Seek(key));
    if (iter->key() == key) {
        *value = iter->value();
        return true;
    }
    return false;
}

}  // namespace toft
