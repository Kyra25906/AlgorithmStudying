#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>
#include "06-ranking-model.hpp"
// 有限域关系检查：只用于找反例，不能代替对任意输入的证明。
template<class Compare>
bool strictWeakOn(const std::vector<Student>& items,Compare comp) {
    auto equivalent = [&](const Student& a,const Student& b) { return !comp(a,b)&&!comp(b,a); };
    for (const auto& a : items) {
        if (comp(a,a)) return false;
        for (const auto& b : items) {
            if (comp(a,b)&&comp(b,a)) return false;
            for (const auto& c : items) {
                if (comp(a,b)&&comp(b,c)&&!comp(a,c)) return false;
                if (equivalent(a,b)&&equivalent(b,c)&&!equivalent(a,c)) return false;
            }
        }
    }
    return true;
}
int main() {
    std::vector<Student> items;
    for (int score : {0,60,100})
        for (const auto& name : {"A","Z"})
            for (int id : {1,2}) items.push_back({name,score,id});
    assert(strictWeakOn(items,before));
    assert(!strictWeakOn(items,[](const Student& a,const Student& b){return a.score>=b.score;}));
    assert(!strictWeakOn(items,[](const Student& a,const Student& b){return a.score>b.score||a.name<b.name;}));
    std::vector<Student> points{{"",1,1},{"",0,3},{"",2,2}};
    assert(!strictWeakOn(points,[](const Student& a,const Student& b){return a.score<b.score&&a.id<b.id;}));
    // 坏比较器从未交给 sort。
    std::vector<Student> tied{{"Z",90,1},{"A",90,2},{"B",100,3}};
    std::stable_sort(tied.begin(),tied.end(),[](const Student& a,const Student& b){return a.score>b.score;});
    assert(tied[0].id==3 && tied[1].id==1 && tied[2].id==2);
    std::sort(tied.begin(),tied.end(),before);
    assert(tied[0].id==3 && tied[1].id==2 && tied[2].id==1);
    std::cout << "comparator checks passed\n";
}
