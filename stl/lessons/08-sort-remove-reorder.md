# 第八章 排序 去重 条件删除与重排

## 已知与所求

已知：你会使用半开区间、谓词和严格弱序，能用 copy_if 把命中项复制到另一个容器。

所求：在已有 vector 中整理数据，完成排序、合并相邻重复、删除命中项、按条件分组及循环移位。重点区分三个问题：内容如何变化、size 是否改变、返回位置表示什么。

本章使用 C++17、无执行策略重载。算法来自 `<algorithm>`，容器成员 erase 来自 `<vector>`。代码：[压缩与删除](../examples/08-compaction.cpp)、[排序与重排](../examples/08-reordering.cpp)、[清洗与循环移位](../examples/08-clean-rotate.cpp)。[解答](../solutions/08-reordering.md)在练习后阅读。

## 1 先确定希望保留什么

输入 [3,-1,3,2,2,1]，下面几种任务答案不同：

| 任务 | 结果或保证 | 主要工具 |
|---|---|---|
| 全部升序排列 | [-1,1,2,2,3,3] | sort |
| 只合并原序列相邻重复 | [3,-1,3,2,1] | unique 加 erase |
| 每个整数只保留一份并升序 | [-1,1,2,3] | sort、unique、erase |
| 删除负数且保持其他顺序 | [3,3,2,2,1] | remove_if 加 erase |
| 把非负项放前面，仍保留全部项 | 前段非负、后段负数 | partition |

删除、排序、分组和去重不能互相代替。先说明原顺序是否需要保持、哪些元素应留下，才能选择合适操作。

## 2 sort 与 stable_sort 回顾

```cpp
std::sort(a.begin(),a.end());
std::sort(a.begin(),a.end(),[](int x,int y){return x>y;});
```

两个调用分别升序、降序，返回 void，就地改变元素顺序，不改变 size。需要随机访问迭代器以及可移动、可交换的元素；不能用 cbegin/cend 写入，也不能对 list 使用 std::sort。

stable_sort 保持比较器等价元素的原相对顺序。例如按分数降序排序两条同分不同名记录，若要求原先在前的人仍在前，可只比较分数并使用 stable_sort。

“等价”仍指 !comp(a,b)&&!comp(b,a)，不一定代表每个字段都相同。普通 sort 可能恰好保留某次输入的平局顺序，但没有该保证。

sort 有 O(n log n) 次比较；C++17 stable_sort 额外内存足够时 O(n log n) 次，否则为 O(n log² n) 上界。比较长字符串的成本需另计。比较器必须保持严格弱序，不使用 <=。

## 3 unique 只压缩相邻等价组

```cpp
std::vector<int> a{1,1,2,1,1};
auto logicalEnd=std::unique(a.begin(),a.end());
```

此时能够依赖的状态是：

```text
实际 size 仍为 5
[begin, logicalEnd) = [1,2,1]
[logicalEnd, end)   = 未指定的有效对象状态，不能当作业务结果
```

这里开头的 1 与后面的 1 不相邻，所以都留下。算法把保留项按顺序移到前面，返回整理结果的尾后位置。它没有拿到容器对象，也没有调用 vector::erase。

不要根据本机看到的尾部数字推断尾部总是原重复项；对 string 等可移动对象，尾部可能处于移动后状态。本章只检查逻辑前缀，不读取尾部作为“被删数据”。

如果需要容器本身只剩这些元素：

```cpp
a.erase(logicalEnd,a.end()); // size 现在变成 3
```

完整写法：

```cpp
a.erase(std::unique(a.begin(),a.end()),a.end());
```

空容器、无重复、全部重复都可采用这套写法。全部重复且非空时保留首项；空输入仍为空。unique 对非空 n 项进行 n-1 次等价判断，整体线性处理。

## 4 全局去重先让同类相邻

整数升序去重：

```cpp
std::sort(a.begin(),a.end());
a.erase(std::unique(a.begin(),a.end()),a.end());
```

排序让相同整数形成连续组，unique 才能一次压缩每组。代价是原顺序已经改变。这不是“保留首次出现顺序”的通用解法；后一需求可在后续哈希容器课程中解决。

自定义 unique 谓词回答“是否等价”，与 sort 回答“谁严格在前”不同：

```cpp
auto sameName=[](const Student& x,const Student& y){
    return x.name==y.name;
};
```

该谓词需要满足等价关系：自反、对称、传递，不能直接传排序的 < 比较器。若要每个姓名保留最高成绩，应先按姓名升序、同名成绩降序排序，再按姓名等价 unique；同名同分还需要明确是否按原始 id 打破平局。

