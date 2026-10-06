# AlgorithmStudying

算法与 C++ 学习记录。讲义以理解数据状态、操作条件、复杂度和独立编程为目标。

## STL 深入学习

- [学习讲义规划](stl/PLAN.md)
- [第一课 vector 的元素、长度与容量](stl/lessons/01-vector-model.md)
- [第一课状态演示代码](stl/examples/01-vector-model.cpp)
- [第一课成绩筛选完整程序](stl/examples/01-scores.cpp)
- [第一课练习参考答案](stl/solutions/01-vector.md)
- [第二课 vector 的构造赋值、插入删除与二维用法](stl/lessons/02-vector-construction-erase.md)
- [第二课状态演示代码](stl/examples/02-vector-ops.cpp)
- [第二课安全遍历删除程序](stl/examples/02-safe-erase.cpp)
- [第二课矩阵程序](stl/examples/02-matrix.cpp)
- [第二课练习参考答案](stl/solutions/02-vector.md)

- [第三课 string 的输入、查找、截取、替换与编码边界](stl/lessons/03-string-parsing.md)
- [第三课练习参考答案](stl/solutions/03-string.md)
- [第二、三课自动检查脚本](stl/tests/check_lessons_02_03.py)

- [第四课 用引用和记录类型准确表达数据](stl/lessons/04-references-records.md)
- [第四课练习参考答案](stl/solutions/04-records.md)
- [第四课自动检查](stl/tests/check_lesson_04.py)

- [第五课 迭代器与半开区间](stl/lessons/05-iterators-ranges.md)
- [第五课练习参考答案](stl/solutions/05-iterators.md)
- [第五课自动检查](stl/tests/check_lesson_05.py)

目前已完成规划和前五课，其余课程尚未撰写。主线为 C++17，C++20 扩展单独标注。

## 运行示例

在仓库根目录用 PowerShell 执行，需已安装支持 C++17 的 g++：

```powershell
New-Item -ItemType Directory -Force build | Out-Null
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/01-vector-model.cpp -o build/01-vector-model.exe
./build/01-vector-model.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/01-scores.cpp -o build/01-scores.exe
"5`n80 59 100 60 40" | ./build/01-scores.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/02-vector-ops.cpp -o build/02-vector-ops.exe
./build/02-vector-ops.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/02-safe-erase.cpp -o build/02-safe-erase.exe
"6`n3 1 2 3 4 3`n3" | ./build/02-safe-erase.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/02-matrix.cpp -o build/02-matrix.exe
"2 3`n1 2 3`n4 5 6" | ./build/02-matrix.exe
```

第三课运行：

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/03-string-ops.cpp -o build/03-string-ops.exe
./build/03-string-ops.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/03-line-input.cpp -o build/03-line-input.exe
"3`nAlice Smith`n`n  Bob" | ./build/03-line-input.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/03-text-parser.cpp -o build/03-text-parser.exe
"name=Alice Smith`nempty=" | ./build/03-text-parser.exe
# 可选：Python 3 自动编译并检查两课全部示例及第三课答案程序
python stl/tests/check_lessons_02_03.py
```

含中文的管道输入受 PowerShell 编码设置影响；自动检查脚本以 UTF-8 字节送入进程。

状态演示的 capacity 数值依赖实现，不要求每台机器一致。编译产物保存在 build，不进入版本管理。练习先独立完成，再阅读答案。


第四课运行：

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/04-reference-auto.cpp -o build/04-reference-auto.exe
./build/04-reference-auto.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/04-record-types.cpp -o build/04-record-types.exe
./build/04-record-types.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/04-student-records.cpp -o build/04-student-records.exe
"3`n5`nAlice 98`nBob 55`nCarol 20" | ./build/04-student-records.exe
python stl/tests/check_lesson_04.py
```

最后的检查包含预期编译失败用例；脚本只在失败符合预期时判定通过。不会实际执行悬空引用或越界访问。


第五课运行：

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/05-iterator-basics.cpp -o build/05-iterator-basics.exe
./build/05-iterator-basics.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/05-iterator-adapters.cpp -o build/05-iterator-adapters.exe
./build/05-iterator-adapters.exe
g++ -std=c++17 -Wall -Wextra -Wpedantic stl/examples/05-range-copy.cpp -o build/05-range-copy.exe
"5 1 4`n10 20 30 40 50" | ./build/05-range-copy.exe
python stl/tests/check_lesson_05.py
```

区间按下标 [l,r) 解释，允许 l=r 和 r=n；先验证边界，再形成迭代器。自动检查还会编译练习中的反向复制片段并验证其边界。
