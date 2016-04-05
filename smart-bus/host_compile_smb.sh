# !bin/zsh
date
echo "=================================================================================="
echo "start compiling smart-bus..."

make distclean

aclocal

autoconf

automake --add-missing

./configure --prefix=/home/roger/SMB/Dev/Code/Demo/Yanjie/smart-bus/release src_path=`pwd` --with-print  --with-rtlib --with-assert

make clean all

# make install-strip

echo "compiling smart-bus done!"
echo "=================================================================================="
