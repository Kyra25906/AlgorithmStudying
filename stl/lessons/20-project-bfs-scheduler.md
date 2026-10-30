# 第20章 综合二：BFS 与任务调度

## 已知与所求

无权图最短路用 vector<vector<int>> 邻接表、queue<int> 按层扩展；入队时设置距离，避免重复入队。不可达节点距离为 -1。任务调度用 priority_queue，按优先级降序、编号升序弹出。BFS 时间 O(V+E)、空间 O(V+E)；堆处理 T 个任务为 O(T log T)。

## 状态与边界

运行示例，先预测每个断言检查的状态，再改动输入测试空容器、重复键或无匹配值。涉及引用、迭代器或视图时，先确认对象生命周期与修改操作。

## 练习

1. 解释示例中的容器选择和时间复杂度。
2. 修改一个输入边界并预测结果，再运行核对。

[示例](../examples/20-project-bfs-scheduler.cpp) · [答案](../solutions/20-project-bfs-scheduler.md)
