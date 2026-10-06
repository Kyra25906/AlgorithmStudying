# 第五课 迭代器与半开区间

## 本课的已知与所求

已知：你会用 vector/string，知道元素、长度、容量和引用，也能读懂 auto 与 const auto&。

所求：描述一段元素而不绑定具体容器；判断某个位置能否解引用、前进或后退；正确复制一个区间，避免把预留容量当作可写元素。

本课顺序：位置模型 → 半开区间 → 能力分类 → 常量访问 → 移动与距离 → 反向迭代 → 插入迭代 → 区间复制。链表和集合只用来比较迭代器能力，不要求现在学完它们的全部操作。

代码：[位置实验](../examples/05-iterator-basics.cpp)、[反向与输出实验](../examples/05-iterator-adapters.cpp)、[区间复制程序](../examples/05-range-copy.cpp)。[练习解答](../solutions/05-iterators.md)在独立尝试后阅读。全部使用 C++17。

## 1 为什么需要迭代器

第一反应是用 a[i] 遍历数组。但链表没有快速下标访问，输入流也不是一段已经存在的数组。算法如果都要求下标，就无法用同一接口处理这些数据。

迭代器提供一套位置操作：取得当前元素、移动到下一个位置、判断是否到达终点。它可能是指针，也可能是包装了位置状态的类；不能把所有迭代器都当作原始地址。

```cpp
std::vector<int> a{10, 20, 30};
auto it = a.begin();
std::cout << *it; // 10，解引用得到当前元素
++it;
std::cout << *it; // 20
*it = 25;        // 修改 a[1]
```

迭代器和元素是两回事：移动 it 不移动元素，修改 *it 则改变元素。对于记录 it->score 与 (*it).score 表达同一个成员访问。

## 2 begin 和 end 是区间边界

```text
元素            10       20       30
位置         begin     begin+1  begin+2   end
下标            0        1        2        3
可解引用        是       是       是       否
```

end 是尾后位置，不是最后一个元素。可以把它用作终止标记，不能读取 *end()。对于空容器 begin()==end()，不存在可以读取的首元素。

```cpp
for (auto it = a.begin(); it != a.end(); ++it) {
    std::cout << *it << ' ';
}
```

写 != 而不是 <，因为并非每种迭代器都支持大小比较。不要把两个不同容器的位置当成同一区间端点，也不要对不同容器的迭代器相减。

## 3 半开区间为什么包含左端而不包含右端

[first,last) 包含 first，不包含 last。沿合法的 ++ 操作能够从 first 到达 last，才构成这里可供顺序算法遍历的有效区间。

```cpp
std::vector<int> a{10,20,30,40,50};
// 下标 [1,4)，即 20、30、40
std::vector<int> part(a.begin()+1, a.begin()+4);
```

优势：空区间就是 [p,p)；[begin,end) 表示全体；相邻区间 [l,m) 与 [m,r) 不重复也不遗漏边界。

在 vector 的同一有效序列上，区间长度可用 last-first 得到；对链表不能用减法。`[end,begin)` 通常不是合法的正向遍历区间，不能因为两个迭代器都存在就传给 copy。

本课综合任务规定 `0 <= l <= r <= n`。l==r 允许选空区间，r==n 允许取到最后一个元素；访问 a[r] 则不是同一个含义。

## 4 迭代器按能力分类

| 类别 | 主要能力 | 典型例子 |
|---|---|---|
| 输入 | 按输入规则读取并向前，可能仅支持单遍 | istream_iterator |
| 输出 | 通过赋值写出，不能假定能读取 | back_insert_iterator |
| 前向 | 可向前、多遍读取同一区间 | forward_list、unordered_set |
| 双向 | 前向能力再加 -- | list、set、map |
| 随机访问 | 双向能力再加 +n、-n、距离、顺序比较 | vector、deque、array、string |

输入与输出描述读写要求；不是所有前向/双向/随机访问迭代器都可写。例如 set 的键不允许通过迭代器修改。表中不讨论 vector<bool> 等代理引用细节。

随机访问不等于连续内存：deque 支持常数时间跳转，但不保证元素整体连续。C++20 的连续迭代器概念留到后续课程；不要把更新标准的标签直接当成 C++17 的五类之一。

```cpp
std::list<int> values{10,20,30}; // <list>
auto it = values.begin();
++it;          // 合法
// it += 2;    // 编译错误：不是随机访问迭代器
```

std::sort 需要随机访问迭代器，所以不能直接用于 list；list 有自己的成员 sort()，后续学习。

多遍保证意味着可以保存位置再回头重新遍历。流输入迭代器会消耗输入，复制一个迭代器不等于复制整份输入流，不能依赖副本把输入恢复。

