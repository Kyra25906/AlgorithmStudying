# 第四课 用引用和记录类型准确表达数据

## 已知 所求和本课顺序

已知：你会用 vector 保存序列，会用 string 处理文本，也见过 const auto& 这样的写法。

所求：把姓名、成绩放在同一条记录中；明确什么时候复制、什么时候修改原对象；函数只读时不复制整个容器；一次返回多个统计值。

本课先回答“现在操作的是原对象还是副本”，再介绍记录的表达方式。阅读顺序：引用 → const → auto → 函数参数 → pair/tuple → struct → 结构化绑定。主线 C++17；不要求先学模板推导理论或移动语义。

完整代码：[类型与别名实验](../examples/04-reference-auto.cpp)、[记录拆解实验](../examples/04-record-types.cpp)、[成绩调整程序](../examples/04-student-records.cpp)。练习在第 10 节，[解答](../solutions/04-records.md)最后阅读。

## 1 从复制和别名开始

```cpp
int score = 80;
int copy = score;
int& alias = score;
copy = 90;
alias = 85;
```

| 步骤 | score | copy | alias 指向谁 |
|---|---:|---:|---|
| 建立变量后 | 80 | 80 | score |
| copy=90 | 80 | 90 | score |
| alias=85 | 85 | 90 | score |

copy 是另一个对象，初始化时拿到了 score 的值。alias 是 score 的别名，通过它赋值会改变 score。

引用声明中的 & 是类型的一部分；表达式 &score 是取地址，两者语法位置不同。引用必须初始化，不能随后改绑到另一对象：

```cpp
int other = 60;
alias = other; // 把 60 赋给 score，不是让 alias 改指向 other
```

C 中常用指针参数修改调用者变量；C++ 也支持引用参数。引用不是自动管理生命周期的智能指针，对象销毁或容器重新分配后，引用仍可能悬空。

## 2 const 限制哪条访问路径

```cpp
int score = 80;
const int& view = score;
// view = 90; // 编译错误：不能通过只读引用赋值
score = 90;  // 合法，此后读取 view 得到 90
const int fixed = 100;
// fixed = 90; // 编译错误：对象本身是 const
```

只读引用没有复制 score，也没有把原来的非 const 对象永久冻结。它限制通过 view 的修改。对本课没有 mutable 字段的 Student，const Student& 不能修改其普通成员。

指针中也有两种不同的 const：

```cpp
int a = 1, b = 2;
const int* p = &a; // 不能通过 *p 修改 a，但 p 可以重新指向 b
p = &b;
int* const q = &a; // q 不能重新指向 b，但可以写 *q = 3
*q = 3;
```

const 不保证指针指向的整个对象图都不可改变。后面遇到指针成员、引用成员或 mutable，再单独分析；不要把“const 等于深度冻结”当作规则。

## 3 auto 是编译时推导，不是动态类型

```cpp
const int original = 7;
auto value = original;       // int，独立副本，顶层 const 被去掉
auto& ref = original;        // const int&，保留所引用对象的 const
const auto& read = original; // const int&
```

| 写法 | 对本课普通值类型意味着什么 | 常用场景 |
|---|---|---|
| auto x = object | 建立副本，不自动保留引用和顶层 const | 需要独立修改 |
| auto& x = object | 绑定原对象，原对象若 const 则仍只读 | 修改原对象或保留其限定 |
| const auto& x = object | 只读绑定，避免复制对象 | 读取较大记录 |

auto 推导完成后类型不会变。`auto x = 1; x = 2.9;` 并不会让 x 变成 double；这是向 int 赋值并发生转换。不要把 auto 当作运行时改变类型的变量。

“去掉 const”指顶层 const，不是删除类型中所有 const：

```cpp
const int n = 3;
const int* ptr = &n;
auto pointerCopy = ptr; // 仍是 const int*，不能通过它修改 n
```

常见易错点：

```cpp
auto text = "hello";          // const char*，不是 std::string
auto word = std::string("hello"); // std::string，需要 <string>
auto one{1};                  // int，C++17
auto list = {1, 2};           // std::initializer_list<int>，需要 <initializer_list>
```

普通 auto 变量需要初始化。裸 C 数组经按值 auto 通常退化成指针；auto& 可以保留数组类型。本课的类型实验使用 `<type_traits>` 中的 static_assert 检查推导结果，不依赖编译器显示的类型名称。decltype 此处仅作为测试工具，进阶推导留待以后。

## 4 函数参数与返回值

```cpp
void changeCopy(std::vector<int> a) { a[0] = 0; }
void changeOriginal(std::vector<int>& a) { a[0] = 0; }
long long sum(const std::vector<int>& a) {
    long long result = 0;
    for (int x : a) result += x;
    return result;
}
```

