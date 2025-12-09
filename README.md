# C Debug Using LLM

A tool that compiles C code, captures compilation errors, and sends them to Groq's LLM API for intelligent debugging suggestions.

## Features

- Compiles C source files and captures compiler errors
- Sends errors to Groq LLM for analysis and suggestions
- Supports environment variable configuration via `.env` file
- Easy to use with Makefile commands

## Prerequisites

- GCC compiler
- libcurl and its dependencies (provided in `lib/` directory)
- Groq API key

## Setup

1. Create a `.env` file in the project root with your Groq API credentials:

```
GROQ_API=your_groq_api_key_here
GROQ_LINK=https://api.groq.com/openai/v1/chat/completions
```

2. Make sure the curl DLL is in the utils directory:
   - `libcurl-x64.dll` should be in `utils/`

## Building

Build the inputfile executable:

```bash
make inputfile
```

Or simply:

```bash
make
```

## Usage

Run the tool on any C source file:

```bash
./utils/inputfile.exe path/to/your/file.c
```

Example:

```bash
./utils/inputfile.exe utils/bug.c
```

The tool will:
1. Compile the C file using GCC
2. Capture any compilation errors
3. Send the errors to Groq LLM
4. Display the LLM's suggestions for fixing the code

## Makefile Commands

- `make help` - Show available commands
- `make inputfile` - Build the inputfile executable
- `make clean` - Remove all build artifacts

## Project Structure

```
.
├── curl/           # curl header files
├── lib/            # curl library files
├── utils/          # Source code and executables
│   ├── inputfile.c     # Main program
│   ├── curl_llm.c      # LLM API interface
│   ├── curl_llm.h      # LLM API header
│   ├── env_parser.c    # .env file parser
│   └── env_parser.h    # .env parser header
├── .env            # Environment variables (API keys)
├── Makefile        # Build configuration
└── README.md       # This file
```

## How It Works

1. **inputfile.c** - Main program that:
   - Takes a C source file as input
   - Compiles it using GCC and captures stderr output
   - Formats the error into a JSON payload
   - Calls the LLM API with the error

2. **curl_llm.c** - Handles HTTP requests to Groq API:
   - Reads API credentials from .env file
   - Sends POST request with JSON payload
   - Captures and displays the LLM response

3. **env_parser.c** - Parses the .env file:
   - Reads key-value pairs from .env
   - Returns values for requested keys
