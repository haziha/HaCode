//
// Created by haziha on 2022/7/26.
//

#ifndef UTF8UTF16_UTF8UTF16_H
#define UTF8UTF16_UTF8UTF16_H

#include <iostream>

class Utf8Utf16 {
public:
    Utf8Utf16() = delete;

    static std::wstring Utf8ToUtf16(std::string utf8);

    static std::string Utf16ToUtf8(std::wstring utf16);
};


#endif //UTF8UTF16_UTF8UTF16_H
