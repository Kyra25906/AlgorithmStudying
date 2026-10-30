# 第九章 二分边界与前 K 项选择：练习解答

1. 分别为 (0,0)、(1,4)、(5,5)。
2. 不满足划分前置条件，无法用普通二分推理保证结果。
3. 排序后，L>R 返回 0，否则 upper_bound(R)-lower_bound(L)。vector 上一次查询 O(log n)。
4. 验证 1≤K≤n 后，用 nth_element(begin,begin+n-K,end)，读取下标 n-K。不要在检查前进行无符号减法。
