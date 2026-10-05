#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct Student {
    std::string name;
    int score = 0;
};
// 引用参数明确表示修改调用者数据。输入已保证范围，加法不会溢出。
void adjust(std::vector<Student>& students, int bonus) {
    for (auto& student : students) {
        student.score += bonus;
        if (student.score < 0) student.score = 0;
        if (student.score > 100) student.score = 100;
    }
}
// 只读引用避免复制整份记录；返回两个小数值，用 pair 表达。
std::pair<int,long long> summarize(const std::vector<Student>& students) {
    int passed = 0;
    long long total = 0;
    for (const auto& student : students) {
        if (student.score >= 60) ++passed;
        total += student.score;
    }
    return {passed,total};
}
int main() {
    int n, bonus;
    if (!(std::cin >> n >> bonus) || n < 0 || n > 100000 || bonus < -100 || bonus > 100) {
        std::cerr << "invalid header\n";
        return 1;
    }
    std::vector<Student> students;
    students.reserve(n);
    for (int i = 0; i < n; ++i) {
        Student student;
        if (!(std::cin >> student.name >> student.score) || student.name.size() > 100 ||
            student.score < 0 || student.score > 100) {
            std::cerr << "invalid student " << i + 1 << '\n';
            return 1;
        }
        students.push_back(student); // 保存独立记录，之后读取下一条
    }
    adjust(students,bonus);
    const auto [passed,total] = summarize(students);
    std::cout << "count=" << students.size() << '\n';
    std::cout << "passed=" << passed << '\n';
    std::cout << "total=" << total << '\n';
    for (const auto& [name,score] : students)
        std::cout << name << ' ' << score << '\n';
}
