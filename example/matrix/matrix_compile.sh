../../build/compiler ./matrix.c
clang-14 ./output.o -no-pie -o matrix
rm output.o