CFLAGS= -std=c99 -Wall -Wextra -pedantic -g

ARCHIVE=xpokor85.zip
BIN=proj

.PHONY: all
all: $(BIN)

.PHONY: run
run: $(BIN)
	./$(BIN)

$(BIN): main.c scanner.o parser.o dynamic_string.o symtable.o ST_stack.o expr_parser.o symbol_stack.o code_gen.o
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: test
test: $(BIN)
	./autotest.sh

.PHONY: zip
zip:
	zip $(ARCHIVE) *.c *.h Makefile

.PHONY: pack
pack: zip
	mkdir -p is_it_ok_dir
	@echo "-------------------------------------------------------------"
	@echo "Checking with is_it_ok_sh:"
	../is_it_ok.sh $(ARCHIVE) is_it_ok_dir

.PHONY: clean
clean:
	-rm -f $(BIN) *.o
