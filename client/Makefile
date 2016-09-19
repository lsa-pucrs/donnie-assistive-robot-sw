CXXFLAGS = `pkg-config --cflags playerc++ ` -g
CFLAGS = `pkg-config --cflags playerc ` -g
OBJS = utils.cpp DonnieClient.cpp Compiler.cpp logoLexer.c logoParser.c
LDLIBS = $(OBJS) `pkg-config --libs playerc++` -fexec-charset=iso-8859-1 -std=c++0x -lreadline -ltermcap  -lantlr3c -pthread
CC = g++ 
	
all:clean Logo

clean: 
	rm -f Logo

# TODO mudar o nome dos arquivos p GoDonnie 
# TODO: fazer uma regra make install que coloque os programas de teste no dir DONNIE_PATH/test/GoDonnie
