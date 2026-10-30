#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <iostream>
int main(){
 std::set<int> unique; assert(unique.insert(3).second); assert(!unique.insert(3).second);
 std::multiset<int> s; std::vector<int> model;
 for(int i=0;i<100;++i){
  int x=(i*7)%13-6;s.insert(x);model.push_back(x);
  if(i%3==0){auto it=s.find(x);assert(it!=s.end());s.erase(it);model.erase(std::find(model.begin(),model.end(),x));}
  auto sorted=model;std::sort(sorted.begin(),sorted.end());
  assert(std::vector<int>(s.begin(),s.end())==sorted);
  for(int key=-8;key<=8;++key){
   assert(s.count(key)==static_cast<std::size_t>(std::count(model.begin(),model.end(),key)));
   auto lo=s.lower_bound(key),hi=s.upper_bound(key);
   auto expectedLo=std::find_if(sorted.begin(),sorted.end(),[key](int v){return v>=key;});
   auto expectedHi=std::find_if(sorted.begin(),sorted.end(),[key](int v){return v>key;});
   assert(std::distance(s.begin(),lo)==expectedLo-sorted.begin());
   assert(std::distance(s.begin(),hi)==expectedHi-sorted.begin());
  }
 }
 std::multiset<int> repeated{1,1,2}; assert(repeated.erase(1)==2);assert(repeated.size()==1);
 for(auto it=s.begin();it!=s.end();) {if(*it<0)it=s.erase(it);else ++it;}
 assert(std::all_of(s.begin(),s.end(),[](int x){return x>=0;}));
 s.clear();assert(s.lower_bound(0)==s.end());
 std::cout<<"lesson 12 passed\n";
}
