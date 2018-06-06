#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  //getopt
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <readline/history.h>
#include <readline/readline.h>

#include "Compiler.h"

//extern HIST_ENTRY **history_list ();

using namespace std;

#define LANG "pt-br"

COMMAND commands[] = {
    {(char *)"pf ", (char *)"Movimenta para frente"},
    {(char *)"pt ", (char *)"Movimenta para trás"},
    {(char *)"gd ", (char *)"Gira para direita"},
    {(char *)"ge ", (char *)"Gira para esquerda"},
    {(char *)"distância", (char *)"Sensor de distância"},
    {(char *)"espiar", (char *)"Observar objetos"},
    {(char *)"cor", (char *)"Procura por uma cor"},
    {(char *)"posição ", (char *)"Posição do robô"},
    {(char *)"estado", (char *)"Informação atual"},
    {(char *)"criar ", (char *)"Criar variável"},
    {(char *)"para ", (char *)"Laço"},
    {(char *)"repita ", (char *)"Laço"},
    {(char *)"enquanto ", (char *)"Laço"},
    {(char *)"se ", (char *)"Condição"},
    {(char *)"aprender ", (char *)"Criar procedimento"},
    {(char *)"falar ", (char *)"Falar"},
    {(char *)"sair", (char *)"Fechar interpretador"},
    {(char *)"som", (char *)"Liga ou desliga o som"},
    {(char *)"histórico", (char *)"Histórico de comandos de movimento"},
    {(char *)NULL, (char *)NULL}};

string code = "";
bool done = 0;
ExprTreeEvaluator Client;

void initialize_readline();
char **fileman_completion(const char *text, int start, int end);
char *command_generator(const char *text, int state);
void usage(char *exec);
int evalCode(int count, int key);

