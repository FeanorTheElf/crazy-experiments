#include <iostream>
#include <functional>

#include "esel.h"

using namespace std;

int main() {
    auto add = (x, y) -> y + x;
    auto eq = (x, y) -> x == y;
    auto foo = (x, y, z) -> x * (y + z);
    auto ternary = (x, y, z) -> if (x) y else z;
    auto bar = (x, y, z) -> if (x + y) x else x + z;

    std::cout << "add(2, 4): " << add(2, 4) << std::endl;
    std::cout << "eq(3, 2): " << eq(3, 2) << std::endl;
    std::cout << "foo(2, 3, 4): " << foo(2, 3, 4) << std::endl;
    std::cout << "ternary(0, 2, 3): " << ternary(0, 2, 3) << std::endl;
    std::cout << "ternary(1, 2, 3): " << ternary(1, 2, 3) << std::endl;
    std::cout << "bar(0, -2, 5): " << bar(0, -2, 5) << std::endl;
    std::cout << "bar(2, -2, 5): " << bar(2, -2, 5) << std::endl;

    return 0;
}