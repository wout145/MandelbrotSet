CC = g++

SRC = main.cpp

TARGET = main

FLAGS = -I/usr/local/include/GLFW -L/usr/lib -lglfw3 -lGLEW -lGLU -lstdc++ -lGL -lm -lpthread -ldl -lrt -lX11

$(TARGET): $(SRC)
	$(CC) $(SRC) $(FLAGS) -o $(TARGET)
