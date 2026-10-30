#include <stack>
#include <queue>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <cassert>
#include <iostream>
bool balanced(const std::string& text){
 std::stack<char> s;
 for(char c:text){
  if(c=='('||c=='['||c=='{') s.push(c);
  else {
   if(c!=')'&&c!=']'&&c!='}') return false;
   if(s.empty()) return false;
   char expected=c==')'?'(':c==']'?'[':'{';
   if(s.top()!=expected) return false;
   s.pop();
  }
 }
 return s.empty();
}
struct Task {int priority,id;};
struct Compare {
 bool operator()(const Task& a,const Task& b) const {
  if(a.priority!=b.priority) return a.priority<b.priority;
  return a.id>b.id;
 }
};
int main(){
 assert(balanced("")); assert(balanced("([]){}"));
 for(const auto& s:{"([)]", "]", "((", "abc"}) assert(!balanced(s));
 for(int n=0;n<=50;++n){
  std::queue<int> q; std::priority_queue<int,std::vector<int>,std::greater<int>> minq;
  std::priority_queue<Task,std::vector<Task>,Compare> tasks;
  std::vector<Task> expected;
  for(int i=0;i<n;++i){q.push(i);minq.push(n-i);tasks.push({i%4,i});expected.push_back({i%4,i});}
  for(int i=0;i<n;++i){assert(q.front()==i);q.pop();assert(minq.top()==i+1);minq.pop();}
  std::sort(expected.begin(),expected.end(),[](const Task&a,const Task&b){return a.priority!=b.priority?a.priority>b.priority:a.id<b.id;});
  for(auto t:expected){assert(tasks.top().id==t.id);tasks.pop();}
  assert(q.empty()&&minq.empty()&&tasks.empty());
 }
 std::cout<<"lesson 11 passed\n";
}
