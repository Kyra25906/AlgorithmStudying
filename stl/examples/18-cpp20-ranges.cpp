#include <map>
#include <vector>
#include <algorithm>
#include <ranges>
#include <cassert>
struct Item{int score;};
int main(){std::map<int,int>m{{1,2}};assert(m.contains(1));std::vector<int>v{1,2,3,4};assert(std::erase_if(v,[](int x){return x%2==0;})==2);std::vector<Item>a{{3},{1},{2}};std::ranges::sort(a,{},&Item::score);assert(a[0].score==1);auto odd=v|std::views::filter([](int x){return x%2;});assert(*odd.begin()==1);}
