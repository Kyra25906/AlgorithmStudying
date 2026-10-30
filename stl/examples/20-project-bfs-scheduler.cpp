#include <vector>
#include <queue>
#include <cassert>
struct Task{int priority,id;};struct Cmp{bool operator()(const Task&a,const Task&b)const{return a.priority!=b.priority?a.priority<b.priority:a.id>b.id;}};
int main(){std::vector<std::vector<int>>g{{1,2},{3},{3},{}, {}};std::vector<int>d(g.size(),-1);std::queue<int>q;d[0]=0;q.push(0);while(!q.empty()){int u=q.front();q.pop();for(int v:g[u])if(d[v]==-1){d[v]=d[u]+1;q.push(v);}}assert((d==std::vector<int>{0,1,1,2,-1}));std::priority_queue<Task,std::vector<Task>,Cmp>tasks;tasks.push({2,9});tasks.push({3,5});tasks.push({3,2});assert(tasks.top().id==2);tasks.pop();assert(tasks.top().id==5);}
