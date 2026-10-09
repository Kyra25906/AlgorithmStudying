"""Lesson 07 checks; Python 3 and g++ on PATH, no third-party modules."""
from pathlib import Path
import os
import random
import re
import subprocess
ROOT=Path(__file__).resolve().parents[2]
BUILD=ROOT/'build/lesson-07'
BUILD.mkdir(parents=True,exist_ok=True)
SUFFIX='.exe' if os.name=='nt' else ''
programs={}
checks=0

def compile_source(source,name):
    target=BUILD/(name+SUFFIX)
    result=subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Wpedantic',str(source),'-o',str(target)],capture_output=True)
    assert result.returncode==0 and not result.stderr,result.stderr.decode(errors='replace')
    programs[name]=target

for source in sorted((ROOT/'stl/examples').glob('07-*.cpp')):
    compile_source(source,source.stem)

def run(name,data,expected,code=0,error=''):
    global checks
    result=subprocess.run([str(programs[name])],input=data.encode(),capture_output=True,timeout=20)
    out=result.stdout.decode().replace('\r\n','\n')
    err=result.stderr.decode().replace('\r\n','\n')
    assert (result.returncode,out,err)==(code,expected,error),(name,result.returncode,out[:500],err)
    checks+=1
run('07-search-count','','search checks passed\n')
run('07-transform-numeric','','numeric checks passed\n')

def pipeline(values,threshold):
    first=next((i for i,x in enumerate(values) if x>=threshold),None)
    squares=[x*x for x in values if x>=threshold]
    prefix=[]
    total=0
    for x in squares:
        total+=x
        prefix.append(total)
    expected=f'first={first if first is not None else "none"}\ncount={len(squares)}\n'
    expected+=f'min={min(values)} max={max(values)}\n' if values else 'min=none max=none\n'
    expected+=f'sum={total}\nsquares:'+''.join(f' {x}' for x in squares)+'\nprefix:'+''.join(f' {x}' for x in prefix)+'\n'
    data=f'{len(values)} {threshold}\n'+' '.join(map(str,values))
    run('07-data-pipeline',data,expected)
for values,threshold in [([],0),([-3,2,5,2,0],2),([0],0),([-1],0),
                         ([3,1,3,1],1),([-1000000,1000000],-1000000),
                         ([-3,-2,-1],-2),([1000000]*100000,0),([1,2,3],1000000)]:
    pipeline(values,threshold)
rng=random.Random(7007)
for _ in range(40):
    pipeline([rng.randint(-1000000,1000000) for _ in range(rng.randrange(50))],rng.randint(-1000000,1000000))
for data in ['', '-1 0','100001 0','1 -1000001','1 1000001','x 0']:
    run('07-data-pipeline',data,'',1,'invalid header\n')
for data in ['1 0\n1000001','1 0\n-1000001','2 0\n1','1 0\nx']:
    run('07-data-pipeline',data,'',1,'invalid value\n')
text=(ROOT/'stl/solutions/07-algorithms.md').read_text(encoding='utf-8-sig')
blocks=re.findall(r'```cpp\n(.*?)```',text,re.S)
function=next(b for b in blocks if 'selectedSquareSum' in b)
prefix_code=next(b for b in blocks if 'intervalSum' in b)
source=BUILD/'answer-check.cpp'
source.write_text('#include <vector>\n#include <numeric>\n#include <cassert>\n'+function+'''
int main(){
    assert(selectedSquareSum({},0)==0);
    assert(selectedSquareSum({-3,2,5,2,0},2)==33);
    assert(selectedSquareSum({-1000000,1000000},-1000000)==2000000000000LL);
    for(int n=0;n<=8;++n){
        std::vector<int> a(n);
        for(int i=0;i<n;++i) a[i]=i-4;
        for(int l=0;l<=n;++l) for(int r=l;r<=n;++r){
'''+prefix_code+'''
            long long expected=0;
            for(int i=l;i<r;++i) expected+=a[i];
            assert(intervalSum==expected);
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
