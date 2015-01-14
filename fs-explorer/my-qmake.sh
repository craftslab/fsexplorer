#!/bin/bash -x

if [ -d install ]; then
  rm -rf install
fi

if [ -d out ]; then
  rm -rf out
fi
mkdir -p out/rcc out/ui out/moc out/obj

qmake -Wall \
"RCC_DIR = out/rcc" \
"UI_DIR = out/ui" \
"MOC_DIR = out/moc" \
"OBJECTS_DIR = out/obj" \
"INSTALL_PREFIX = install" \
"MACH = 64"
