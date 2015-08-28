all: main

main:
	g++ src/*.cpp -o segmenter `pkg-config --cflags --libs opencv` -std=c++11 -g

# limpa:
# 	rm *.o