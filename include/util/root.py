# TODO make this maybe not hot garbage

import os
import time

from math import sqrt, log, floor

start_time = time.time()
# compute a table for 1024 ranges, from 0 to 2^64,
top = 2 ** 63
num_slots = 2 ** 10

num_samples = 2**7 # samples to take per slot

# Slots need to be defined logarthmitically, otherwise most of the number we end up
# needing to deal with land in the first slot alone
slot_top = top
edges = []
divisor = num_slots ** (log(top, num_slots) / num_slots)


print(f"Calculating with (approximately) {num_slots} slots, \
a logarithmic step with a {round(divisor, 5)} divisor, \
and (at most) {num_samples} samples per slot.")


for i in range(0, num_slots):
    edges.append(floor(slot_top))
    slot_top = slot_top / divisor

edges = list(set(edges))
edges.sort()


# there is probably some linear approximations tricks we could use this make this more efficient,
# but just let the computer crunch the numbers for us

__location__ = os.path.realpath(
    os.path.join(os.getcwd(), os.path.dirname(__file__)))
root_file = open(os.path.join(__location__, "roots.h"), "w")
root_file.write("#include<array>\n\n")
root_file.write("struct Range {\n")
root_file.write("  int64_t min;\n")
root_file.write("  int64_t max;\n")
root_file.write("  double avg;\n")
root_file.write("};\n\n")

start = 0
i = 0
for e in edges:
    end = e

    roots = []

    if end - start < num_samples:
        step = 1
    else:
        step = floor( (end - start) / num_samples)

    for j in range(start, end + 1, step):
        roots.append(sqrt(j))

    avg = sum(roots) / len(roots)

    # hack, fixme
    if end == top:
        end = end - 1

    root_file.write(f"constexpr Range r{i}{{{start}, {end}, {avg}}};\n")

    roots.clear()

    start = end + 1
    i = i + 1


root_file.write("\n\n");
root_file.write(f"constexpr std::array<Range, {i}> ranges = {{")
for j in range(0, i):
    if j == i - 1:
        root_file.write(f" r{j}")
    else:
        root_file.write(f" r{j},")
    if j % 10 == 0 and j > 0:
        root_file.write("\n")

root_file.write("};")
root_file.close()

end_time = time.time()
print(f"Done! Calculating root samples took {round(1000 * (end_time - start_time), 3)}ms")
