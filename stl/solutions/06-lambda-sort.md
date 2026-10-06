# 第六课参考答案

先完成[讲义练习](../lessons/06-lambda-comparators.md#11-练习与验收)，再阅读。

## 1 捕获时间

值捕获保存创建时的 60，所以 70 合格；引用捕获读取外部当前 80，所以 70 不合格。修改外部变量不会反向更新闭包内部的 int 副本。

## 2 mutable 与复制闭包

mutable 允许修改闭包自己的值成员。外部 count 不受影响。复制闭包会复制当时的计数状态，之后两个普通值捕获计数器分别变化，不共享状态。对应演示见 06-lambda-captures.cpp。

## 3 返回安全规则

```cpp
auto makePredicate(int limit) {
    return [limit](int value) { return value >= limit; };
}
```

limit 按值保存在闭包中。若写 [&limit]，引用的是函数参数，函数返回后参数不再存在；引用捕获不延长它的生命。

## 4 错误的或条件

A=(90,"Z")、B=(80,"A")。A 分高使 comp(A,B) 为 true，B 名字小使 comp(B,A) 也为 true。改成先判断分数是否不同，仅同分才比较名字。

## 5 等价和稳定

只按分数比较时，同分记录两方向都 false，构成同一个等价组；并不要求名字相同。stable_sort 只比成绩可以保留该组原顺序；或者 sort 按成绩与输入 id 两项排序。不稳定排序偶然保持顺序不是保证。

## 6 改变优先级

```cpp
bool byName(const Student& a,const Student& b) {
    if (a.name != b.name) return a.name < b.name;
    if (a.score != b.score) return a.score > b.score;
    return a.id < b.id;
}
```

例如 Bob 100、Alice 60：原规则 Bob 在前，新规则 Alice 在前。先比较最高优先级；同名同分时使用 id，使顺序可确定。

## 7 闭区间谓词

```cpp
auto inRange = [low,high](const Student& s) {
    return low <= s.score && s.score <= high;
};
```

这里的 && 表示一元筛选条件，不是两个记录的排序比较器，因此与讲义的错误坐标比较例子不同。可约定 low>high 报非法输入并返回 1，然后再开始筛选；测试等端点、0/100、全部不匹配及两端点命中。

## 8 负奇数

```cpp
auto odds = std::count_if(values.begin(),values.end(),
                          [](int x){return x%2 != 0;});
```

C++ 的负奇数除以 2 的余数是 -1，所以 ==1 会漏掉它们。[-3,-2,0,1,2] 的奇数数量为 2。

## 9 检查不等于证明

comp(a,a) 为 false 只检查不自反；还需检查不对称、先后传递和等价传递。有限数据测试只能发现覆盖到的反例。多关键字规则的依据是：每一关键字已有合法顺序，再按优先级作字典序组合，比较过程中不改变规则。不要对错误比较器实际运行 sort 试探结果。
