#include <unordered_map>
#include <cassert>
#include <functional>
struct P{int x,y;bool operator==(const P& b)const{return x==b.x&&y==b.y;}};
struct H{std::size_t operator()(const P& p)const{return std::hash<int>{}(p.x)^ (std::hash<int>{}(p.y)<<1);}};
int main(){std::unordered_map<P,int,H> m; m.reserve(100);m[{1,2}]=7;assert(m.at({1,2})==7);assert(m.find({2,1})==m.end());}
