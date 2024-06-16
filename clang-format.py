import subprocess
import glob
from pathlib import Path
# Format script

def fmt_file(path):
        if Path(path).is_dir():
             return # Skip
        try:
            subprocess.run(['clang-format', '-i', path, '--style=Google'], check=True)
        except subprocess.CalledProcessError as e:
            print('Error for ' + Path(path).stem + ': ' + e)

def fmt_dir(path):
    sources = glob.glob(path+'/*')
    for file in sources:
        fmt_file(file)

print('Formatting...')
fmt_dir('source')
fmt_dir('include')
fmt_dir('include/renderd7')

# Format LE and TF as well
fmt_dir('rd7tf/source')
#fmt_dir('rd7le/source')
print('Done')