"""C++17 lesson 04 checks. Requires Python 3 and g++ on PATH."""
from pathlib import Path
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT/'build/lesson-04'
BUILD.mkdir(parents=True, exist_ok=True)
SUFFIX = '.exe' if os.name == 'nt' else ''
programs = {}
checks = 0
for source in sorted((ROOT/'stl/examples').glob('04-*.cpp')):
    target = BUILD/(source.stem+SUFFIX)
    result = subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Wpedantic',str(source),'-o',str(target)],capture_output=True)
    assert result.returncode == 0 and not result.stderr, result.stderr.decode(errors='replace')
    programs[source.stem] = target

def run(name, data, expected, code=0, error=''):
    global checks
    result = subprocess.run([str(programs[name])],input=data.encode('utf-8'),capture_output=True,timeout=20)
    out = result.stdout.decode('utf-8').replace('\r\n','\n')
    err = result.stderr.decode('utf-8').replace('\r\n','\n')
    assert (result.returncode,out,err) == (code,expected,error), (name,result.returncode,out,err)
    checks += 1

run('04-reference-auto','','reference and auto checks passed\n')
run('04-record-types','','record and binding checks passed\n')
for records,bonus in [([],0),([('Alice',98),('Bob',55),('Carol',20)],5),
                     ([('A',0),('B',99)],-100),([('A',0),('B',99)],100),
                     ([('same',59),('same',60),('中文',100)],0),
                     ([('A',60),('B',61)],-1),([('x'*100,100)],0),
                     ([('S',100)]*100000,0)]:
    adjusted=[(name,max(0,min(100,score+bonus))) for name,score in records]
    data=f'{len(records)} {bonus}\n'+''.join(f'{name} {score}\n' for name,score in records)
    expected=f'count={len(records)}\npassed={sum(score>=60 for _,score in adjusted)}\ntotal={sum(score for _,score in adjusted)}\n'
    expected+=''.join(f'{name} {score}\n' for name,score in adjusted)
    run('04-student-records',data,expected)
for data in ['', '-1 0','100001 0','1 -101','1 101','x 0']:
    run('04-student-records',data,'',1,'invalid header\n')
for data,line in [('1 0\nA -1',1),('1 0\nA 101',1),('1 0\nA',1),
                  ('1 0\n'+'x'*101+' 10',1),('2 0\nA 50\nB nope',2)]:
    run('04-student-records',data,'',1,f'invalid student {line}\n')
# Compiler rejection is the expected result; do not run undefined behavior.
invalid = {
 'const-write': 'int main(){int x=1; const int& r=x; r=2;}',
 'tuple-arity': '#include <tuple>\nint main(){auto [a,b]=std::tuple<int,int,int>{1,2,3};}',
 'vector-binding': '#include <vector>\nint main(){auto [a,b]=std::vector<int>{1,2};}',
 'const-binding': '#include <utility>\nint main(){std::pair<int,int> p{1,2}; const auto& [a,b]=p; a=3;}'
}
for name,code in invalid.items():
    source=BUILD/(name+'.cpp')
    source.write_text(code,encoding='utf-8')
    result=subprocess.run(['g++','-std=c++17','-pedantic-errors','-fsyntax-only',str(source)],capture_output=True)
    assert result.returncode != 0 and result.stderr, name
    checks += 1
for file in [ROOT/'README.md',*(ROOT/'stl').rglob('*.md')]:
    for link in re.findall(r'\]\(([^)]+)\)',re.sub(r'```[\s\S]*?```|`[^`\n]*`', '', file.read_text(encoding='utf-8-sig'))):
        if '://' not in link and not link.startswith('#'):
            assert (file.parent/link.split('#',1)[0]).exists(), (file,link)
print(f'PASS: {len(programs)} programs; {checks} checks (including {len(invalid)} expected compilation failures); local links exist')
