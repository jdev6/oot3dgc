CFLAGS=-O3 -Wall
LIBS=-lSDL2 -lX11

all:
	$(CC) $(CFLAGS) *.c $(LIBS) -o oot3dgc
