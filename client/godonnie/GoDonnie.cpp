#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <unistd.h> //getopt
#include <sys/types.h>
#include <sys/stat.h>


#include <readline/readline.h>
#include <readline/history.h>

#include "Compiler.h"

// Localization libraries 
#include <boost/locale.hpp>
using namespace boost::locale;
using boost::locale::translate;
using boost::locale::format;

//extern HIST_ENTRY **history_list ();

using namespace std;

#define LANG "pt-br"

// Comandos em pt_BR
COMMAND comandos[] = {
  { (char*)"pf ", (char*)"Movimenta para frente" },
  { (char*)"pt ", (char*)"Movimenta para trás" },
  { (char*)"gd ", (char*)"Gira para direita" },
  { (char*)"ge ", (char*)"Gira para esquerda" },
  { (char*)"distância", (char*)"Sensor de distância" },
  { (char*)"espiar", (char*)"Observar objetos" },
  { (char*)"cor", (char*)"Procura por uma cor" },
  { (char*)"posição ", (char*)"Posição do robô" },
  { (char*)"estado", (char*)"Informação atual" },
  { (char*)"criar ", (char*)"Criar variável" },
  { (char*)"para ", (char*)"Laço" },
  { (char*)"repita ", (char*)"Laço" },
  { (char*)"enquanto ", (char*)"Laço" },
  { (char*)"se ", (char*)"Condição" },
  { (char*)"aprender ", (char*)"Criar procedimento" },
  { (char*)"falar ", (char*)"Falar" },
  { (char*)"sair", (char*)"Fechar interpretador" },
  { (char*)"som", (char*)"Liga ou desliga o som" },
  { (char*)"histórico", (char*)"Histórico de comandos de movimento" },
  { (char*)"cinto", (char*)"Liga ou desliga o cinto" },
  { (char*)"vibrar", (char*)"Aciona individualmente os motores de vibração quando o cinto é utilizado" },
  { (char *)NULL, (char *)NULL }

};

// Comandos em en_US
COMMAND commands[] = {
  { (char*)"fw ", (char*)"Forward movement" },
  { (char*)"bw ", (char*)"Backward movement" },
  { (char*)"tr ", (char*)"Turn right" },
  { (char*)"tl ", (char*)"Turn left" },
  { (char*)"distance", (char*)"Distance sensor" },
  { (char*)"scan", (char*)"Scan for objects" },
  { (char*)"color", (char*)"Searches for a color" },
  { (char*)"position ", (char*)"Robot position" },
  { (char*)"state", (char*)"Current state of the robot" },
  { (char*)"var ", (char*)"Create variable" },
  { (char*)"for ", (char*)"For loop" },
  { (char*)"repeat ", (char*)"Repeat loop" },
  { (char*)"while ", (char*)"While laço" },
  { (char*)"if ", (char*)"Conditional" },
  { (char*)"procedure ", (char*)"Create procedure" },
  { (char*)"speak ", (char*)"Speak" },
  { (char*)"quit", (char*)"Close the interpreter" },
  { (char*)"sound", (char*)"Turns the sound on and off" },
  { (char*)"history", (char*)"Movement command history" },
  { (char*)"belt", (char*)"Turns the belt on and off" },
  { (char*)"vibrate", (char*)"Individually activate the vibration motors when the belt is used" },
  { (char *)NULL, (char *)NULL }
};

string code = "";
bool done = 0;
ExprTreeEvaluator Compiler;
FILE *logFile=NULL; // file handle for the log file

void initialize_readline ();
char ** fileman_completion (const char *text, int start, int end);
char * command_generator (const char *text, int state);
void usage(char *exec);
int evalCode(int count, int key);
void setupLog();

