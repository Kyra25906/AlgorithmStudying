#include <algorithm>
#include <iostream>
#include <vector>
int main() {
    int n,k;
    if (!(std::cin>>n>>k) || n<0 || n>100000 || k<0 || k>1000000000) {
        std::cerr << "invalid header\n";
        return 1;
    }
    std::vector<int> a(n);
    for (int& x : a) {
        if (!(std::cin>>x) || x< -1000000 || x>1000000) {
            std::cerr << "invalid value\n";
            return 1;
        }
    }
    const auto originalSize=a.size();
    a.erase(std::remove_if(a.begin(),a.end(),[](int x){return x<0;}),a.end());
    const auto negativeRemoved=originalSize-a.size();
    const auto beforeUnique=a.size();
    std::sort(a.begin(),a.end());
    a.erase(std::unique(a.begin(),a.end()),a.end());
    const auto duplicateRemoved=beforeUnique-a.size();
    const auto sorted=a; // 保留一份用于展示旋转前结果
    if (!a.empty()) {
        const auto shift=static_cast<std::size_t>(k)%a.size();
        std::rotate(a.begin(),a.begin()+shift,a.end());
    }
    std::cout << "negative_removed=" << negativeRemoved << '\n';
    std::cout << "duplicate_removed=" << duplicateRemoved << '\n';
    std::cout << "count=" << a.size() << "\nsorted:";
    for (int x : sorted) std::cout << ' ' << x;
    std::cout << "\nrotated:";
    for (int x : a) std::cout << ' ' << x;
    std::cout << '\n';
}
