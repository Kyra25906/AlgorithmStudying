#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

// 常量引用打印，不复制容器，也不允许修改。
void show(const char* label, const std::vector<int>& a) {
    std::cout << label << " size=" << a.size()
              << " capacity=" << a.capacity() << " values:";
    for (int x : a) std::cout << ' ' << x;
    std::cout << '\n';
}

int main() {
    // 1. 拷贝构造与赋值：彼此独立
    std::vector<int> a{1, 2, 3};
    std::vector<int> b(a);
    assert((b == std::vector<int>{1, 2, 3}));
    b[0] = 9;
    assert((a == std::vector<int>{1, 2, 3})); // a 不受影响
    b = a;                                     // 拷贝赋值覆盖 b
    assert((b == std::vector<int>{1, 2, 3}));
    show("copy", b);

    // 2. 移动：源对象进入“有效但未指定”状态，不能再读内容
    std::vector<int> c = std::move(b);         // b 被搬走
    assert((c == std::vector<int>{1, 2, 3}));
    b = std::vector<int>{5, 6};                // 重新赋值是安全的
    assert((b == std::vector<int>{5, 6}));
    show("moved", c);

    // 3. assign：用数量、列表或区间整体替换
    std::vector<int> d;
    d.assign(3, 7);
    assert((d == std::vector<int>{7, 7, 7}));
    d.assign({1, 2, 3, 4});
    assert((d == std::vector<int>{1, 2, 3, 4}));
    std::vector<int> src{9, 8, 7};
    d.assign(src.begin(), src.begin() + 2);
    assert((d == std::vector<int>{9, 8}));
    show("assign", d);

    // 4. swap：内容交换，引用与迭代器不失效
    std::vector<int> x{1}, y{2, 3};
    x.swap(y);
    assert((x == std::vector<int>{2, 3}) && (y == std::vector<int>{1}));
    show("swap", x);

    // 5. insert：注意返回值指向新插入元素
    std::vector<int> e{1, 2, 3};
    auto it = e.insert(e.begin() + 1, 10);
    assert((e == std::vector<int>{1, 10, 2, 3}));
    assert(*it == 10);
    e.insert(e.end(), 2, 0);
    assert((e == std::vector<int>{1, 10, 2, 3, 0, 0}));
    e.insert(e.begin(), {7, 8});
    assert((e == std::vector<int>{7, 8, 1, 10, 2, 3, 0, 0}));
    show("insert", e);

    // 6. erase：返回值指向被删元素之后的元素
    std::vector<int> f{1, 2, 3, 4, 5};
    auto jt = f.erase(f.begin() + 1);           // 删 2，jt 指向 3
    assert((f == std::vector<int>{1, 3, 4, 5}));
    assert(*jt == 3);
    auto kt = f.erase(f.begin(), f.begin() + 2); // 删 1,3，kt 指向 4
    assert((f == std::vector<int>{4, 5}));
    assert(*kt == 4);
    show("erase", f);

    // 7. 二维：vector 的 vector 与拍平数组，按位置一一对应
    int rows = 2, cols = 3;
    std::vector<std::vector<int>> m(rows, std::vector<int>(cols));
    std::vector<int> flat(rows * cols);
    int val = 0;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j) {
            m[i][j] = val;
            flat[i * cols + j] = val;
            ++val;
        }
    assert(m[1][2] == flat[1 * cols + 2]);

    // 8. 复制交换缩容：capacity 不再大于 size
    std::vector<int> g;
    g.reserve(100);
    g.push_back(1);
    const auto before = g.capacity();
    assert(before >= 100);
    std::vector<int>(g).swap(g);   // 临时容器恰好大小，再交换
    assert(g.size() == 1 && g.capacity() == 1);

    std::cout << "all checks passed\n";
}
