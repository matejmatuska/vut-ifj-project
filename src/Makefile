CFLAGS= -std=c99 -Wall -Wextra -pedantic -g

BIN=proj

.PHONY: all
all : $(BIN)

.PHONY: run
run: $(BIN)
	./$(BIN)

$(BIN): main.c scanner.o parser.o dynamic_string.o symtable.o ST_stack.o expr_parser.o symbol_stack.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	-rm -f $(BIN) *.o
