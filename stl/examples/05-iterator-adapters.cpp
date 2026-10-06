#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <vector>
int main() {
    std::vector<int> source{1,2,3};
    std::vector<int> sized(source.size()); // 已经建立可赋值元素
    auto finish = std::copy(source.cbegin(),source.cend(),sized.begin());
    assert(sized == source && finish == sized.end());
    std::vector<int> appended;
    std::copy(source.cbegin(),source.cend(),std::back_inserter(appended));
    assert(appended == source);
    std::deque<int> front{9};
    std::copy(source.cbegin(),source.cend(),std::front_inserter(front));
    assert((front == std::deque<int>{3,2,1,9}));
    std::vector<int> inserted{9};
    std::copy(source.cbegin(),source.cend(),std::inserter(inserted,inserted.begin()));
    assert((inserted == std::vector<int>{1,2,3,9}));
    auto reverse = source.rbegin();
    assert(*reverse == 3 && reverse.base() == source.end());
    ++reverse;
    assert(*reverse == 2 && *reverse.base() == 3);
    assert(*std::prev(reverse.base()) == 2);
    std::vector<int> reversed;
    std::copy(source.crbegin(),source.crend(),std::back_inserter(reversed));
    assert((reversed == std::vector<int>{3,2,1}));
    assert((source == std::vector<int>{1,2,3}));
    std::vector<int> empty;
    assert(empty.rbegin() == empty.rend());
    std::copy(empty.begin(),empty.end(),std::back_inserter(reversed));
    assert(reversed.size() == 3);
    std::vector<int> shifted{1,2,3,4};
    std::copy(shifted.begin()+1,shifted.end(),shifted.begin()); // 合法的向左覆盖
    assert((shifted == std::vector<int>{2,3,4,4}));
    std::cout << "adapter checks passed\n";
}
