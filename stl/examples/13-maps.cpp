#include <map>
#include <cassert>
int main(){std::map<int,int> m; assert(m.find(3)==m.end()); assert(m[3]==0); m.try_emplace(3,8); assert(m.at(3)==0); m.insert_or_assign(3,8); assert(m.at(3)==8); std::multimap<int,int> mm{{1,2},{1,3}}; assert(mm.count(1)==2);}
