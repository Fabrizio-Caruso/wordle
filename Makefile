CC=x86_64-w64-mingw32-gcc
CFLAGS=-I. 

wordle: wordle.c
	$(CC) $(CFLAGS) -o wordle.exe wordle.c

test: test.c
	$(CC) test.c -o test   -I../CSFML-2.5.1-windows-64-bit/include/  -L../CSFML-2.5.1-windows-64-bit/lib/gcc/ -lcsfml-system -lcsfml-graphics -lcsfml-window  
