#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include "06-ranking-model.hpp"
auto makePredicate(int limit) {
    return [limit](int x) { return x >= limit; }; // 副本随返回的闭包保存
}
int main() {
    int limit = 60;
    auto snapshot = [limit](int x) { return x >= limit; };
    auto live = [&limit](int x) { return x >= limit; };
    limit = 80;
    assert(snapshot(70) && !live(70));
    int count = 0;
    auto localCounter = [count]() mutable { return ++count; };
    assert(localCounter() == 1);
    auto copiedCounter = localCounter;
    assert(localCounter() == 2 && copiedCounter() == 2 && count == 0);
    auto outerCounter = [&count]() { return ++count; };
    assert(outerCounter() == 1 && count == 1);
    auto rule = makePredicate(75);
    assert(rule(75) && !rule(74));
    std::vector<int> values{59,60,90};
    assert(std::count_if(values.begin(),values.end(),AtLeast{60}) == 2);
    assert(std::count_if(values.begin(),values.end(),[limit](int x){ return x>=limit; }) == 1);
    auto less = [](const auto& a,const auto& b) { return a < b; };
    assert(less(1,2) && less(std::string("A"),std::string("B")));
    std::vector<int> signedValues{-3,-2,0,1,2};
    assert(std::count_if(signedValues.begin(),signedValues.end(),[](int x){return x%2!=0;}) == 2);
    std::cout << "lambda checks passed\n";
}
