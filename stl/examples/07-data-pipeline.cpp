#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>
int main() {
    int n,threshold;
    if (!(std::cin>>n>>threshold) || n<0 || n>100000 || threshold< -1000000 || threshold>1000000) {
        std::cerr << "invalid header\n";
        return 1;
    }
    std::vector<int> source(n);
    for (int& x : source) {
        if (!(std::cin>>x) || x< -1000000 || x>1000000) {
            std::cerr << "invalid value\n";
            return 1;
        }
    }
    auto accept=[threshold](int x){return x>=threshold;};
    const auto first=std::find_if(source.cbegin(),source.cend(),accept);
    const auto count=std::count_if(source.cbegin(),source.cend(),accept);
    const auto [lo,hi]=std::minmax_element(source.cbegin(),source.cend());
    std::vector<int> kept;
    std::copy_if(source.cbegin(),source.cend(),std::back_inserter(kept),accept);
    std::vector<long long> squares;
    // 提升发生在乘法之前，而不只是赋值给 long long 时。
    std::transform(kept.cbegin(),kept.cend(),std::back_inserter(squares),[](int x){return 1LL*x*x;});
    const long long total=std::accumulate(squares.cbegin(),squares.cend(),0LL);
    std::vector<long long> prefix(squares.size());
    std::partial_sum(squares.cbegin(),squares.cend(),prefix.begin());
    std::cout << "first=";
    if (first==source.cend()) std::cout << "none\n";
    else std::cout << first-source.cbegin() << '\n';
    std::cout << "count=" << count << '\n';
    if (source.empty()) std::cout << "min=none max=none\n";
    else std::cout << "min=" << *lo << " max=" << *hi << '\n';
    std::cout << "sum=" << total << "\nsquares:";
    for (long long x : squares) std::cout << ' ' << x;
    std::cout << "\nprefix:";
    for (long long x : prefix) std::cout << ' ' << x;
    std::cout << '\n';
}
