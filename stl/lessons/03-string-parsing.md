# 第三课 string 的输入、查找、截取、替换与编码边界

## 本课的已知和所求

已知：你会 C 数组、循环、函数，以及前两课的 vector、下标、引用和基本遍历。

所求：处理包含空格的一整行文本，按分隔符找出字段，截取和修改内容；区分“找不到”和“在开头找到”；不把 UTF-8 字节数误认为汉字数。最终写出一个有明确规则和错误提示的 `key=value` 文本解析器。

完成本课应能：正确混用 `>>` 与 `getline`；解释 `find`、`npos`、`substr(pos, count)`；使用 `insert/erase/replace`；判断字符串边界及修改后的引用风险；实现解析、测试并说明复杂度。

代码入口：[状态演示](../examples/03-string-ops.cpp) · [整行输入](../examples/03-line-input.cpp) · [文本解析器](../examples/03-text-parser.cpp)。先学正文，再做第 11 节练习，最后看[参考答案](../solutions/03-string.md)。

## 1 从 C 字符数组出发：为什么需要 string

假设输入一行 `name = Alice Smith`，希望得到键 `name` 和值 `Alice Smith`。

第一反应可能是用 `char buf[100]`，一边扫描一边寻找 `=`。问题不是这种办法不能写，而是还得自己处理容量、有效长度、终止符和复制边界；按单词读取又会把名字截断。

`std::string` 管理一串连续的 `char` 元素，并记录长度和存储空间。头文件是 `<string>`，不是 `<cstring>`。

```cpp
#include <string>
std::string a;                 // 空串
std::string b = "hello";      // 5 个 char 元素
std::string c(3, 'x');         // "xxx"：数量 + 单个字符
std::string d = b;            // 独立副本
```

单引号 `'x'` 是字符，双引号 `"x"` 是字符串字面量，两者不是同一类型。`string` 是标准库字符串类型，不应认为它完全等同于 `vector<char>`。

## 2 数据模型：内容、长度和终止符

```text
s = "name=alice"，size() = 10
下标    0 1 2 3 4 5 6 7 8 9 | 10
内容    n a m e = a l i c e | '\0'
        [有效字符元素 0,10)  | 结尾终止符
```

- `size()` 与 `length()` 都返回 `char` 元素数量，不包含结尾终止符；返回类型是 `std::string::size_type`，一种无符号整数类型。
- `empty()` 判断长度是否为零。
- `capacity()` 是当前存储容量信息，不是字符串内容长度。容量增长倍率和短字符串优化（SSO）不是本课依赖的标准保证。
- 正常访问、修改内容时，使用 `0 <= i < s.size()`。`front()`、`back()` 和 `pop_back()` 要求非空。
- C++17 有一个与 vector 不同的特殊边界：读取 `s[s.size()]` 合法，得到 `'\0'`；把这个位置改成非零字符是未定义行为。它不是可追加的普通元素。
- `s.at(i)` 会检查边界，`i >= size()` 抛出 `std::out_of_range`；`s[i]` 在 `i > size()` 时是未定义行为，不能指望它报错。

### 字符串中间也能有零字符

```cpp
std::string bytes("A\0B", 3);
// bytes.size() == 3；B 的下标是 2。
std::string cstyle(bytes.c_str());
// cstyle == "A"：按 C 字符串规则遇到第一个 '\0' 就结束。
```

`c_str()` 提供结尾有零字符的只读指针，用于某些 C 接口。`string` 自己按长度管理内容；C 接口可能只按终止符处理内容，两者不能混为一谈。不要在字符串销毁后继续使用这个指针；修改字符串后也应重新取得它。

## 3 输入：读单词，还是读一整行

输入设施来自 `<iostream>`；`std::getline` 的字符串重载来自 `<string>`。

| 写法 | 从输入中读取什么 | 如何处理分隔符 | 返回值与修改 |
|---|---|---|---|
| `std::cin >> s` | 默认跳过前导空白，再读到下一个空白之前 | 终止读取的空白留在流里 | 返回输入流引用，改写 s |
| `std::getline(std::cin, s)` | 从当前位置读到换行或 EOF | 换行被取走，不放进 s | 返回输入流引用，改写 s |
| `std::getline(std::cin, s, ',')` | 读到逗号或 EOF | 逗号被取走，不放进 s | 同上 |