前两个函数要求非空。对调用 `changeCopy(v)`，传入左值 v 会复制整个容器；修改只发生在副本中。第二种不复制 v，可以改变原数据。第三种不复制 v，并承诺不通过该参数修改它。

小整数按值传递通常很自然；只读大对象常用 const T&；明确要修改调用者对象用 T&。并非“引用总比值传递好”：需要自己的副本时按值就是合理设计。

返回记录或容器可以按值返回，让编译器应用相应复制消除或移动规则，不要为避免复制而返回局部对象引用：

```cpp
// 错误示意，不执行：函数结束后 local 被销毁
// const std::string& bad() {
//     std::string local = "hello";
//     return local;
// }
```

`const std::string& localView = std::string("hello");` 这种直接局部绑定会延长临时对象生命周期，但该规则不能推广为“函数返回的所有引用都安全”。

## 5 pair 把两个值放在一起

头文件 `<utility>`。例如“合格人数与总分”：

```cpp
std::pair<int, long long> stats{3, 240};
int count = stats.first;
long long total = stats.second;
auto point = std::make_pair(2, 5); // pair<int,int>
```

first 与 second 是成员，读写成本取决于成员类型；访问本身 O(1)。复制 pair 会复制两个成员，若有长字符串，不能把复制成本一概算成 O(1)。

C++17 的普通值 pair 支持按字典序比较：先比 first，只有第一项等价时才比 second。例如 pair<int,int>{1,9} 小于 {2,0}，不是比较两项之和。两个成员必须支持相应比较；这里并未要求你提前学习排序。

不要把所有业务记录都写成 first/second。如果读代码时经常需要猜“second 是年龄还是成绩”，应该给字段起名字。

## 6 tuple 保存多个位置字段

头文件 `<tuple>`。适合少量临时组合或多返回值：

```cpp
std::tuple<std::string, int, bool> item{"Alice", 80, true};
std::get<1>(item) = 85; // get 的下标是编译期常量，不是运行时变量
```

tuple 是异构记录，不是可以用运行时整数下标遍历的 vector。`std::get<0/1/2>` 分别具有不同类型。C++17 的 tuple 也可以按成员顺序进行字典序比较，要求相关成员支持比较。

`std::tie` 把已有变量组成引用元组，可用于接收结果：

```cpp
int count = 0;
long long total = 0;
std::tie(count, total) = std::make_tuple(3, 240LL);
std::tie(std::ignore, total) = std::make_tuple(9, 500LL);
```

第二句忽略第一项，只更新 total。make_tuple 对这里的普通值生成值元组；tie 生成引用元组，不拥有被引用的对象。成员含引用或 reference_wrapper 时要另作分析。

## 7 struct 给长期记录清楚的名字

```cpp
struct Student {
    std::string name;
    int score = 0;
};
Student s{"Alice", 80};
s.score += 5;
std::vector<Student> students{{"Alice", 80}, {"Bob", 59}};
```

C++ 中声明后可直接用 Student，无需额外 typedef。这里字段默认 public，末尾分号不能省略。Student{} 使 name 为空字符串、score 使用默认值 0。

| 需求 | 建议表达 |
|---|---|
| 两个含义简单的临时值 | pair |
| 少量异构多返回值 | tuple 或有名结构体 |
| 需要反复阅读、维护的记录 | 有清楚成员名的 struct |
| 数量运行时变化的一串同类记录 | vector<Student> |

本课 Student 是简单聚合结构，没有继承和自定义构造。不要把复杂类也默认当作能任意拆解的聚合。C++17 不会自动为自定义 Student 生成可用的 == 或 <；pair/tuple 的内置比较不能直接推广给 struct。

## 8 结构化绑定 把字段取成名字

C++17 的结构化绑定是语言语法，不是函数。对于本课只有值成员的记录：

```cpp
Student s{"Alice", 80};
auto [nameCopy, scoreCopy] = s; // 隐藏对象是 s 的副本
scoreCopy = 0;                 // s.score 仍为 80
auto& [nameRef, scoreRef] = s;  // 绑定原对象
scoreRef = 90;                 // s.score 变为 90
const auto& [nameRead, scoreRead] = s; // 只读观察原对象
```

可以先想成“先决定背后的记录是副本还是引用，再为字段起名字”。不是拆完就把原记录销毁，也不是根据绑定名字去匹配字段；本课 struct 按成员声明顺序绑定。

| 遍历写法 | 是否复制本课的 Student | 可否通过绑定修改原记录 |
|---|---|---|
| for (auto [name, score] : students) | 是 | 否 |
| for (auto& [name, score] : students) | 否 | 是 |
| for (const auto& [name, score] : students) | 否 | 否 |

