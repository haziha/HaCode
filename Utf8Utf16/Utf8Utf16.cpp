//
// Created by hazih on 2022/7/26.
//

#include "Utf8Utf16.h"

#include <codecvt>
#include <locale>

std::wstring Utf8Utf16::Utf8ToUtf16(std::string utf8) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::u16string utf16 = convert.from_bytes(utf8);
    std::wstring wstr(utf16.begin(), utf16.end());
    return wstr;
}

std::string Utf8Utf16::Utf16ToUtf8(std::wstring utf16) {
    std::u16string u16str(utf16.begin(), utf16.end());
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::string utf8 = convert.to_bytes(u16str);
    return utf8;
}
