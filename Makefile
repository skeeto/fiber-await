.POSIX:
CC      = x86_64-w64-mingw32-gcc
CFLAGS  = -std=c99 -pedantic -Wall -Wextra -O3
LDFLAGS =
LDLIBS  =

demo.exe: demo.c async.c
	$(CC) $(CFLAGS) -o $@ demo.c async.c

check: demo.exe
	wine64 ./demo.exe

clean:
	rm -f demo.exe
