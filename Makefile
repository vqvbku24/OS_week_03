CC = gcc
CFLAGS = -O2 -std=c11 -Wall -Wextra -pthread
LDFLAGS = -pthread -lm

BIN_DIR = bin
SRC_DIR = src

# Auto-detect OS executable extension and shell commands
ifeq ($(OS),Windows_NT)
    EXE = .exe
    MKDIR_BIN = if not exist $(BIN_DIR) mkdir $(BIN_DIR)
    CLEAN_CMD = if exist $(BIN_DIR) rmdir /S /Q $(BIN_DIR)
    PYTHON ?= python
else
    EXE =
    MKDIR_BIN = mkdir -p $(BIN_DIR)
    CLEAN_CMD = rm -rf $(BIN_DIR)
    PYTHON ?= python3
endif

TARGETS = $(BIN_DIR)/pi_single$(EXE) \
          $(BIN_DIR)/pi_local$(EXE) \
          $(BIN_DIR)/pi_mutex$(EXE) \
          $(BIN_DIR)/pi_mutex_k$(EXE) \
          $(BIN_DIR)/pi_atomic$(EXE)

.PHONY: all clean bench tradeoff plot check

all: $(TARGETS)

$(TARGETS): | $(BIN_DIR)

$(BIN_DIR):
	@$(MKDIR_BIN)

$(BIN_DIR)/%$(EXE): $(SRC_DIR)/%.c $(SRC_DIR)/common.h
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

check: all
	@echo === Quick Smoke Test: pi_single ===
	@$(BIN_DIR)/pi_single$(EXE) 1000000 1 12345 1
	@echo === Quick Smoke Test: pi_local ===
	@$(BIN_DIR)/pi_local$(EXE) 1000000 2 12345 1
	@echo === Quick Smoke Test: pi_mutex ===
	@$(BIN_DIR)/pi_mutex$(EXE) 1000000 2 12345 1
	@echo === Quick Smoke Test: pi_mutex_k ===
	@$(BIN_DIR)/pi_mutex_k$(EXE) 1000000 2 12345 1 1000
	@echo === Quick Smoke Test: pi_atomic ===
	@$(BIN_DIR)/pi_atomic$(EXE) 1000000 2 12345 1

bench: all
	bash scripts/run_bench.sh

tradeoff: all
	bash scripts/run_tradeoff.sh

plot:
	$(PYTHON) scripts/plot.py

clean:
	@$(CLEAN_CMD)
