all: main

main: main.cc
	g++ -o main main.cc

clean:
	rm main