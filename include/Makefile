
CC = gcc
SRC = main.c
OBJ = $(SRC:.c=.exe)

INCLUDE_PATH = -Iinclude
LIBRARY_PATH = -Llib


LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm


debug:
	$(CC) $(SRC) -o $(OBJ) $(INCLUDE_PATH) $(LIBRARY_PATH) $(LIBS) -g


release:
	$(CC) $(SRC) -o $(OBJ) $(INCLUDE_PATH) $(LIBRARY_PATH) $(LIBS) -s -O2 -mwindows

clean:
	del $(OBJ)