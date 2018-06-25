#!/bin/bash

export CPUS=`grep -c processor /proc/cpuinfo`
export ARCH=arm
export CROSS_COMPILE=/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin/arm-none-linux-gnueabi-
export PATH=/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin:$PATH


if [ "$1" == "zImage" ]; then
	cp ./linux_imx6ul_emmc_config .config
    make zImage -j${CPUS}
elif [ "$1" == "dtbs" ]; then
	cp ./linux_imx6ul_emmc_config .config
	make dtbs
elif [ "$1" == "modules" ]; then
	cp ./linux_imx6ul_emmc_config .config
	make modules
elif [ "$1" == "all" ]; then
	make distclean
	echo "build emmc"
    cp ./linux_imx6ul_emmc_config .config
    make zImage -j${CPUS}
	make dtbs
	make modules
else
	echo "parameter error"
fi
