#include <vector>
#include <string>
#include <string_view>
#include <cassert>
#include <utility>
int main(){std::vector<std::string> v;v.emplace_back(3,'a');assert(v[0]=="aaa");std::string owner="hello";std::string_view view=owner;assert(view.substr(0,2)=="he");std::string moved=std::move(owner);assert(moved=="hello");}
