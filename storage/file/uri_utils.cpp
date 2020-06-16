// Copyright 2013, For authors.
// Author: An Qin (anqin.qin@gmail.com)
// A Util class for processing URI.

#include "uri_utils.h"

#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "toft/base/string/algorithm.h"

namespace toft{

bool UriUtils::Explode(std::string const & uri, char delim, std::vector<std::string>* exploded) {
    std::istringstream iss(uri);

    for (std::string token; std::getline(iss, token, delim); ) {
        if (token == "") {
            continue;
        }
        exploded->push_back(token);
    }
    return true;
}


bool UriUtils::ParseParam(const std::string& section, std::string* cluster_name,
        std::map<std::string, std::string>* params) {
    std::vector<std::string> main_tmp;
    if (!Explode(section, '?', &main_tmp)) {
        return false;
    }
    *cluster_name = main_tmp[0];
    if (main_tmp.size() == 1) {
        return true;
    }
    std::string remains = main_tmp[1];

    std::vector<std::string> pairs;
    Explode(remains, ',', &pairs);
    for (size_t i = 0; i < pairs.size(); i++) {
        std::vector<std::string> kv;
        SplitString(pairs[i], "=", &kv);
        if (kv.size() == 1) {
            kv.push_back("");
        }
        (*params)[kv[0]] = kv[1];
    }
    return true;
}

bool UriUtils::Shift(const std::string& path, std::string* shifted, int n, char delim) {
    size_t last_found = 0;
    int i = 0;
    while (i < n) {
        last_found = path.find(delim, last_found + 1);
        if (last_found == std::string::npos) {
            return false;
        }
        i++;
    }
    *shifted = path.substr(last_found);
    return true;
}

}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
