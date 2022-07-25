//
// Created by haziha on 2022/7/25.
//

#ifndef HADEFER_DEFERCLASS_H
#define HADEFER_DEFERCLASS_H


/*
 * 一个 C++ 实现的 defer
 * 例程
 *
 * int main()
 * {
 *      USE_DEFER;
 *
 *      defer << []()
 *      {
 *          std::cout << 1 << std::endl;
 *      };
 *
 *      deferExpr(std::cout << 2 << std::endl;);
 *
 *      defer << [&]()
 *      {
 *          std::cout << 3 << std::endl;
 *      };
 *
 *      return 0;
 * }
 *
 * 最后将输出:
 * 3
 * 2
 * 1
 *
 * 与栈类似, 最先defer的函数最后执行
*/

#include <vector>
#include <functional>

class DeferClass {
public:
    DeferClass();

    void operator<<(const std::function<void()> &defer_func);

    void operator+=(const std::function<void()> &defer_func);

    ~DeferClass();

private:
    std::function<void()> defer_function = nullptr;
};

class DeferClassMember {
public:
    DeferClassMember();

    void operator<<(const std::function<void()> &defer_func);

    void operator+=(const std::function<void()> &defer_func);

    ~DeferClassMember();

private:
    std::vector<std::function<void()>> defer_function;
};

#define DEFER_NEW(count) DeferClass DEFER_OBJECT_##count; DEFER_OBJECT_##count
#define DEFER_COUNT(count) DEFER_NEW(count)

#define defer DEFER_COUNT(__COUNTER__)
#define deferExpr(expr) defer << [&]() {expr}

#define USE_DEFER_MEMBER DeferClassMember __defer_class_object_member__;
#define deferM __defer_class_object_member__
#define deferMExpr(expr) deferM << [&]() {expr}


#endif //HADEFER_DEFERCLASS_H
