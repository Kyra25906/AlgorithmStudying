# 第八章参考答案

先完成[练习](../lessons/08-sort-remove-reorder.md#11-练习与验收)。全部按 C++17。

## 1 相邻去重

逻辑前缀 [2,1,2]，size 仍为 5。erase(logicalEnd,end) 后 size=3，内容 [2,1,2]。不预测剩余尾部；非相邻的两个 2 不会自动合并。

## 2 谓词方向

remove_if 命中 true 的项应排除；copy_if 命中 true 的项应复制。remove_if 通过移动保留项形成前缀，尾部不是稳定收集的删除项。要保留两组内容，可以用两个独立目标或 partition_copy，而非读压缩后的未指定尾部。

## 3 删除奇数

```cpp
std::size_t eraseOdd(std::vector<int>& a) {
    const auto oldSize=a.size();
    a.erase(std::remove_if(a.begin(),a.end(),
                          [](int x){return x%2!=0;}),a.end());
    return oldSize-a.size();
}
```

负奇数余数可能是 -1，所以用 !=0。时间 O(n)；保留的偶数顺序不变。测试空、全奇、全偶、[-3,-2,0,1,2]。

## 4 每个姓名保留最高分

排序先比 name 升序，同名比 score 降序，同名同分再比输入 id 升序。unique 的等价谓词仅比较 name==name。因此同姓名形成连续组，第一条正是最高分且并列时最早输入的记录。不能把 name<name 交给 unique。

## 5 分组与稳定

partition 保证前两项为 2 和 4、后两项为 3 和 1，但不规定各段内部次序。stable_partition 的确切结果是 [2,4,3,1]。两者 size 均为 4，也没有把每段排序。

## 6 rotate 返回值

左旋两项后 [3,4,5,1,2]，原首项 1 位于下标 3，返回 begin+3。middle==begin 时返回 end；middle==end 时返回 begin。空区间两个返回位置一致。

## 7 右旋

```cpp
void rotateRight(std::vector<int>& a,std::size_t k) {
    if (a.empty()) return;
    const auto shift=k%a.size();
    std::rotate(a.begin(),a.end()-shift,a.end());
}
```

k=0 或 n 的倍数时 middle=end，不改变内容。空容器在取模前返回；[1,2,3,4,5] 右旋 2 得 [4,5,1,2,3]，右旋 7 相同。时间 O(n)。

## 8 只删除子区间中的零

下面函数假设调用者已经验证 0<=l<=r<=a.size()：

```cpp
void eraseZerosInRange(std::vector<int>& a,std::size_t l,std::size_t r) {
    auto first=a.begin()+l;
    auto last=a.begin()+r;
    auto logicalEnd=std::remove(first,last,0);
    a.erase(logicalEnd,last);
}
```

区间外前缀没有参与压缩，区间外后缀随后被 erase 正常左移。若写 a.erase(logicalEnd,a.end())，会误删整个外部后缀。测试 [9,0,2,0,8] 的 [1,4) 得 [9,2,8]，空区间不变，全区间则等同删除全部零。erase 后不再使用旧 last。
