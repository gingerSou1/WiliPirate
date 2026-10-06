"""Check direct ELF version needs against the selected Trixie target libraries."""
from pathlib import Path
import json
import re
import subprocess
import sys

binary = Path(sys.argv[1])
runtime = Path('/usr/aarch64-linux-gnu/lib')


def readelf(*args):
    return subprocess.check_output(['aarch64-linux-gnu-readelf', *args], text=True)


header = readelf('-h', str(binary))
assert 'AArch64' in header and 'ELF64' in header and 'little endian' in header
assert 'DYN (Position-Independent Executable file)' in header
program = readelf('-l', str(binary))
assert '/lib/ld-linux-aarch64.so.1' in program
dynamic = readelf('-d', str(binary))
needed = re.findall(r'\(NEEDED\).*\[(.*?)\]', dynamic)
assert set(needed) == {'libstdc++.so.6', 'libgcc_s.so.1', 'libc.so.6'}
versions = readelf('-V', str(binary)).split('Version needs section')[1]
requirements = {}
for chunk in re.split(r'File: ', versions)[1:]:
    library = chunk.split()[0]
    names = re.findall(r'Name: (\S+)', chunk)
    provided = set(re.findall(r'Name: (\S+)', readelf('-V', str(runtime / library)).split('Version needs section')[0]))
    assert set(names) <= provided, (library, set(names) - provided)
    requirements[library] = names
print(json.dumps({'binary': str(binary), 'bytes': binary.stat().st_size,
                  'machine': 'AArch64', 'type': 'ELF64 little-endian PIE',
                  'interpreter': '/lib/ld-linux-aarch64.so.1',
                  'needed': needed, 'version_requirements': requirements,
                  'runtime': str(runtime), 'version_check': 'PASS'}, indent=2))
