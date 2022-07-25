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
    defer << []() {
        std::cout << 1 << std::endl;
    };
    deferExpr(std::cout << 2 << std::endl;);

    defer += []() {
        std::cout << 3 << std::endl;
    };
    std::cout << 0 << std::endl;

    demo _;

    return 0;
}
