#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <unistd.h> //getopt

#include <readline/readline.h>
#include <readline/history.h>

#include "Compiler.h"

extern HIST_ENTRY **history_list ();

using namespace std;


COMMAND commands[] = {
  { (char*)"pf ", (char*)"Movimenta" },
  { (char*)"pt ", (char*)"Movimenta" },
  { (char*)"gd ", (char*)"Gira" },
  { (char*)"ge ", (char*)"Gira" },
  { (char*)"distância ", (char*)"Sensor" },
  { (char*)"posição ", (char*)"Posição" },
  { (char*)"estado", (char*)"Informação" },
  { (char*)"espiar", (char*)"Observar objetos" },
  { (char*)"criar ", (char*)"Criar variavel" },
  { (char*)"para ", (char*)"Laço" },
  { (char*)"repita ", (char*)"Laço" },
  { (char*)"se ", (char*)"Condição" },
  { (char*)"aprender ", (char*)"Criar procedimento" },
  { (char*)"falar ", (char*)"Falar" },
  { (char*)"sair", (char*)"Fechar interpretador" },
  { (char *)NULL, (char *)NULL }
};

void initialize_readline ();
char ** fileman_completion (const char *text, int start, int end);
char * command_generator (const char *text, int state);
void usage(char *exec);

int main(int argc, char* argv[])
{
	bool done = 0, termMode = 0, scriptMode=0;
	string filename ;
	int c=0;
	
    if ( argc <= 1 ) {  // there is NO input...
        cerr << "No argument provided!" << endl;
        usage(argv[0]);
        return 1;
    }
    
   while ((c = getopt (argc, argv, "thf:")) != -1){
    switch (c){
      case 't': // terminal mode
        termMode = 1;
		if ( argc !=2 ) {  // check extra useless argumets in terminal mode
			cerr << "Terminal mode requires only one argument" << endl;
			usage(argv[0]);
			return 1;
		}        
        break;
      case 'f': // script file  mode
        filename = optarg;
        scriptMode=1;
		if ( argc !=3 ) {  // check extra useless argumets in script mode
			cerr << "Script mode requires only two arguments" << endl;
			usage(argv[0]);
			return 1;
		}     
        // test if file exists
        if( access( optarg, F_OK ) == -1 ) {
			cerr << "File " << filename << " not found!" << endl;
			return 1;
		}
        break;
      case 'h':  // help 
		usage(argv[0]);
        return 0;
      case '?': // error
        if (optopt == 'f'){
          fprintf (stderr, "Option -%c requires a filename with GoDonnie code.\n", optopt);
        }else if (isprint (optopt)){
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        }else{
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
	    }
	    usage(argv[0]);
        return 1;
      default:
        usage(argv[0]);
        return 1;
      }
  }

  if(!termMode && !scriptMode){
	  cerr << "No mode selected" << endl;
	 usage(argv[0]);
	 return 1;
  }
  if(termMode && scriptMode){
	  cerr << "Cannot have both modes selected at the same time" << endl;
	 usage(argv[0]);
	 return 1;
  }
  
  ExprTreeEvaluator Client;
  initialize_readline ();
  char *temp, *prompt;

  temp = (char *)NULL;
  prompt = (char*)"GoDonnie$ ";

  // terminal mode
  if(termMode)
  {    	  
	while(!done)
	{		
      temp = (char *)NULL;

      temp = readline (prompt);

      if (!temp)
        exit (1);

      if (strcmp(temp,"") == 1){
		  // empty line
		  continue;
	  }

      if (*temp)
      {
        //fprintf (stderr, "%s\r\n", temp);
        add_history (temp);
      }

      done = Client.terminalMode(temp);
    };
  }else if(scriptMode){    // script mode
  
	if( Client.scriptMode((char *)filename.c_str()) != 0){
		cerr << "Erro de sintaxe no arquivo " << filename << endl ;
	}
  }

}

void usage(char *exec){
	cout << endl
		 << "\nUso: " << exec << " <arg>" << endl
		 << "Argumentos:" << endl
		 << "   -t                   : Executa em modo terminal." << endl 
		 << "   -f <nome do arquivo> : Executa em modo script." << endl
		 << "   -h                   : Ajuda " << endl << endl;
}

void initialize_readline ()
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "FileMan";

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = fileman_completion;
}

char ** fileman_completion (const char *text, int start, int end)
{
  char **matches;

  matches = (char **)NULL;

  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
  //if (start == 0)
    matches = rl_completion_matches (text, command_generator);

  return (matches);
}

char * command_generator (const char *text, int state)
{
  static int list_index, len;
  char *name;

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
  if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }

  /* Return the next name which partially matches from the command list. */
  while (name = commands[list_index].name)
    {
      list_index++;

      if (strncmp (name, text, len) == 0)
        return (strdup(name));
    }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}
