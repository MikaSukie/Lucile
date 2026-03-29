CC      = clang
CFLAGS  = -std=c11 -Wall -Wextra -g
SRCS    = lucile.c
OBJS    = $(SRCS:.c=.o)
TARGET  = lucile

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c lucile.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET) *.ll

.PHONY: all clean
