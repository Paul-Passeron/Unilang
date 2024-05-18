CC=gcc
CFLAGS=-Wall -Wextra -pedantic
SRC=src/
BUILD=build/
BIN=bin/

DEPS=$(BUILD)lexer.o $(BUILD)ul_allocator.o $(BUILD)ul_io.o $(BUILD)ul_flow.o   $(BUILD)ul_types.o $(BUILD)name_table.o  $(BUILD)token.o $(BUILD)ul_ast.o $(BUILD)ul_dyn_arrays.o $(BUILD)location.o $(BUILD)main.o $(BUILD)ul_compiler_globals.o $(BUILD)parser.o $(BUILD)logger.o $(BUILD)ul_assert.o $(BUILD)generator.o
all: lines Unilang
lines:
	@echo "C:"
	@wc -l $$( find -wholename './*.[hc]') | tail -n 1
$(BUILD)%.o: $(SRC)%.c
	 $(CC) $(CFLAGS) -o $@ $^ -c
$(BIN)Unilang: $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^
Unilang: $(BIN)Unilang
clean:
	rm -rf $(BIN)*
	rm -rf $(BUILD)*
install:
	cp $(BIN)Unilang /bin/