例如输入 `Alice Smith`：`>>` 先得到 `Alice`，`getline` 可以得到整行 `Alice Smith`。若确实读到一个换行，`getline` 成功且允许结果为空；若 EOF 前还有非空内容，即使末尾没有换行，也能成功读出最后一行。

始终检查读取结果，不要使用 `while (!std::cin.eof())`：只有尝试读取后才知道是否真的到达输入结尾。

```cpp
std::string line;
while (std::getline(std::cin, line)) {
    // 处理这一行；line 可能是空字符串。
}
```

### 混用 >> 和 getline 的“空行”从哪里来

输入 `2\nAlice Smith\nBob\n`，执行 `cin >> n` 后，`2` 被读走，但它后面的换行还在。下一次 `getline` 立刻读到这个换行，于是得到空串。

```cpp
#include <limits>
int n;
if (!(std::cin >> n)) return 1;
std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
std::string line;
if (!std::getline(std::cin, line)) return 1;
```

`ignore` 丢弃当前行剩余内容（包括换行），返回流引用，不修改已有字符串。这里的前提是：数量独占第一行，正文从下一行开始。如果正文紧跟在数量同一行，不能照抄这段丢弃逻辑。只调用一次无参数 `ignore()` 仅丢弃一个字符，遇到数字后的多个空格会出错。

`getline(cin >> std::ws, line)` 可以跳过前面的所有空白，但也会跳过空行、吃掉正文开头的空格；本课需要保留它们，所以不采用这个捷径。

## 4 查找：find 返回的是位置，不是布尔值

```cpp
std::string s = "name=alice";
std::string::size_type pos = s.find('=');  // pos == 4
if (pos != std::string::npos) {
    // 现在才可以把 pos 当作找到的位置。
}
```

| 接口（均来自 `<string>`） | 参数含义 | 返回值 | 修改原串？ |
|---|---|---|---|
| `s.find(ch, pos = 0)` | 从 pos 开始向后找字符 ch | 首次匹配的位置，失败为 npos | 否 |
| `s.find(pattern, pos = 0)` | 从 pos 开始向后找子串 | 首次匹配的起点，失败为 npos | 否 |
| `s.rfind(ch)` | 从后向前找字符 | 最后一次出现的位置，失败为 npos | 否 |
| `s.find_first_not_of(chars)` | 从开头找“不在字符集合 chars 中”的元素 | 位置或 npos | 否 |
| `s.find_last_not_of(chars)` | 从末尾找“不在字符集合 chars 中”的元素 | 位置或 npos | 否 |

`find_first_not_of(" \t\r")` 中的参数是一组字符，不是要匹配的连续子串。

**两种常见错误：**

```cpp
// 错误逻辑：在开头找到返回 0，反而进入不了分支。
if (s.find("name")) { /* ... */ }

// 错误逻辑：无符号返回值不会小于 0，不能靠 >= 0 判断找到。
if (s.find("xyz") >= 0) { /* ... */ }
```

`npos` 是 `size_type` 可表示的最大值，用作“无位置”哨兵。不要把查找结果先塞进 `int`，也不要在判断前做 `pos + 1`：`npos + 1` 会按无符号运算回绕为 0，而不是自动报错。

空模式也有定义：`s.find("", p)` 在 `p <= s.size()` 时返回 p，否则返回 npos。因此“替换所有匹配”的循环必须先规定空模式怎么处理。

## 5 截取：substr 的第二个参数是数量

```cpp
std::string s = "name=alice";
const auto pos = s.find('=');  // auto 从表达式推导类型，此处是 size_type。
if (pos != std::string::npos) {
    std::string key = s.substr(0, pos);  // 从 0 开始取 4 个元素：name
    std::string value = s.substr(pos + 1); // 从 5 开始取到末尾：alice
}
```

`substr(pos = 0, count = npos)` 返回一个新的独立字符串，**不修改原串，也不是对原串的引用**。实际截取数量为 `min(count, size() - pos)`，前提是 pos 合法。

```text
s.substr(2, 4) 截取下标 [2,6)，得到 "me=a"
                       ^   ^
                     起点  起点+数量
```

- `substr(size())`：合法，得到空串。
- `substr(size() + 1)`：抛出 `std::out_of_range`。
- count 比剩余长度大：只取到末尾，不因这个原因越界。
- `substr(a, b)` 不是区间 `[a,b)`。若要该区间，在已验证 `a <= b <= size()` 后写 `substr(a, b - a)`。

