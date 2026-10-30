#include <map>
#include <cassert>
int main(){std::map<int,int>a{{1,10}},b{{1,20},{2,30}};auto node=a.extract(1);node.key()=3;auto result=a.insert(std::move(node));assert(result.inserted);a.insert({1,10});a.merge(b);assert(a.size()==3&&b.size()==1&&b.count(1)==1);}


