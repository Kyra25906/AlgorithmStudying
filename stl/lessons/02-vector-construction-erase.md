# 第二课 vector 的构造赋值、插入删除与二维用法

## 本课的已知和所求

已知：第一课的 size/capacity、reserve/resize、下标与尾部操作、遍历与引用。

所求：复制与赋值整个容器；在中间插入 insert 和删除 erase；用两种方式表达二维矩阵；完整地说清每种操作会让哪些引用、指针和迭代器失效；写出不会跳过、不会越界的安全遍历删除，并独立完成一个矩阵程序。

完成本课应能：解释拷贝与移动、assign 与 swap；正确使用 insert/erase 的返回值；说明二维数组 `vector<vector<int>>` 与拍平数组的区别；逐条说出失效规则；独立写出安全删除和矩阵处理程序。代码见 [状态演示](../examples/02-vector-ops.cpp)、[安全删除](../examples/02-safe-erase.cpp) 与 [矩阵程序](../examples/02-matrix.cpp)。

## 1 从 C 数组的局限出发

C 数组不能整体复制：`int b[3] = a;` 不成立，把一个数组赋给另一个要靠逐个拷贝；运行时定长还要自己管理内存。vector 的对象可以整体复制、赋值、交换，这大大简化了函数传参和返回值。

```cpp
std::vector<int> a{1, 2, 3};
std::vector<int> b(a);          // 拷贝构造：内容相同，彼此独立
std::vector<int> c = a;         // 等价于拷贝构造
b = a;                          // 拷贝赋值：先丢弃 b 原有内容
```

拷贝之后 b 和 a 是两份独立数据，改 b 不影响 a。想要“整体搬走、不必再拷贝一份”时用移动：

```cpp
std::vector<int> d = std::move(a); // 移动构造
d = std::move(b);                  // 移动赋值
```

被移动走的对象进入“有效但未指定”状态：仍能安全析构，也能重新赋值，但不该再读取它的内容。常见实现会把源对象留成空，但标准不承诺这一点——不能把“本机看起来是空”当成保证。需要 `<utility>` 中的 std::move。

## 2 assign 与 swap

assign 用于“用别的数据整体替换”，与赋值类似，但能指定数量或区间：

```cpp
std::vector<int> a;
a.assign(3, 7);                  // [7, 7, 7]
a.assign({1, 2, 3, 4});          // [1, 2, 3, 4]
std::vector<int> src{9, 8, 7};
a.assign(src.begin(), src.begin() + 2); // [9, 8]
```

swap 交换两个容器的内容，时间复杂度 O(1)，不搬移元素：

```cpp
std::vector<int> x{1}, y{2, 3};
x.swap(y);   // x=[2,3], y=[1]
```

swap 不使任何引用或迭代器失效：它们仍然有效，只是指向的元素现在位于另一个容器里。可用它就地“缩小容量”（见第 8 节的复制交换技巧）。

## 3 insert 在中间插入

```cpp
std::vector<int> a{1, 2, 3};
auto it = a.insert(a.begin() + 1, 10);   // [1,10,2,3]，it 指向新插入的 10
a.insert(a.end(), 2, 0);                 // 末尾追加两个 0
a.insert(a.begin(), {7, 8});             // 头部插入 7,8
```

| 调用 | 行为 | 返回值（C++11 起） |
|---|---|---|
| insert(pos, v) | 在 pos 前插一个 v | 指向新插入元素的迭代器 |
| insert(pos, n, v) | 插 n 个 v | 指向第一个新元素，n=0 时返回 pos |
| insert(pos, first, last) | 插入区间副本 | 指向第一个新元素，区间为空时返回 pos |
| insert(pos, {…}) | 插入初始化列表 | 同上 |

pos 必须来自同一个 vector 且合法，插入后旧的 pos 迭代器不再可靠，应改用返回值。

## 4 erase 删除中间元素