这是本课最重要的一条区分：**位置告诉你从哪里开始，数量告诉你取多少。**

## 6 修改：先推演状态，再写接口

```text
"name=alice"
  replace(5, 5, "bob") → "name=bob"
  insert(0, "user.")   → "user.name=bob"
  erase(0, 5)          → "name=bob"
```

下表仅讨论“下标 + 数量”重载，不混入迭代器重载：

| 接口（头文件 `<string>`） | 行为与参数 | 返回值 | 边界 |
|---|---|---|---|
| `s += text` / `s.append(text)` | 追加整个 text | `string&`，引用 s | 最终长度不能超过 max_size |
| `s.push_back(ch)` | 追加一个字符 | void | 同上 |
| `s.pop_back()` | 删除最后一个字符 | void | s 必须非空 |
| `s.insert(pos, text)` | 在 pos 之前插入 text | `string&` | pos <= size |
| `s.erase(pos, count)` | 从 pos 起删除最多 count 个元素 | `string&` | pos <= size，数量过大截到末尾 |
| `s.replace(pos, count, text)` | 把从 pos 起的最多 count 个元素替换为 text | `string&` | 同上，text 可以与原片段不同长度 |

位置大于 size 时，上表的下标版 insert/erase/replace 抛出 `std::out_of_range`。`erase(size(), n)` 是合法空操作；`insert(size(), text)` 相当于在末尾插入。长度过大可能抛出 `std::length_error`，分配失败可能抛出 `std::bad_alloc`，本课输入上限不是“绝不会分配失败”的承诺。

注意第二课的 vector `erase` 返回迭代器，而这里的 string 下标版 `erase` 返回原字符串的引用；不能把不同重载的返回值规则直接照搬。

### 修改后，旧引用和旧位置还可信吗

```cpp
std::string s = "abc";
char& ref = s[0];
s += "some more text";
// 不再使用 ref；需要时重新取得 s[0]。
```

C++17 的 string 不能直接套用 vector 的全部失效规则。传给标准库的非 const 引用（如输入函数），以及大多数非 const 成员修改，都可能使元素指针、引用或迭代器失效。元素访问类成员（`operator[]/at/front/back/data`）及 `begin/end/rbegin/rend` 等取迭代器操作有相应不失效保证；具体操作的更强保证以其标准条款为准。

本课采用保守安全规则：**append、insert、erase、replace、赋值或重新输入之后，重新取得所需的引用、指针和迭代器；不要仅凭 capacity 没变推断安全。** 同时，中间插删会移动内容；整数下标本身虽不会悬空，但可能已经不再对应原来的字符。

## 7 复杂度：别把一次调用看成一步

令 n 为原串长度，m 为模式长度，k 为截取或插入的长度。

| 操作 | 分析 |
|---|---|
| size/length/empty、下标访问 | O(1) |
| substr 得到 k 个元素 | O(k)，复制出新字符串，额外空间 O(k) |
| 复制字符串 | O(n) |
| 查找一个字符 | 按通常的顺序扫描模型分析为 O(n) |
| 查找一个长度 m 的子串 | 朴素匹配最坏 O(nm)；不能假定所有库都采用同一种算法 |
| 中间 insert/erase/replace | 常见连续存储实现需要搬移后缀，按 O(n+k) 估算 |
| getline 读取一行 | 与读取的元素数量成正比（通常实现分析） |

特别说明：**C++17 没有为 `basic_string::find` 规定渐近复杂度上界**；上面的搜索分析是在说明常见扫描/朴素算法，不是给库接口补上标准承诺。不要宣称一次子串 find 必然是 O(n)。

若在长度 n 的字符串开头反复 `erase(0, 1)`，搬移总量可能是 `n + (n-1) + ... + 1`，达到 O(n²)。去掉首尾空白可以先找边界，再一次性 substr；批量过滤可以向新串追加，避免反复移动中间元素。

## 8 编码边界：string 不知道“一个汉字”是什么

在本课的 UTF-8 数据中，ASCII 字符占 1 字节，一个汉字通常占 3 字节，但不能推广成“所有文字都占 3 字节”。某些 Unicode 字符需 4 字节；用户眼中的一个图形还可能由多个码点组成。

