OUT = gamesprog
UNAME := $(shell uname -s)

SRC = $(filter-out tests.c, $(wildcard *.c))
TEST_SRC = $(filter-out gamesprog.c, $(wildcard *.c))

ifeq ($(UNAME),Darwin)
	INCDIR = -I/Library/Frameworks/SDL.framework/Headers/ -I/Library/Frameworks/SDL_image.framework/Headers/
	CFLAGS = -Wall -Wno-overflow -Wno-uninitialized -Wno-objc-protocol-method-implementation -pedantic -std=c99 -O0 $(INCDIR)
	LFLAGS = -framework SDL -framework SDL_image -framework Cocoa -lobjc
	OBJS = $(SRC:.c=.o) SDLMain.o
else
	CC = gcc
	CFLAGS = -Wall -g -lm -std=c99 -pedantic `sdl-config --cflags` -Isdl/linux/sdl_image/include/SDL
	LFLAGS = `sdl-config --libs` -Wl,-rpath,sdl/linux/sdl_image/lib -Lsdl/linux/sdl_image/lib/ -lSDL_image
	OBJS = $(SRC:.c=.o)
endif

all: $(OUT)
# %: %.c

$(OUT): $(OBJS)
	$(CC) $^ $(LFLAGS) -g -o $@

# here's how to make a .o file from a .c file
%.o: %.c
		$(CC) $(OPT) $(CFLAGS) $^ -c -g -o $@

# osx specific
SDLMain.o: .FORCE
		$(CC) $(OPT) $(CFLAGS) -c -g sdl/osx/SDLmain.m

clean:
		rm -f $(OUT) *.o

.FORCE:
.PHONY: all clean .FORCE
