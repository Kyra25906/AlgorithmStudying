#include <iostream>
#include <vector>
int main() {
    int n;
    if (!(std::cin >> n) || n < 0 || n > 100000) {
        std::cerr << "invalid count\n";
        return 1;
    }
    std::vector<int> scores;
    scores.reserve(n); // 只有空间，元素由 push_back 建立
    for (int i=0; i<n; ++i) {
        int x;
        if (!(std::cin >> x) || x < 0 || x > 100) {
            std::cerr << "invalid score\n";
            return 1;
        }
        scores.push_back(x);
    }
    long long total = 0;
    std::vector<int> passed;
    passed.reserve(scores.size());
    for (int x : scores) {
        total += x;
        if (x >= 60) passed.push_back(x);
    }
    std::cout << "total=" << total << '\n';
    std::cout << "passed=" << passed.size() << '\n';
    std::cout << "scores:";
    for (int x : passed) std::cout << ' ' << x;
    std::cout << '\n';
}
