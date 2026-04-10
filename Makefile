CC = gcc
EMCC = emcc

# Path to Raylib (change this to your local Raylib directory)
RAYLIB_PATH = /usr/local

# Raylib source and include paths
RAYLIB_INCLUDE = $(RAYLIB_PATH)/include
RAYLIB_LIB = $(RAYLIB_PATH)/lib

# Raylib library name
RAYLIB_LIB_NAME = raylib

# Flags for compiling
CCFLAGS = -I$(RAYLIB_INCLUDE)
LDFLAGS = -L$(RAYLIB_LIB) -l$(RAYLIB_LIB_NAME)

# Source files
SRC = $(wildcard ./*.c)
OBJ = $(SRC:.c=.o)

# Output files
DESKTOP_OUTPUT = ray_chip
WEB_OUTPUT = ray_chip.html

# Web-specific flags for Emscripten
EMCC_FLAGS = -s USE_GLFW=3 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s ENVIRONMENT=web -s FULL_ES3=1

# Default target is to build for desktop
all: desktop

# Compile for desktop
desktop: $(OBJ)
	$(CC) $(OBJ) -o $(DESKTOP_OUTPUT) $(LDFLAGS)

# Compile for web
web: $(OBJ)
	$(EMCC) $(OBJ) -o $(WEB_OUTPUT) $(EMCC_FLAGS) $(LDFLAGS)

# Compile the source files into object files
%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

# Clean the generated files
clean:
	rm -f $(OBJ) $(DESKTOP_OUTPUT) $(WEB_OUTPUT)
