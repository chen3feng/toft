// Copyright 2013, Baidu Inc.
// Author: Lu Yan <luyan02@baidu.com>
// A Util class for processing URI.

#ifndef  TOFT_STORAGE_FILE_URI_UTILS_H_
#define  TOFT_STORAGE_FILE_URI_UTILS_H_

#include <map>
#include <string>
#include <vector>

namespace toft {

class UriUtils {
public:
    /**
     *  A normal explode for string.
     *  @param s string to explode
     *  @param delim delim char used in explode
     *  @return a vector of exploded string pieces
     */
    static bool Explode(std::string const & s, char delim, std::vector<std::string>* exploded);
    /**
     * Parse params in URI sections. URI could have params, the pattern is like:
     *      /hdfs/szwg-ecomon:2243?username=abcd,password=dfcg,blockSize=256000000/user/...
     *            <--main_str----> <--key-> <v-> <--key-> <v-> <--key--> <---v--->
     *            <-------------------------section------------------------------>
     * delims is fixed:
     *      ? -- start of params part
     *      , -- kv-pair delim
     *      = -- delimiting key and value.
     * @param section certain to parse from, passing a string not meeting desc above,
     *        result will be uncertain.
     * @param main_str main string parsed from section
     * @param params key-value pairs
     * @return parse succeeded or failed
     */
    static bool ParseParam(const std::string& section, std::string* cluster_name,
            std::map<std::string, std::string>* params);
    /**
     * Shift sections from a path. Like from path
     *      /hdfs/szwg-ecomon:23234/user/logging/...
     * shifting 2 section will resulting in
     *      /user/logging/...
     * @param path path to shift
     * @param shifted returned shifted string
     * @param n section count to shift
     * @param delim delim between sections
     * @return succeeded or failed
     */
    static bool Shift(const std::string& path, std::string* shifted, int n, char delim);
};

} // namespace toft

#endif  //__URI_UTILS_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
