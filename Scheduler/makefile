CC=gcc
CFLAGS=-std=c11 -Wall -g `pkg-config --cflags gtk+-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0`
SRC=src/scheduler.c src/algorithms.c src/metrics.c src/gui_gtk.c src/report.c
OBJ=$(SRC:.c=.o)

all: build/bin/scheduler

build/bin/scheduler: $(SRC)
mkdir -p build/bin
$(CC) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)

clean:
rm -rf build *.o

.PHONY: all clean








