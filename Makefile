# Compiler and flags
CC = gcc
CFLAGS = -I./curl
LDFLAGS = -L./lib
LIBS = -lcurl -lnghttp2 -lnghttp3 -lngtcp2 -lngtcp2_crypto_libressl -lssl -lcrypto -lssh2 -lz -lzstd -lbrotlidec -lbrotlicommon -lpsl -lws2_32 -lcrypt32 -lbcrypt

# Directories
UITLS_DIR = uitls

# Targets
.PHONY: all clean curl_llm

all: curl_llm

curl_llm: $(UITLS_DIR)/curl_llm.c
	$(CC) -o $(UITLS_DIR)/curl_llm.exe $(UITLS_DIR)/curl_llm.c $(CFLAGS) $(LDFLAGS) $(LIBS)

run_curl_llm: curl_llm
	./$(UITLS_DIR)/curl_llm.exe

clean:
	rm -f $(UITLS_DIR)/*.exe
