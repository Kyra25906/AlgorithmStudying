#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
struct Record { int score; int id; };
int main() {
    std::vector<Record> records{{60,1},{90,2},{60,3}};
    std::stable_sort(records.begin(),records.end(),[](const Record& a,const Record& b){return a.score>b.score;});
    assert(records[0].id==2 && records[1].id==1 && records[2].id==3);
    auto even=[](int x){return x%2==0;};
    std::vector<int> a{3,2,1,4};
    auto cut=std::partition(a.begin(),a.end(),even);
    assert(cut-a.begin()==2);
    assert(std::all_of(a.begin(),cut,even));
    assert(std::none_of(cut,a.end(),even));
    assert(std::is_partitioned(a.begin(),a.end(),even));
    auto sorted=a;
    std::sort(sorted.begin(),sorted.end());
    assert((sorted==std::vector<int>{1,2,3,4})); // 校验多重集合，不卡组内次序
    a={3,2,1,4};
    cut=std::stable_partition(a.begin(),a.end(),even);
    assert((a==std::vector<int>{2,4,3,1}) && cut-a.begin()==2);
    for (int n=0;n<=8;++n) {
        for (int k=0;k<=n;++k) {
            std::vector<int> values(n);
            for (int i=0;i<n;++i) values[i]=i;
            auto result=std::rotate(values.begin(),values.begin()+k,values.end());
            assert(result==values.begin()+(n-k));
            for (int i=0;i<n;++i) assert(values[i]==(i+k)%n);
            std::reverse(values.begin(),values.end());
            for (int i=0;i<n;++i) assert(values[i]==(n-1-i+k)%n);
        }
    }
    std::vector<int> empty;
    assert(std::partition(empty.begin(),empty.end(),even)==empty.end());
    std::cout << "reordering checks passed\n";
}
