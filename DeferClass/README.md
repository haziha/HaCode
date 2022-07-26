# DeferClass

> 一个模仿 Golang 的 defer

---

### 普通作用域
> 希望退出作用域时执行
> 
```c++
#include <iostream>
#include "DeferClass.h"

int main() {
    defer += []() {
        std::cout << 1 << std::endl;
    };
    defer << []() {
        std::cout << 2 << std::endl;
    };
    deferExpr(std::cout << 3 << std::endl;);
    std::cout << 0 << std::endl;
}
```

#### 输出

```text
0
3
2
1
```

---

### 对象作用域
> 希望对象析构时执行

```c++
#include <iostream>
#include "DeferClass.h"

class demo {
    USE_DEFER_MEMBER

public:
    demo() {
        deferM << []() {
            std::cout << "class defer " << 1 << std::endl;
        };
        deferM += []() {
            std::cout << "class defer " << 2 << std::endl;
        };
        deferMExpr(std::cout << "class defer " << 3 << std::endl;);
        std::cout << "construction end" << std::endl;
    }

    ~demo(){
        std::cout << "destructor end" << std::endl;
    }
};

int main() {
    demo _;
}
```

#### 输出

```text
construction end
destructor end
class defer 3
class defer 2
class defer 1
```

---

### 注意

以下代码为错误示范
```c++
#include <iostream>
#include "DeferClass.h"

void func(bool f) {
    auto nums = new int[10];
    if(f)
    {
        deferExpr(delete []nums;);
    }
    std::cout << nums[1] << std::endl;
}

int main() {
    func(true);
}
```

> 可能执行没问题, 但是其实代码执行完 func 里的 if 分支后, nums 就会 delete掉  
> 所以后面执行的代码有风险, 正确写法如下

```c++
#include <iostream>
#include "DeferClass.h"

void func(bool f) {
    auto nums = new int[10];
    defer << [=]() {
        if(f)
        {
            delete []nums;
        }
    };
    std::cout << nums[1] << std::endl;
}

int main() {
    func(true);
}
```
