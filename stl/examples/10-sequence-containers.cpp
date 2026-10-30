#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>
#include <cassert>
#include <iostream>
int main(){
 std::array<int,3> a{}; assert(a[0]==0);
 std::deque<int> q; q.push_back(2); q.push_front(1); assert(q.front()==1); q.pop_front(); assert(q.back()==2);
 std::list<int> src{1,2,3},dst{9}; auto it=src.begin(); ++it; dst.splice(dst.begin(),src,it);
 assert(*it==2); assert((std::vector<int>(src.begin(),src.end())==std::vector<int>{1,3}));
 assert((std::vector<int>(dst.begin(),dst.end())==std::vector<int>{2,9}));
 for(int n=0;n<=30;++n){
  std::forward_list<int> f; for(int i=n-1;i>=0;--i) f.push_front(i);
  auto prev=f.before_begin(); auto cur=f.begin();
  while(cur!=f.end()){if(*cur%2==0) cur=f.erase_after(prev); else {prev=cur;++cur;}}
  std::vector<int> expected; for(int i=0;i<n;++i) if(i%2) expected.push_back(i);
  assert(std::vector<int>(f.begin(),f.end())==expected);
 }
 std::cout<<"lesson 10 passed\n";
}
