mkdir -p bin/static
gcc -c main.c -o bin/main.o
gcc -c revert_string.c -o bin/static/revert_string.o
ar rcs bin/static/librevert.a bin/static/revert_string.o
gcc bin/main.o -Lbin/static -l revert -o bin/statically-linked
# RUN: ./bin/statically-linked