#include <iostream>
#include <vector>

// 读入 n 与 n 个整数，再读入目标值 x，删除所有等于 x 的元素。
// 两种写法对照：写法一利用 erase 返回值，写法三筛选到新容器。
int main() {
    int n;
    if (!(std::cin >> n) || n < 0 || n > 100000) {
        std::cerr << "invalid count\n";
        return 1;
    }
    std::vector<int> a;
    a.reserve(n);
    for (int i = 0; i < n; ++i) {
        int v;
        if (!(std::cin >> v)) {
            std::cerr << "invalid value\n";
            return 1;
        }
        a.push_back(v);
    }
    int x;
    if (!(std::cin >> x)) {
        std::cerr << "invalid target\n";
        return 1;
    }

    // 写法一：erase 返回下一个元素，删除时不自增迭代器
    std::vector<int> byErase = a;
    for (auto it = byErase.begin(); it != byErase.end(); ) {
        if (*it == x) it = byErase.erase(it);
        else ++it;
    }

    // 写法三：筛选到新容器，O(n) 时间
    std::vector<int> kept;
    kept.reserve(a.size());
    for (int v : a)
        if (v != x) kept.push_back(v);

    if (kept != byErase) {
        std::cerr << "mismatch between erase and filter\n";
        return 2;
    }

    std::cout << "count=" << kept.size() << '\n';
    std::cout << "kept:";
    for (int v : kept) std::cout << ' ' << v;
    std::cout << '\n';
}
