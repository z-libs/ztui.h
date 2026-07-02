CC      ?= cc
CFLAGS  ?= -Wall -Wextra -pedantic -std=c11
LDFLAGS ?=

HEADER   = ztui.h
EXAMPLES = minimal forms table_dialog dashboard editor tinyvim
TEST     = test_reduced

.SUFFIXES:
.PHONY: all examples test clean

all: examples test

examples: $(EXAMPLES:%=examples/%)

test: $(TEST)
	./$(TEST)

examples/%: examples/%.c $(HEADER)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

$(TEST): test.c $(HEADER)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f $(TEST)
	$(RM) $(EXAMPLES:%=examples/%)
