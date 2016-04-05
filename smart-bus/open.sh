# !/bin/bash
echo "======================================================================="
echo "$(basename $0) executing start"
date
# vim $(pwd)/include/*.h $(pwd)/src/os/os_*.h $(pwd)/src/os/os_*.cpp $(pwd)/src/app/smb/main/main.cpp
#  find . -type f -name "*.cpp" -exec vi {} \;
vim $(find . -type f -name "*.cpp" -o -name "*.h")
echo "$(basename $0) executing done"
echo "======================================================================="
