#include <cassert>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>
int main() {
    std::vector<int> a{10,20,30};
    auto it = a.begin();
    assert(*it == 10);
    ++it;
    *it = 25;
    assert(a[1] == 25);
    const auto locked = a.begin(); // 固定位置，不固定元素
    *locked = 11;
    auto read = a.cbegin();
    ++read;
    assert(*read == 25);
    assert(std::distance(a.begin(),a.end()) == 3);
    assert(std::distance(a.end(),a.begin()) == -3); // 随机访问支持负差值
    std::vector<int> empty;
    assert(empty.begin() == empty.end());
    std::vector<int> middle(a.begin()+1,a.end());
    assert((middle == std::vector<int>{25,30}));

    std::list<int> nodes{10,20,30,40};
    auto first = nodes.begin();
    auto third = std::next(first,2);
    assert(*first == 10 && *third == 30); // next 不修改普通容器原迭代器
    std::advance(first,1);
    assert(*first == 20);
    assert(*std::prev(nodes.end()) == 40);
    assert(std::distance(nodes.begin(),nodes.end()) == 4);
    // 不调用 distance(nodes.end(),nodes.begin())：正向无法到达。
    std::cout << "iterator checks passed\n";
}
