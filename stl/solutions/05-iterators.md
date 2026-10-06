# 第五课参考答案

先独立完成[讲义练习](../lessons/05-iterators-ranges.md#11-分层练习与验收)。所有代码按 C++17 理解。

## 1 位置与尾后

两次 ++ 后指向 6；再前进一步等于 end，可以与同一容器的 end 比较，不能解引用，也不能继续 ++。

## 2 区间

[begin,begin) 和 [end,end) 都为空；[begin+1,begin+4) 包含下标 1、2、3 共三个元素。空区间不读取端点，因此尾后位置可以作为空区间两端。

## 3 const 的位置

const auto it=a.begin() 限制 it 本身，不能 ++it，但 a 非 const 时可以写 *it。auto it=a.cbegin() 可以移动，不能写 *it。只读迭代器仍受容器修改失效规则约束。

## 4 链表的移动

list 节点之间需要逐个前进，next(begin,100) 需要 100 次步进，即 O(k)。必须事先保证足够长度。没有随机访问能力，所以迭代器减法不成立；正向 distance 也需要扫描，不适合反复用于求循环序号。

## 5 插入顺序

front_inserter 得到 [3,2,1,9]；back_inserter 得到 [9,1,2,3]。区别来自每次写入分别调用 push_front 或 push_back。vector 没有 push_front，不能使用 front_inserter(vector)。

## 6 两种修复

```cpp
std::vector<int> dest(source.size());
std::copy(source.begin(),source.end(),dest.begin());
```

或：

```cpp
std::vector<int> dest;
dest.reserve(source.size()); // 可选优化，不建立元素
std::copy(source.begin(),source.end(),std::back_inserter(dest));
```

第一种覆盖现存元素，第二种通过 push_back 增加元素。空源时两种都合法。

## 7 反向建立目标

在完整程序的输入与范围检查后，可用以下直观写法，不需要额外模板知识：

```cpp
std::vector<int> reversed;
auto first = source.cbegin()+l;
auto current = source.cbegin()+r;
while (current != first) {
    --current;
    reversed.push_back(*current);
}
```

先确认未到左边界才递减，因此空区间不操作，单元素恰好复制一次。循环不变量：目标等于原区间已从右侧处理部分的反向序列，未处理部分是 [first,current)。时间 O(r-l)，输出存储 O(r-l)。测试 [1,4) 的 [10,20,30,40,50] 得 [40,30,20]；[2,2) 得空；[0,1) 得 [10]。

也可以学习 `<iterator>` 的 make_reverse_iterator：从正向右端建立反向起点，从正向左端建立反向终点；端点顺序需互换。

## 8 base 的偏移

反向指向 20 时 base 正向指向 30，直接 erase(base) 会删除 30。对应 20 的正向位置是 prev(base)。真正执行删除后，应重新分析迭代器失效，不能继续盲目递增旧反向迭代器。

## 9 错误类别

list 迭代器加整数、给 cbegin 指向元素赋值通常是编译错误。空容器 prev(end())、解引用 end() 可能编译成功，但违反前置条件；C++17 中不能合法执行。不要把“编译通过”或“没有崩溃”当作正确性证据。
