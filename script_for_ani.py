import sys
import os
from shutil import copy2

filename_start = "pagestore"

begin = int(sys.argv[1])
end = int(sys.argv[2])
src_directory = sys.argv[3]
dest_directory = sys.argv[4]

onlyfiles = [f for f in os.listdir(src_directory) if os.path.isfile(os.path.join(src_directory, f)) and f[:len(filename_start)] == filename_start]
for f in onlyfiles:
    probably_numbers = f[len(filename_start):]

    try:
        index = int(probably_numbers)
    except ValueError:
        continue

    if index >= begin and index < end:
        actual_file = os.path.join(src_directory, f)
        print("Copying ", actual_file)
        copy2(actual_file, dest_directory)
