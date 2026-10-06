"""Run from any directory with Python 3 and g++ on PATH; no third-party modules."""
from pathlib import Path
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / 'build' / 'lesson-review'
BUILD.mkdir(parents=True, exist_ok=True)
SUFFIX = '.exe' if os.name == 'nt' else ''
count = 0

def compile_source(source, name):
    target = BUILD / (name + SUFFIX)
    result = subprocess.run(['g++', '-std=c++17', '-Wall', '-Wextra', '-Wpedantic',
                             str(source), '-o', str(target)], capture_output=True)
    if result.returncode or result.stderr:
        raise AssertionError(result.stderr.decode(errors='replace'))
    return target

programs = {}
for source in sorted((ROOT / 'stl/examples').glob('0[23]-*.cpp')):
    programs[source.stem] = compile_source(source, source.stem)

def run(name, data='', expected=None, code=0, error=None):
    global count
    result = subprocess.run([str(programs[name])], input=data.encode('utf-8'),
                            capture_output=True, timeout=20)
    stdout = result.stdout.decode('utf-8').replace('\r\n', '\n')
    stderr = result.stderr.decode('utf-8').replace('\r\n', '\n')
    assert result.returncode == code, (name, result.returncode, stderr)
    if expected is not None:
        assert stdout == expected, (name, repr(stdout), repr(expected))
    if error is not None:
        assert stderr == error, (name, stderr, error)
    if code == 0:
        assert not stderr, (name, stderr)
    count += 1
    return stdout

for name in ('02-vector-ops', '03-string-ops'):
    assert run(name).endswith('all checks passed\n')

for values, target in [([],3),([3,3,1],3),([1,2,4],3),([3,3,3],3),
                       ([3,1,2,3,4,3],3),([-1,0,-1],-1),([1],1),([3]*5000,3)]:
    kept = [x for x in values if x != target]
    data = f'{len(values)}\n' + ' '.join(map(str,values)) + f'\n{target}\n'
    run('02-safe-erase', data, f'count={len(kept)}\nkept:' + ''.join(f' {x}' for x in kept)+'\n')
for data, error in [('-1','invalid count'),('5001','invalid count'),
                    ('2\n1','invalid value'),('0','invalid target')]:
    run('02-safe-erase',data,'',1,error+'\n')
for matrix in [[[1,2,3],[4,5,6]],[[9]],[[-1,2,-3]],[[1],[2],[3]],
               [[2147483647,2147483647],[-2147483648,-2147483648]]]:
    r,c=len(matrix),len(matrix[0])
    data=f'{r} {c}\n'+'\n'.join(' '.join(map(str,row)) for row in matrix)
    expected='row sums:'+''.join(f' {sum(row)}' for row in matrix)+'\ntranspose:\n'
    expected+=''.join(' '.join(str(matrix[i][j]) for i in range(r))+'\n' for j in range(c))
    run('02-matrix',data,expected)
for data,error in [('0 2','invalid dimensions'),('501 1','invalid dimensions'),
                   ('1 -1','invalid dimensions'),('1 2\n4','invalid matrix element')]:
    run('02-matrix',data,'',1,error+'\n')

run('03-line-input','3\nAlice Smith\n\n  Bob\n',
    'line 1: [Alice Smith], size=11\nline 2: [], size=0\nline 3: [  Bob], size=5\n')
run('03-line-input','1   \nlast','line 1: [last], size=4\n')
run('03-line-input','1\n\n','line 1: [], size=0\n')
run('03-line-input','0','')
run('03-line-input','1\n中文','line 1: [中文], size=6\n')
for data,error in [('-1','invalid count'),('10001','invalid count'),
                   ('abc','invalid count'),('1\n','missing line 1'),('2\nx\n','missing line 2')]:
    run('03-line-input',data,'',1,error+'\n')

for data,expected in [('', 'count=0\n'),(' \t\n # comment\n','count=0\n'),
 (' name = Alice Smith \ncity=中文\nexpr=a=b\nempty=\nname=Bob',
  'count=5\nname=Alice Smith\ncity=中文\nexpr=a=b\nempty=\nname=Bob\n'),
 ('x=a#b\n_9= ok\n','count=2\nx=a#b\n_9=ok\n'),
 ('x=1\r\ny=2\r\n','count=2\nx=1\ny=2\n')]:
    run('03-text-parser',data,expected)
for data,error in [('broken','line 1: missing \'=\''),('=x','line 1: invalid key'),
 ('1name=x','line 1: invalid key'),('good=1\nbad key=2','line 2: invalid key')]:
    run('03-text-parser',data,'',1,error+'\n')
# Exact input limits and one-over boundaries, including delayed output on error.
run('03-text-parser','\n'*10000,'count=0\n')
run('03-text-parser','\n'*10001,'',1,'line 10001: input limit exceeded\n')
run('03-text-parser','#'+'x'*99999,'count=0\n')
run('03-text-parser','#'+'x'*100000,'',1,'line 1: input limit exceeded\n')
limit_input=('#'+'x'*99999+'\n')*10
run('03-text-parser',limit_input,'count=0\n')
run('03-text-parser',limit_input+'x','',1,'line 11: input limit exceeded\n')

# The two complete answer programs must compile as printed, not merely look plausible.
answers=(ROOT/'stl/solutions/03-string.md').read_text(encoding='utf-8-sig')
blocks=[b for b in re.findall(r'```cpp\n(.*?)```',answers,re.S) if 'int main()' in b]
assert len(blocks)==2
for index,block in enumerate(blocks,1):
    name=f'03-answer-{index}'
    source=BUILD/(name+'.cpp')
    source.write_text(block,encoding='utf-8')
    programs[name]=compile_source(source,name)
    run(name,'','')

# Check repository-local Markdown links in the learning materials.
for file in [ROOT/'README.md', *(ROOT/'stl').rglob('*.md')]:
    for link in re.findall(r'\]\(([^)]+)\)',re.sub(r'```[\s\S]*?```|`[^`\n]*`', '', file.read_text(encoding='utf-8-sig'))):
        if '://' in link or link.startswith('#'):
            continue
        assert (file.parent/link.split('#',1)[0]).exists(), (file,link)
print(f'PASS: {len(programs)} compiled programs, {count} executions; local links exist')
