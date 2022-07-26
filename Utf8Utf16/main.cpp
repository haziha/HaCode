#include <iostream>
#include "Utf8Utf16.h"

int main() {
    std::cout << Utf8Utf16::Utf16ToUtf8(L"Hello, World!") << std::endl;
    std::wcout << Utf8Utf16::Utf8ToUtf16("Hello, World!") << std::endl;

    return 0;
}
