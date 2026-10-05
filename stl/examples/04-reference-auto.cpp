#include <cassert>
#include <initializer_list>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

void changeCopy(std::vector<int> values) { values.at(0) = 0; }
void changeOriginal(std::vector<int>& values) { values.at(0) = 0; }
long long sum(const std::vector<int>& values) {
    long long result = 0;
    for (int value : values) result += value;
    return result;
}
int main() {
    int score = 80;
    int copy = score;
    int& alias = score;
    copy = 90;
    alias = 85;
    assert(score == 85 && copy == 90 && alias == 85);
    int other = 60;
    alias = other; // 给原对象赋值，不是改绑
    assert(score == 60 && &alias == &score);
    const int& view = score;
    score = 91;
    assert(view == 91);

    const int original = 7;
    auto value = original;
    auto& ref = original;
    const auto& read = original;
    static_assert(std::is_same_v<decltype(value), int>);
    static_assert(std::is_same_v<decltype(ref), const int&>);
    static_assert(std::is_same_v<decltype(read), const int&>);
    auto text = "hello";
    auto word = std::string("hello");
    auto one{1};
    auto list = {1, 2};
    static_assert(std::is_same_v<decltype(text), const char*>);
    static_assert(std::is_same_v<decltype(word), std::string>);
    static_assert(std::is_same_v<decltype(one), int>);
    static_assert(std::is_same_v<decltype(list), std::initializer_list<int>>);
    const int* pointer = &original;
    auto pointerCopy = pointer;
    static_assert(std::is_same_v<decltype(pointerCopy), const int*>);
    int array[2]{1,2};
    auto arrayPointer = array;
    auto& arrayRef = array;
    static_assert(std::is_same_v<decltype(arrayPointer), int*>);
    static_assert(std::is_same_v<decltype(arrayRef), int (&)[2]>);

    std::vector<int> values{1,2,3};
    changeCopy(values);
    assert(values.front() == 1);
    changeOriginal(values);
    assert(values.front() == 0 && sum(values) == 5);
    assert(sum({}) == 0);
    std::cout << "reference and auto checks passed\n";
}