如果先按成绩为主排序，不同姓名可能穿插，按姓名 unique 就不能保证全局去重。排序规则必须把希望合并的等价组聚到一起。

## 5 remove 与 remove_if 返回保留段终点

```cpp
std::vector<int> a{3,-1,2,-4,5};
auto logicalEnd=std::remove_if(a.begin(),a.end(),
                              [](int x){return x<0;});
```

谓词为 true 表示应删除；第七章 copy_if 的 true 表示应复制，方向相反。现在 size 仍为 5，逻辑前缀为 [3,2,5]，保留项原顺序不变；尾部不能当作全部负数。

```cpp
a.erase(logicalEnd,a.end()); // 真正移除尾部对象
```

remove(first,last,value) 按 == 判断，remove_if 按谓词判断。对 n 个元素分别做 n 次比较/谓词调用。二者要求可以向区间内元素移动赋值；set 的键不可改，不能直接使用这种压缩办法。

不要把删除目标值引用到同一个会被改写的范围：

```cpp
int target=a[0]; // 先保存独立副本；这里要求非空
// 比起直接传 a[0]，副本不会随压缩而改变。
a.erase(std::remove(a.begin(),a.end(),target),a.end());
```

remove 的 value 参数按 const 引用接收，直接传范围内元素可能在压缩时改变所比较的值。谓词捕获删除依据时也要避免引用到将被改写的元素。

## 6 算法压缩与成员删除是两步

| 阶段 | size | 内容 | 旧位置风险 |
|---|---|---|---|
| unique/remove_if | 不变 | 保留结果移到前部 | 位置仍在，但可能已是别的值 |
| vector::erase 尾部范围 | 变小 | 尾部对象被销毁 | 删除点及之后、旧 end 失效 |

remove_if 返回的是将要传入 erase 的位置，不能 erase 后继续用它读取元素。要知道删了多少，先保存数量或记录操作前后 size 差。

```cpp
auto before=a.size();
a.erase(std::remove_if(a.begin(),a.end(),pred),a.end());
auto removed=before-a.size();
```

逐个 vector.erase 中间元素可能反复搬移，总体 O(n²)；先线性压缩、再一次删除尾部可线性完成。erase 不保证释放容量。C++20 有 std::erase_if 简化写法，本章保留 C++17 组合。

若只处理 [first,last) 子区间，不能随便 erase(newEnd,a.end())，否则会连原区间之外的后缀一起删。需要删除的是 [newEnd,last)，且修改后重新取得后续位置；本章完整程序都处理全区间。

## 7 partition 按真假分成两段

```cpp
auto cut=std::partition(a.begin(),a.end(),
                        [](int x){return x%2==0;});
```

[begin,cut) 全部满足谓词，[cut,end) 全部不满足。所有元素仍在，size 不变；每段内部未必有序，也不保证保持原顺序。不要为普通 partition 的内部排列写固定预期值。

例：[3,2,1,4] 用偶数谓词，只保证前两项为偶数、后两项为奇数，并保留原多重集合；具体排列可以不同。空区间返回 end，全部为 true 返回 end，全部为 false 返回 begin。

需要两组分别保留原顺序时用 stable_partition，该例确定得到 [2,4,3,1]。返回值仍是分界，不是元素个数，可对 vector 用 cut-begin 求第一组长度。

C++17 无执行策略 partition 需要前向可交换迭代器；stable_partition 需要双向迭代器及相应可移动元素。partition 线性谓词调用与线性交换上界；stable_partition 谓词调用线性，额外内存足够时线性交换，否则有 O(n log n) 交换上界。不要把稳定分组一概说成保证 O(1) 额外空间。

is_partitioned 检查是否已满足同一谓词的“真在前、假在后”。分组边界不代表整个区间排好序，不能据此套用普通有序查找。

## 8 reverse 与 rotate

reverse(first,last) 把序列反转，返回 void，需要双向迭代器。空或单元素不变。它真的交换元素，区别于第五章的反向迭代器仅改变读取方向。

rotate(first,middle,last) 把两个相邻区间整体换位：

```text
原来 [first,middle) [middle,last)
后来 [middle,last)  [first,middle)
```

例如 [1,2,3,4,5]，middle=begin+2，结果为 [3,4,5,1,2]，两个块内部顺序保留。需要合法的前向迭代器和可移动/交换元素，middle 必须位于区间边界之间。

返回值指向原 first 元素移动后的新位置；对 vector 可写为 first+(last-middle)。特殊边界：middle==first 时不移动，返回 last；middle==last 时不移动，返回 first；全空时这些位置相同。

左旋 k 项，先处理空容器再归一化：