```cpp
std::vector<int> a{1, 2, 3, 4, 5};
auto it = a.erase(a.begin() + 1);   // 删 2，返回指向 3 的迭代器，a=[1,3,4,5]
a.erase(a.begin(), a.begin() + 2);  // 删前两个，返回指向区间后元素的迭代器
```

erase 总是返回“被删元素之后那个元素”的迭代器：erase(pos) 返回原 pos+1 位置的元素（删除末位时返回 end()）；erase(first, last) 返回原 last 位置的元素。这个返回值是安全遍历删除的基石（见第 7 节）。

删除或插入会使下标整体后移或前移，因此“记住下标”不再通用：同一位置在操作前后可能对应不同元素。

## 5 二维数组的两种表达

```cpp
// 方式一：vector 的 vector，逐行独立分配
int rows = 3, cols = 4;
std::vector<std::vector<int>> m(rows, std::vector<int>(cols));
m[1][2] = 7;   // 第 1 行第 2 列

// 方式二：拍平成一行
std::vector<int> flat(rows * cols);
flat[1 * cols + 2] = 7;   // 同一位置
```

两种方式按下标访问都是 O(1)。区别在于内存布局：`vector<vector<int>>` 每一行是独立的动态数组，行与行之间不保证连续，`m[i][j]` 需要两次间接访问；拍平数组整体连续，缓存更友好，`flat[i*cols+j]` 只需一次下标计算。

行数固定、需要“整行整体处理”时，`vector<vector<int>>` 更直观；对大规模、强调遍历速度的场景，拍平更优。选择时说明理由即可，不必执着于某一种。

## 6 完整失效规则

失效指的是：操作之后，之前取得的元素引用、指针或迭代器不再保证指向原元素，继续使用属于未定义行为。规则分“是否触发重新分配”两档。

| 操作 | 失效范围 |
|---|---|
| push_back / insert / emplace | 重新分配则全部失效；否则仅插入位置及其后的引用/迭代器失效 |
| erase(pos) | pos 及之后位置（含 end()）失效 |
| erase(first, last) | first 及之后位置失效 |
| pop_back | 仅被删元素和 end() 失效，其余有效 |
| resize | 增长时若重新分配则全部失效；缩小使被删元素失效 |
| reserve | 参数超过当前 capacity 才重新分配；未重新分配则无一失效 |
| clear | 所有元素被销毁，指向它们的引用/迭代器失效，capacity 不变 |
| shrink_to_fit | 可能重新分配，若发生则全部失效；可能什么都不做 |
| swap | 引用/迭代器不失效，但归属到另一容器 |
| 赋值/assign | 视实现可能复用或释放存储，原有引用/迭代器不应再使用 |

核心记忆：**任何改变 size 且发生在中间的操作，会让“插入/删除点之后”的所有引用失效；任何可能重新分配的操作，可能让全部引用失效。** 判断是否重新分配看 capacity，而不是 size。

## 7 安全遍历删除

删除 while 遍历最容易写出两种 bug：跳过元素，或下标越界。下面这种写法是错的：

```cpp
for (std::size_t i = 0; i < a.size(); ++i) {
    if (a[i] == x) a.erase(a.begin() + i); // 错：删后元素左移，i++ 会跳过一个
}
```

erase 把后面的元素左移一位，随即 i++，就会漏看原本紧挨着的下一个元素；若连续命中，还会越过 size 造成越界。正确写法有三种。

**写法一：利用 erase 的返回值。**

```cpp
for (auto it = a.begin(); it != a.end(); ) {
    if (*it == x) it = a.erase(it); // 删并移动到下一个，it 不自增
    else ++it;
}
```

**写法二：从后往前删。**

```cpp
for (int i = (int)a.size() - 1; i >= 0; --i) {
    if (a[i] == x) a.erase(a.begin() + i);
}
```

用 int 而不是 size_t，避免空容器时 size()-1 回绕成巨大数。从后往前删，前面元素下标不受影响。

**写法三：筛选到新容器。**

```cpp
std::vector<int> kept;
kept.reserve(a.size());
for (int v : a) if (v != x) kept.push_back(v);
a.swap(kept);
```

