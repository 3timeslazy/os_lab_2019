cd ../revert_string
./link_dynamically.sh
cd ../tests

mkdir bin
gcc -c ./tests.c -I ../revert_string -o bin/tests.o
gcc bin/tests.o -o bin/tests -L ../revert_string/bin/shared -l revert -l cunit
LD_LIBRARY_PATH=$(pwd)/../revert_string/bin/shared bin/tests

# rm -rf bin