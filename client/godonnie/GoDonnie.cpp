#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>

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



int main(int argc, char* argv[])
{

	bool done = 0;

	//Caso não tenha a quantidade de argumentos corretos da um aviso
	if(argc < 2)
  {
    cout << "\nUso: " << argv[0] << " <arg>" << endl;
    cout << "Argumentos: \n    -t                :      Executa em modo terminal. \n    <nome do arquivo> :      Executa em modo script." << endl << endl;
    return -1;
  }


  for(int i = 1; i < argc; i++)
  {
    if(argv[i][0] == '-' && argv[i][1] == 't')
    {
      done = 1;
      break;
    }
    else
      done = 0;
  }

  ExprTreeEvaluator Client;

  initialize_readline ();

  char *temp, *prompt;

  temp = (char *)NULL;
  prompt = (char*)"GoDonnie$ ";

  do
  {
    if(done)
    {
      temp = (char *)NULL;

      temp = readline (prompt);

      if (!temp)
        exit (1);

      if (*temp)
      {
        //fprintf (stderr, "%s\r\n", temp);
        add_history (temp);
      }

      done = Client.terminalMode(temp);

    }
    else
    {
      done = Client.scriptMode(argv[1]);      //  Utilizar para modo script
    }

  }while(done);

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