int main(int argc, char* argv[])
{
  string donnie_path = GetEnv("DONNIE_PATH");
  string donnie_lang = GetEnv("DONNIE_LANG");
  
  if (donnie_path=="") {
	cerr << translate("variable DONNIE_PATH not defined. Please execute 'export DONNIE_PATH=<path-to-donnie>'") << endl;
	exit(1);
  }
  if (donnie_lang=="") {
	cerr << translate("variable DONNIE_LANG not defined. Please execute 'export DONNIE_LANG=$LANGUAGE'") << endl;
	exit(1);
  }  
    	
  // Set up language environment
  generator gen;
  gen.add_messages_path(donnie_path + "/resources/loc");
  gen.add_messages_domain("GoDonnie");
  locale loc = gen(donnie_lang + ".UTF-8");
  locale::global(loc);
  cout.imbue(loc);
  cerr.imbue(loc);

	bool termMode = 0, scriptMode=0;
	string filename ;
	int c=0, argcnt=0;
	/// this is from getopt lib. it is used to reset de idx of the parameter list
	extern int optind; 
	extern char *optarg; // used by getopt
	
    if ( argc <= 1 ) {  // there is NO input...
        //Compiler.speak("No argument provided!");
        Compiler.speak(string(translate("Comando sem argumentos!")));
        usage(argv[0]);
        return 1;
    }

  // the arguments are case insensitive
   while ((c = getopt (argc, argv, "mMlLtThHf:F:")) != -1){
   // a single : means that the argument is required. double :: means the argument is optional
   //while ((c = getopt (argc, argv, "mlthf:")) != -1){
    switch (c){
      case 'T': 
      case 't': // terminal mode
        termMode = 1;
        /*
		if ( argc !=2 ) {  // check extra useless argumets in terminal mode
			Compiler.speak(string(translate("Modo terminal requer somente um argumento.")));
			usage(argv[0]);
			return 1;
		} 
		*/
        break;
      case 'F':
      case 'f': // script file  mode
		if (optarg == NULL){
			cerr << translate("Erro inesperado no argumento de nome do arquivo") << endl;
			exit(1);			
		}
        filename = optarg;
        scriptMode=1;
        /*
		if ( argc !=3 ) {  // check extra useless argumets in script mode
			Compiler.speak(string(translate("Modo script requer somente dois argumentos.")));
			usage(argv[0]);
			return 1;
		}
		*/
        // test if file exists
        if( access( optarg, F_OK ) == -1 ) {
			Compiler.speak(string(translate("Arquivo ")) + string(filename) + string(translate(" não encontrado.")));
			return 1;
		}
        break;
      case 'M':
      case 'm':  // mute
		 //if mute ('m') is on, do it first, before any other command
		 if (argcnt == 0){
			Compiler.muteTTS(true);
			break;
		 }else{
			 Compiler.speak(string(translate("Parâmetro m deve vir primeiro.")));
			 return 1;
		 }
		 break;
      case 'L':
      case 'l':  // log
		 //log is disabled by default
		 Compiler.setLog(true);
		 /*
		 if (argcnt == 0){
			Compiler.setLog(true);
			break;
		 }else{
			 Compiler.speak(string(translate("Parâmetro l deve vir primeiro.")));
			 return 1;
		 }
		 */
		 break;
      case 'H':
      case 'h':  // help 
		usage(argv[0]);
        return 0;
      case '?': // error
        if (optopt == 'f'){
          Compiler.speak(string(translate("Parâmetro -f requer um nome de arquivo com código GoDonnie.")));
        }else if (isprint (optopt)){
          //fprintf (stderr, "Unknown option `-%c'.\n", optopt);
          char buffer[50];
          string aux1 = translate("Parâmetro");
          string aux2 = translate("desconhecido");
          sprintf (buffer, "%s `-%c' %s.", aux1.c_str(), optopt, aux2.c_str());
          Compiler.speak(string(buffer));
        }else{
          //fprintf (stderr,
          //         "Unknown option character `\\x%x'.\n",
          //         optopt);
          char buffer[50];
          string aux1 = translate("Parâmetro");
          string aux2 = translate("desconhecido");
          sprintf (buffer, "%s `\\x%x' %s.", aux1.c_str(), optopt, aux2.c_str());
          Compiler.speak(string(buffer));
	    }
	    usage(argv[0]);
        return 1;
      default:
        usage(argv[0]);
        return 1;
      }
      argcnt++;
  }

  if(!termMode && !scriptMode){
	 //cerr << "No mode selected" << endl;
	 Compiler.speak(string(translate("Nenhum modo selecionado.")));
	 usage(argv[0]);
	 return 1;
  }
  if(termMode && scriptMode){
	 //cerr << "Cannot have both modes selected at the same time" << endl;
	 Compiler.speak(string(translate("Não pode ter dois modos selecionados ao mesmo tempo.")));
	 usage(argv[0]);
	 return 1;
  }
  
  initialize_readline ();
  static char *temp = (char *)NULL; 
  string preCode;

  // bind the ESC key to the evalCode function
  //rl_attempted_completion_function = fileman_completion;
  rl_bind_key (27, evalCode); /* "27" ascii code for ESC */
  //rl_unbind_key('\t');
  //rl_bind_key('a',rl_complete);

  // create log file if it was enabled in command line
  setupLog();

  // terminal mode
  if(termMode)
  {

    while(!done)
    {
	    // If the buffer has already been allocated, return the memory to the free pool.
	    if (temp){
		  free (temp);
		  temp = (char *)NULL;
		}        
        temp = readline ("GoDonnie$ ");

        if (!temp)
        exit (1);

		// If the line has any text in it, save it on the history.
        if (temp && *temp)
        {
          add_history (temp);
          if(code.empty())
			code += string(temp);
		  else
			code += "\n" + string(temp);
        }
        else
          rl_on_new_line ();
      
    };
  }else if(scriptMode){    // script mode
	// read the entire file
	std::ifstream t(filename.c_str());
	std::stringstream buffer;
	
	if(t.is_open()) {
		// read the entire file
		buffer << t.rdbuf();
		Compiler.parseGD((char *)buffer.str().c_str());
	}else{
		Compiler.speak(string(translate("Erro ao abrir arquivo ")) + string(filename));
	}
	t.close();
  }


  if (Compiler.getLog()){
	// save remaining buffered data on disk and close the log file
	fflush(logFile);
	fclose(logFile);
  }


}

