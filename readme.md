# SysY Compiler
A compiler designed for SysY2022.

## Compile and run
cd to the project directory
```shell
mkdir build
cd build
cmake ..
make
```
generate object file
```shell
./compiler test.c
```
This command will generate a object file(.o).

Then use clang to generate the executable file.
```shell
clang-14 output.o ../library/sylib.a -o program
```

## Generate static library
```shell
clang-14 -c sylib.c -o sylib
ar cr sylib.a sylib
```