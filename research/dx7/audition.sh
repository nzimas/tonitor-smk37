#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# DX7 research: generation 6 wired into a copy of this tree (build/gen6: fm_dx7.c + gen6.patch; the tree itself is
# left alone), the host tests that matter for it run there, and the audition rendered into research/dx7/wav.
#   research/dx7/audition.sh            (after research/dx7/unpack.py, dx7scan, analyse.py and learn.py: README.md)
set -e
cd "$(dirname "$0")/../.."
mkdir -p build/gen research/dx7/wav/sources
python3 tools/gen_tables.py build/gen/tonitor_tables.h >/dev/null
python3 tools/gen_font.py build/gen/tonitor_font.h >/dev/null
rm -rf build/gen6
mkdir -p build/gen6/firmware
cp -R firmware/src firmware/hal build/gen6/firmware/
cp -R tests build/gen6/
cp research/dx7/fm_dx7.c build/gen6/firmware/src/
patch -s -p1 -d build/gen6 < research/dx7/gen6.patch
I="-Ibuild/gen6/firmware/src -Ibuild/gen6/firmware/hal -Ibuild/gen"
clang -O2 -std=gnu99 -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-unused-function -Werror $I \
    build/gen6/tests/host/host.c -o build/gen6/host
clang -O2 -std=gnu99 -w $I build/gen6/tests/golden/golden.c -o build/gen6/golden
for g in 0 1 2 3 4 5; do
    build/gen6/golden $g | diff -q - tests/golden/gen$g.txt >/dev/null || { echo "FAIL golden: generation $g changed"; exit 1; }
done
echo "golden: generations 0 to 5 as they were, with generation 6 wired in"
clang -O2 -std=gnu99 -w $I research/dx7/audition.c -o build/gen6/audition
SRC=$(python3 -c "
import json; j = json.load(open('build/dx7/learned.json'))
print(' '.join(f'{k}:{i}' for k, v in j.items() for i in v['ids'][:3]))")
build/gen6/audition research/dx7/wav build/dx7/voices155.bin $SRC > research/dx7/wav/list.txt
cat research/dx7/wav/list.txt
