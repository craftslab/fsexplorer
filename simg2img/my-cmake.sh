#!/bin/bash -x

if [ -d install ]; then
  rm -rf install
fi

if [ ! -d out ]; then
  mkdir out
else
  rm -rf out/*
fi

pushd out

ccmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${PWD}/../install ../

popd