void usage(char *exec){
	sleep(0.5);
	Compiler.speak(string(translate("Uso: ")) + string(exec) + string(translate(" argumentos. \nArgumentos: \n\t-t: Executa em modo terminal; \n\t-f nome do arquivo: Executa em modo script; \n\t-m: Quando habilitado, imprime mensagens na tela;  \n\t-l: Quando habilitado, salva log dos comandos; \n\t-h: Ajuda")));

/*	POR ALGUM MOTIVO O TTS NAO FUNCIONA C STRING NESTE FORMATO	 
	Compiler.speak(
		string(
		 "Uso " + string(exec) + " <arg> \
Argumentos:\
   -t                   : Executa em modo terminal.\
   -f <nome do arquivo> : Executa em modo script.\
   -m                   : Quando habilitado, imprime\
                            mensagens na tela.\
   -h                   : Ajuda \n")	);
	cout << endl
		 << "\nUso: " << exec << " <arg>" << endl
		 << "Argumentos:" << endl
		 << "   -t                   : Executa em modo terminal." << endl 
		 << "   -f <nome do arquivo> : Executa em modo script." << endl
		 << "   -h                   : Ajuda " << endl << endl;
		 */
}

//! this code is executed in terminal mode every time ESC key is pressed
int evalCode(int count, int key) 
{
  if (!code.empty())
  {
    cout << code << endl;
    done = Compiler.parseGD((char *)code.c_str());
    code.clear();
    code = "";
    rl_on_new_line ();
    return 1;
  }
  else{
    rl_on_new_line ();
    code.clear();

    generator gen;
    gen.add_messages_path(string(getenv("DONNIE_PATH")) + "/resources/loc");
    gen.add_messages_domain("GoDonnie");
    locale loc = gen(string(getenv("DONNIE_LANG")) + ".UTF-8");
    locale::global(loc);
    
    string str_speak = translate("\nNão há código para ser executado.");
    Compiler.speak(str_speak);
    return 0;
  }
}


void initialize_readline ()
{
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "FileMan";

  /* Tell the completer that we want a crack first. */
  rl_attempted_completion_function = fileman_completion;
}

char ** fileman_completion(const char *text, int start, int end)
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

char * command_generator(const char *text, int state)
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
  /* In pt_BR */  
  char *lang = getenv("DONNIE_LANG");
  if (strcmp(lang,"pt_BR") == 0)
  {
    while (name = comandos[list_index].name)
    {
      list_index++;

      if (strncmp (name, text, len) == 0)
        return (strdup(name));
    }
  }
  /* In en_US */
  else if (strcmp(lang,"en_US") == 0)
  {
    while (name = commands[list_index].name)
    {
      list_index++;

      if (strncmp (name, text, len) == 0)
        return (strdup(name));
    }
  }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}

/// When the log is enabled, it creates the log file
void setupLog()
{

	// if log is enabled, then create the log file
	if (Compiler.getLog()){
		// find the appropriate file number to the log file. it follows sequential order
		int fileno=0;
		bool success = true;
		char fileName[40] = "";
		struct stat info;

		if( stat( "Log", &info ) != 0 )
			mkdir("Log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //Log directory does not exist. Creating log directory.
		
		ifstream ifs;
		while(success &&fileno < 999) {
		   sprintf(fileName, "Log/log_%03d.txt", fileno);
		   ifs.open(fileName, std::ifstream::in);// attempt read file to check if it exists
		   success = ifs.good();
		   ifs.close();
		   fileno++;//increase by one to get a new file name
		} 

		sprintf(fileName, "Log/log_%03d.txt", fileno-1);
		logFile = fopen(fileName, "w");

		if (logFile == NULL)
		{
			Compiler.speak(string(translate("Não foi possivel criar o arquivo de log")));
		}
		else
			Compiler.logFile(logFile);
	}
      	
}