## 5 const iterator 和 const_iterator

```cpp
std::vector<int> a{10,20};
const auto locked = a.begin();
*locked = 11;  // 元素可以改；只是 locked 本身不能 ++
auto read = a.cbegin();
++read;        // 位置可以移动
// *read = 99; // 编译错误：不能通过它改元素
```

const iterator 限制位置对象；const_iterator 限制通过该位置修改元素。cbegin/cend 给出只读元素访问，const 容器的 begin/end 也给出相应只读迭代器。

这些限制并不冻结其他合法访问路径：原容器非 const 时，可以通过其他非 const 迭代器改值。const_iterator 同样可能因容器修改失效，不是“更耐用的迭代器”。

## 6 advance next prev distance

头文件 `<iterator>`。

| 操作 | 返回值或效果 | 注意 |
|---|---|---|
| advance(it,n) | 修改 it，无返回值 | 不自动检查边界 |
| next(it,n=1) | 返回移动后的迭代器副本 | 对普通容器迭代器，原 it 不变 |
| prev(it,n=1) | 返回向前退 n 步的位置副本 | 要求双向能力 |
| distance(first,last) | 返回 difference_type 类型的距离 | 随机访问 O(1)，其余顺序前进 O(k) |

```cpp
std::list<int> a{10,20,30,40};
auto first = a.begin();
auto third = std::next(first,2); // 指向 30，first 仍指向 10
std::advance(first,1);           // first 变为指向 20
auto last = std::prev(a.end());  // 非空时取得 40
```

advance/next 的负步数只能用于支持向后移动的迭代器；prev 的负参数反向变成前进。无论语法能否编译，都必须保证移动不超出合法范围。空容器不能 prev(end())，任何容器都不能 ++end()。

对于随机访问位置，distance 可以在同一序列内给出负差值。对 list 这样的非随机访问迭代器，distance(first,last) 只能在 last 可从 first 反复 ++ 到达时使用；它不会自动判断应该后退。

不要对单遍流迭代器先 distance 再假定输入仍完整可用。也不要在 list 循环中反复 distance(begin,it) 求下标：每次重新扫描会把总时间推到 O(n²)。需要序号时另设计数器。

## 7 反向迭代器不是把容器倒过来

```cpp
std::vector<int> a{10,20,30};
for (auto it = a.rbegin(); it != a.rend(); ++it)
    std::cout << *it << ' '; // 30 20 10，a 本身未重排
```

rbegin 对应最后一个元素，rend 是反向终点，不可解引用；反向 ++ 沿原顺序向前走。空容器 rbegin==rend。

关键关系：反向迭代器的 base() 指向它当前元素之后的正向位置。

```text
rbegin 指向 30       rbegin.base() == end()
反向移动一步指向 20  此时 base() 正向指向 30
rend 是反向终点      rend.base() == begin()
```

所以 base() 不是“同一元素的正向位置”。若反向 it 可解引用，其对应元素的正向位置是 prev(it.base())。删除该元素后旧反向迭代器可能失效，必须按容器规则重新取得；本课不直接在反向遍历中删除。

反向区间 [rfirst,rlast) 对应的正向区间是 [rlast.base(),rfirst.base())，左右端顺序互换。这个关系可用三元素示意图逐个核对。

## 8 输出位置和插入迭代器

std::copy 在 `<algorithm>`；back_inserter、front_inserter、inserter 在 `<iterator>`。

```cpp
std::vector<int> source{1,2,3};
std::vector<int> dest(3);
auto finish = std::copy(source.begin(),source.end(),dest.begin());
// dest 已有 3 个可赋值元素，finish == dest.end()
```

普通目标迭代器用于覆盖已有元素。`vector<int> dest; dest.reserve(3);` 只有容量，没有可写元素，不能把 dest.begin() 作为三个赋值位置。

```cpp
std::vector<int> dest;
std::copy(source.begin(),source.end(),std::back_inserter(dest));
```

back_inserter 把写入转换成 push_back，因此能建立新元素。它是输出适配器，不是“指向某个已经存在的元素”，不要读取 *back_inserter 的元素值。

| 适配器 | 写入动作 | 限制及顺序 |
|---|---|---|
| back_inserter(c) | c.push_back(value) | 容器要有 push_back；保持来源顺序 |
| front_inserter(c) | c.push_front(value) | vector 不支持；重复前插会反转来源顺序 |
| inserter(c,pos) | 在 pos 前插入并更新保存的位置 | 容器要支持相应 insert；可用于 list/vector/set 等 |

