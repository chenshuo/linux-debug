#!/bin/bash

make O=../build-g defconfig
cd ../build-g
cp .config .config.bak
make kvm_guest.config
make study.config

echo "To build:"
echo "    cd ../build-g"
echo "    make -j8"
