# !/bin/bash
date
echo "================================================================"
echo "exe start"
cmake -Dboard=ON -DCMAKE_TOOLCHAIN_FILE=../CMakeScripts/arm-hisiv100nptl.toolchain.cmake -DCMAKE_INSTALL_PREFIX=/home/roger/HiSDK/Hi3520D_SDK_V1.0.2.2/osdrv/pub/rootfs_glibc/yanjie ..
make clean all
make install/strip
echo "exe end"
echo "================================================================"
