# 第十一章 栈、队列与优先队列：练习解答

1. stack 剩 1，因此 top=1；queue 剩 2，因此 front=2。
2. true、true、false、false、false。末尾仍有左括号同样失败。
3. 首关键字返回 a.priority > b.priority；次关键字仍是 a.id > b.id。
4. top 返回 const 引用，并且修改键会破坏堆结构；需要通过维护堆的操作或另外设计更新机制处理。
5. 完整代码的 balanced 函数给出实现。遇右括号时先判断 empty，再访问 top；结束检查 empty。字符串长度 n，时间 O(n)、空间 O(n)。
