CFLAG = -g -Wall -Wextra -lm

build: bmp.o
	gcc $(CFLAG) bmp.o bmp_header.h -o bmp

bmp.o:
	gcc -c $(CFLAG) bmp.c -o bmp.o

run:
	./bmp

.PHONY: clean

clean:
	rm -f *.o bmp
