#!/bin/bash -x

if [ ! -d out ]; then
    mkdir ./out
fi

if [ ! -d install ]; then
    mkdir ./install
fi

pushd out
cmake.exe -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install ../
popd

