CXXFLAGS = `pkg-config --cflags playerc++ ` -g
CFLAGS = `pkg-config --cflags playerc ` -g
OBJS = utils.cpp DonnieClient.cpp Compiler.cpp logoLexer.c logoParser.c
LDLIBS = $(OBJS) `pkg-config --libs playerc++` -std=c++0x -lreadline -ltermcap  -lantlr3c 
CC = g++ 
	
all:Logo

clean: 
	rm Logo



