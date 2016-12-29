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
    /// this flag is true when command Exit is executed
    bool done;

    FILE *log;

    DonnieClient *Donnie; /// pointer to Donnie middleware class
    Historic *History;    /// pointer to History class

    /// main method of GoDonnie Interpreter where each GoDonnie command is executed
    int run(pANTLR3_BASE_TREE);

public:
    ExprTreeEvaluator();
    ~ExprTreeEvaluator();
    void logFile(FILE *file);
    //int parse(char* textIn, bool enable_log);
    void speak(string text);
    void muteTTS(bool m);

	/// parse input stream in GoDonnie syntax
	/*! build the ANTLR objects (lexer and parser), 
	 * build the parse tree and run if no parse errors were found.
	 * In Terminal mode, this also saves input stream into the log file
	 */ 
    int parseGD(char* textIn, bool enable_log);
    //int parser(pANTLR3_INPUT_STREAM input);


};

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE, unsigned);
const char* getText(pANTLR3_BASE_TREE tree);
bool compare (int a, int b, string comp);
void split(const std::string &s, char delim, std::vector<std::string> &elems);
/// remove all the special characters (accentuation such as çãáé ...) to avoid antlr error
string cleanAccents(string code);


#endif
