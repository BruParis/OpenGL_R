
#test

CC = g++
LD = g++
CPPFLAGS = -Wall -g #-Wno-deprecated-declarations  #-O2   # pour optimiser  -pedantic -ansi
LDFLAGS  =

INCLUDE_DIR = -I/usr/include/ -I/usr/local/Cellar/freeglut/2.8.1/include/ -I/opt/X11/include/ -I./GL -I.
LIB_DIR =
LIBS = -L/usr/X11R6/lib  -lGL -lGLU -lglut

FINAL_TARGET = opengl

OBJ_DIR = bin
SRC_DIR = .
BIN_DIR = bin

SRCS = opengl.cpp Scene.cpp Vec3f.cpp

default: $(BIN_DIR)/$(FINAL_TARGET)

# rules

$(BIN_DIR)/$(FINAL_TARGET): $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
	$(LD) $+ -o $@ $(LDFLAGS) $(LIB_DIR) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $(CPPFLAGS) $(INCLUDE_DIR) $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/$(FINAL_TARGET)
