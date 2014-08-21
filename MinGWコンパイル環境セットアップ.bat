@echo OFF
title MinGW-gcc
echo MinGW-gccのディレクトリにパスを通します 
rem path C:\Program Files\CodeBlocks\MinGW\bin;%PATH%
path C:\Program Files (x86)\CodeBlocks\MinGW\bin;%PATH%
rem path
set LANG=ja_JP.UTF-8
echo ------- gcc check ------------
gcc -v
rem pause
echo ------------------------------
cmd.exe