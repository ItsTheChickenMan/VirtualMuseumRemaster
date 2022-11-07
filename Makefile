# makefile for virtual museum project

# compiler info
CC=g++

# project info
OUT=VirtualMuseum

# directories
INSTALL_DIR=./bin/
INCLUDE_DIR=./include/
LIB_DIRS=./lib/
SRC_DIR=./src/
OBJ_DIR=$(INSTALL_DIR)obj/

# commands
ifndef ENV
	ENV=make
endif

mkdir=mkdir -p $(1)

# commands for window environment
ifeq ($(ENV),mingw)
	mkdir=if not exist $(1) mkdir $(1)
	
	# change backslashes
	INSTALL_DIR:=$(subst /,\\,$(INSTALL_DIR))
	INCLUDE_DIR:=$(subst /,\\,$(INCLUDE_DIR))
	LIB_DIRS:=$(subst /,\\,$(LIB_DIRS))
	SRC_DIR:=$(subst /,\\,$(SRC_DIR))
	OBJ_DIR:=$(subst /,\\,$(OBJ_DIR))
endif

# obj formatting
_OBJ=glad.o utils.o audio.o mouse.o texture.o lighting.o shader.o camera.o graphics.o world.o engine.o main.o
OBJ=$(patsubst %,$(OBJ_DIR)%,$(_OBJ))

# lib directories string (-L./dir/ -L./otherdir/)
LIB=$(patsubst %,-L%,$(LIB_DIRS))

# lib includes
LIBS=-lglfw3 -lopengl32 -lsfml-audio-s -lsfml-system-s -lopenal32 -lFLAC -lvorbisfile -lvorbisenc -lvorbis -logg -lgdi32 -lassimp.dll -luser32 -lkernel32 -lwinmm

# compiler flags
CFLAGS=-Werror -g

# make all targets
.PHONY: all
all: $(INSTALL_DIR)$(OUT)

# clean o files
.PHONY: clean
clean: clearobj all
	
.PHONY: clearobj
clearobj:
	@rm -rf $(OBJ_DIR)
	
.PHONY: wintest
wintest:
	@echo "env = $(ENV)"
	@echo "mkdir = $(call mkdir,$(INSTALL_DIR))"
	@echo "objects = $(OBJ)"

# main target
$(INSTALL_DIR)$(OUT): $(OBJ) 
	@echo building $@
	@$(CXX) -o $@ $^ $(CFLAGS) -I$(INCLUDE_DIR) $(LIB) $(LIBS)
	@echo built $@
	
# define obj prerequisites
$(OBJ_DIR)graphics.o: $(SRC_DIR)graphics.cpp $(INCLUDE_DIR)graphics.h $(INCLUDE_DIR)texture.h $(INCLUDE_DIR)camera.h $(INCLUDE_DIR)shader.h $(INCLUDE_DIR)utils.h
$(OBJ_DIR)texture.o: $(SRC_DIR)texture.cpp $(INCLUDE_DIR)texture.h $(INCLUDE_DIR)utils.h
$(OBJ_DIR)lighting.o: $(SRC_DIR)lighting.cpp $(INCLUDE_DIR)lighting.h $(INCLUDE_DIR)utils.h
$(OBJ_DIR)shader.o: $(SRC_DIR)shader.cpp $(INCLUDE_DIR)shader.h $(INCLUDE_DIR)lighting.h $(INCLUDE_DIR)texture.h $(INCLUDE_DIR)utils.h
$(OBJ_DIR)camera.o: $(SRC_DIR)camera.cpp $(INCLUDE_DIR)camera.h $(INCLUDE_DIR)utils.h
$(OBJ_DIR)glad.o: $(SRC_DIR)glad/glad.c $(INCLUDE_DIR)glad/glad.h

$(OBJ_DIR)audio.o: $(SRC_DIR)audio.cpp $(INCLUDE_DIR)audio.h

$(OBJ_DIR)engine.o: $(SRC_DIR)engine.cpp $(INCLUDE_DIR)engine.h $(INCLUDE_DIR)audio.h $(INCLUDE_DIR)camera.h $(INCLUDE_DIR)graphics.h $(INCLUDE_DIR)mouse.h $(INCLUDE_DIR)shader.h $(INCLUDE_DIR)texture.h $(INCLUDE_DIR)utils.h $(INCLUDE_DIR)world.h
$(OBJ_DIR)world.o: $(SRC_DIR)world.cpp $(INCLUDE_DIR)world.h $(INCLUDE_DIR)graphics.h $(INCLUDE_DIR)lighting.h $(INCLUDE_DIR)texture.h $(INCLUDE_DIR)audio.h $(INCLUDE_DIR)shapes.h $(INCLUDE_DIR)utils.h

$(OBJ_DIR)mouse.o: $(SRC_DIR)mouse.cpp $(INCLUDE_DIR)mouse.h $(INCLUDE_DIR)graphics.h
$(OBJ_DIR)utils.o: $(SRC_DIR)utils.cpp $(INCLUDE_DIR)utils.h

$(OBJ_DIR)main.o: $(SRC_DIR)main.cpp $(INCLUDE_DIR)shapes.h

# obj rule
$(OBJ):
	@echo building $@
	@$(call mkdir,$(OBJ_DIR))
	@$(CXX) -c -o $@ $< $(CFLAGS) -I$(INCLUDE_DIR) $(LIB) $(LIBS)
	@echo built $@