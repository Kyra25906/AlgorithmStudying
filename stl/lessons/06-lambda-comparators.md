# 第六课 lambda 谓词与自定义排序

## 已知与所求

已知：你能用 vector 保存记录，理解 const 引用、结构体、迭代器与 [first,last) 区间。

所求：给算法提供“怎么判断”的规则。先筛出成绩达到阈值的学生，再按成绩降序、姓名升序、输入序号升序排列。解释捕获了什么、是否修改原变量、比较规则为什么正确。

本课按 C++17 编写。主线是“函数怎么交给算法”，不是一开始背 lambda 的全部语法。代码：[捕获实验](../examples/06-lambda-captures.cpp)、[比较器检查](../examples/06-comparator-check.cpp)、[排行榜](../examples/06-ranking.cpp)。[练习解答](../solutions/06-lambda-sort.md)最后阅读。

## 1 从具体判断走到可调用对象

判断一个成绩是否合格，可以先写普通函数：

```cpp
bool passed(int score) { return score >= 60; }
```

循环可以调用 passed(x)，标准库算法也可以接收这个函数。谓词就是用于判断条件、结果能按真假解释的可调用对象。常见一元谓词接受一个元素，二元谓词接受两个。

```cpp
std::vector<int> scores{59,60,90};
auto count = std::count_if(scores.begin(),scores.end(),passed); // 2
```

count_if 来自 `<algorithm>`，接收有效区间和一元谓词，返回匹配数量，结果类型是迭代器的 difference_type；不修改元素，对 n 个元素调用谓词 n 次。这里第三个参数是 passed，不是 passed()：传入规则，交给算法随后调用。

若想临时改成 80 分以上，是否必须为每个阈值写一个全局函数？lambda 可以在使用处定义规则，并带上所需环境。

## 2 逐块读懂 lambda

```cpp
int limit = 80;
auto enough = [limit](int score) -> bool {
    return score >= limit;
};
bool ok = enough(85); // true
```

| 语法部分 | 意义 |
|---|---|
| [limit] | 捕获列表，保存所需外部局部变量 |
| (int score) | 调用时传入的参数 |
| -> bool | 显式返回类型，本例可省略 |
| { ... } | 调用时执行的函数体 |
| enough | 保存可调用对象的变量 |

创建 lambda 不等于执行函数体。`enough(85)` 才是调用。闭包可以先理解成“保存了捕获数据、又能调用的对象”；每个 lambda 表达式产生自己的闭包类型，auto 让我们不用写出类型名。

lambda 是语言语法，没有专用头文件。调用 sort/count_if/copy_if 要包含 `<algorithm>`；尾插适配器还要 `<iterator>`。

```cpp
auto count = std::count_if(scores.begin(),scores.end(),
                          [limit](int score) { return score >= limit; });
```

## 3 值捕获与引用捕获

```cpp
int limit = 60;
auto snapshot = [limit](int x) { return x >= limit; };
auto live = [&limit](int x) { return x >= limit; };
limit = 80;
```

| 调用 | 实际阈值 | 结果 |
|---|---:|---|
| snapshot(70) | 创建时保存的 60 | true |
| live(70) | 外部当前的 80 | false |

`[limit]` 保存这个普通 int 的副本；`[&limit]` 引用原变量。捕获动作发生在创建闭包时，不是在每次调用时重新复制。

常见形式：`[]` 不捕获局部环境；`[x,y]` 明确值捕获；`[&x]` 明确引用捕获；`[=]` 默认值捕获需要捕获的变量；`[&]` 默认引用捕获。可组合 `[limit,&count]`。初学优先显式列出依赖，便于检查生命周期和副作用。

引用捕获不延长被引用对象生命。把引用了局部变量的闭包返回出去，再访问已销毁对象是错误的。若要返回带阈值的规则，使用值捕获：

```cpp
auto makePredicate(int limit) {
    return [limit](int x) { return x >= limit; };
}
```

值捕获也不自动取得所有数据的所有权：捕获原始指针会复制指针值，并不复制其指向的对象；捕获 string_view 也不复制字符。本课阈值使用 int，避免这些额外问题。

## 4 mutable 改的是谁

```cpp
int count = 0;
auto localCounter = [count]() mutable { return ++count; };
// localCounter() 依次得到 1、2；外面的 count 仍是 0。
auto outerCounter = [&count]() { return ++count; };
// outerCounter() 会修改外面的 count。
```

普通值捕获在默认 lambda 调用中不可修改。mutable 允许改变闭包中保存的副本，不会把值捕获变成引用捕获，也不会去除一个本来就是 const 的捕获对象的 const。

算法可能复制函数对象，不能依赖算法内部某一个闭包副本的计数状态作为最终结果。统计匹配数量用 count_if 的返回值。尤其不要让排序比较器的返回规则随调用次数变化。

