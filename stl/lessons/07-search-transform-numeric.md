# 第七章 查找 计数 变换与数值算法

## 已知与所求

已知：你会描述 [first,last) 区间、用插入迭代器追加数据，并能把 lambda 当作谓词交给算法。

所求：对一批整数查找、计数、取极值、筛选和计算统计量；明确算法返回的是位置、数量、输出终点还是数值；正确处理空输入、目标空间与中间运算类型。

本章按 C++17 无执行策略的常用重载讲解。不要求每个循环都改写成算法；重点是读懂接口约定，再选择更清楚的表达。

代码入口：[查找与判断](../examples/07-search-count.cpp)、[变换与数值实验](../examples/07-transform-numeric.cpp)、[数据处理程序](../examples/07-data-pipeline.cpp)。[练习答案](../solutions/07-algorithms.md)最后阅读。

## 1 先辨认任务需要什么结果

假设 a={4,-2,4,7}：

| 问题 | 合适接口 | 返回结果的种类 |
|---|---|---|
| 第一个 4 在哪里 | find | 迭代器，不是下标 |
| 第一个负数在哪里 | find_if | 迭代器 |
| 有几个 4 或负数 | count / count_if | 数量 |
| 是否全部非负、是否有负数 | all_of / any_of | bool |
| 最小值在哪里 | min_element | 迭代器 |
| 每项变成平方 | transform | 写出结果，并返回输出终点 |
| 总和是多少 | accumulate | 累加值 |

查找、计数、极值、复制和变换来自 `<algorithm>`；accumulate、iota、partial_sum、adjacent_difference、inner_product 来自 `<numeric>`；back_inserter 来自 `<iterator>`。包含 `<vector>` 不等于包含了所有算法声明。

所有例子都要求输入区间有效。改变容器大小、返回失效引用、在谓词中破坏被访问数据，都不会因为使用标准算法自动变安全。

## 2 find 找位置 count 数数量

```cpp
std::vector<int> a{4,-2,4,7};
auto it = std::find(a.begin(),a.end(),4);
if (it != a.end()) {
    std::cout << *it;            // 4
    std::cout << it-a.begin();   // 下标 0，仅适用于同序列随机访问位置
}
auto negatives = std::count_if(a.begin(),a.end(),
                              [](int x){return x<0;}); // 1
```

| 接口 | 返回值 | 查不到或空区间 | 成本 |
|---|---|---|---|
| find(first,last,value) | 第一个相等元素的位置 | last | 最多 n 次比较 |
| find_if(first,last,pred) | 第一个使 pred 为真的位置 | last | 最多 n 次谓词调用 |
| find_if_not(first,last,pred) | 第一个使 pred 为假的位置 | last | 最多 n 次谓词调用 |
| count(first,last,value) | 相等元素数量 | 0 | n 次比较 |
| count_if(first,last,pred) | 满足条件的数量 | 0 | n 次谓词调用 |

计数返回迭代器的 difference_type，可用 auto 接收。不要把 find 的返回值直接当 bool，也不要不检查就解引用。查找第一次出现与统计全部出现是两种需求，count 即使已找到一次也要继续扫描。

本章的 std::find 是通用线性查找，不会因为容器有序就自动二分。后面的 set/map 应优先考虑成员 find；二分算法将在第九课讲解。

## 3 all_of any_of none_of 的空区间

```cpp
auto nonnegative = [](int x){return x>=0;};
bool all = std::all_of(a.begin(),a.end(),nonnegative);
bool any = std::any_of(a.begin(),a.end(),nonnegative);
bool none = std::none_of(a.begin(),a.end(),nonnegative);
```

对空区间，all_of 为 true，any_of 为 false，none_of 为 true。直觉是：没有违反“全部满足”的反例，也没有任何满足条件的元素。

若任务要求“至少一个元素且全部合格”，应写 `!a.empty() && all_of(...)`。三个接口最多检查 n 个元素，不应依赖某个实现具体在哪一步调用或短路；谓词应表达稳定的条件，不借调用次序维护业务状态。

## 4 极值接口返回的是元素位置

```cpp
auto lo = std::min_element(a.begin(),a.end());
auto hi = std::max_element(a.begin(),a.end());
if (lo != a.end()) std::cout << *lo << ' ' << *hi;
```

min_element/max_element 在空区间返回 last，非空时分别返回第一个最小/最大元素。两者分别扫描一次。minmax_element 一次调用返回 pair<iterator,iterator>，也可结构化绑定：

```cpp
auto [lo,hi] = std::minmax_element(a.begin(),a.end());
```

注意重复极值：minmax_element 返回第一个最小位置和最后一个最大位置；不是两项都与分别调用 min_element/max_element 的结果相同。空区间返回 {last,last}。

