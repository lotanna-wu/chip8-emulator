CC      = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c11 -O2
LIBS    = -lSDL2

OUTDIR  = out
TARGET  = $(OUTDIR)/chip8
SRCS    = src/main.c src/chip8.c src/display.c src/input.c
OBJS    = $(SRCS:src/%.c=$(OUTDIR)/%.o)

all: $(OUTDIR) $(TARGET)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

$(OUTDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OUTDIR)

.PHONY: all run clean
