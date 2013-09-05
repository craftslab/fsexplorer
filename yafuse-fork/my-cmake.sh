#!/bin/bash -x

if [ ! -d out ]; then
    mkdir ./out
fi

if [ ! -d install ]; then
    mkdir ./install
fi

pushd out
ccmake -DCMAKE_INSTALL_PREFIX=/home/lemonjia/my-project/fricando/yafuse-fork-test/install ../ 
popd

