CFLAGS  = -std=c99 -g -Wall -Wextra
LDFLAGS = -lncurses

BIN  = life
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) $(BIN) $(OBJS)
