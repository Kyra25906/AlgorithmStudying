#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
int main(){std::map<std::string,int>freq;for(auto s:{"cat","dog","cat"})++freq[s];std::vector<std::pair<std::string,int>>rank(freq.begin(),freq.end());std::sort(rank.begin(),rank.end(),[](const auto&a,const auto&b){return a.second!=b.second?a.second>b.second:a.first<b.first;});assert(rank[0].first=="cat"&&rank[0].second==2);std::map<int,int>scores;scores.insert_or_assign(7,80);scores.insert_or_assign(7,90);assert(scores.at(7)==90&&scores.size()==1);}
