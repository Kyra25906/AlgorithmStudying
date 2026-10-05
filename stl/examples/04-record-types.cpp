#include <cassert>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
struct Student { std::string name; int score = 0; };
int main() {
    std::pair<int,long long> stats{3,240};
    assert(stats.first == 3 && stats.second == 240);
    assert((std::pair<int,int>{1,9} < std::pair<int,int>{2,0}));
    std::tuple<std::string,int,bool> item{"Alice",80,true};
    std::get<1>(item) = 85;
    assert(std::get<1>(item) == 85);
    int count = 0;
    long long total = 0;
    std::tie(count,total) = std::make_tuple(3,240LL);
    std::tie(std::ignore,total) = std::make_tuple(9,500LL);
    assert(count == 3 && total == 500);

    Student s{"Alice",80};
    auto [nameCopy,scoreCopy] = s;
    scoreCopy = 0;
    assert(s.score == 80 && scoreCopy == 0 && nameCopy == "Alice");
    auto& [nameRef,scoreRef] = s;
    scoreRef = 90;
    assert(s.score == 90 && &nameRef == &s.name);
    const auto& [nameRead,scoreRead] = s;
    assert(nameRead == "Alice" && scoreRead == 90);
    std::vector<Student> students{{"A",10},{"B",20}};
    for (auto& [name,score] : students) { score += 1; assert(!name.empty()); }
    assert(students[0].score == 11 && students[1].score == 21);
    int x = 1, y = 2;
    auto tied = std::tie(x,y); // 引用元组不拥有 x/y
    auto [a,b] = tied;        // 复制引用元组，仍关联原对象
    a = 9;
    assert(x == 9 && b == 2);
    Student empty{};
    assert(empty.name.empty() && empty.score == 0);
    std::cout << "record and binding checks passed\n";
}
