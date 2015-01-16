#!/bin/bash -x

export PREFIX=$PWD

if [ -d install ]; then
  rm -rf install
fi

if [ -d out ]; then
  rm -rf out
fi
mkdir -p out/rcc out/ui out/moc out/obj

qmake -Wall \
"RCC_DIR = $PREFIX/out/rcc" \
"UI_DIR = $PREFIX/out/ui" \
"MOC_DIR = $PREFIX/out/moc" \
"OBJECTS_DIR = $PREFIX/out/obj" \
"INSTALL_PREFIX = $PREFIX/install" \
"MACH = 64"