# !bin/bash
date
echo "=================================================================================="
echo "start compiling smart-bus..."

make distclean

aclocal

autoconf

automake --add-missing

./configure --host=arm-linux-gnueabihf --build=i686-pc-linux-gnu src_path=`pwd` --with-board --with-print --with-rtlib 

make clean all

echo "compiling smart-bus done!"
echo "=================================================================================="
