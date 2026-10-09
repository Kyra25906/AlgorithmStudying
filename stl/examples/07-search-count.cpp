#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
int main() {
    std::vector<int> a{3,1,3,1};
    auto found=std::find(a.begin(),a.end(),1);
    assert(found==a.begin()+1);
    assert(std::find(a.begin(),a.end(),9)==a.end());
    assert(std::count(a.begin(),a.end(),3)==2);
    auto positive=[](int x){return x>0;};
    assert(std::find_if_not(a.begin(),a.end(),positive)==a.end());
    assert(std::count_if(a.begin(),a.end(),positive)==4);
    assert(std::min_element(a.begin(),a.end())==a.begin()+1);
    assert(std::max_element(a.begin(),a.end())==a.begin());
    auto [lo,hi]=std::minmax_element(a.begin(),a.end());
    assert(lo==a.begin()+1 && hi==a.begin()+2); // 最小首次，最大末次
    std::vector<int> empty;
    assert(std::all_of(empty.begin(),empty.end(),positive));
    assert(!std::any_of(empty.begin(),empty.end(),positive));
    assert(std::none_of(empty.begin(),empty.end(),positive));
    auto [elo,ehi]=std::minmax_element(empty.begin(),empty.end());
    assert(elo==empty.end() && ehi==empty.end()); // 不解引用
    std::vector<int> mixed{-2,0,7};
    assert(!std::all_of(mixed.begin(),mixed.end(),positive));
    assert(std::any_of(mixed.begin(),mixed.end(),positive));
    assert(!std::none_of(mixed.begin(),mixed.end(),positive));
    std::cout << "search checks passed\n";
}
