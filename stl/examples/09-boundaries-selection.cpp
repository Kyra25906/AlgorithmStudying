#include <algorithm>
#include <cassert>
#include <vector>
#include <iostream>
int main(){
 for(int n=0;n<=40;++n){
  std::vector<int> a; for(int i=0;i<n;++i) a.push_back((i*7)%9-4);
  auto sorted=a; std::sort(sorted.begin(),sorted.end());
  for(int x=-6;x<=6;++x){auto r=std::equal_range(sorted.begin(),sorted.end(),x); assert(r.second-r.first==std::count(a.begin(),a.end(),x));}
  for(int k=0;k<=n;++k){auto b=a; std::partial_sort(b.begin(),b.begin()+k,b.end()); assert(std::equal(b.begin(),b.begin()+k,sorted.begin()));}
  for(int k=1;k<=n;++k){auto b=a; std::nth_element(b.begin(),b.begin()+k-1,b.end()); assert(b[k-1]==sorted[k-1]);}
 }
 std::cout<<"lesson 09 passed\n";
}
