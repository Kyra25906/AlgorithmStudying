#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
int main() {
    int n,l,r;
    if (!(std::cin >> n >> l >> r) || n < 0 || n > 100000 || l < 0 || l > r || r > n) {
        std::cerr << "invalid range\n";
        return 1;
    }
    std::vector<int> source(n);
    for (int& value : source) {
        if (!(std::cin >> value)) {
            std::cerr << "invalid value\n";
            return 1;
        }
    }
    // 检查完边界再形成区间；目标独立，不会使源迭代器失效。
    std::vector<int> selected;
    selected.reserve(r-l);
    std::copy(source.cbegin()+l,source.cbegin()+r,std::back_inserter(selected));
    std::cout << "count=" << selected.size() << '\n';
    std::cout << "forward:";
    for (auto it=selected.cbegin(); it!=selected.cend(); ++it) std::cout << ' ' << *it;
    std::cout << "\nreverse:";
    for (auto it=selected.crbegin(); it!=selected.crend(); ++it) std::cout << ' ' << *it;
    std::cout << '\n';
}
