all: main.c parse.c
	g++ main.c parse.c -lcurl -ljson-c -lpthread -L ~/rpi-rgb-led-matrix/lib -lrgbmatrix -o clock