```cpp
// C++17 写法；用转义确保不依赖编辑器是否直接写入汉字。
std::string s = u8"\u4E2D\u6587"; // "中文" 的 UTF-8 编码
// s.size() == 6，而不是 2。
// s.substr(0, 3) 是完整的 "中"。
// s.substr(0, 1) 只是编码的第一个字节，不是完整的汉字。
```

本课限定 C++17；C++20 中 u8 字面量的元素类型改为 `char8_t`，不能直接照抄上面的 string 初始化。

结论：

1. string 的 size、find 位置和 substr 数量以 char 元素计，本课 UTF-8 场景就是字节，不是汉字数。
2. 逐字节反转、截断 UTF-8 可能破坏编码；“截前 5 个字”不能直接写 `substr(0, 5)`。
3. 本课解析器只识别 ASCII 的 `=`、`#`、空格等语法字符，不对值中的中文逐字切割；合法 UTF-8 输入的中文值可按原字节保留。
4. 这不等于解析器验证了 UTF-8，也不等于它支持通用 Unicode 空白、大小写转换或字素处理。全角空格不属于本课 trim 的空白集合。

## 9 完整任务：一个规则明确的文本解析器

### 9.1 输入与输出约定

读到 EOF，每行是一条记录或注释。不要先引入 map，先复用学过的 `vector<string>` 保存规范化结果。

- 去掉行首尾的 ASCII 空格、制表符和回车（`" \t\r"`）；空行忽略。
- 去掉前导空白后的第一个字符为 `#` 时，整行是注释。
- 非注释行必须含 `=`，**只用第一个等号分割**。
- key/value 两侧都去掉上述空白，值内部空格保留。
- key 的首字符只能是 ASCII 字母或下划线，其余字符还允许数字。key 不能为空。
- value 允许为空，允许中文、更多等号和 `#`。不支持行尾注释、引号语法或转义语法。
- 重复 key 保留，按输入顺序输出；本程序不是字典，不采用“后者覆盖前者”。
- 最多 10000 行、单行最多 100000 个 char 元素、累计最多 1000000 个 char 元素（不计 getline 取走的换行）。长度是读完该行后检查，不是抵御超长恶意输入的流式内存限额。
- 发现错误时向标准错误输出行号和原因，退出码 1；只有所有输入合法才向标准输出打印结果。

```text
输入
# demo
 name = Alice Smith
city=中文
expr = a=b
empty =
name=Bob

输出
count=5
name=Alice Smith
city=中文
expr=a=b
empty=
name=Bob
```

空输入或全是注释时输出 `count=0`。`broken` 报缺少等号；`=x`、`1name=x`、`first name=x` 报非法 key。

### 9.2 核心步骤与不变量

```text
getline 读行 → trim → 忽略空行/注释 → find('=')
                                     ↓ 先检查 npos
                             substr 得到左右字段
                                     ↓
                              trim → 检查 key
                                     ↓
                           保存 key + "=" + value
```

循环不变量：处理完前 i 行后，records 恰好保存其中全部合法的非注释记录，顺序与输入一致；未校验完所有输入前不打印成功结果。

去掉空白的辅助函数：

```cpp
std::string trim(const std::string& text) {
    const auto first = text.find_first_not_of(" \t\r");
    if (first == std::string::npos) return "";
    const auto last = text.find_last_not_of(" \t\r");
    return text.substr(first, last - first + 1);
}
```

`const string&` 表示只读引用：函数不先复制整个参数，也不能通过它修改原串。第 4 课会系统讲 const、auto 与引用；这里先掌握这个用法。`first == npos` 分支保证全空白输入不会进行错误的位置减法。

完整可编译代码在 [03-text-parser.cpp](../examples/03-text-parser.cpp)，其中还包含 key 检查、输入上限、流错误处理和延迟输出。

### 9.3 成本与测试

令 L 为读入字符总数、N 为行数、R 为保留记录数。采用常见顺序搜索实现时，每行只进行常数次扫描和复制，正常输入总体时间 O(L+N)，保存结果与当前行的额外空间 O(L+R+1)。加上 N 是因为很多空行也要逐行处理。这里依然不是对 string 搜索接口作额外的标准复杂度承诺。

必须覆盖：空输入、全空白、注释、空值、值内等号、重复键、首尾空格、非法键、缺少等号、UTF-8 值、末行没有换行、超出输入限制。若前几行合法但后面出错，标准输出应仍为空。

## 10 三个完整程序的运行顺序