```cpp
if (!a.empty()) {
    const auto shift=k%a.size(); // 本例 k 为非负整数
    std::rotate(a.begin(),a.begin()+shift,a.end());
}
```

不能在空序列上先做 k%size。负数 k 不能直接混用无符号 size，要先明确负值的语义；本章输入拒绝负数。右旋 k 项等价于左旋 (n-k%n)%n，仅在 n>0 时计算。

reverse 和 rotate 都线性操作、不改变容器大小，不会自动建立或删除对象。

## 9 接口速查与验证边界

| 接口 | 返回 | size 是否变化 | 关键保证 |
|---|---|---|---|
| sort / stable_sort | void | 否 | 按严格弱序排序；后者保持等价项顺序 |
| unique | 逻辑尾后 | 否 | 每个相邻等价组保留首项 |
| remove / remove_if | 逻辑尾后 | 否 | 稳定保留不命中项 |
| vector.erase(first,last) | 删除段之后的新位置 | 是，除非空范围 | 删除实际元素 |
| partition / stable_partition | 真/假分界 | 否 | 全体元素保留；后者两段都稳定 |
| reverse | void | 否 | 顺序反转 |
| rotate | 指定的新位置 | 否 | 两相邻块换位 |

验证时只断言标准保证：不检查 unique/remove 的尾部具体值，不检查 partition 的组内固定次序，也不根据 sort 的一次结果推断它稳定。空区间不解引用返回的 end。

## 10 综合任务 清洗 去重与循环移位

输入 n、k，0<=n<=100000，0<=k<=1000000000，然后 n 个 [-1000000,1000000] 内整数。

依次删除负数、把剩余值升序去重、保存一份排序结果，再左旋 k 项。输出删除的负数数量、删除的重复项数量、最终长度、升序结果和旋转结果。全部输入校验成功才输出；非法输入返回 1 且标准输出为空。约定字段之外的额外输入忽略。

```text
输入
8 2
3 -1 3 2 0 -2 2 5

输出
negative_removed=2
duplicate_removed=2
count=4
sorted: 0 2 3 5
rotated: 3 5 0 2
```

完整代码：[08-clean-rotate.cpp](../examples/08-clean-rotate.cpp)。阶段不变量：删除后只剩原顺序非负项；排序去重后每个不同非负整数出现一次并升序；旋转只改变顺序，数量和元素集合保持不变。

n=0 或全部负数时两个序列为空，跳过取模。无重复时重复删除数为零。时间由排序主导，O(n log(n+1))；存储输入与结果副本 O(n)，排序还需实现相关的工作空间。本例额外副本服务于同时输出两种顺序，只有旋转输出时可省去。

## 11 练习与验收

1. 对 [2,2,1,2,2] 执行 unique，写出逻辑前缀和 size，不猜尾部内容；再 erase 后是什么？
2. 区分 remove_if 的 true 与 copy_if 的 true；说明为何不能直接把 remove_if 后的尾部当作被删项。
3. 写 C++17 函数，原地删除所有奇数，保留偶数顺序并返回删除数量；考虑负奇数与空输入。
4. 对记录按姓名去重并保留最高分，写排序与等价谓词，说明同名同分的保留规则。
5. 对 [3,2,1,4]，partition 与 stable_partition 分别能保证哪些结果？
6. 左旋 [1,2,3,4,5] 两项后 rotate 返回的位置下标是多少？middle==begin/end 时呢？
7. 写右旋非负 k 项的函数，测试空、单元素、k=0、k=n、k>n。
8. 只在 vector 的 [l,r) 中删除零，保留区间外内容；解释为何不能 erase(newEnd,end())。

验收：能画出逻辑终点与实际终点；按需要选择稳定性；知道去重的等价关系与排序关系不同；独立完成第 3、7、8 题并验证边界。[参考答案](../solutions/08-reordering.md)。

建议三次学习：unique/remove 状态实验；稳定分组与旋转；综合程序与练习。下一章进入二分边界、部分排序和第 K 项选择。

## 规则来源与复现

C++17 工作草案：[remove](https://timsong-cpp.github.io/cppwp/n4659/alg.remove)、[unique](https://timsong-cpp.github.io/cppwp/n4659/alg.unique)、[partition](https://timsong-cpp.github.io/cppwp/n4659/alg.partitions)、[rotate](https://timsong-cpp.github.io/cppwp/n4659/alg.rotate)。均讨论本章无执行策略重载。

运行 `python stl/tests/check_lesson_08.py` 编译三个示例与答案片段，检查压缩前缀、稳定性、旋转边界和完整输出。测试通过仅说明材料自检完成，不替代学习者独立练习。
