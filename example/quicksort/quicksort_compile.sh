../../build/compiler ./quicksort.c
clang-14 ./output.o -no-pie -o quicksort
rm output.o