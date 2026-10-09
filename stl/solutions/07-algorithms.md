# 第七章参考答案

先完成[练习](../lessons/07-search-transform-numeric.md#11-练习与验收)。以下代码按 C++17，所用数值范围遵循讲义。

## 1 查找与极值

find(1) 指向下标 1；count(3) 为 2；min_element 指向 1；max_element 指向 0；minmax_element 的两个位置为 1 和 2。它们分别返回首次最小与末次最大，后者不等于独立 max_element 的首次最大。

## 2 空序列

all_of 为 true，any_of 为 false，none_of 为 true。“非空且全正”为 `!a.empty() && std::all_of(a.begin(),a.end(),[](int x){return x>0;})`。空区间没有反例，不代表它含有某个合格元素。

## 3 筛选与取反

先用 copy_if 选 x<0，再 transform 返回 -x。输入限定 [-10^6,10^6]，取反可在 int 内表示；不能推广成任意 int 的最小值取反都安全。也可直接把运算提升到 long long，仍须理解允许范围。

## 4 二元变换

先判断 a.size()==b.size()；不相等就拒绝该任务。相等时建立 result(a.size())，用二元 transform 返回 `1LL*x-y` 并存到 vector<long long>。这里先提升，避免两个 int 先相减溢出。空输入返回空结果。

## 5 两类累计的差别

accumulate 的类型来自 init，因此用 0LL 并证明累计不越界。partial_sum 的类型来自输入 value_type，必须先建立 vector<long long> wide(a.begin(),a.end()) 再累计，或者使用显式 long long 循环；仅扩大目标元素类型不够。

## 6 首项为零的前缀和

```cpp
std::vector<long long> prefix(a.size()+1,0);
for (std::size_t i=0;i<a.size();++i)
    prefix[i+1]=prefix[i]+a[i];
// 校验 0<=l<=r<=a.size() 后
long long intervalSum=prefix[r]-prefix[l];
```

prefix[i] 保存前 i 个元素总和，空区间 l==r 得 0。仍需保证累计和与最后减法可表示。本章数量与数值范围下安全。

## 7 前缀再差分

[2,5,-1] → [2,7,6] → [2,5,-1]。第一项照抄，后续相邻前缀相减消去共同部分。但有符号整数溢出不能用数学恒等式补救；必须先保证每一步类型范围安全。

## 8 不保存平方数组

以下函数假设输入满足本章范围：

```cpp
long long selectedSquareSum(const std::vector<int>& a,int threshold) {
    return std::accumulate(a.begin(),a.end(),0LL,
        [threshold](long long sum,int x){
            return x>=threshold ? sum+1LL*x*x : sum;
        });
}
```

时间 O(n)，额外空间 O(1)，直接返回一个总和；它不再提供筛选项、平方序列和前缀序列，不能冒充完成了原任务全部输出。空输入返回 0。
