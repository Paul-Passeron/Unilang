CC=gcc
CFLAGS= -Wall -Wextra -g -pedantic
SRC=src/
BUILD=build/
BIN=bin/
DEPS=$(BUILD)logger.o $(BUILD)lexer.o $(BUILD)token.o $(BUILD)parser.o $(BUILD)main.o $(BUILD)location.o $(BUILD)ul_assert.o $(BUILD)ul_flow.o $(BUILD)ul_allocator.o $(BUILD)ul_compiler_globals.o $(BUILD)ul_io.o $(BUILD)ul_dyn_arrays.o

all: lines main

main: $(BIN)main

$(BUILD)%.o: $(SRC)%.c 
	$(CC) $(CFLAGS) -o $@ $^ -c

$(BIN)main: $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^

lines:
	@echo "C:"
	@wc -l $$( find -wholename './*.[hc]' -not -path "./src/generation" ) | tail -n 1

clean:
	rm -rf $(BIN)*
	rm -rf $(BUILD)/*