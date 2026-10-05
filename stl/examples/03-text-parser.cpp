#include <iostream>
#include <string>
#include <vector>

// This small grammar trims only ASCII space, tab and carriage return.
std::string trim(const std::string& text) {
    const std::string::size_type first = text.find_first_not_of(" \t\r");
    if (first == std::string::npos) {
        return "";
    }
    const std::string::size_type last = text.find_last_not_of(" \t\r");
    return text.substr(first, last - first + 1);
}

bool isLetter(char ch) {
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool isValidKey(const std::string& key) {
    if (key.empty() || (!isLetter(key[0]) && key[0] != '_')) {
        return false;
    }
    for (char ch : key) {
        if (!isLetter(ch) && !(ch >= '0' && ch <= '9') && ch != '_') {
            return false;
        }
    }
    return true;
}

int main() {
    constexpr std::size_t maxLines = 10000;
    constexpr std::size_t maxLineBytes = 100000;
    constexpr std::size_t maxTotalBytes = 1000000;
    std::size_t lineNumber = 0;
    std::size_t totalBytes = 0;  // Excludes line-feed delimiters.
    std::vector<std::string> records;
    std::string line;

    while (std::getline(std::cin, line)) {
        ++lineNumber;
        if (lineNumber > maxLines || line.size() > maxLineBytes ||
            line.size() > maxTotalBytes - totalBytes) {
            std::cerr << "line " << lineNumber << ": input limit exceeded\n";
            return 1;
        }
        totalBytes += line.size();
        const std::string text = trim(line);
        if (text.empty() || text[0] == '#') {
            continue;
        }
        const std::string::size_type pos = text.find('=');
        if (pos == std::string::npos) {
            std::cerr << "line " << lineNumber << ": missing '='\n";
            return 1;
        }
        const std::string key = trim(text.substr(0, pos));
        const std::string value = trim(text.substr(pos + 1));
        if (!isValidKey(key)) {
            std::cerr << "line " << lineNumber << ": invalid key\n";
            return 1;
        }
        // Rebuild instead of repeatedly erasing characters from the middle.
        records.push_back(key + "=" + value);
    }
    if (std::cin.bad() || !std::cin.eof()) {
        std::cerr << "input error\n";
        return 1;
    }

    // Delay output until the whole input is valid: no partial success output.
    std::cout << "count=" << records.size() << '\n';
    for (const std::string& record : records) {
        std::cout << record << '\n';
    }
}
