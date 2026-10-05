#include <iostream>
#include <vector>

// 读入行数 r、列数 c 与 r×c 个整数（行优先），输出每行之和与转置矩阵。
int main() {
    int r, c;
    if (!(std::cin >> r >> c) || r < 1 || c < 1 || r > 500 || c > 500) {
        std::cerr << "invalid dimensions\n";
        return 1;
    }
    std::vector<std::vector<int>> m(r, std::vector<int>(c));
    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j)
            if (!(std::cin >> m[i][j])) {
                std::cerr << "invalid matrix element\n";
                return 1;
            }

    std::cout << "row sums:";
    for (int i = 0; i < r; ++i) {
        long long sum = 0;
        for (int j = 0; j < c; ++j) sum += m[i][j];
        std::cout << ' ' << sum;
    }
    std::cout << '\n';

    std::cout << "transpose:\n";
    for (int j = 0; j < c; ++j) {
        for (int i = 0; i < r; ++i) {
            if (i) std::cout << ' ';
            std::cout << m[i][j];
        }
        std::cout << '\n';
    }
}