C++14 起可写泛型 lambda，参数类型由调用推导，C++17 可用：

```cpp
auto less = [](const auto& a, const auto& b) { return a < b; };
```

它不是“任何类型都支持”：只有 a<b 对实际参数合法时才能调用。若返回分支类型不一致，可统一表达式类型或显式写 `-> 返回类型`。本课学生比较器显式写 Student，帮助阅读。

## 5 普通函数 函数对象与 lambda

同一个阈值规则也可以写成有名字的函数对象：

```cpp
struct AtLeast {
    int limit;
    bool operator()(int score) const { return score >= limit; }
};
AtLeast check{60};
bool ok = check(70);
```

operator() 让对象能像函数一样调用；这里 const 承诺不通过调用修改 limit。普通函数适合独立逻辑，有名函数对象适合复用规则，lambda 适合在使用点写短逻辑。

std::function 来自 `<functional>`，可以用统一签名保存不同可调用类型，但并非把 lambda 传给 STL 的必需步骤，也不能无条件认为其包装和复制免费。本课直接用 auto 或普通函数。

## 6 比较器回答的是谁排前面

```cpp
std::sort(a.begin(),a.end(),
          [](int left,int right) { return left > right; });
```

比较器 `comp(left,right)` 为 true，意思是 left 应严格排在 right 前面。它不是“这两项是否需要交换”，也不是返回 -1/0/1 的三路比较函数。排序算法自己决定调用次序、移动与交换。

升序用 `<`，降序用 `>`；相等时两方向都返回 false。`return left-right;` 错在非零值无论正负都会转成 true，而且相减可能溢出。直接使用比较运算。

sort 就地改变 [first,last) 内元素顺序，返回 void，需要随机访问且元素可按要求交换/移动。不能用 cbegin/cend 排序，也不能对 list 直接调用 std::sort。空区间合法。

对 vector 排序不会改变 size 或 capacity，但某个位置上的值可能换成别的记录：元素位置仍可访问，不代表“之前第 0 位那位学生还在那里”。若需要保留原顺序，先复制容器再排序。

## 7 严格弱序 用反例理解规则

算法要求比较器定义严格弱序。至少检查：

1. 自己不排在自己前面：comp(a,a) 为 false。
2. 不能互相抢前面：comp(a,b) 为 true 时 comp(b,a) 必须 false。
3. 先后关系传递：a 在 b 前、b 在 c 前，必须 a 在 c 前。
4. 等价关系传递：定义 equiv(a,b)=!comp(a,b)&&!comp(b,a)，a 与 b 等价、b 与 c 等价，则 a 与 c 也等价。

最后的等价不必是所有字段 ==。仅按分数排序时，同分不同名可以是比较器眼中的等价记录。

**反例一：用 <=。** 自己与自己比较也为 true，第一条就失败。

**反例二：两个关键字直接用 ||。**

```cpp
// 错误示意，不传入 sort
return a.score > b.score || a.name < b.name;
```

A=(90,"Z")、B=(80,"A") 时，A 通过成绩排前，B 又通过姓名排前，两个方向都 true。

**反例三：每一项都小才算小。** 对坐标 `(x,y)` 使用 `a.x<b.x && a.y<b.y` 并不是通用排序规则：A=(1,1)、B=(0,3)、C=(2,2)，A 与 B 等价，B 与 C 等价，但 A 严格在 C 前，等价传递失败。

比较器应不修改被比较元素，排序过程中规则保持一致。不要修改捕获的排序方向或阈值来改变比较结果。浮点 NaN、按 epsilon 模糊比较也需专门处理，本课只使用整数与字符串。

比较器实验在有限数据集上检查四类关系，用反例拒绝坏规则，但不把坏规则交给 sort。有限测试不能证明所有输入合法，最终仍需解释为什么规则成立。实验中的 template<class Compare> 只是让同一个检查函数接收不同规则；暂时看不懂模板声明时，可以先阅读 main 中的测试对象和本节反例。

## 8 多关键字排序 先比什么写在前面

```cpp
bool before(const Student& a,const Student& b) {
    if (a.score != b.score) return a.score > b.score;
    if (a.name != b.name) return a.name < b.name;
    return a.id < b.id;
}
```

依次是成绩降序、姓名升序、输入序号升序。只有当前关键字相同，才看下一个。对同一记录比较，所有关键字相等，最终 id<id 为 false。

这个规则是各个合法关键字顺序的字典序组合，因此满足所需的先后与等价传递。只检查“自己与自己 false”远远不够。

std::tie(a.score,a.name) 会让两项都按默认升序比较，不能直接代表本题一降一升。把分数取负后建键也要考虑最小整数取负溢出；这里显式分支更直观。

姓名按 std::string 的字典序，不是拼音或自然语言排序。本课输入限定 ASCII 字母姓名，以便验证结果清楚。

## 9 sort 与 stable_sort 的区别