在仓库根目录编译（构建目录的建立及 PowerShell 示例见仓库 README）：

```text
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/03-string-ops.cpp -o build/03-string-ops.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/03-line-input.cpp -o build/03-line-input.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/03-text-parser.cpp -o build/03-text-parser.exe
```

1. 状态演示：不需要输入，检查查找、截取、替换、独立副本、异常边界、内嵌零字符和 UTF-8 字节数。不要用 `-DNDEBUG` 禁用练习断言。
2. 整行输入：第一行 n（0～10000），接下来读 n 行，输出方括号中的原内容及长度；空行和前导空格保留。文本长度不另设上限，需内存足够。n 行之后的输入不继续处理。练习输入 `3\nAlice Smith\n\n  Bob\n`。
3. 文本解析器：输入第 9 节样例，再自行添加错误。批量输入推荐文件重定向或管道；交互式终端的 EOF 操作依终端而异。

## 11 分层练习与验收

### A. 预测与诊断

1. `string s="ab=cd=ef"; auto p=s.find('=');` 预测 p、`s.substr(0,p)`、`s.substr(p+1)`、`s.substr(2,4)`，并解释原串是否改变。
2. 找出 `if(s.find("ab"))` 与 `if(s.find("zz")>=0)` 的错误；说明“先算 find('=')+1 再判断”为什么危险。
3. 对输入 `2\n\n  Bob\n`，解释 `>> n` 后立刻 getline、ignore 到换行后 getline、使用 `std::ws` 后 getline 的不同结果。
4. 判断 `substr(size())`、`substr(size()+1)`、读取 `s[s.size()]`、`at(size())` 各自的行为；解释 C++17 UTF-8 `"中文"` 为什么不能 `substr(0,1)` 取一个字。

### B. 独立编程

5. 按逗号分割一行字符串，保留所有空字段：`a,,b,` 得到 `["a","","b",""]`，空串得到 `[""]`。不引入正则表达式，使用 find + substr，先判断 npos 再推进下标。
6. 实现“从左往右、非重叠、只处理原串匹配”的 replaceAll：`"aaaa"` 中 `"aa"` 换成 `"b"` 得到 `"bb"`；`"a"` 中 `"a"` 换成 `"aa"` 得到 `"aa"` 并结束。约定空模式不修改输入。测试替换为空串、无匹配、替换内容包含原模式。
7. 闭卷重写第 9 节解析器，至少覆盖 8 组边界输入；说明为什么只分割第一个等号、为什么值可以为空、为什么不把中文值逐字节截断。

通过标准：前三题能说明原因，而不是仅背输出；第 5、6 题能独立运行；解析器正常、空输入和错误输入均有验证；说清字节、码点和用户看到的字符不必一一对应。参考代码通过测试，不代表学习者已经通过验收。

下一课：const、auto、引用、pair、tuple、结构体与结构化绑定。

## 12 可复现检查与输入边界补充

从仓库根目录运行 `python stl/tests/check_lessons_02_03.py`，脚本会用 g++ 编译两课示例，并检查读行保留空白、无末尾换行、UTF-8 值、错误时无部分输出，以及行数/单行/累计长度限制。它也提取第三课答案中的两个完整程序进行编译和断言检查。依赖 Python 3 和支持 C++17 的 g++。

整行输入示例的数量行剩余内容按约定丢弃，后续 n 行之外的内容忽略；这不是通用文件格式校验器。解析器的长度限制在 getline 完成后检查，所以超长一行仍可能先分配较多内存。Windows 文本模式可能把 CRLF 转成 LF，累计上限按程序实际读到的 char 计，不等同于磁盘文件字节数。

## 规则来源与版本说明

- [basic_string 总览](https://en.cppreference.com/w/cpp/string/basic_string.html)
- [find](https://en.cppreference.com/w/cpp/string/basic_string/find.html)、[substr](https://en.cppreference.com/w/cpp/string/basic_string/substr.html)、[operator[]](https://en.cppreference.com/w/cpp/string/basic_string/operator_at.html)
- [getline](https://en.cppreference.com/w/cpp/string/basic_string/getline.html)
- [C++17 工作草案 N4659](https://timsong-cpp.github.io/cppwp/n4659/strings)

在线参考页面会包含 C++20/C++23/C++26 信息，请注意版本标签。本课不使用 C++20 的 starts_with/ends_with、C++23 的 contains 或后续版本的边界强化行为。
