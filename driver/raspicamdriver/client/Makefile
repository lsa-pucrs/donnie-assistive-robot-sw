main: main.cc
	g++ -o main `pkg-config --cflags playerc++ opencv` main.cc `pkg-config --libs playerc++ opencv` -lboost_system

run:
	./main

clean: 
	rm main
