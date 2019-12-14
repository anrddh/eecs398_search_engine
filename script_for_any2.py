import sys
import os
from shutil import copy2, move
import subprocess


filename_start = "pagestore"

num_partitions = 16

def get_size(filename):
    with open(filename, 'rb') as f:
        f.seek(8)
        sizebits = f.read(8)
        size = int.from_bytes(sizebits, 'little')
        return size

src_directory = sys.argv[1]
chandlers_program = sys.argv[2]

onlyfiles = [f for f in os.listdir(src_directory) if os.path.isfile(os.path.join(src_directory, f)) and f[:len(filename_start)] == filename_start]

partitions = [ [] for i in range(num_partitions) ]
partition_sizes = [0] * num_partitions

for f in onlyfiles:
    probably_numbers = f[len(filename_start):]

    try:
        index = int(probably_numbers)
    except ValueError:
        continue

    actual_filename = os.path.join(src_directory, f)

    min_index = partition_sizes.index(min(partition_sizes))

    partitions[min_index].append(actual_filename)
    partition_sizes[min_index] += get_size(actual_filename)

partition_dirs = []

for i, (partition, parition_size) in enumerate(zip(partitions, partition_sizes)):
    if not parition_size:
        continue

    dirname = os.path.join(src_directory, "pagestore_parition" + str(i))
    os.mkdir(dirname)
    for file in partition:
        copy2(file, dirname)

    partition_dirs.append(dirname)

procs = [subprocess.Popen([chandlers_program, pd]) for pd in partition_dirs]

for p in procs:
    p.wait()

for i, pd in enumerate(partition_dirs):
    move(os.path.join(pd, "mergedPS"), os.path.join(src_directory, "mergedPS" + str(i)))


