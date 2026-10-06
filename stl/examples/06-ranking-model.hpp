#ifndef STL_LESSON_06_RANKING_MODEL_HPP
#define STL_LESSON_06_RANKING_MODEL_HPP
#include <string>
struct Student {
    std::string name;
    int score = 0;
    int id = 0;
};
// 从最重要的关键字开始，仅在相同时进入下一项。
inline bool before(const Student& a,const Student& b) {
    if (a.score != b.score) return a.score > b.score;
    if (a.name != b.name) return a.name < b.name;
    return a.id < b.id;
}
struct AtLeast {
    int limit;
    bool operator()(int score) const { return score >= limit; }
};
#endif
