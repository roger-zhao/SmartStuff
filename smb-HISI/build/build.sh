# !/bin/bash
date
echo "================================================================"
echo "exe start"
cmake -Dboard=OFF ..
make clean all
echo "exe end"
echo "================================================================"
