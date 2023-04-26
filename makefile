
INC_PATH = -Iinclude
SRC_FILES = $(wildcard source/*.cpp)
VEN_INC = -Iext/inc
STD_LINK = -lstdc++ -lm

CC = g++
BIN_OUT = bin

demo_graphics:
	$(CC) -o $(BIN_OUT)/demo_graphics demo_graphics.cpp $(SRC_FILES) ext/bin/libglad.a ext/bin/libglfw3.a -lGL -lX11 $(STD_LINK) $(INC_PATH) $(VEN_INC)
