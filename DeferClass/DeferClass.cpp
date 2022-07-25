//
// Created by haziha on 2022/7/25.
//

#include "DeferClass.h"

/*
 * DeferClass 用于函数作用域，当函数退出时，会调用 DeferClass 的析构函数
 *      DeferClass 的析构函数会调用初始化时传入的函数，从而实现defer
 *
 * DeferClassMember 用于父对象作用域，当父对象析构时，会调用 DeferClassMember 的析构函数
 *      原理与 DeferClass 相同，只不过父对象析构函数的内容可以放在 DeferClassMember 里面
*/

DeferClass::DeferClass() = default;

void DeferClass::operator<<(const std::function<void()> &defer_func) {
    this->defer_function = defer_func;
}

void DeferClass::operator+=(const std::function<void()> &defer_func) {
    this->defer_function = defer_func;
}

DeferClass::~DeferClass() {
    if (this->defer_function) {
        this->defer_function();
    }
}

DeferClassMember::DeferClassMember() = default;

void DeferClassMember::operator<<(const std::function<void()> &defer_func) {
    this->defer_function.push_back(defer_func);
}

void DeferClassMember::operator+=(const std::function<void()> &defer_func) {
    this->defer_function.push_back(defer_func);
}

DeferClassMember::~DeferClassMember() {
    while (!this->defer_function.empty()) {
        if (this->defer_function.back()) {
            this->defer_function.back()();
        }
        this->defer_function.pop_back();
    }
}
