# 第四课参考答案

先完成[练习](../lessons/04-references-records.md#10-练习与验收)，再阅读。示例均按 C++17。

## 1 副本与别名

最终 x=9、y=9、z=5、r=9。r 始终引用 x；z 初始化时复制值 2；r=y 把 9 赋给 x，不改变引用关系。

## 2 auto 与 const

a 是 int，可以赋值；b 是 const int&，不能通过 b 修改 x；c 同样是 const int&。按值推导忽略顶层 const，引用推导保留被引用对象的限定。

## 3 修改原记录

```cpp
for (auto& student : students) ++student.score;
// 或
for (auto& [name, score] : students) ++score;
```

省略 & 得到副本，去掉 const 只能允许修改这个副本。本题只说明引用效果；如果仍要求成绩不超过 100，还需加入上限规则。

## 4 统计函数

完整可编译示例见 [04-student-records.cpp](../examples/04-student-records.cpp) 的 summarize。使用 const vector<Student>&，不复制容器；计数从 0 开始，以 >=60 判断，和用 long long。空记录返回 {0,0}，[59,60,100] 返回 {2,219}。遍历 n 条，时间 O(n)，函数额外空间 O(1)。

## 5 三项记录

```cpp
std::tuple<std::string, int, bool> row{"Alice", 80, true};
struct Attendance {
    std::string name;
    int score = 0;
    bool present = false;
};
Attendance record{"Alice",80,true};
```

get<2>(row) 依赖记忆字段顺序，record.present 直接说明含义；长期维护时结构体更清楚。结构化绑定仍按字段顺序，并不会根据变量名识别 present。

## 6 绑定和所有权

const auto& 对普通 Student 绑定原记录，不建立含名字的副本；读取成本不包含复制 string。tie 返回引用元组，复制这种元组不会复制被引用对象，因此 auto [a,b]=tie(x,y) 中 a/b 仍关联 x/y。只读引用避免复制，不等于替原对象管理生命周期。

## 7 输出不合格名字

在 adjust 之后加入以下输出代码，不另建记录副本：

```cpp
std::cout << "failed:";
bool any = false;
for (const auto& student : students) {
    if (student.score < 60) {
        std::cout << ' ' << student.name;
        any = true;
    }
}
if (!any) std::cout << " none";
std::cout << '\n';
```

测试空记录、全部合格、全部不合格、调整前后跨越 60、重复姓名。讲义样例输出 failed: Carol；n=0 输出 failed: none。按原顺序扫描保证输出顺序，时间 O(n+输出字符数)，额外空间 O(1)。

## 8 错误分类

通过 const int& 赋值是编译错误。用两个名字绑定三项 tuple 是编译错误。返回局部 string 的引用可能编译并产生告警，但对象已销毁，随后通过该引用读取是未定义行为；不应靠运行观察证明安全。

建议修复是按值返回 string，或在接口明确保证所引用外部对象足够长寿时返回引用。不要把 const 当作延长任意对象生命的机制。
