#!/bin/sh
export CPUS=`grep -c processor /proc/cpuinfo`
export ARCH=arm
export CROSS_COMPILE=/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin/arm-none-linux-gnueabi-
export PATH=/opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin:$PATH
make distclean

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

rm drivers/bluetooth/rtk_bt/rtk_btusb.c
rm drivers/media/platform/mxc/subdev/ov9650.c
mv drivers/bluetooth/rtk_bt/rtk_btusb.o  drivers/bluetooth/rtk_bt/rtk_btusb.mod
mv drivers/media/platform/mxc/subdev/ov9650.o drivers/media/platform/mxc/subdev/ov9650.mod
cp non-open/Makefile-rtk_btusb  drivers/bluetooth/rtk_bt/Makefile
cp non-open/Makefile-ov9650 drivers/media/platform/mxc/subdev/Makefile
rm -r non-open

