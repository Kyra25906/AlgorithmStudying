# 第15章 归并、集合、排列与堆

## 已知与所求

`merge` 和 set_union/intersection/difference 要求输入按同一比较关系有序，输出区间需有足够空间，常配 back_inserter。集合算法处理重复值的次数按标准多重集语义。`next_permutation` 找下一个字典序排列，返回 false 时重排为最小排列。`make_heap/push_heap/pop_heap` 默认最大堆；pop_heap 将最大值放到末尾，随后再 pop_back。

## 状态与边界

运行示例，先预测每个断言检查的状态，再改动输入测试空容器、重复键或无匹配值。涉及引用、迭代器或视图时，先确认对象生命周期与修改操作。

## 练习

1. 解释示例中的容器选择和时间复杂度。
2. 修改一个输入边界并预测结果，再运行核对。

[示例](../examples/15-advanced-algorithms.cpp) · [答案](../solutions/15-advanced-algorithms.md)