名字数量必须与可拆解成员数相符。可以拆 pair、tuple、固定大小数组以及满足条件的类；不能写 `auto [a,b] = vector<int>{1,2};` 来拆动态序列，也不能在 C++17 写 `auto [a,b];` 后再赋值。

进阶边界：`auto [a,b]` 的“副本互不影响”只适用于本课普通值成员。比如 `auto tied = std::tie(x,y); auto [a,b] = tied;` 复制的是引用元组，a/b 仍关联 x/y。记录拆解实验专门核对这个例外。const 也不会把引用元组引用的原对象自动变成 const。

在 vector 元素上建立的绑定仍受前两课失效规则约束。别在持有引用绑定时对同一 vector 做可能扩容的 push_back；引用语法不延长容器元素生命。

## 9 综合任务 成绩统一调整与统计

输入约定：先输入 n 和 bonus，n 在 0～100000，bonus 在 -100～100；随后 n 条 `姓名 成绩`。姓名是 1～100 字节且不含空白的单词（UTF-8 名字按字节计数），成绩是 0～100 的整数。此例用 >>，不支持姓名中的空格。

把每条成绩加 bonus，再截在 [0,100]。按原顺序输出调整后的姓名和成绩，并输出合格人数与总分。不排序、不去重，同名记录保留。全部输入成功才输出；输入非法返回 1。约定所需字段后的额外输入不处理，姓名长度在读入后检查，不是流式内存限额。

完整程序：[04-student-records.cpp](../examples/04-student-records.cpp)。职责分配：

- Student 给字段命名，vector 保存记录顺序。
- adjust(vector<Student>&, int) 修改原数据，不复制全体记录。
- summarize(const vector<Student>&) 只读扫描，返回 pair<int,long long>。
- main 用结构化绑定接收统计结果，输出时使用 const auto&。

```text
输入
3 5
Alice 98
Bob 55
Carol 20

输出
count=3
passed=2
total=185
Alice 100
Bob 60
Carol 25
```

调整循环不变量：前 i 条已按规则修正，后面的还未修正，顺序与姓名不变。统计循环不变量：计数与和只包含已处理前缀。n=0 时三个统计量均为 0，不访问首尾。

若姓名总字节数为 B，读入和输出按 O(n+B) 分析，两个成绩扫描为 O(n)；存储 O(n+B)。绑定与传引用本身不复制名字，但返回或复制一个含 string 的记录仍可能复制其内容。输入/输出和存储分配也不等于零成本。

## 10 练习与验收

1. 推演 `int x=2,y=9; int& r=x; auto z=r; r=y; z=5;` 后 x、y、z、r 的值。
2. 对 const int x=3，分别解释 auto a=x、auto& b=x、const auto& c=x 的类型与能否赋值。
3. 把对 vector<Student> 的按值范围循环改成真正给原成绩加 1，说明为什么只去掉 const 不够。
4. 写函数 `pair<int,long long> summarize(const vector<Student>&)`，统计合格数与总分，检查空容器和 59/60 边界。
5. 给记录增加出勤字段，用 tuple 暂存三项，再改成有字段名的结构体，解释可读性差别。
6. 为什么读取 const auto& 绑定不会复制名字？为什么 `auto [a,b]=std::tie(x,y)` 仍可能修改 x/y？
7. 修改综合程序：输出不合格学生的名字，保持原顺序；所有人合格时输出 `failed: none`。
8. 判断三种错误属于编译错误还是生命周期错误：修改 const 引用、用两个名字拆三项 tuple、返回局部 string 的引用后读取。

通过标准：能用对象状态解释副本/引用；知道只读访问不是深度冻结；能选择记录类型；不靠试运行悬空引用判断合法性；独立完成第 4、7 题的边界测试。

建议分三次学习：第 1～4 节和类型实验；第 5～8 节和记录实验；综合程序及练习。下一课把这些知识用于迭代器与半开区间。

## 规则来源与运行

C++17 工作草案：[auto](https://timsong-cpp.github.io/cppwp/n4659/dcl.spec.auto)、[结构化绑定](https://timsong-cpp.github.io/cppwp/n4659/dcl.struct.bind)。这里只学习常见值类型，泛型代码、代理引用和完整转发规则留待进阶。

编译命令见仓库 README。运行 `python stl/tests/check_lesson_04.py` 可编译三个示例、核对类型断言和输入输出，并验证几段应当无法编译的代码。不要添加 -DNDEBUG 禁用运行期断言。脚本通过只代表材料自检通过，不替代独立练习。
