CXXFLAGS = `pkg-config --cflags playerc++ ` -g -I.
CFLAGS = `pkg-config --cflags playerc ` -g -I.
OBJS = utils.cpp DonnieClient.cpp Compiler.cpp GoDonnieLexer.c GoDonnieParser.c
LDLIBS = $(OBJS) `pkg-config --libs playerc++` -fexec-charset=iso-8859-1 -std=c++0x -lreadline -ltermcap  -lantlr3c -pthread
CC = g++ 
	
all:clean grammar GoDonnie

clean: 
	rm -f GoDonnie*

grammar: GoDonnie.g
	java -jar Antlr/antlr-3.2.jar GoDonnie.g
	
install:
	sudo cp $(DONNIE_PATH)/lib
