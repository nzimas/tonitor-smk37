#!/usr/bin/env python3
"""DX7 32-voice bulk (packed VMEM, 128 bytes a voice) -> the engine's 155-byte single-voice layout (fm6_core.c FP_*),
de-duplicated by the first 118 bytes (the voice without its name).

    python3 research/dx7/unpack.py dx7-presets.bin build/dx7/voices155.bin
writes the unique voices one after another (155 bytes each) and, beside them, index.txt: the bank position of each."""
import sys

def unpack(v):
    out = bytearray(155)
    for k in range(6):                       # packed op k = the sixth operator first, as the unpacked layout
        s, d = v[k * 17:k * 17 + 17], k * 21
        out[d:d + 8] = s[0:8]                # R1..R4, L1..L4
        out[d + 8] = s[8]                    # BP
        out[d + 9] = s[9]                    # LD
        out[d + 10] = s[10]                  # RD
        out[d + 11] = s[11] & 3              # LC
        out[d + 12] = (s[11] >> 2) & 3       # RC
        out[d + 13] = s[12] & 7              # RS
        out[d + 14] = s[13] & 3              # AMS
        out[d + 15] = (s[13] >> 2) & 7       # KVS
        out[d + 16] = s[14]                  # OL
        out[d + 17] = s[15] & 1              # MODE
        out[d + 18] = (s[15] >> 1) & 31      # FC
        out[d + 19] = s[16]                  # FF
        out[d + 20] = (s[12] >> 3) & 15      # DET
    out[126:134] = v[102:110]                # PR1..4, PL1..4
    out[134] = v[110] & 31                   # ALG
    out[135] = v[111] & 7                    # FB
    out[136] = (v[111] >> 3) & 1             # OKS
    out[137:141] = v[112:116]                # LFS LFD LPMD LAMD
    out[141] = v[116] & 1                    # LKS
    out[142] = (v[116] >> 1) & 7             # LFW
    out[143] = (v[116] >> 4) & 7             # LPMS
    out[144] = v[117]                        # TRNSP
    out[145:155] = bytes(c if 32 <= c < 127 else 32 for c in v[118:128])
    return bytes(out)

def main():
    data = open(sys.argv[1], 'rb').read()
    seen, keep, where = set(), [], []
    for i in range(len(data) // 128):
        v = data[i * 128:(i + 1) * 128]
        key = v[:118]
        if key in seen:
            continue
        seen.add(key)
        keep.append(unpack(v))
        where.append(i)
    open(sys.argv[2], 'wb').write(b''.join(keep))
    with open(sys.argv[2].rsplit('/', 1)[0] + '/index.txt', 'w') as f:
        f.write('\n'.join(map(str, where)) + '\n')
    print(f'{len(data) // 128} voices, {len(keep)} unique')

main()
