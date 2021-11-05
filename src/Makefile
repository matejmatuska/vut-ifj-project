CFLAGS= -std=c99 -Wall -Wextra -pedantic

BIN=proj

.PHONY: run
run: $(BIN)
	./$(BIN)

$(BIN): main.c scanner.o parser.o dynamic_string.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	-rm -f $(BIN) *.o
