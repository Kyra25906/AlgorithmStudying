#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

int main() {
    std::string s = "name=alice";
    const std::string::size_type pos = s.find('=');
    assert(pos == 4);
    assert(s.substr(0, pos) == "name");
    assert(s.substr(pos + 1) == "alice");
    assert(s == "name=alice");  // substr does not change the source.
    std::cout << "key=" << s.substr(0, pos) << '\n';
    std::cout << "value=" << s.substr(pos + 1) << '\n';

    s.replace(pos + 1, 5, "bob");
    assert(s == "name=bob");
    s.insert(0, "user.");
    assert(s == "user.name=bob");
    s.erase(0, 5);
    assert(s == "name=bob");
    std::cout << "edited=" << s << '\n';

    assert(s.find("xyz") == std::string::npos);
    assert(s.find("name") == 0);  // Zero is a valid match, not failure.
    assert(s.find("", s.size()) == s.size());
    assert(s.substr(s.size()).empty());
    assert(s.substr(5, 100) == "bob");  // Count is clipped at the end.
    assert(s[s.size()] == '\0');       // Read-only boundary demonstration.
    bool threw = false;
    try {
        (void)s.at(s.size());
    } catch (const std::out_of_range&) {
        threw = true;
    }
    assert(threw);

    std::string copy = s;
    copy[0] = 'N';
    assert(s[0] == 'n');
    std::string repeated(3, 'x');
    assert(repeated == "xxx");
    repeated += "yz";
    repeated.push_back('!');
    assert(repeated == "xxxyz!");
    repeated.pop_back();
    assert(repeated == "xxxyz");

    const std::string bytes("A\0B", 3);
    assert(bytes.size() == 3);
    assert(bytes.find('B') == 2);
    const std::string asCString(bytes.c_str());
    assert(asCString == "A");
    std::cout << "embedded_nul_size=" << bytes.size() << '\n';

    // In C++17, u8 literals explicitly provide UTF-8 char code units.
    const std::string utf8 = u8"\u4E2D\u6587";
    assert(utf8.size() == 6);
    assert(utf8.substr(0, 3) == u8"\u4E2D");
    assert(utf8.substr(0, 1).size() == 1);  // Not a complete UTF-8 character.
    std::cout << "utf8_bytes=" << utf8.size() << '\n';
    std::cout << "all checks passed\n";
}
