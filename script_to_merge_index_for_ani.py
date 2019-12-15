import sys
import os
from shutil import copy2, move
import subprocess


filename_start = "Index"

num_partitions = 16

def get_size(filename):
    statinfo = os.stat(filename)
    return statinfo.st_size

src_directory = sys.argv[1]
grants_program = sys.argv[2]

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


procs = []
for i, (partition, parition_size) in enumerate(zip(partitions, partition_sizes)):
    if not parition_size:
        continue
    command = [grants_program] + partition + ["MergedIndex" + str(i)]
    print("Command:", ' '.join(c for c in command))
    procs.append(subprocess.Popen(command))

for p in procs:
    p.wait()
