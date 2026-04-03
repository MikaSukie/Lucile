CXX      := clang++
CXXFLAGS := -std=c++23 -O2 -Wall -Wextra -Wpedantic -fcolor-diagnostics
LDFLAGS  :=

# Debug build: make DEBUG=1
ifdef DEBUG
  CXXFLAGS += -g -O0 -fsanitize=address,undefined -DDEBUG
  LDFLAGS  += -fsanitize=address,undefined
endif

SRCDIR := src
SRCS   := $(SRCDIR)/lexer.cpp     \
           $(SRCDIR)/parser.cpp    \
           $(SRCDIR)/types.cpp     \
           $(SRCDIR)/sema.cpp      \
           $(SRCDIR)/codegen.cpp   \
           $(SRCDIR)/codegen2.cpp  \
           $(SRCDIR)/main.cpp

TARGET := lucilec

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@ $(LDFLAGS)
	@echo "Built: $@"

# Run the bundled test suite
test: all
	@echo "=== Running tests ==="
	@$(MAKE) -C tests run COMPILER=../$(TARGET) --no-print-directory

clean:
	rm -f $(TARGET)
	rm -f tests/*.ll tests/*.out
