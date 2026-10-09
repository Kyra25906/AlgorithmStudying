#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>
int main() {
    std::vector<int> a{4,-2,4,7};
    std::vector<int> kept;
    std::copy_if(a.begin(),a.end(),std::back_inserter(kept),[](int x){return x>=0;});
    assert((kept==std::vector<int>{4,4,7}));
    std::vector<long long> squared;
    std::transform(a.begin(),a.end(),std::back_inserter(squared),[](int x){return 1LL*x*x;});
    assert((squared==std::vector<long long>{16,4,16,49}));
    std::vector<int> right{1,2,3,4}, result(a.size());
    auto end=std::transform(a.begin(),a.end(),right.begin(),result.begin(),[](int x,int y){return x+y;});
    assert(end==result.end() && (result==std::vector<int>{5,0,7,11}));
    std::transform(result.begin(),result.end(),result.begin(),[](int x){return x+1;});
    assert((result==std::vector<int>{6,1,8,12}));
    std::fill(result.begin(),result.end(),7);
    assert((result==std::vector<int>{7,7,7,7}));
    std::iota(result.begin(),result.end(),3);
    assert((result==std::vector<int>{3,4,5,6}));
    std::vector<int> big{1000000000,1000000000,1000000000};
    assert(std::accumulate(big.begin(),big.end(),0LL)==3000000000LL);
    // partial_sum 内部累计跟随输入类型，因此先建立宽类型来源。
    std::vector<long long> wide(big.begin(),big.end()), prefix(wide.size()), restored(wide.size());
    assert(std::partial_sum(wide.begin(),wide.end(),prefix.begin())==prefix.end());
    assert(prefix.back()==3000000000LL);
    std::adjacent_difference(prefix.begin(),prefix.end(),restored.begin());
    assert(restored==wide);
    std::vector<long long> x{1000000,1000000};
    assert(std::inner_product(x.begin(),x.end(),x.begin(),0LL)==2000000000000LL);
    std::vector<long long> empty, output;
    assert(std::accumulate(empty.begin(),empty.end(),7LL)==7);
    assert(std::partial_sum(empty.begin(),empty.end(),output.begin())==output.begin());
    assert(std::adjacent_difference(empty.begin(),empty.end(),output.begin())==output.begin());
    assert(std::inner_product(empty.begin(),empty.end(),empty.begin(),9LL)==9);
    std::cout << "numeric checks passed\n";
}