前两种写法每次 erase 都要搬移后续元素，最坏 O(n²)；写法三只扫一遍，O(n) 时间、O(n) 额外空间。删除很多元素时优先写法三。一次性按条件删除的通用工具（remove + erase 配合）留到第八课。

## 8 为什么是这些复杂度

| 操作 | 复杂度 |
|---|---|
| 拷贝构造 / 赋值 / assign | O(n)，逐个复制或搬移 |
| 移动构造 / 移动赋值 / swap | O(1)（交换指针，通常不搬元素） |
| insert / erase 中间位置 | O(n)，搬移插入点之后的元素 |
| push_back / pop_back | 均摊 O(1) / O(1) |
| 二维按下标访问 | O(1) |

交换后缩小容量的技巧：`std::vector<int>(a).swap(a);` 用拷贝构造造一个恰好大小的临时容器，再与原容器交换，从而把 capacity 缩到与 size 相等。这是一种请求，是否真的释放内存仍由实现决定，与 shrink_to_fit 类似。

## 9 完整程序

**安全删除程序**（[02-safe-erase.cpp](../examples/02-safe-erase.cpp)）：读入 n 与 n 个整数，再读入目标值 x，删除所有等于 x 的元素并保持其余元素原顺序。循环不变量：处理过的前缀里不含 x，且其余元素顺序不变。程序同时演示写法一（主逻辑）与写法三（对照），并核对两者结果一致。

```text
输入
6
3 1 2 3 4 3
3

输出
count=3
kept: 1 2 4
```

n=0 时输出 count=0 与空序列；全部等于 x 时输出空序列。时间 O(n)（写法三），或 O(n²) 最坏（逐次 erase）。

**矩阵程序**（[02-matrix.cpp](../examples/02-matrix.cpp)）：读入行数 r、列数 c 与 r×c 个整数（行优先），输出每行之和，再输出转置矩阵。行和用 long long 累加防止溢出；转置即把第 i 行第 j 列写到第 j 行第 i 列。矩阵本身用 `vector<vector<int>>` 表达。

```text
输入
2 3
1 2 3
4 5 6

输出
row sums: 6 15
transpose:
1 4
2 5
3 6
```

r 或 c 为 0、或行列超上限、或输入不足时，报告错误并返回非零退出码。行和 O(r·c)，转置同样 O(r·c)。

## 10 练习和验收

1. 预测 `vector<int> a{1,2}; vector<int> b=a; b[0]=9;` 之后 a 的内容，解释为什么 a 不受影响。
2. 预测 `a.erase(a.begin())` 对 [1,2,3] 的返回值和结果；`insert(a.end(), 2, 5)` 会插入到哪里。
3. 判断正误并说明理由：`for (size_t i=0;i<a.size();++i) if(a[i]==x) a.erase(a.begin()+i);` 会漏删哪些元素。
4. 独立读入 n 个整数，删除所有负数，保持其余顺序。测试空序列、全负、无负、负号在首尾。
5. 独立读入 r 行 c 列矩阵，删除第一列后输出新矩阵。测试单列、单行、1×1。
6. 说明 `push_back` 在未重新分配时哪些引用失效，哪些保留；`erase(begin(), begin()+1)` 使哪些迭代器失效。
7. 分别用 `vector<vector<int>>` 和拍平数组实现同一矩阵，说明你更倾向哪一种、为什么。

通过标准：闭卷写出拷贝/移动/assign/swap 的语义；正确使用 insert/erase 返回值；完成第 4、5 题并验证边界；能逐条说出失效规则。解答见 [参考答案](../solutions/02-vector.md)。

下一课进入 string 的输入、查找、截取、替换与编码边界。

## 规则来源

参见 [C++ 工作草案 vector.capacity](https://eel.is/c++draft/vector.capacity) 与 [vector.modifiers](https://eel.is/c++draft/vector.modifiers)。链接随草案更新，包含晚于 C++17 的接口；本课只使用 C++17 已有功能。失效规则以标准保证为准，本机观测不替代保证。
