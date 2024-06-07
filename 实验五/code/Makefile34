CC=gcc  
CCFLAGS=-fPIC -c -Wall  
LDFLAGS=-shared -Wl,-soname,libmalloc.so

all: libmallocff.so test  
.PHONY: all

libmallocff.so: ff_malloc.c  
	$(CC) $(CCFLAGS) ff_malloc.c  
	$(CC) $(LDFLAGS) -o libmalloc.so

libmallocbf.so: bf_malloc.c  
	$(CC) $(CCFLAGS) bf_malloc.c  
	$(CC) $(LDFLAGS) -o libmalloc.so

test: test.c  
	gcc -o test test.c
clean:  
	rm -f *.o test libmalloc.so  
.PHONY: clean  