#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include "06-ranking-model.hpp"
bool validName(const std::string& name) {
    if (name.empty() || name.size()>40) return false;
    for (char c : name)
        if (!((c>='A'&&c<='Z')||(c>='a'&&c<='z'))) return false;
    return true;
}
int main() {
    int n,limit;
    if (!(std::cin>>n>>limit) || n<0 || n>10000 || limit<0 || limit>100) {
        std::cerr << "invalid header\n";
        return 1;
    }
    std::vector<Student> students;
    students.reserve(n);
    for (int i=0;i<n;++i) {
        Student student;
        if (!(std::cin>>student.name>>student.score) || !validName(student.name) || student.score<0 || student.score>100) {
            std::cerr << "invalid student " << i+1 << '\n';
            return 1;
        }
        student.id=i+1;
        students.push_back(student);
    }
    std::vector<Student> selected;
    selected.reserve(students.size());
    std::copy_if(students.cbegin(),students.cend(),std::back_inserter(selected),
                 [limit](const Student& s){return s.score>=limit;});
    std::sort(selected.begin(),selected.end(),before);
    std::cout << "count=" << selected.size() << '\n';
    for (std::size_t i=0;i<selected.size();++i) {
        const auto& s=selected[i];
        std::cout << i+1 << ' ' << s.name << ' ' << s.score << ' ' << s.id << '\n';
    }
    return 0;
}
