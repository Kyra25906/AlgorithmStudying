"""Lesson 08 checks; Python 3 and g++ on PATH."""
from pathlib import Path
import os
import random
import re
import subprocess
ROOT=Path(__file__).resolve().parents[2]
BUILD=ROOT/'build/lesson-08'
BUILD.mkdir(parents=True,exist_ok=True)
SUFFIX='.exe' if os.name=='nt' else ''
programs={}
checks=0

def compile_source(source,name):
    target=BUILD/(name+SUFFIX)
    result=subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Wpedantic',str(source),'-o',str(target)],capture_output=True)
    assert result.returncode==0 and not result.stderr,result.stderr.decode(errors='replace')
    programs[name]=target
for source in sorted((ROOT/'stl/examples').glob('08-*.cpp')):
    compile_source(source,source.stem)

def run(name,data,expected,code=0,error=''):
    global checks
    result=subprocess.run([str(programs[name])],input=data.encode(),capture_output=True,timeout=20)
    out=result.stdout.decode().replace('\r\n','\n')
    err=result.stderr.decode().replace('\r\n','\n')
    assert (result.returncode,out,err)==(code,expected,error),(name,result.returncode,out[:500],err)
    checks+=1
run('08-compaction','','compaction checks passed\n')
run('08-reordering','','reordering checks passed\n')

def clean_case(values,k):
    kept=[x for x in values if x>=0]
    ordered=sorted(set(kept))
    shift=k%len(ordered) if ordered else 0
    rotated=ordered[shift:]+ordered[:shift]
    expected=f'negative_removed={len(values)-len(kept)}\nduplicate_removed={len(kept)-len(ordered)}\ncount={len(ordered)}\nsorted:'
    expected+=''.join(f' {x}' for x in ordered)+'\nrotated:'+''.join(f' {x}' for x in rotated)+'\n'
    run('08-clean-rotate',f'{len(values)} {k}\n'+' '.join(map(str,values)),expected)
for values,k in [([],0),([],1000000000),([3,-1,3,2,0,-2,2,5],2),
                 ([-1,-2,-3],7),([0],1000000000),([2,2,2],1),
                 ([1000000,-1000000,0],0),([i%1000 for i in range(100000)],1000000000)]:
    clean_case(values,k)
rng=random.Random(8008)
for _ in range(60):
    clean_case([rng.randint(-10,10) for _ in range(rng.randrange(50))],rng.randrange(100))
for data in ['', '-1 0','100001 0','1 -1','1 1000000001','x 0']:
    run('08-clean-rotate',data,'',1,'invalid header\n')
for data in ['1 0\n1000001','1 0\n-1000001','2 0\n1','1 0\nx']:
    run('08-clean-rotate',data,'',1,'invalid value\n')
# Compile the actual three complete functions printed in the answers.
text=(ROOT/'stl/solutions/08-reordering.md').read_text(encoding='utf-8-sig')
blocks=re.findall(r'```cpp\n(.*?)```',text,re.S)
assert len(blocks)==3
source=BUILD/'answer-check.cpp'
source.write_text('#include <algorithm>\n#include <vector>\n#include <cassert>\n#include <cstddef>\n'+ '\n'.join(blocks)+'''
int main(){
 for(int n=0;n<=7;++n){
  std::vector<int> original(n);
  for(int i=0;i<n;++i) original[i]=i%5-2;
  auto evens=original;
  std::vector<int> expectedEven;
  for(int x:original) if(x%2==0) expectedEven.push_back(x);
  assert(eraseOdd(evens)==original.size()-expectedEven.size());
  assert(evens==expectedEven);
  for(int k=0;k<=2*n+2;++k){
   auto actual=original;
   rotateRight(actual,k);
   std::vector<int> expected(n);
   for(int i=0;i<n;++i) expected[(i+k)%n]=original[i];
   assert(actual==expected);
  }
  for(int l=0;l<=n;++l) for(int r=l;r<=n;++r){
   auto actual=original;
   eraseZerosInRange(actual,l,r);
   std::vector<int> expected;
   for(int i=0;i<n;++i) if(i<l||i>=r||original[i]!=0) expected.push_back(original[i]);
   assert(actual==expected);
  }
 }
}
''',encoding='utf-8')
compile_source(source,'answer-check')
run('answer-check','','')
for file in [ROOT/'README.md',*(ROOT/'stl').rglob('*.md')]:
    clean=re.sub(r'```[\s\S]*?```|`[^`\n]*`','',file.read_text(encoding='utf-8-sig'))
    for link in re.findall(r'\]\(([^)]+)\)',clean):
        if '://' not in link and not link.startswith('#'):
            assert (file.parent/link.split('#',1)[0]).exists(),(file,link)
print(f'PASS: {len(programs)} compiled programs; {checks} executions; local links exist')
