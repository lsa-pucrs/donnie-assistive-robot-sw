CXXFLAGS = `pkg-config --cflags playerc++ ` -g -I.
CFLAGS = `pkg-config --cflags playerc ` -g -I.
OBJS = utils.cpp DonnieClient.cpp Compiler.cpp Exception.cpp GoDonnieLexer.c GoDonnieParser.c
LDLIBS = $(OBJS) `pkg-config --libs playerc++` -fexec-charset=iso-8859-1 -std=c++0x -lreadline -lantlr3c
CC = g++ 
	
all:clean grammar GoDonnie

clean: 
	rm -f GoDonnieLexer* GoDonnieParser* GoDonnie GoDonnie.tokens

grammar: GoDonnie.g
	java -jar Antlr/antlr-3.2.jar GoDonnie.g
	
install:
	sudo cp $(DONNIE_PATH)/lib