sort 不保证等价记录维持输入顺序；stable_sort 保证比较器判定等价的元素相对顺序不变。

仅按成绩降序，若同分要维持原顺序，可以用 stable_sort；也可以给每条记录记原始 id，用 sort 按“成绩降序、id 升序”。本课还要求姓名第二关键字，所以比较器中加上姓名，再用 id 打破剩余平局。

不要用一次运行中 sort 恰好没打乱同分项来推断稳定性。也不要先 sort 姓名再 sort 成绩，并假设第一次顺序会保留；后一次若用稳定排序才有相应保证，优先先掌握单个多关键字比较器。

C++17 sort 为 O(n log n) 次比较。stable_sort 在额外内存足够时 O(n log n) 次比较，否则上界 O(n log² n)。这不是把字符串比较视为免费：姓名前缀很长时，一次比较也要查看多个字符。

## 10 综合任务 筛选后的学生排行榜

输入 n 和 limit，0<=n<=10000，0<=limit<=100；接着 n 条 `姓名 成绩`。姓名是 1～40 个 ASCII 字母，成绩为 0～100 的整数。按输入顺序给记录 id=1,2,...,n。只保留成绩>=limit 的记录，然后按第 8 节规则排序。

重复姓名和完全重复的姓名/成绩均保留；id 用于区分。输出排名是从 1 连续编号的位置，不采用同分并列排名。无匹配时只输出 count=0。所需字段后的多余输入忽略；长度限制在读入姓名后检查，不是流式内存限额。所有输入合法才开始输出，非法输入退出码 1。

```text
输入
5 60
Bob 90
Alice 90
Bob 90
Zoe 59
Carl 100

输出
count=4
1 Carl 100 5
2 Alice 90 2
3 Bob 90 1
4 Bob 90 3
```

完整程序：[06-ranking.cpp](../examples/06-ranking.cpp)。共享的 [06-ranking-model.hpp](../examples/06-ranking-model.hpp) 放 Student、比较器和阈值函数对象，让程序与规则测试使用同一份实现；#include "文件名" 引入本地头文件，读者不需要先理解大型项目构建。

步骤：读入并验证 → 用捕获 limit 的 lambda 交给 copy_if → back_inserter 创建输出元素 → sort → 输出。copy_if 不修改来源，并按来源顺序复制满足谓词的元素；输出空间由适配器提供，源与目标独立。

筛选不变量：处理前 i 条后，selected 恰好包含其中所有达到阈值的记录，原顺序不变。排序随后改变这个顺序，得到规定的排行。

设匹配数 k、姓名最大长度 L，筛选复制成本 O(n+kL)，排序比较成本可按 O(k log(k+1)·L) 估算；本题 L<=40，可视为有界。记录存储 O(nL)，筛选副本 O(kL)，排序还需实现相关的工作空间，不承诺整段程序原地 O(1)。

## 11 练习与验收

1. limit=60 时分别建立值捕获、引用捕获，再改为 80，预测对 70 的判断。
2. mutable 的值捕获计数器调用两次，外部变量为什么不变？如果闭包也复制一份会如何？
3. 写一个返回 lambda 的工厂，使返回后阈值仍然有效。不要引用捕获函数局部参数。
4. 给“成绩大或姓名小”的错误规则构造两方向都 true 的反例。
5. 仅按成绩排序时，同分不同名为什么可以等价？想保持输入顺序如何写？
6. 将排行榜改成姓名升序、成绩降序、id 升序，写出条件顺序和一组能区别两种排名的输入。
7. 修改阈值筛选为闭区间 low<=score<=high，用值捕获 low/high；明确 low>high 时如何处理。
8. 用 count_if 统计奇数，包括负奇数；为什么应写 x%2!=0，而非 x%2==1？
9. 能否用检测 comp(a,a) 就证明比较器正确？说明还缺什么。

验收：能解释捕获数据和生命周期；知道谓词与排序比较器的区别；能写多关键字规则并说明严格弱序；能覆盖无匹配、全同分、同名同分和非法输入。解答见 [参考答案](../solutions/06-lambda-sort.md)。

建议分三次学：捕获与调用实验；比较规则与反例实验；排行榜及独立改写。下一课系统学习查找、计数、变换和数值算法。

## 规则来源与复现

C++17 工作草案：[lambda 捕获](https://timsong-cpp.github.io/cppwp/n4659/expr.prim.lambda.capture)、[排序与严格弱序](https://timsong-cpp.github.io/cppwp/n4659/alg.sorting)。本文只取 C++17 的用法，不依赖 C++20 ranges 或投影。

运行 `python stl/tests/check_lesson_06.py` 自动编译示例，验证捕获行为、比较器性质、排行输出与预期编译失败。不要以实际执行无效比较器排序或悬空引用作为验证方法。测试通过不代替学习者独立完成练习。
