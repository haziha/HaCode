# Utf8Utf16
> UTF8 与 UTF16 互转  
> 来源: [https://gist.github.com/gchudnov/c1ba72d45e394180e22f](https://gist.github.com/gchudnov/c1ba72d45e394180e22f)

---

```c++
#include <iostream>
#include "Utf8Utf16.h"

int main() {
    std::cout << Utf8Utf16::Utf16ToUtf8(L"Hello, World!") << std::endl;
    std::wcout << Utf8Utf16::Utf8ToUtf16("Hello, World!") << std::endl;

    return 0;
}
```
