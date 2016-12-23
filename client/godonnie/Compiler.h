/*
 * Desc: Main parser for GoDonnie language
 * Author: Augusto Bergamin
 * Date:  November 2016
 * Laboratório de Sistemas Autônomos 
 *  - https://lsa.pucrs.br/
 *  - https://github.com/lsa-pucrs
 * Faculdade de Informática - PUCRS  
 *  - www.inf.pucrs.br
 */

#pragma once
#ifndef COMPILER_H
#define COMPILER_H

#include "GoDonnieLexer.h"
#include "GoDonnieParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include <stack>
#include <string>
#include <iostream>

#include "utils.h"
#include "DonnieMemory.h"

class DonnieClient;
class Historic;
 

using std::map;
using std::vector;
using std::stack;
using std::string;
using std::cout;
using std::endl;

typedef struct {
  char *name;     /* User printable name of the function. */
  char *doc;      /* Documentation for this function.  */
} COMMAND;


/// Main parser for GoDonnie language
class ExprTreeEvaluator 
{
    bool for_itFlag;
    bool done;

    FILE *log;

    DonnieClient *Donnie; /// pointer to Donnie middleware class
    Historic *History;    /// pointer to History class

	/*! build the ANTLR objects (lexer and parser), 
	 * build the parse tree and run if no parse errors were found
	 */ 
    int parser(pANTLR3_INPUT_STREAM input);
    int parser(pANTLR3_INPUT_STREAM input, char *textCode);

    /// main method of GoDonnie Interpreter where each GoDonnie command is executed
    int run(pANTLR3_BASE_TREE);

public:
    ExprTreeEvaluator();
    ~ExprTreeEvaluator();
    void logFile(FILE *file);
    int terminalMode(char* textIn);
    int scriptMode(char* fileIn);
    void speak(string text);
    void muteTTS(bool m);

};

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE, unsigned);
const char* getText(pANTLR3_BASE_TREE tree);
bool compare (int a, int b, string comp);
void split(const std::string &s, char delim, std::vector<std::string> &elems);

#endif
