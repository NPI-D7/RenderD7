import os
import glob
import shutil
from pathlib import Path

# Simple Script to generate/update Assets

def file2array(path, custom_incluse_path):
    print(path)
    cip = len(custom_incluse_path)
    sip = ''
    if cip > 0:
        sip = custom_incluse_path
    name = Path(path).stem
    filei = open(path, 'rb')
    buf = filei.read()
    filei.close()
    fs = open(name + '.cpp', 'w')
    fs.write('#include <'+ sip + name + '.hpp>\n\n')
    fs.write('// clang-format off\n')
    fs.write('unsigned char ' + name + '[] = {\n')
    for byte in buf:
        fs.write(hex(byte) + ', ')
    fs.write('\n};\n')
    fs.write('// clang-format on\n')
    fs.write('size_t ' + name + '_size = ' + hex(len(buf)) + ';')
    fs.close()
    fh = open(name + '.hpp', 'w')
    fh.write('#pragma once\n\n')
    fh.write('#include <cstddef>\n\n')
    fh.write('extern unsigned char ' + name + '[];\n')
    fh.write('extern size_t ' + name + '_size;')
    fh.close()

def tex3ds_build(path):
    p = os.path.dirname(path)
    n = Path(path).stem
    os.system('tex3ds -o ' + p + '/' + n + '.t3x -i ' + path)

def cleanup():
    t3x = glob.glob('assets/resources/*.t3x')
    for f in t3x:
        os.remove(f)

def install_code(what, where):
    if Path(what).is_dir:
        os.error('Must be a file!!!')
    shutil.move(what, where + Path(what).name)

print("Generating...")
t3xtg = glob.glob('assets/resources/*.t3s')
for object in t3xtg:
    name = Path(object).stem
    bp = os.path.dirname(object)
    tex3ds_build(object)
    file2array(bp + '/' + name + '.t3x', 'renderd7/')
    install_code(name + '.cpp', 'source/')
    install_code(name + '.hpp', 'include/renderd7/')

cleanup()
print("Done")