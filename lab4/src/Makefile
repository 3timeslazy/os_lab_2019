CC=gcc
CFLAGS=-I.


parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

process_memory :
	$(CC) -o process_memory process_memory.c $(CFLAGS)

parallel_sum : lib_libsum utils.o utils.h
	$(CC) -o parallel_sum parallel_sum.c parallel_sum.h utils.o -lpthread -lsum -L=./ $(CFLAGS)

clean :
	- rm utils.o find_min_max.o sequential_min_max parallel_min_max parallel_sum libsum.a libsum/sum.o 2> /dev/null

lib_libsum: ./libsum/sum.c
	gcc -o ./libsum/sum.o -c ./libsum/sum.c -I./libsum/
	ar rcs ./libsum.a ./libsum/sum.o