#include <algorithm>
#include <vector>
#include <iterator>
#include <cassert>
int main(){std::vector<int>a{1,2,2},b{2,3},o;std::set_union(a.begin(),a.end(),b.begin(),b.end(),std::back_inserter(o));assert((o==std::vector<int>{1,2,2,3}));std::vector<int>v{1,2,3};assert(std::next_permutation(v.begin(),v.end()));assert((v==std::vector<int>{1,3,2}));std::make_heap(v.begin(),v.end());assert(v.front()==3);}
