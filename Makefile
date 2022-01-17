CC=x86_64-w64-mingw32-gcc
CFLAGS=-I. 

wordle.o: wordle.c
	$(CC) $(CFLAGS) -c wordle.c

wordle_ncurses.o: wordle_ncurses.c
	$(CC) $(CFLAGS) -c wordle_ncurses.c

common.o: common.c
	$(CC) $(CFLAGS) -c common.c

wordle: wordle.o common.o
	$(CC) $(CFLAGS) -o wordle.exe wordle.o common.o

wordle_ncurses: wordle_ncurses.o common.o
	$(CC) $(CFLAGS) -o wordle.exe wordle.o common.o


test: test.c
	$(CC) test.c -o test   -I../CSFML-2.5.1-windows-32-bit/include/  -L../CSFML-2.5.1-windows-32-bit/lib/gcc/ -lmingw32 -lcsfml-system -lcsfml-graphics -lcsfml-window  


clean:
	rm -f *.o
	rm *.exe

