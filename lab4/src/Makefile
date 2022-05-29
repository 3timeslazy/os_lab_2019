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

parallel_sum : utils.o utils.h
	- rm parallel_sum 2> /dev/null
	$(CC) -lpthread -o parallel_sum utils.o parallel_sum.c $(CFLAGS)

clean :
	- rm utils.o find_min_max.o sequential_min_max parallel_min_max execer 2> /dev/null