对 [3,1,3,1]：最小首次为下标 1，最大首次为 0，minmax_element 的最大位置却为 2。先检查非空再读取。

可以传比较器，但必须满足第六课的排序关系要求。`std::min(x,y)` 比较两个值，而 min_element 处理区间；不要混淆名字相近的接口。极值查找按 O(n) 比较分析，不需要先 O(n log n) 排序。

## 5 copy 与 copy_if 筛选后仍是独立数据

```cpp
std::vector<int> kept;
std::copy_if(a.begin(),a.end(),std::back_inserter(kept),
             [](int x){return x>=0;}); // [4,4,7]
```

copy 复制全部元素，copy_if 只复制命中项，并保持命中项原顺序，不会改变原容器。它们返回写入完成的输出迭代器；back_inserter 的返回对象仍是输出适配器，不是 kept.end()。

第五课讲过两种目标准备方式：先 resize/构造出足够元素后覆盖，或用插入适配器建立新元素。只有 reserve 仍不能对空容器 begin 写入。

copy_if 的输入输出范围不能重叠。本章统一使用独立目标容器，避免一边扫描一边向同一 vector 追加。删除原数据的 remove/erase 技巧留到下一章。

## 6 transform 是逐项映射 不是筛选

```cpp
std::vector<long long> squared;
std::transform(a.begin(),a.end(),std::back_inserter(squared),
               [](int x){return 1LL*x*x;}); // [16,4,16,49]
```

一元 transform 对每个源元素生成一个结果，输出数量等于输入数量。copy_if 可以减少数量，但并不变换命中元素。这两个接口不能互相替代。

二元版本把两个序列逐项配对：

```cpp
std::vector<int> left{1,2,3}, right{10,20,30};
std::vector<int> result(left.size());
std::transform(left.begin(),left.end(),right.begin(),result.begin(),
               [](int x,int y){return x+y;}); // [11,22,33]
```

这个 C++17 重载没有接收 right.end()，不会替你检查第二序列长度。right 至少要有 left.size() 项，目标也要有足够元素或使用插入适配器。

一元版本允许目标从同一输入的 first 开始，做原地映射；二元版本也允许目标起点等于其中一个输入起点。但不能据此推断任意错位重叠都安全。lambda 不要自行改变输入区间或依赖调用先后；算法通过写入返回值完成修改。

这里先写 1LL*x*x，使乘法从 long long 开始。把结果写入 long long 容器并不能挽救此前已经在 int 中发生的溢出。返回值类型和中间表达式类型都要看。

## 7 fill 和 iota 修改已有元素

```cpp
std::vector<int> a(4);
std::fill(a.begin(),a.end(),7); // [7,7,7,7]，<algorithm>
std::iota(a.begin(),a.end(),3); // [3,4,5,6]，<numeric>
```

fill 给每个元素赋同一个值；iota 从给定起始值连续递增赋值。两者返回 void，不改变 size，空区间什么也不写。容量预留仍不等于建立元素。

iota 的递增状态类型来自传入起始值，不是自动由目标元素类型决定。需要宽整数序列时，目标与起始值都选好类型，如 vector<long long> 配合 0LL；还需保证递增过程可表示，不能从类型极限附近无限增加。

## 8 accumulate 的初始值也决定类型

```cpp
std::vector<int> a{1000000000,1000000000,1000000000};
long long total = std::accumulate(a.begin(),a.end(),0LL); // 3000000000
```

accumulate 从 init 开始，按从左到右的顺序累积；空区间直接返回 init。累计类型由 init 的模板类型确定。写 `long long total=accumulate(...,0)`，左侧类型不会倒推算法内部使用 long long。

0、0LL、0.0 分别代表 int、long long、double 初始值。初始值不一定为零：例如从已有余额开始加；但它会真实参与结果，不能随便选一个值当类型标签。

```cpp
long long sumSquares = std::accumulate(a.begin(),a.end(),0LL,
    [](long long acc,int x){return acc+1LL*x*x;});
```

上式说明“变换后累积”的接口，使用时必须另证平方与总和不超过 long long；本章完整任务限定每项绝对值不超过 10^6、数量不超过 10^5，总平方和至多 10^17，才有明确范围保证。

C++17 的 reduce 可以重新组合运算，不保证 accumulate 的顺序。减法、浮点加法等对组合方式敏感，不能把名字相近的 reduce 当成完全等价替换；本章先使用 accumulate。

## 9 前缀和 差分与点积

### 9.1 partial_sum 返回包含当前位置的累计值

```cpp
std::vector<long long> a{2,5,-1};
std::vector<long long> prefix(a.size());
std::partial_sum(a.begin(),a.end(),prefix.begin()); // [2,7,6]
```

输出与输入等长，不自动在开头补一个 0。若要算法题里长度 n+1、prefix[0]=0 的形式，需要自己建立这一项。

