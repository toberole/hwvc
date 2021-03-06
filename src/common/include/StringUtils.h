/*
* Copyright (c) 2018-present, lmyooyo@gmail.com.
*
* This source code is licensed under the GPL license found in the
* LICENSE file in the root directory of this source tree.
*/
#ifndef HARDWAREVIDEOCODEC_STRINGUTILS_H
#define HARDWAREVIDEOCODEC_STRINGUTILS_H

#include <string.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

class StringUtils {
public:

    static std::string trimLeft(const std::string &str, const std::string &token = " ");

    static std::string trimRight(const std::string &str, const std::string &token = " ");

    static std::string trim(const std::string &str, const std::string &token = " ");

    static std::string toLower(const std::string &str);

    static std::string toUpper(const std::string &str);

    static bool startsWith(const std::string &str, const std::string &substr);

    static bool endsWith(const std::string &str, const std::string &substr);

    static bool equalsIgnoreCase(const std::string &str1, const std::string &str2);

    static std::vector<std::string> split(const std::string &str, const std::string &delimiter);

    static std::string valueOf(long value);

    static bool isEmpty(std::string *str);
};


#endif //HARDWAREVIDEOCODEC_STRINGUTILS_H
