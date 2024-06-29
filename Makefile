# Variables
BUILD_DIR := ./build
SRC_DIR := ./src
INCLUDE_DIR := ./include
LIB_DIR := ./lib
EXAMPLES_DIR := ./examples

# Targets
sort: $(BUILD_DIR)/sort_main

run: sort
	@echo "Running sort_main..."
	@./$(BUILD_DIR)/sort_main

clean:
	@echo "Cleaning up..."
	@rm -rf $(BUILD_DIR)/*
	@rm -f data.db out*.db

# Build rule for sort_main
$(BUILD_DIR)/sort_main: $(EXAMPLES_DIR)/sort_main.c $(SRC_DIR)/record.c $(SRC_DIR)/sort.c $(SRC_DIR)/merge.c $(SRC_DIR)/chunk.c | $(BUILD_DIR)
	@echo "Compiling sort_main..."
	gcc -g -I $(INCLUDE_DIR) -L $(LIB_DIR) -Wl,-rpath,$(LIB_DIR) $^ -lbf -lhp_file -o $@ -O2

# Create build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
