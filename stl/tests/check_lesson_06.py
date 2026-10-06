"""Lesson 06 verification. Requires Python 3 and g++ on PATH."""
from pathlib import Path
import os
import random
import re
import subprocess
ROOT=Path(__file__).resolve().parents[2]
BUILD=ROOT/'build/lesson-06'
BUILD.mkdir(parents=True,exist_ok=True)
SUFFIX='.exe' if os.name=='nt' else ''
programs={}
checks=0
for source in sorted((ROOT/'stl/examples').glob('06-*.cpp')):
    target=BUILD/(source.stem+SUFFIX)
    result=subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Wpedantic',str(source),'-o',str(target)],capture_output=True)
    assert result.returncode==0 and not result.stderr,result.stderr.decode(errors='replace')
    programs[source.stem]=target

def run(name,data,expected,code=0,error=''):
    global checks
    result=subprocess.run([str(programs[name])],input=data.encode('utf-8'),capture_output=True,timeout=20)
    out=result.stdout.decode('utf-8').replace('\r\n','\n')
    err=result.stderr.decode('utf-8').replace('\r\n','\n')
    assert (result.returncode,out,err)==(code,expected,error),(name,result.returncode,out,err)
    checks+=1
run('06-lambda-captures','','lambda checks passed\n')
run('06-comparator-check','','comparator checks passed\n')

def ranking_case(records,limit):
    data=f'{len(records)} {limit}\n'+''.join(f'{name} {score}\n' for name,score in records)
    # Independent oracle: Python tuple key rather than the C++ branch comparator.
    chosen=sorted([(name,score,i+1) for i,(name,score) in enumerate(records) if score>=limit],key=lambda row:(-row[1],row[0],row[2]))
    expected=f'count={len(chosen)}\n'+''.join(f'{rank} {name} {score} {id}\n' for rank,(name,score,id) in enumerate(chosen,1))
    run('06-ranking',data,expected)
for records,limit in [([],60),([('Bob',90),('Alice',90),('Bob',90),('Zoe',59),('Carl',100)],60),
                     ([('A',0)],0),([('A',99)],100),([('A',100)]*10,100),
                     ([('Z',60),('A',60),('A',59)],60),([('x'*40,100)],0),
                     ([('Z',0),('A',100)]*5000,0)]:
    ranking_case(records,limit)
rng=random.Random(6006)
for _ in range(50):
    records=[(rng.choice(['A','Alice','Bob','Z','a']),rng.choice([0,59,60,90,100])) for _ in range(rng.randrange(31))]
    ranking_case(records,rng.choice([0,60,90,100]))
for data in ['', '-1 60','10001 60','1 -1','1 101','x 60']:
    run('06-ranking',data,'',1,'invalid header\n')
for data,line in [('1 60\nA -1',1),('1 60\nA 101',1),('1 60\nA',1),
                  ('1 60\n'+'x'*41+' 90',1),('1 60\nA1 90',1),('1 60\n中文 90',1),
                  ('2 60\nA 90\nB nope',2)]:
    run('06-ranking',data,'',1,f'invalid student {line}\n')
invalid={
 'uncaptured':'int main(){int x=3; auto f=[](){return x;}; return f();}',
 'immutable-copy':'int main(){int x=0; auto f=[x](){return ++x;}; return f();}',
 'const-sort':'#include <algorithm>\n#include <vector>\nint main(){std::vector<int> a{2,1}; std::sort(a.cbegin(),a.cend());}'
}
for name,code in invalid.items():
    source=BUILD/(name+'.cpp'); source.write_text(code,encoding='utf-8')
    result=subprocess.run(['g++','-std=c++17','-pedantic-errors','-fsyntax-only',str(source)],capture_output=True)
    assert result.returncode!=0 and result.stderr,name
    checks+=1
# Compile the actual alternative comparator from the answer and check its order.
text=(ROOT/'stl/solutions/06-lambda-sort.md').read_text(encoding='utf-8-sig')
fragment=next(b for b in re.findall(r'```cpp\n(.*?)```',text,re.S) if 'bool byName' in b)
source=BUILD/'answer-order.cpp'
source.write_text('#include "06-ranking-model.hpp"\n#include <cassert>\n#include <algorithm>\n#include <vector>\n'+fragment+'\nint main(){std::vector<Student> a{{"Bob",100,1},{"Alice",60,2},{"Alice",90,3},{"Alice",90,4}}; std::sort(a.begin(),a.end(),byName); assert(a[0].id==3 && a[1].id==4 && a[2].id==2 && a[3].id==1);}\n',encoding='utf-8')
target=BUILD/('answer-order'+SUFFIX)
result=subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Wpedantic','-I',str(ROOT/'stl/examples'),str(source),'-o',str(target)],capture_output=True)
assert result.returncode==0 and not result.stderr,result.stderr.decode(errors='replace')
programs['answer-order']=target
run('answer-order','','')
for file in [ROOT/'README.md',*(ROOT/'stl').rglob('*.md')]:
    for link in re.findall(r'\]\(([^)]+)\)',re.sub(r'```[\s\S]*?```|`[^`\n]*`', '', file.read_text(encoding='utf-8-sig'))):
        if '://' not in link and not link.startswith('#'):
            assert (file.parent/link.split('#',1)[0]).exists(),(file,link)
print(f'PASS: {len(programs)} compiled programs; {checks} checks (including {len(invalid)} expected compilation failures); local links exist')
