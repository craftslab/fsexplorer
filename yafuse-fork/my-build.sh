#!/bin/bash -x

pushd out
make
sudo make install
popd

