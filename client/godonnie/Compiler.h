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

// Localization libraries 
#include <boost/locale.hpp>
using namespace boost::locale;
using boost::locale::translate;
using boost::locale::format;

typedef struct {
  char *name;     /* User printable name of the function. */
  char *doc;      /* Documentation for this function.  */
} COMMAND;


/// Main parser for GoDonnie language
class ExprTreeEvaluator 
{
    int for_itFlag;
    /// this flag is true when command Exit is executed
    bool done;

	/// this file pointer is used to log into file the commands executed in terminal mode
    FILE *log;
    
    /// when enabled, it generated a Log directory in the current path
    bool enable_log;

    DonnieClient *Donnie; /// pointer to Donnie middleware class
    Historic *History;    /// pointer to History class

    /// main method of GoDonnie Interpreter where each GoDonnie command is executed
    int run(pANTLR3_BASE_TREE);

public:
    ExprTreeEvaluator();
    ~ExprTreeEvaluator();
    
    /// set the log file pointer, used only in terminal mode
    void logFile(FILE *file);
    
    /// uses the TTS to say a sentence
    void speak(string text);
    
    /// mute the TTS and just print the sentence on the stdout
    void muteTTS(bool m);

    /// Enable logging when true
    void setLog(bool m);

    /// Get the logging status
    bool getLog();

	/// parse input stream in GoDonnie syntax
	/*! build the ANTLR objects (lexer and parser), 
	 * build the parse tree and run if no parse errors were found.
	 * In Terminal mode, this also saves input stream into the log file
	 */ 
    int parseGD(char* textIn);

};

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE, unsigned);
const char* getText(pANTLR3_BASE_TREE tree);
bool compare (int a, int b, string comp);
void split(const std::string &s, char delim, std::vector<std::string> &elems);
/// remove all the special characters (accentuation such as çãáé ...) to avoid antlr error
string cleanAccents(string code);

#endif