例：向 deque{9} 使用 front_inserter 写 1、2、3，得到 [3,2,1,9]。向 vector{9} 的 begin 位置用 inserter 写同样数据，得到 [1,2,3,9]；适配器会在每次插入后更新内部位置，保持这一批来源顺序。对 set，最终仍按比较器排列，并可能丢弃重复键，不能承诺复制顺序。

vector 中间反复 insert 可能搬移大量元素。适配器让写法通用，不会自动让底层操作变快。

## 9 copy 的范围与别名边界

对于本课无执行策略的 copy，源区间必须有效；目标必须能接受整个区间的写入。普通返回值指向目标已写区间之后；输出适配器返回的仍是适配器，不能把它当作 dest.end() 使用。

```cpp
std::vector<int> a{1,2,3,4};
std::copy(a.begin()+1,a.end(),a.begin()); // 合法左移覆盖，得到 [2,3,4,4]
```

不能因此认为任意重叠都安全。copy 要求目标起点不在源 [first,last) 内；向右覆盖通常需要 copy_backward，并且目标元素已经存在。后者从后向前复制，第三个参数是目标尾后位置，后续算法课再展开。

尤其不要 `copy(a.begin(),a.end(),back_inserter(a))` 自己追加自己：追加会使旧 end() 失效，重新分配还会影响全部源位置。先复制来源到独立容器，或采用经过单独证明的方案。

对于 k 个 int，向已有元素 copy 进行 k 次赋值；向 vector 尾插按均摊 O(k) 分析。向链表前进 k 步是 O(k)，不是因为代码只有一次 next 调用就变成 O(1)。

## 10 综合任务 安全的区间复制

输入 n、l、r，然后 n 个 int。要求 0<=n<=100000 且 0<=l<=r<=n。复制原序列下标 [l,r) 到独立 vector，打印长度、正向结果和反向结果；原序列不变。

完整代码见 [05-range-copy.cpp](../examples/05-range-copy.cpp)。所有输入校验通过才输出；非法输入向标准错误打印原因并返回 1；约定字段以后的额外输入忽略。

```text
输入
5 1 4
10 20 30 40 50

输出
count=3
forward: 20 30 40
reverse: 40 30 20
```

步骤：检查边界 → 保存源元素 → 建立合法迭代器区间 → copy 到 back_inserter → 用只读正向/反向迭代器输出。复制不变量：目标始终等于已处理源区间的前缀；完成后目标恰好是 [l,r)。

l==r 时输出 count=0、forward:、reverse: 三行。n=0 时唯一合法边界是 l=r=0。读取 O(n)，复制及两次输出 O(k)，总时间 O(n+k)，空间 O(n+k)，k=r-l。

## 11 分层练习与验收

1. a={4,5,6}，从 begin 开始 ++ 两次指向什么？再 ++ 一次可否比较、可否解引用？
2. 对 5 个元素，分别解释 [begin,begin)、[begin+1,begin+4)、[end,end)。哪些区间为空？
3. 区分 const auto it=a.begin() 和 auto it=a.cbegin() 的移动与写入权限。
4. 为什么 list 的 next(begin,100) 不能算 O(1)？为什么不能对 list 使用 end()-begin()？
5. 预测向 deque{9} 用 front_inserter 复制 {1,2,3} 的结果；改为 back_inserter 呢？
6. 修复“reserve 后 copy 到空目标 begin”的代码，分别用预建元素和插入适配器两种方式。
7. 独立扩展综合程序：按反向顺序复制选定区间到新容器，不修改源，测试空区间、单元素、全区间。可以先 copy 再反向读取，但若题目要求目标本身反向，就必须反向建立目标。
8. 推演指向 20 的反向迭代器在 [10,20,30] 中 base() 指向哪里；解释为什么不能直接 erase(it.base()) 删除 20。
9. 判断编译错误与运行期前置条件错误：list 迭代器加整数、修改 cbegin 指向的元素、空容器 prev(end())、解引用 end()。

验收：能画出边界而不把 end 当作元素；根据迭代器能力选择操作；独立完成两种安全复制；解释输出空间与失效；知道编译成功不保证移动/解引用合法。

建议三次学习：第 1～5 节配位置实验；第 6～9 节配适配器实验；综合程序和练习。下一课学习 lambda、谓词与自定义排序。

## 规则来源和复现

C++17 工作草案：[迭代器操作](https://timsong-cpp.github.io/cppwp/n4659/iterator.operations)、[copy](https://timsong-cpp.github.io/cppwp/n4659/alg.copy)。接口的复杂度取决于所支持操作与底层目标，不应由表面代码行数推断。

运行 `python stl/tests/check_lesson_05.py` 编译三个示例、检查区间边界与预期编译失败。不要添加 -DNDEBUG 关闭断言。验证不执行失效迭代器或越界移动，也不替代学习者独立验收。
