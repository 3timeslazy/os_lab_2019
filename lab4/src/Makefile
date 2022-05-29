CC=gcc
CFLAGS=-I.


build_parallel_min_max : 
	$(CC) -o parallel_min_max.out ./parallel_min_max/*.c $(CFLAGS)

build_process_memory :
	$(CC) -o process_memory process_memory.c $(CFLAGS)

clean :
	- rm utils.o find_min_max.o sequential_min_max parallel_min_max execer 2> /dev/null