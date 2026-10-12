#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
int main() {
    std::vector<int> a{2,2,1,2,2};
    auto end=std::unique(a.begin(),a.end());
    assert(a.size()==5 && end-a.begin()==3);
    assert((std::vector<int>(a.begin(),end)==std::vector<int>{2,1,2}));
    // 不断言尾部内容；它不是去重后的业务结果。
    a.erase(end,a.end());
    assert((a==std::vector<int>{2,1,2}));
    std::sort(a.begin(),a.end());
    a.erase(std::unique(a.begin(),a.end()),a.end());
    assert((a==std::vector<int>{1,2}));
    std::vector<int> b{3,-1,2,-4,5};
    auto kept=std::remove_if(b.begin(),b.end(),[](int x){return x<0;});
    assert(b.size()==5 && (std::vector<int>(b.begin(),kept)==std::vector<int>{3,2,5}));
    b.erase(kept,b.end());
    assert((b==std::vector<int>{3,2,5}));
    std::vector<int> repeated{7,7,7};
    const int target=repeated.front(); // 独立副本，避免删除目标引用被压缩区间
    repeated.erase(std::remove(repeated.begin(),repeated.end(),target),repeated.end());
    assert(repeated.empty());
    std::vector<int> empty;
    assert(std::unique(empty.begin(),empty.end())==empty.end());
    empty.erase(std::remove(empty.begin(),empty.end(),0),empty.end());
    assert(empty.empty());
    std::cout << "compaction checks passed\n";
}