int main(int argc, char *argv[]) {
    bool termMode = 0, scriptMode = 0;
    string filename;
    int c = 0, argcnt = 0;
    /// this is from getopt lib. it is used to reset de idx of the parameter list
    extern int optind;

    if (argc <= 1) {  // there is NO input...
	//Client.speak("No argument provided!");
	Client.speak("Comando sem argumentos!");
	usage(argv[0]);
	return 1;
    }

    while ((c = getopt(argc, argv, "mthf:")) != -1) {
	switch (c) {
	    case 't':  // terminal mode
		termMode = 1;
		if (argc != 2) {  // check extra useless argumets in terminal mode
		    //cerr << "Terminal mode requires only one argument" << endl;
		    Client.speak("Modo terminal requer somente um argumento.");
		    usage(argv[0]);
		    return 1;
		}
		break;
	    case 'f':  // script file  mode
		filename = optarg;
		scriptMode = 1;
		if (argc != 3) {  // check extra useless argumets in script mode
		    //cerr << "Script mode requires only two arguments" << endl;
		    Client.speak("Modo script requer somente dois argumentos.");
		    usage(argv[0]);
		    return 1;
		}
		// test if file exists
		if (access(optarg, F_OK) == -1) {
		    //cerr << "File " << filename << " not found!" << endl;
		    Client.speak("Arquivo " + string(filename) + " não encontrado.");
		    return 1;
		}
		break;
	    case 'm':  // mute
		//if mute ('m') is on, do it first, before any other command
		if (argcnt == 0) {
		    Client.muteTTS(true);
		    break;
		} else {
		    Client.speak("Parâmetro m deve vir primeiro.");
		    return 1;
		}
		break;
	    case 'h':  // help
		usage(argv[0]);
		return 0;
	    case '?':  // error
		if (optopt == 'f') {
		    //fprintf (stderr, "Option -%c requires a filename with GoDonnie code.\n", optopt);
		    Client.speak("Parâmetro -f requer um nome de arquivo com código GoDonnie.");
		} else if (isprint(optopt)) {
		    //fprintf (stderr, "Unknown option `-%c'.\n", optopt);
		    char buffer[50];
		    sprintf(buffer, "Parâmetro `-%c' desconhecido.", optopt);
		    Client.speak(string(buffer));
		} else {
		    //fprintf (stderr,
		    //         "Unknown option character `\\x%x'.\n",
		    //         optopt);
		    char buffer[50];
		    sprintf(buffer, "Parâmetro `\\x%x' desconhecido.", optopt);
		    Client.speak(string(buffer));
		}
		usage(argv[0]);
		return 1;
	    default:
		usage(argv[0]);
		return 1;
	}
	argcnt++;
    }

    if (!termMode && !scriptMode) {
	//cerr << "No mode selected" << endl;
	Client.speak("Nenhum modo selecionado.");
	usage(argv[0]);
	return 1;
    }
    if (termMode && scriptMode) {
	//cerr << "Cannot have both modes selected at the same time" << endl;
	Client.speak("Não pode ter dois modos selecionados ao mesmo tempo.");
	usage(argv[0]);
	return 1;
    }

    initialize_readline();
    static char *temp = (char *)NULL;
    string preCode;

    // terminal mode
    if (termMode) {
	FILE *log;
	//rl_attempted_completion_function = fileman_completion;
	rl_bind_key(27, evalCode); /* "27" ascii code for ESC */
	//rl_unbind_key('\t');
	//rl_bind_key('a',rl_complete);

	// find the appropriate file number to the log file. it follows sequential order
	int fileno = 0;
	bool success = true;
	char fileName[40] = "";

	struct stat info;

	if (stat("Log", &info) != 0)
	    mkdir("Log", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);  //Log directory does not exist. Creating log directory.

	ifstream ifs;
	while (success && fileno < 999) {
	    sprintf(fileName, "Log/log_%03d.txt", fileno);
	    ifs.open(fileName, std::ifstream::in);  // attempt read file to check if it exists
	    success = ifs.good();
	    ifs.close();
	    fileno++;  //increase by one to get a new file name
	}

	sprintf(fileName, "Log/log_%03d.txt", fileno - 1);
	log = fopen(fileName, "w");

	if (log == NULL) {
	    Client.speak("Não foi possivel criar o arquivo de log");
	} else
	    Client.logFile(log);

	while (!done) {
	    // If the buffer has already been allocated, return the memory to the free pool.
	    if (temp) {
		free(temp);
		temp = (char *)NULL;
	    }
	    temp = readline("GoDonnie$ ");

	    if (!temp)
		exit(1);

	    // If the line has any text in it, save it on the history.
	    if (temp && *temp) {
		add_history(temp);
		if (code.empty())
		    code += string(temp);
		else
		    code += "\n" + string(temp);
	    } else
		rl_on_new_line();
	};
	// save remaining buffered data on disk and close the log file
	fflush(log);
	fclose(log);
    } else if (scriptMode) {  // script mode
	// read the entire file
	std::ifstream t(filename.c_str());
	std::stringstream buffer;

	if (t.is_open()) {
	    // read the entire file
	    buffer << t.rdbuf();
	    Client.parseGD((char *)buffer.str().c_str(), false);
	} else {
	    Client.speak("Erro ao abrir arquivo " + string(filename));
	}
	t.close();
    }
}

void usage(char *exec) {
    sleep(0.5);
    Client.speak("Uso: " + string(exec) + " argumentos. \nArgumentos: \n\t-t: Executa em modo terminal; \n\t-f nome do arquivo: Executa em modo script; \n\t-m: Quando habilitado, imprime mensagens na tela; \n\t-h: Ajuda");

    /*	POR ALGUM MOTIVO O TTS NAO FUNCIONA C STRING NESTE FORMATO	 
	Client.speak(
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
int evalCode(int count, int key) {
    if (!code.empty()) {
	cout << code << endl;
	done = Client.parseGD((char *)code.c_str(), true);
	code.clear();
	code = "";
	rl_on_new_line();
	return 1;
    } else
	rl_on_new_line();
    code.clear();
    Client.speak("\nNão há código para ser executado.");
    return 0;
}

void initialize_readline() {
    /* Allow conditional parsing of the ~/.inputrc file. */
    rl_readline_name = "FileMan";

    /* Tell the completer that we want a crack first. */
    rl_attempted_completion_function = fileman_completion;
}

char **fileman_completion(const char *text, int start, int end) {
    char **matches;

    matches = (char **)NULL;

    /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
    //if (start == 0)
    matches = rl_completion_matches(text, command_generator);

    return (matches);
}

char *command_generator(const char *text, int state) {
    static int list_index, len;
    char *name;

    /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
    if (!state) {
	list_index = 0;
	len = strlen(text);
    }

    /* Return the next name which partially matches from the command list. */
    while (name = commands[list_index].name) {
	list_index++;

	if (strncmp(name, text, len) == 0)
	    return (strdup(name));
    }

    /* If no names matched, then return NULL. */
    return ((char *)NULL);
}
