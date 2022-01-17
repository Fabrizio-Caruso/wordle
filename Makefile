CC=x86_64-w64-mingw32-gcc
CFLAGS=-I. 

wordle: wordle.c common.c
	$(CC) $(CFLAGS) -o wordle.exe wordle.c common.c

test: test.c
	$(CC) test.c -o test   -I../CSFML-2.5.1-windows-32-bit/include/  -L../CSFML-2.5.1-windows-32-bit/lib/gcc/ -lmingw32 -lcsfml-system -lcsfml-graphics -lcsfml-window  
