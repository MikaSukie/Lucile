CC      := clang
CFLAGS  := -std=c17 -Wall -Wextra -O2
LDFLAGS :=

SRCS    := lucile.c
OBJS    := $(SRCS:.c=.o)
TARGET  := lucile

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c lucile.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) *.ll

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g -O0
debug: clean $(TARGET)

release: CFLAGS += -O3 -DNDEBUG
release: clean $(TARGET)

.PHONY: all clean run debug release
