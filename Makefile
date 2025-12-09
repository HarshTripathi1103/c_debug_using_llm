# Compiler and flags
CC = gcc
CFLAGS = -I./curl -Wall -Wextra
LDFLAGS = -L./lib
LIBS = -lcurl -lnghttp2 -lnghttp3 -lngtcp2 -lngtcp2_crypto_libressl -lssl -lcrypto -lssh2 -lz -lzstd -lbrotlidec -lbrotlicommon -lpsl -lws2_32 -lcrypt32 -lbcrypt

# Directories
UITLS_DIR = utils

# Common object files
COMMON_OBJS = $(UITLS_DIR)/curl_llm.o $(UITLS_DIR)/env_parser.o

# Targets
.PHONY: all clean inputfile run_inputfile help

# Default target
all: inputfile

# Help target
help:
	@echo "Available targets:"
	@echo "  make inputfile       - Build the inputfile executable"
	@echo "  make run_inputfile FILE=path/to/file.c - Run inputfile on a C source file"
	@echo "  make clean          - Remove all build artifacts"
	@echo ""
	@echo "Example usage:"
	@echo "  make inputfile"
	@echo "  ./utils/inputfile.exe utils/bug.c"

# 1. Compile env_parser to an object file
$(UITLS_DIR)/env_parser.o: $(UITLS_DIR)/env_parser.c
	$(CC) $(CFLAGS) -c $(UITLS_DIR)/env_parser.c -o $(UITLS_DIR)/env_parser.o

# 2. Compile curl_llm to an object file
# Note: We do NOT link LIBS here. We only compile.
$(UITLS_DIR)/curl_llm.o: $(UITLS_DIR)/curl_llm.c
	$(CC) $(CFLAGS) -c $(UITLS_DIR)/curl_llm.c -o $(UITLS_DIR)/curl_llm.o

# 3. Build inputfile executable
inputfile: $(UITLS_DIR)/inputfile.c $(UITLS_DIR)/curl_llm.c $(UITLS_DIR)/env_parser.c
	$(CC) -o $(UITLS_DIR)/inputfile.exe $(UITLS_DIR)/inputfile.c $(UITLS_DIR)/curl_llm.c $(UITLS_DIR)/env_parser.c $(CFLAGS) $(LDFLAGS) $(LIBS)

# 4. Run inputfile (requires a .c file argument)
run_inputfile: inputfile
	@echo "Usage: make run_inputfile FILE=path/to/file.c"
	@if [ -z "$(FILE)" ]; then echo "Error: FILE argument is required. Example: make run_inputfile FILE=utils/bug.c"; exit 1; fi
	./$(UITLS_DIR)/inputfile.exe $(FILE)

clean:
	rm -f $(UITLS_DIR)/*.o $(UITLS_DIR)/*.exe
