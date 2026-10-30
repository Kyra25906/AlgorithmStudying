# 第十二章 set 与 multiset：动态有序集合：练习解答

1. 返回 2，剩下 {2}。只删一个需 find 后 erase(iterator)。
2. 键经迭代器暴露为不可修改的值，以维护有序性。
3. auto it=s.lower_bound(x); 若 it==s.begin() 则没有前驱，否则 --it 后读取。空容器同样返回没有。
4. 被视为等价键；后一次插入的 bool 为 false。若身份不同都应保存，把 id 纳入排序规则。
5. 可用 s.erase(s.begin(),s.lower_bound(L))，范围删除复杂度 O(log n + 删除数量)。也可循环 it=s.erase(it)，不要删除后再递增已失效的旧迭代器。
