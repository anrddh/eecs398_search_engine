import sys
import os
from shutil import copy2, move
import subprocess

dirname_start = "pagestore_parition"

begin = int(sys.argv[1])
end = int(sys.argv[2])
pagestore_directory = sys.argv[3]
chandlers_program = sys.argv[4]

onlydirs = [f for f in os.listdir(pagestore_directory) if os.path.isdir(os.path.join(pagestore_directory, f)) and f[:len(dirname_start)] == dirname_start]

procs = []
actual_dirs = []
for d in onlydirs:
    probably_numbers = d[len(dirname_start):]

    try:
        index = int(probably_numbers)
    except ValueError:
        continue

    if index >= begin and index < end:
        actual_dir = os.path.join(pagestore_directory, d)
        actual_dirs.append(actual_dir)
        procs.append(subprocess.Popen([chandlers_program, actual_dir]))

for p in procs:
    p.wait()

for i, pd in enumerate(actual_dirs):
    move(os.path.join(pd, "mergedPS"), os.path.join(pagestore_directory, "mergedPS" + str(i)))

