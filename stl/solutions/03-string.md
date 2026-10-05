# 第三课 string 练习参考答案

先独立完成[讲义练习](../lessons/03-string-parsing.md#11-分层练习与验收)，再看答案。以下均按 C++17 解释。

## 1 位置与截取

对于 `s="ab=cd=ef"`：

- p 是 2。
- `s.substr(0,p)` 是 `"ab"`。
- `s.substr(p+1)` 是 `"cd=ef"`，后面的等号不会自动参与第二次分割。
- `s.substr(2,4)` 是 `"=cd="`，取的是下标 `[2,6)`，不是 `[2,4)`。
- 原串仍为 `"ab=cd=ef"`；substr 返回独立的新串。

## 2 查找结果不是 bool

`find("ab")` 返回 0，在 if 中转成 false，但这恰恰表示开头匹配成功。未找到返回的 npos 是无符号最大值，反而转成 true。

`find("zz") >= 0` 对无符号返回值总成立，不能用来判断成功。

应写：

```cpp
const auto pos = s.find("ab");
if (pos != std::string::npos) {
    // 找到了，pos 才是可以使用的位置。
}
```

若 `find('=')` 返回 npos，先做 `+1` 会按无符号规则回绕为 0；随后 substr(0) 可能把整行当成值，掩盖缺少分隔符的问题。顺序必须是：查找 → 判断 → 运算。

## 3 三种读行方式

每次都从同一输入 `2\n\n  Bob\n` 开始，并已执行 `cin >> n`：

| 操作 | 第一次 getline 结果 | 原因 |
|---|---|---|
| 直接 getline | 空串 | 读走数量后面尚未取走的换行 |
| ignore 到换行，再 getline | 空串 | 数量行换行已丢弃，读到的是本来就存在的空正文行 |
| getline(cin >> ws, line) | `"Bob"` | ws 吃掉了两个换行和 Bob 前面的两个空格 |

前两种虽然第一次结果相同，读取的位置却不同：直接 getline 的下一次还是空串；ignore 后的下一次是 `"  Bob"`。要保留空行和前导空格，就按输入格式正确丢弃数量行，之后只用 getline。

## 4 边界与编码

- `substr(size())` 返回空串。
- `substr(size()+1)` 抛出 `std::out_of_range`（通常可表示的长度范围下）。
- C++17 读取 `s[s.size()]` 得到零字符；该位置不能改成非零字符，不算普通内容元素。
- `at(size())` 抛出 `std::out_of_range`。
- C++17 的 `u8"\u4E2D\u6587"` 编码长度为 6 个 char 元素。`substr(0,1)` 只取到“中”编码的一个字节，会截断 UTF-8 编码。

不要通过实际运行未定义行为验证边界规则。

## 5 保留空字段的逗号分割

下面是一份可独立编译运行的完整程序：

```cpp
#include <cassert>
#include <string>
#include <vector>

std::vector<std::string> splitComma(const std::string& text) {
    std::vector<std::string> fields;
    std::string::size_type begin = 0;
    while (true) {
        const auto pos = text.find(',', begin);
        if (pos == std::string::npos) {
            fields.push_back(text.substr(begin));
            break;
        }
        fields.push_back(text.substr(begin, pos - begin));
        begin = pos + 1;
    }
    return fields;
}

int main() {
    assert((splitComma("a,,b,") ==
            std::vector<std::string>{"a", "", "b", ""}));
    assert((splitComma("") == std::vector<std::string>{""}));
    assert((splitComma(",") == std::vector<std::string>{"", ""}));
    assert((splitComma(",a") == std::vector<std::string>{"", "a"}));
    assert((splitComma("abc") == std::vector<std::string>{"abc"}));
}
```

循环不变量：begin 总是下一个尚未输出字段的起点，且不超过 size。遇到尾逗号时 begin 会等于 size，但仍必须追加 `substr(size())` 这个空字段；所以循环不能写成 `while(begin < text.size())`。

使用逗号分隔的 getline 很方便，但空输入和尾逗号的空字段需要额外处理；不要假定它自动满足本题约定。

按常见顺序字符搜索实现分析，总扫描与复制时间 O(n+1)，额外空间 O(n+1)。这里不处理带引号的 CSV，`"a,b"` 不具有引号转义语义。

## 6 非重叠 replaceAll：读原串，写新串

不修改正在搜索的输入，不会重新扫描替换出来的内容，也不会因替换文本包含原模式而无限增长。

```cpp
#include <cassert>
#include <string>

std::string replaceAll(const std::string& text,
                       const std::string& from,
                       const std::string& to) {
    if (from.empty()) return text;
    std::string result;
    std::string::size_type begin = 0;
    while (true) {
        const auto pos = text.find(from, begin);
        if (pos == std::string::npos) {
            result += text.substr(begin);
            break;
        }
        result += text.substr(begin, pos - begin);
        result += to;
        begin = pos + from.size();
    }
    return result;
}

int main() {
    assert(replaceAll("aaaa", "aa", "b") == "bb");
    assert(replaceAll("a", "a", "aa") == "aa");
    assert(replaceAll("ababa", "aba", "X") == "Xba");
    assert(replaceAll("abcabc", "bc", "") == "aa");
    assert(replaceAll("abc", "z", "x") == "abc");
    assert(replaceAll("", "a", "b").empty());
    assert(replaceAll("abc", "", "x") == "abc");
    assert(replaceAll("aaaaa", "aa", "b") == "bba");
}
```

循环不变量：result 已包含原串 `[0,begin)` 按规则替换后的结果；begin 之后的原串尚未处理。from 非空时，每次命中至少前进一个元素，因此可以结束。

令输入长度 n、模式长度 m、输出长度 q。在朴素子串搜索与通常的摊销追加实现模型下，时间可按 O(nm+q+n) 估算，额外空间 O(n+q+1)（包含临时 substr）；这不是声称 C++17 string::find 有标准规定的 O(nm) 上界。大量文本的高效匹配算法留到后续算法专题。

本题要求使用 find/substr，所以采用新串写法；单次替换可以直接调用 `s.replace(pos, count, text)`，不需要自己循环复制。

## 7 文本解析器验收

参考完整实现：[03-text-parser.cpp](../examples/03-text-parser.cpp)。不要求逐字一样，但要满足相同输入约定。

| 输入场景（\n 表示换行） | 预期 |
|---|---|
| 空输入 | `count=0` |
| `  \t\n # comment\n` | `count=0` |
| ` name = Alice Smith \n` | 1 条，`name=Alice Smith` |
| `empty=\n` | 1 条，保留空值 `empty=` |
| `expr=a=b\n` | 1 条，值为 `a=b` |
| `x=1\nx=2\n` | 2 条，保留顺序和重复 key |
| `city=中文`，末尾无换行 | 1 条，按 UTF-8 原字节保留值 |
| `x=a#b\n` | 1 条，`#` 是值的一部分，不是行尾注释 |
| `broken\n` | 退出码 1，标准错误提示第 1 行缺少等号 |
| `=x\n` 或 `1key=x\n` | 退出码 1，第 1 行非法 key |
| `good=1\nbad key=2\n` | 退出码 1，第 2 行非法 key；标准输出为空 |
| 超出行数、单行或总长度限制 | 退出码 1，报告超限所在行 |

解释要点：

1. “第一个等号”是本任务的语法规则，后续等号属于值；不是 string 自动理解了键值对。
2. 空 key 不合法，空 value 合法，这是两套不同约束。
3. 保存到 `vector<string>` 可以维持原顺序和重复记录，不需要提前学 map。
4. 只裁剪 ASCII 边界空白，且不截断中文值中的字节，因此能保留合法 UTF-8；仍未做 UTF-8 合法性检查。
5. 延迟成功输出便于验收：输入中途出错，不会混入看似成功的前半段结果。
6. 解析器采用“重建规范化记录”，避免反复在原串中间 erase；状态演示负责练习 insert/erase/replace 本身。

这些参考实现和测试仅用于本地教学，不代表在线评测通过，也不能替代学习者独立编程。
