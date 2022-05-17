mkdir -p bin/shared
gcc -c main.c -o bin/main.o
gcc -c -fPIC revert_string.c -o bin/shared/revert_string.o
gcc -shared bin/shared/revert_string.o -o bin/shared/librevert.so
gcc  bin/main.o -L bin/shared -l revert -o bin/dynamically-linked
# RUN: LD_LIBRARY_PATH=$(pwd)/bin/shared ./bin/dynamically-linked