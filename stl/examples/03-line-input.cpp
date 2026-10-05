#include <iostream>
#include <limits>
#include <string>
#include <vector>

// Input: n on the first line, followed by exactly n text lines.
// The remainder of the first line is intentionally discarded.
int main() {
    int n = 0;
    if (!(std::cin >> n) || n < 0 || n > 10000) {
        std::cerr << "invalid count\n";
        return 1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::vector<std::string> lines;
    lines.reserve(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) {
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cerr << "missing line " << i + 1 << '\n';
            return 1;
        }
        lines.push_back(line);
    }
    if (std::cin.bad()) {
        std::cerr << "input error\n";
        return 1;
    }

    for (std::size_t i = 0; i < lines.size(); ++i) {
        std::cout << "line " << i + 1 << ": [" << lines[i]
                  << "], size=" << lines[i].size() << '\n';
    }
}
