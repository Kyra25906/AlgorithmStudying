#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

// 常量引用不复制整个容器，也不允许打印函数修改原数据。
void show(const char* label, const std::vector<int>& a) {
    std::cout << label << " size=" << a.size()
              << " capacity=" << a.capacity() << " values:";
    for (int x : a) std::cout << ' ' << x;
    std::cout << '\n';
}
int main() {
    std::vector<int> a;
    show("empty", a);
    a.reserve(5);
    const auto capacity = a.capacity();
    assert(a.empty() && capacity >= 5);
    show("reserve(5)", a);
    bool caught = false;
    try { (void)a.at(0); }
    catch (const std::out_of_range&) { caught = true; }
    assert(caught);
    std::cout << "at(0): caught out_of_range\n";
    a.push_back(80);
    a.push_back(95);
    show("two pushes", a);
    a.resize(4);
    assert((a == std::vector<int>{80,95,0,0}));
    show("resize(4)", a);
    a.resize(2);
    a.pop_back();
    assert((a == std::vector<int>{80}));
    show("resize(2), pop", a);
    a.clear();
    assert(a.empty() && a.capacity() == capacity);
    show("clear", a);
    std::vector<int> b{1,2,3};
    for (int x : b) { x += 1; (void)x; } // 只改副本
    assert((b == std::vector<int>{1,2,3}));
    for (int& x : b) x += 1; // 修改原值
    assert((b == std::vector<int>{2,3,4}));
    show("reference loop", b);
    std::vector<int> growth;
    for (int i=0; i<12; ++i) {
        growth.push_back(i);
        show("growth", growth); // 不假设固定增长倍数
    }
    std::cout << "all checks passed\n";
}