特别注意：C++17 partial_sum 的内部累计类型是输入迭代器的值类型。仅把目标换成 vector<long long>，不能保证 int 输入的累计不溢出。先把输入转换成 long long，或显式写宽类型循环。

### 9.2 adjacent_difference 的第一项照抄

对 [2,7,6]，默认差分为 [2,5,-1]：第一项原样输出，之后是当前项减去前一项。不是只输出 n-1 个变化量。

两个算法都返回输出结束位置，空区间不写数据并返回原输出起点；普通目标需有足够现存元素。无执行策略版本允许同起点原地计算，但不代表任意重叠合法。差分的减法也有类型范围要求。

### 9.3 inner_product 配对乘积再累加

```cpp
std::vector<long long> x{2,3}, y{10,20};
auto dot = std::inner_product(x.begin(),x.end(),y.begin(),0LL); // 80
```

第二序列至少与第一个区间一样长。初始值决定累计类型，但默认每一对乘法由操作数类型决定；两个 int 先相乘可能溢出，即使 init 是 0LL。可像上例先用宽类型输入，或提供自定义乘法使运算提前提升。

本节算法均按线性次数处理元素，前提是把单次算术视为常数成本。复杂度小不代表数值范围必然安全。

## 10 综合任务 筛选 平方与累计统计

输入 n 与 threshold，0<=n<=100000，threshold 在 [-1000000,1000000]；随后 n 个同范围内的整数。

找原序列第一个 >=threshold 的下标；保留所有 >=threshold 的值且维持顺序；计算保留数据的平方、总平方和及平方前缀和。同时输出原序列的最小值和最大值。原序列不修改。

空原序列输出 first=none、count=0、min=none max=none；空筛选结果的 sum=0，两个序列输出行保持标签。输入非法时标准输出为空，标准错误报告原因，退出码 1。所需字段后的额外输入忽略。

```text
输入
5 2
-3 2 5 2 0

输出
first=1
count=3
min=-3 max=5
sum=33
squares: 4 25 4
prefix: 4 29 33
```

完整实现见 [07-data-pipeline.cpp](../examples/07-data-pipeline.cpp)：find_if 查找 → count_if 计数 → minmax_element 极值 → copy_if 筛选 → transform 写出 long long 平方 → accumulate 与 partial_sum 汇总。

这是教学分阶段写法，几次线性扫描仍是 O(n)，但实际常数不等于只扫描一次。保留中间容器便于核对每步；若追求更小内存可以融合循环，不必强行保留所有中间结果。

筛选不变量：kept 等于已扫描前缀中全部合格元素。变换后 squared[i]=kept[i]^2；累计后 prefix[i] 等于 squared 的前 i+1 项之和。数量最多 10^5、单项平方最多 10^12，总和不超过 10^17，处于 long long 可表示范围内。时间 O(n)，额外存储 O(n)。

## 11 练习与验收

1. 对 [3,1,3,1] 预测 find(1)、count(3)、min_element、max_element、minmax_element 的位置或数量。
2. 解释空序列 all_of/any_of/none_of 的值，并写出“非空且全部为正数”。
3. 筛出负数后输出它们的相反数；输入限定 [-10^6,10^6]，说明为什么保留这个范围约束。
4. 用二元 transform 逐项相减；先检查两个 vector 长度相同，测试空序列和长度不同。
5. 修正 accumulate(...,0) 与 int 输入 partial_sum 到 long long 目标这两类溢出风险，解释修正点为什么不同。
6. 构造长度 n+1、首项 0 的宽类型前缀和，并回答 [l,r) 的区间和；检查 0<=l<=r<=n。
7. 对 [2,5,-1] 做前缀和再差分，解释为什么恢复原数据；如果发生整数溢出，还能用数学恒等式保证吗？
8. 不保存平方数组，直接用带二元操作的 accumulate 求满足阈值项的平方和。说明时间、额外空间与原程序的区别。

验收：能区分迭代器与数值返回；空区间不解引用；准备正确输出空间；在算术发生前选对类型；独立完成第 4、6、8 题。答案见 [解答](../solutions/07-algorithms.md)。

建议三次学习：查找与判断实验；变换和数值实验；综合程序及独立练习。下一章学习排序、去重、条件删除和重排。

## 规则来源与运行

C++17 工作草案：[数值算法](https://timsong-cpp.github.io/cppwp/n4659/numeric.ops)、[transform](https://timsong-cpp.github.io/cppwp/n4659/alg.transform)。本章使用无执行策略重载，不依赖 C++20 ranges。

`python stl/tests/check_lesson_07.py` 编译三个示例与答案片段，验证空输入、重复极值、宽类型累计和独立随机对照。不要运行越界或溢出来“观察答案”；保持断言启用，不加 -DNDEBUG。
