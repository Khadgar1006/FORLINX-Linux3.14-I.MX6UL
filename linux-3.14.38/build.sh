#!/bin/bash

export CPUS=`grep -c processor /proc/cpuinfo`
export ARCH=arm
export CROSS_COMPILE=/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin/arm-none-linux-gnueabi-
export PATH=/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin:$PATH
make distclean

para=$1
echo "build for $para"
if [ "$1" == "emmc" ]; then
    cp ./linux_imx6ul_emmc_config .config
elif [ "$1" == "nand" ]; then
    cp ./linux_imx6ul_nand_config .config
else
    echo "not support now"
fi

make zImage -j${CPUS}
make dtbs
make modules
