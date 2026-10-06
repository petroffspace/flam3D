#!/bin/sh
# Build flam3 in ./flam3 using the libxml2/libjpeg development headers in
# ./deps (extracted from the Ubuntu libxml2-dev and libjpeg-turbo8-dev
# packages; ICU support is disabled in deps/include/libxml2/libxml/xmlversion.h).
# The binaries link against the system runtime libraries libxml2.so.2,
# libjpeg.so.8 and libpng16.so.16. Nothing is installed system-wide;
# "make install" would go to ./flam3/local.
set -e

TOP=$(cd "$(dirname "$0")" && pwd)
DEPS=$TOP/deps
cd "$TOP/flam3"

# A git checkout gives the autotools files arbitrary timestamps, which makes
# make try to regenerate them (needs aclocal-1.15).  Mark them up to date.
touch aclocal.m4; sleep 1
touch configure Makefile.in config.h.in; sleep 1

./configure --prefix="$TOP/flam3/local" \
   CPPFLAGS="-I$DEPS/include -I$DEPS/include/libxml2 -I$DEPS/include/x86_64-linux-gnu" \
   LDFLAGS="-L$DEPS/lib" > configure.log
# Header dependencies are not tracked, so always rebuild everything
make clean > /dev/null
make -j"$(nproc)" > make.log

echo "built: flam3-render flam3-animate flam3-genome flam3-convert in $TOP/flam3"
