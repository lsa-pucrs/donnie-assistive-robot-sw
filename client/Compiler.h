#include "logoLexer.h"
#include "logoParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include <stack>
#include <string>
#include <iostream>

#include "DonnieClient.h"
#include "utils.h"

using std::map;
using std::vector;
using std::stack;
using std::string;
using std::cout;
using std::endl;


struct mem
{
   map<string,int> memory;
};

struct element
{
  int typeInt;
  float typeFloat;
};

struct procDec
{
  pANTLR3_BASE_TREE node;
  int argNum;
  vector<string> args;
};

typedef struct {
  char *name;     /* User printable name of the function. */
  char *doc;      /* Documentation for this function.  */
} COMMAND;


class ExprTreeEvaluator 
{
    map<string,int> memory;         // Dicionário de variáveis globais
    map<string,procDec> proc;       // Dicionário de procedimentos
    stack<mem> localMem;            // Stack de dicionários de variáveis locais
    map<string,int>::iterator it;   // Iterados dos dicionários
    int memFlag;                    // Flag de indicação variáves globais/locais
    bool for_itFlag;
    char* for_it;

    bool done;

    DonnieClient *Donnie;

    int parser(pANTLR3_INPUT_STREAM input);

public:
    ExprTreeEvaluator();
    int run(pANTLR3_BASE_TREE);
    int terminalMode(char* textIn);
    int scriptMode(char* fileIn);

};

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE, unsigned);
const char* getText(pANTLR3_BASE_TREE tree);
bool compare (int a, int b, string comp);


