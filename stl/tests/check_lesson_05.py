"""Check lesson 05 with Python 3 and g++ (C++17)."""
from pathlib import Path
import os
import re
import subprocess
ROOT=Path(__file__).resolve().parents[2]
BUILD=ROOT/'build/lesson-05'
BUILD.mkdir(parents=True,exist_ok=True)
SUFFIX='.exe' if os.name=='nt' else ''
programs={}
checks=0
for source in sorted((ROOT/'stl/examples').glob('05-*.cpp')):
    target=BUILD/(source.stem+SUFFIX)
    result=subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Wpedantic',str(source),'-o',str(target)],capture_output=True)
    assert result.returncode==0 and not result.stderr,result.stderr.decode(errors='replace')
    programs[source.stem]=target

def run(name,data,expected,code=0,error=''):
    global checks
    result=subprocess.run([str(programs[name])],input=data.encode(),capture_output=True,timeout=20)
    out=result.stdout.decode().replace('\r\n','\n')
    err=result.stderr.decode().replace('\r\n','\n')
    assert (result.returncode,out,err)==(code,expected,error),(name,result.returncode,out,err)
    checks+=1
run('05-iterator-basics','','iterator checks passed\n')
run('05-iterator-adapters','','adapter checks passed\n')
# Every valid interval for lengths 0 through 6, with negative/repeated values.
for n in range(7):
    values=[i%3-1 for i in range(n)]
    for l in range(n+1):
        for r in range(l,n+1):
            selected=values[l:r]
            data=f'{n} {l} {r}\n'+' '.join(map(str,values))
            expected=f'count={r-l}\nforward:'+''.join(f' {x}' for x in selected)+'\nreverse:'+''.join(f' {x}' for x in reversed(selected))+'\n'
            run('05-range-copy',data,expected)
run('05-range-copy','5 1 4\n10 20 30 40 50','count=3\nforward: 20 30 40\nreverse: 40 30 20\n')
run('05-range-copy','100000 99999 100000\n'+'7 '*100000,'count=1\nforward: 7\nreverse: 7\n')
for data in ['', '-1 0 0','100001 0 0','3 -1 2','3 2 1','3 0 4','0 0 1','x 0 0']:
    run('05-range-copy',data,'',1,'invalid range\n')
for data in ['2 0 2\n1','1 0 1\nx','1 0 1\n2147483648']:
    run('05-range-copy',data,'',1,'invalid value\n')
invalid={
 'list-add':'#include <list>\nint main(){std::list<int> a{1,2}; auto it=a.begin()+1; (void)it;}',
 'const-write':'#include <vector>\nint main(){std::vector<int> a{1}; *a.cbegin()=2;}',
 'locked-increment':'#include <vector>\nint main(){std::vector<int> a{1}; const auto it=a.begin(); ++it;}',
 'vector-front':'#include <iterator>\n#include <vector>\nint main(){std::vector<int> a; *std::front_inserter(a)=1;}'
}
for name,code in invalid.items():
    source=BUILD/(name+'.cpp'); source.write_text(code,encoding='utf-8')
    result=subprocess.run(['g++','-std=c++17','-pedantic-errors','-fsyntax-only',str(source)],capture_output=True)
    assert result.returncode!=0 and result.stderr,name
    checks+=1
# Compile the actual answer fragment and exhaustively compare small intervals.
solution=(ROOT/'stl/solutions/05-iterators.md').read_text(encoding='utf-8-sig')
fragment=next(b for b in re.findall(r'```cpp\n(.*?)```',solution,re.S) if 'auto current' in b)
source=BUILD/'exercise-7.cpp'
source.write_text('#include <vector>\n#include <algorithm>\n#include <cassert>\nint main(){\nfor(int n=0;n<=8;++n){std::vector<int> source(n); for(int i=0;i<n;++i) source[i]=i; for(int l=0;l<=n;++l) for(int r=l;r<=n;++r){\n'+fragment+'\nstd::vector<int> expected(source.begin()+l,source.begin()+r); std::reverse(expected.begin(),expected.end()); assert(reversed==expected);\n}}}\n',encoding='utf-8')
target=BUILD/('exercise-7'+SUFFIX)
result=subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Wpedantic',str(source),'-o',str(target)],capture_output=True)
assert result.returncode==0 and not result.stderr,result.stderr.decode(errors='replace')
programs['exercise-7']=target
run('exercise-7','','')
for file in [ROOT/'README.md',*(ROOT/'stl').rglob('*.md')]:
    for link in re.findall(r'\]\(([^)]+)\)',file.read_text(encoding='utf-8-sig')):
        if '://' not in link and not link.startswith('#'):
            assert (file.parent/link.split('#',1)[0]).exists(),(file,link)
print(f'PASS: {len(programs)} compiled programs; {checks} checks (including {len(invalid)} expected compilation failures); local links exist')
