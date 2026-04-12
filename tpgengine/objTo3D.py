import struct
import sys
def convert(obj_path, out_path):
    verts = []
    tris = []

    with open(obj_path, 'r') as f:
        for line in f:
            parts = line.split()
            if not parts:
                continue
            if parts[0] == 'v':
                x, y, z = float(parts[1]), float(parts[2]), float(parts[3])
                verts.append((x, z, -y))  # swap Y and Z
            elif parts[0] == 'f':
                # OBJ indices are 1-based
                i1 = int(parts[1].split('/')[0]) - 1
                i2 = int(parts[2].split('/')[0]) - 1
                i3 = int(parts[3].split('/')[0]) - 1
                tris.append((verts[i1], verts[i2], verts[i3]))

    with open(out_path, 'wb') as f:
        f.write(struct.pack('I', len(tris)))
        for t in tris:
            for v in t:
                f.write(struct.pack('fff', v[0], v[1], v[2]))

if len(sys.argv) != 3:
    print("stupid")
    sys.exit(1)

convert(sys.argv[1], sys.argv[2])