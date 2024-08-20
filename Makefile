CC = gcc

TARGET = main

SRC = main.cpp

FLAGS = -I/usr/include/raylib -L/usr/lib -lstdc++ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

$(TARGET): $(SRC)
	$(CC) -g -o $(TARGET) $(SRC) $(FLAGS)
