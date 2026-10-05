# AlgorithmStudying

算法与 C++ 学习记录。讲义以理解数据状态、操作条件、复杂度和独立编程为目标。

## STL 深入学习

- [学习讲义规划](stl/PLAN.md)
- [第一课 vector 的元素、长度与容量](stl/lessons/01-vector-model.md)
- [状态演示代码](stl/examples/01-vector-model.cpp)
- [成绩筛选完整程序](stl/examples/01-scores.cpp)
- [第一课练习参考答案](stl/solutions/01-vector.md)

目前已完成规划和第一课，其余课程尚未撰写。主线为 C++17，C++20 扩展单独标注。

## 运行示例

在仓库根目录用 PowerShell 执行，需已安装支持 C++17 的 g++：

```powershell
New-Item -ItemType Directory -Force build | Out-Null
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/01-vector-model.cpp -o build/01-vector-model.exe
./build/01-vector-model.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/01-scores.cpp -o build/01-scores.exe
"5`n80 59 100 60 40" | ./build/01-scores.exe
```

状态演示的 capacity 数值依赖实现，不要求每台机器一致。编译产物保存在 build，不进入版本管理。练习先独立完成，再阅读答案。
