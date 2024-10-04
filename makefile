sum: threaded_sum.o
	cc threaded_sum.o -o sum

threaded_sum.o: threaded_sum.c
	cc -pthread -c threaded_sum.c -o threaded_sum.o

clean:
	rm -f a.out sum threaded_sum.o
