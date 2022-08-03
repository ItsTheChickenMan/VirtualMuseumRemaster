# makefile for virtual museum project

# compiler info
CC=gcc

# project info
OUT=VirtualMuseum

# directories
INSTALL_DIR=./bin/
INCLUDE_DIR=./include/
LIB_DIRS=./lib/
SRC_DIR=./src/
OBJ_DIR=$(INSTALL_DIR)obj/

# obj formatting
_OBJ=glad.o utils.o texture.o shader.o camera.o graphics.o main.o
OBJ=$(patsubst %,$(OBJ_DIR)%,$(_OBJ))

# lib directories string (-L./dir/ -L./otherdir/)
LIB=$(patsubst %,-L%,$(LIB_DIRS))

# lib includes
LIBS=-lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32

# compiler flags
CFLAGS=-Werror

# make all targets
.PHONY: all
all: $(INSTALL_DIR)$(OUT)

# clean o files
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR)

# main target
$(INSTALL_DIR)$(OUT): $(OBJ) 
	@echo building $@
	@$(CXX) -o $@ $^ $(CFLAGS) -I$(INCLUDE_DIR) $(LIB) $(LIBS)
	@echo built $@
	
# define obj prerequisites
$(OBJ_DIR)graphics.o: $(SRC_DIR)graphics.cpp $(INCLUDE_DIR)graphics.h
$(OBJ_DIR)texture.o: $(SRC_DIR)texture.cpp $(INCLUDE_DIR)texture.h
$(OBJ_DIR)shader.o: $(SRC_DIR)shader.cpp $(INCLUDE_DIR)shader.h
$(OBJ_DIR)camera.o: $(SRC_DIR)camera.cpp $(INCLUDE_DIR)camera.h
$(OBJ_DIR)utils.o: $(SRC_DIR)utils.cpp $(INCLUDE_DIR)utils.h

$(OBJ_DIR)glad.o: $(SRC_DIR)glad/glad.c

$(OBJ_DIR)main.o: $(SRC_DIR)main.cpp

# obj rule
$(OBJ):
	@echo building $@
	@mkdir -p $(OBJ_DIR)
	@$(CXX) -c -o $@ $< $(CFLAGS) -I$(INCLUDE_DIR) $(LIB) $(LIBS)
	@echo built $@