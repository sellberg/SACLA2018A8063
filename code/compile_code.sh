#!/bin/bash
g++ average_SACLA.cpp -o av.out
echo "compiled average_SACLA.cpp --> av.out"
g++ integrate_SACLA.cpp -o integ.out
echo "compiled integrate_SACLA.cpp --> integ.out"
g++ data_process.cpp -o data_process.out
echo "compiled data_process.cpp --> data_process.out"
