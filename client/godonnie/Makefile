CXXFLAGS = `pkg-config --cflags playerc++ ` -g -I. -I../../utils/
CFLAGS = `pkg-config --cflags playerc ` -g -I. -I../../utils/
OBJS = ../../utils/utils.cpp DonnieClient.cpp DonnieMemory.cpp Compiler.cpp Exception.cpp Historic.cpp GoDonnieLexer.c GoDonnieParser.c
LDLIBS = $(OBJS) `pkg-config --libs playerc++` -std=c++0x -lreadline -lantlr3c
CC = g++ 
	
all:clean grammar GoDonnie

clean: 
	rm -f GoDonnieLexer* GoDonnieParser* GoDonnie GoDonnie.tokens

grammar: GoDonnie.g
	java -jar Antlr/antlr-3.2.jar GoDonnie.g


