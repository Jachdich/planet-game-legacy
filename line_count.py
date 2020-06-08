import os
ending = []
def endswith(s, end):
    for i in end:
        if s.endswith(i):
            return True
    return False

def get_subdirs(a_dir):
    out = []
    for name in os.listdir(a_dir):
        #print(os.path.join(a_dir, name))
        if os.path.isdir(os.path.join(a_dir, name)):
            if name in ("client", "server"):
                out.extend(get_subdirs(os.path.join(a_dir, name)))
        else:
            if os.path.exists(os.path.join(a_dir, ".ignore")):
                print(a_dir)
                continue
            
            if endswith(name, [".cpp", ".h"]):
                out.append(os.path.join(a_dir, name))
            else:
                if endswith(name, [".css",]):
                    print(a_dir)
                if not name.split(".")[-1] in ending and "." in name:
                    ending.append(name.split(".")[-1])
    return out

line_count = 0
whitespace = 0
comment    = 0
total = ""
for i in get_subdirs("."):
    print(i)
    if "FastNoise" in i or "olcPixelGameEngine" in i:
        continue
    with open(i, "r") as f:
        for line in f.read().split("\n"):
            total += line + "\n"
            if len(line.strip("\t ")) > 0 and not line.strip("\t ").startswith("//"):
                line_count += 1
            elif line.strip("\t ").startswith("//"):
                comment += 1
            else:
                whitespace += 1

#for i in ending:
#    print(i)
print(line_count)
print(whitespace)
print(comment)
print(line_count + comment + whitespace)

with open("total.txt", "w") as f:
    f.write(total)
