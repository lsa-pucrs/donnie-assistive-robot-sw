#include <algorithm> // remove
#include <sstream>
#include <iomanip>
#include "Compiler.h"
#include "Exception.h"
#include "DonnieClient.h"
#include "Historic.h"

// this parser uses antlr 3.4, with support to C code generation
// manual is available at
//https://theantlrguy.atlassian.net/wiki/display/ANTLR3/ANTLR+v3+printable+documentation
//http://www.antlr3.org/api/C/index.html
// this is the source code of -lantlr3 library. run doxygen to generate the docs
//https://github.com/antlr/antlr3/tree/master/runtime/C

using std::map;
using std::vector;
using std::stack;
using std::string;
using std::cout;
using std::endl;
using std::string;

/// definition of the ranger tokens in Portuguese
#define RANGER_N "f"   // 0
#define RANGER_S "t"   // 1
#define RANGER_NW "fe" // 2
#define RANGER_NE "fd" // 3
#define RANGER_SW "te" // 4
#define RANGER_SE "td" // 5
//#define RANGER_HEAD "c" // 6

/// definition of the position tokens in Portuguese
#define POSITION_X "x"
#define POSITION_Y "y"
#define POSITION_YAW "a"
#define POSITION_X_CAP "X"
#define POSITION_Y_CAP "Y"
#define POSITION_YAW_CAP "A"

/// definition of the color tokens in Portuguese
//#define COLOR_BLUE "azul"
//#define COLOR_RED "vermelho"
//#define COLOR_GREEN "verde"

/// definition of the on/off tokens in Portuguese
//#define TOKEN_ON "ligado"
//#define TOKEN_OFF "desligado"

/// definition of variable return tokens
#define IDLE 4
#define WAITING 3
#define ASSIGNED 2
#define CREATED 1
#define NEXIST 0
#define EXIST -1

DonnieMemory *DonnieMemory::singleton = 0;

extern "C" char messageError[2000]; //global variable for error messages generated by antlr (see GoDonnie.g)
									//It cames from GoDonnieParser.c to be printed here
extern "C" char flagMsgError; //flag to clean the 'messageError' string after it is printed by compiler

ExprTreeEvaluator::ExprTreeEvaluator()
{

	Donnie = DonnieClient::getInstance();
	History = Historic::getInstance();

  	for_itFlag = IDLE;
    done = 0;
    log = NULL;
    enable_log = false;
}

ExprTreeEvaluator::~ExprTreeEvaluator()
{
	DonnieClient::ResetInstance();
	Donnie = NULL;

	Historic::ResetInstance();
	History = NULL;
}

void ExprTreeEvaluator::logFile(FILE *file)
{
  log = file;
}


void ExprTreeEvaluator::setLog(bool log)
{
  enable_log = log;
}


bool ExprTreeEvaluator::getLog()
{
  return enable_log;
}


int ExprTreeEvaluator::parseGD(char* textIn)
{
  // Set up language environment
  generator gen;
  gen.add_messages_path(string(getenv("DONNIE_PATH")) + "/resources/loc");
  gen.add_messages_domain("Compiler");
  locale loc = gen(string(getenv("DONNIE_LANG")) + ".UTF-8");
  locale::global(loc);
  cout.imbue(loc);

  // remove accented characters
  string code = cleanAccents(string(textIn));
  uint8_t* bufferData = (uint8_t*)code.c_str();
  uint32_t bufferSize = strlen(code.c_str());
  //uint8_t* bufferData = (uint8_t*)textIn;
  //uint32_t bufferSize = strlen(textIn);
  pANTLR3_UINT8 bufferName = (pANTLR3_UINT8)"INPUT text";
  pANTLR3_INPUT_STREAM input = antlr3NewAsciiStringInPlaceStream( bufferData, bufferSize, bufferName);
  if (input == NULL) {
      cout << translate("Out of memory trying to allocate ANTLR input stream\n");
	  exit(ANTLR3_ERR_NOMEM);
  }

  //TODO: amory. com esta funcao eu acredito que eh possivel tornar a linguagem case insentive,
  // simplificando o arquivo .g
  //https://github.com/antlr/antlr3/blob/master/runtime/C/include/antlr3input.h#L152
  //https://track.hpccsystems.com/browse/WSSQL-118
  //input->setUcaseLA(input,ANTLR3_TRUE)

  	pGoDonnieLexer lex = GoDonnieLexerNew(input);
  	if (lex == NULL)
  	{
      cout << translate("Unable to create the lexer due to malloc() failure\n");
	  exit(ANTLR3_ERR_NOMEM);

  	}

  	pANTLR3_COMMON_TOKEN_STREAM tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
  	if (tokens == NULL)
  	{
      cout << translate("Out of memory trying to allocate token stream\n");
	  exit(ANTLR3_ERR_NOMEM);
  	}

  	// Finally, now that we have our lexer constructed, we can create the parser
  	pGoDonnieParser parser = GoDonnieParserNew(tokens);
  	if (parser == NULL)
  	{
  	  cout << translate("Out of memory trying to allocate parser\n");
	  exit(ANTLR3_ERR_NOMEM);
  	}


	//try to parse the GoDonnie code
	GoDonnieParser_start_rule_return r;
	try{
		// parsing
		r = parser->start_rule(parser);
		if (r.tree == NULL)
		{
		  cout << translate("Parse error") << endl;
		  exit(ANTLR3_ERR_NOMEM);
		}
		// print the parse tree
		#ifndef NDEBUG
			cout << translate("Tree : ") << r.tree->toStringTree(r.tree)->chars << endl;
		#endif
	}
	catch(exception& e)
	{
		cout << e.what();
		return -1;
	}

    // If the parser ran correctly, we will have a tree to parse. In general I recommend
    // keeping your own flags as part of the error trapping, but here is how you can
    // work out if there were errors if you are using the generic error messages
    // http://www.antlr3.org/api/Java/org/antlr/runtime/BaseRecognizer.html
    // http://www.antlr3.org/api/Java/org/antlr/runtime/RecognizerSharedState.html
    int errors = parser->pParser->rec->getNumberOfSyntaxErrors(parser->pParser->rec) + lex->pLexer->rec->getNumberOfSyntaxErrors(lex->pLexer->rec);
    if (errors > 0)
    {
		cout << messageError << endl; //messageError is a global variable from GoDonnieParser.c. It contains the errors messages generated by antlr in GoDonnie.g

		Donnie->speak(messageError);
		flagMsgError = 1; //flag to clean the 'messageError' string in grammar file (GoDonnie.g) after it is printed here
	}

    else{
      //cout << translate("Tree : ") << r.tree->toStringTree(r.tree)->chars << endl;

		// if text is parsed without error and logging is enabled, then save log
		// log is enabled only in terminal mode and not in the script mode
		if (getLog()){
		  if(log != NULL)
		  {
			if(fprintf(log, "%s\n", textIn) == EOF)
			  cout << translate("Erro ao salvar comando no log") << endl;
			else
			  fflush(log);
		  }
		}

		//if all tests passed, try to run the GoDonnie code
		try{
			this->run(r.tree);
		}
		catch(exception& e)
		{
			Donnie->speak(e.what());
		}
	}

  	parser->free(parser);
  	tokens->free(tokens);
  	lex->free(lex);
  	input->close(input);

  	return done;
}

//const char* reservedWords[] = {"sair", "criar", "falar", "som", "pf", "pt", "gd", "fe", "espiar", "estado", "distancia", "pos", "cor", "se", "entao", "senao", "fim", "para", "faça", "repita", "vezes", "enquanto", "aprender", "esperar" };
std::vector<std::string> reservedWords = {"vibrar", "vibrate", "distance", "scan", "tl", "tr", "bw", "fw", "quit", "exit", 
	                                      "sair", "criar", "falar", "som", "pf", "pt", "gd", "fe", "espiar", "estado", 
	                                      "distancia", "pos", "position", "posicao", "cor", "se", "entao", "senao", "fim",
	                                      "para", "faça", "repita", "vezes", "enquanto", "aprender", "esperar", "var", "wait",
	                                      "repeat", "end", "if", "else", "times", "then", "belt", "sound", "color", "state",
	                                       "historico", "history", "speak", "while", "for", "procedure", "cinto"};
bool reserved(char* str){
	for(int i = 0; i < reservedWords.size(); i++){
		
		//cout << i << ": " << reservedWords.size() << " " << str << " " << reservedWords[i] << endl;
		//if(strcmp(str, reservedWords[i]) == 0){
		if(string(str) == reservedWords[i]){
			//cout << "Entrou " <<  str << endl;
			//cout << reservedWords[i] << endl;
			//cout << "ENTROU" << endl;
			return true;
		}
	}
	return false;
}

int ExprTreeEvaluator::run(pANTLR3_BASE_TREE tree)
{
    // Set up language environment
    generator gen;
    gen.add_messages_path(string(getenv("DONNIE_PATH")) + "/resources/loc");
    gen.add_messages_domain("Compiler");
    locale loc = gen(string(getenv("DONNIE_LANG")) + ".UTF-8");
    locale::global(loc);
    cout.imbue(loc);

    string crash_str = translate("bateu");
    string ncrash_str = translate("não bateu");

	std::ostringstream sayStr;
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    //printf("%d", tok->type);
    if(tok) {
        switch(tok->type) {

          case INT:
          {
            return atoi(getText(tree));
          }

          case ID:
          {
            return DonnieMemory::getInstance()->getVar(getText(tree));
            break;
          }

          case STRINGE:
          {
            break;
          }

          case PLUS:
          {
            if (tree->getChildCount(tree) < 2)
              return run(getChild(tree,0));
            else
              return run(getChild(tree,0)) + run(getChild(tree,1));
          }

          case MINUS:
          {
            if (tree->getChildCount(tree) < 2)
             return -1*(run(getChild(tree,0)));
            else
             return run(getChild(tree,0)) - run(getChild(tree,1));
          }

          case MULT:
          {
            return run(getChild(tree,0)) * run(getChild(tree,1));
          }

          case DIV:
          {
            return run(getChild(tree,0)) / run(getChild(tree,1));
          }

          case FW:
          { // forward
			  //printf("TESTE");
			#ifndef NDEBUG
            cout << translate("PF: ") << run(getChild(tree,0)) << endl;
            #endif
            // run the command
            int distance = (int)run(getChild(tree,0));
            int steps_taken = Donnie->moveForward(distance);
            // if less steps were taken, then report a bump
            std::ostringstream distanceStr;

            distanceStr.imbue(loc);

            distanceStr << translate("andou ") << steps_taken;
            if ((steps_taken < distance) || Donnie->bumped()){
              distanceStr << translate(", bateu");
            }else{
		      distanceStr << translate(", não bateu");
            }
			// save into history
            string command = string(getText(tree)) + " " + to_string(int(distance));
            History->addCommand(command,distanceStr.str());
            break;
          }

          case BW:
          { // backward
			#ifndef NDEBUG
            cout << translate("PT: ") << run(getChild(tree,0)) << endl;
            #endif
            // run the command
            int distance = (int)run(getChild(tree,0));
            int steps_taken = Donnie->moveBackward(distance);
            // if less steps were taken, then report a bump
            std::ostringstream distanceStr;

            distanceStr.imbue(loc);

            distanceStr << translate("andou ") << steps_taken;
            if ((steps_taken < distance) || Donnie->bumped()){
              distanceStr << translate(", bateu");
            }else{
		      distanceStr << translate(", não bateu");
            }
			// save into history
            string command = string(getText(tree)) + " " + to_string(int(distance));
            History->addCommand(command,distanceStr.str());
            break;
          }

          case RTURN:
          { // right turn
			int exp = run(getChild(tree,0));
			#ifndef NDEBUG
            cout << translate("PD: ") << exp << endl;
            #endif
            // run the command
            float distance = (float)exp;
            Donnie->GotoTTS(-1*distance);
			// save into history
			// TODO: o comando pode ser interrompido por uma colizao.
			// assim, está errado assumir que a distancia pedida será a distancia percorrida
            string command = string(getText(tree)) + " " + to_string(int(distance));
            History->addCommand(command,(Donnie->bumped() ? crash_str : ncrash_str));
            break;
          }

          case LTURN:
          { // left turn
			int exp = run(getChild(tree,0));
			#ifndef NDEBUG
            cout << translate("PE: ") << exp << endl;
            #endif
            // run the command
            float distance = (float)exp;
            Donnie->GotoTTS(distance);
			// save into history
			// TODO: o comando pode ser interrompido por uma colizao.
			// assim, está errado assumir que a distancia pedida será a distancia percorrida
            string command = string(getText(tree)) + " " + to_string(int(distance));
            History->addCommand(command,(Donnie->bumped() ? crash_str : ncrash_str));
            break;

          }

          case SCAN:
          {

			// get 7 sonar and blob readings by moving the head from 0o to 180o
			// every 30 degrees
			Donnie->Scan();
            break;
          }

          case RANGER:
          {
            int arg;
            float range;
            vector<string> tokens;
            split((char*)getText(tree),' ',tokens);
			//if (tokens.size() != 2)
				//throw sintaxeException("Sintaxe não conhecida ild(trepara comando '"+tokens[0]+"'\n");

			// get the ranger id
			if (tokens[1] == RANGER_N)
				arg = 0;
			else if (tokens[1] == RANGER_S)
				arg = 1;
			else if (tokens[1] == RANGER_NW)
				arg = 2;
			else if (tokens[1] == RANGER_NE)
				arg = 3;
			else if (tokens[1] == RANGER_SW)
				arg = 4;
			else if (tokens[1] == RANGER_SE)
				arg = 5;
			//else if (tokens[1] == RANGER_HEAD)
			//	arg = 6;
			//else
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");

            range  = Donnie->GetRange(arg);
			#ifndef NDEBUG
            cout << "RANGER: " << arg << " " << (int)range << endl;
            #endif
            return (int)range;
          }

          case VIBRATE:
          {
            int arg;
            vector<string> tokens;
            split((char*)getText(tree),' ',tokens);
			//if (tokens.size() != 2)
				//throw sintaxeException("Sintaxe não conhecida ild(trepara comando '"+tokens[0]+"'\n");

			// get the ranger id
			// TODO ajustar a numeracao dos motores de vibracao
			if (tokens[1] == RANGER_N)
				arg = 8;
			else if (tokens[1] == RANGER_S)
				arg = 9;
			else if (tokens[1] == RANGER_NW)
				arg = 10;
			else if (tokens[1] == RANGER_NE)
				arg = 11;
			else if (tokens[1] == RANGER_SW)
				arg = 12;
			else if (tokens[1] == RANGER_SE)
				arg = 13;
			//else if (tokens[1] == RANGER_HEAD)
			//	arg = 6;
			//else
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");

			// generate a vibration pulse of 1 second
			//TODO test this pulse strategy . 1 second seem to be too long.
            Donnie->vibrate(arg,'A');
            //sleep(1);
            //Donnie->vibrate(arg,'P');
            //sleep(1);
            #ifndef NDEBUG
            cout << "VIBRATE: " << arg << endl;
            #endif
            break;
          }
          
          case POS:
          {
            int arg;
            float pos;
            char* tokens;
            //tokens = strtok((char*)getText(tree)," ");
            tokens = strpbrk((char*)getText(tree), "xyaXYA"); //verifica qual é o argumento de POS: x, y, a ou NULL

		//if (tokens.size() != 2)
			//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");

		// get the ranger id
		if (tokens != NULL){
			if ((strcmp(tokens,POSITION_X) == 0) || (strcmp(tokens,POSITION_X_CAP) == 0))
				arg = 0;
			else if ((strcmp(tokens,POSITION_Y) == 0) || (strcmp(tokens,POSITION_Y_CAP) == 0))
				arg = 1;
			else if ((strcmp(tokens,POSITION_YAW) == 0) || (strcmp(tokens,POSITION_YAW_CAP) == 0))
				arg = 2;
		}
		    else
				arg = 3;

			//else
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");

	    pos = Donnie->GetPos("body",arg);
            #ifndef NDEBUG
               cout << "POS: " << arg << " " << (int)pos << endl;
            #endif
            return (int)pos;
          }

          case COLOR:
          {

            int arg;
			// get 7 sonar and blob readings by moving the head from 0o to 180o
			// every 30 degrees. report only the blobs with requested color (r,g, or b)

            vector<string> tokens;
            split((char*)getText(tree),' ',tokens);
			//if (tokens.size() != 2)
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");

			arg = Donnie->color_to_value(tokens[1]);
			if(arg == 0xFFFFFFFF)
				throw sintaxeException(string(translate("Sintaxe não conhecida para comando '"))+tokens[0]+"'\n");

			// only report when there are blobs with the selected color
            return Donnie->Color(arg);
            break;
          }

          case SOUND:
          {

            vector<string> tokens;
            split((char*)getText(tree),' ',tokens);
			//if (tokens.size() != 2)
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");

			// sound on or off
			if ((tokens[1] == "ligado") || (tokens[1] == "on")){
				Donnie->muteTTS(false);
				Donnie->speak(translate("O som foi ligado com sucesso"));
				
			}
			else if ((tokens[1] == "desligado") || (tokens[1] == "off")){
				Donnie->muteTTS(true);
				Donnie->speak(translate("O som foi desligado com sucesso"));
				
			}
			// error treatmet is performed at the parser level. it is not necessary to test for error here
			//else {
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");
				//Donnie->speak(translate("Sintaxe não conhecida para comando 'som'"));
			//}

            return 0;
            break;
          }

          case BELT:
          {
            vector<string> tokens;
            split((char*)getText(tree),' ',tokens);
			//if (tokens.size() != 2)
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");

			// sound on or off
			if ((tokens[1] == "ligado") || (tokens[1] == "on")){
				Donnie->belt(true);
				Donnie->speak(translate("O cinto foi ligado com sucesso"));
			}
			else if ((tokens[1] == "desligado") || (tokens[1] == "off")){
				Donnie->belt(false);
				Donnie->speak(translate("O cinto foi desligado com sucesso"));
			}
			// error treatmet is performed at the parser level. it is not necessary to test for error here
			//else{
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");
				//Donnie->speak(translate("Sintaxe não conhecida para comando 'cinto'"));
			//}

            return 0;
            break;
          }

          case COMENT:
          {
			#ifndef NDEBUG
			cout << translate("COMMENT: ") << getText(tree) << endl;
			#endif
            break;
          }

          case SPEAKE:
          {
            std::ostringstream arg;
            // if it is a string, get the text, otherwise, parse the expression
            if(tree->getToken(getChild(tree,0))->type == STRINGE) {
              string auxstr;
              auxstr = getText(getChild(tree,0));
              // Remove all double-quote characters
			  auxstr.erase(
				remove( auxstr.begin(), auxstr.end(), '\"' ),
				auxstr.end()
				);
			  arg << auxstr;
            }else{
			  arg << run(getChild(tree,0));
		    }
		    #ifndef NDEBUG
			cout << translate("SPEAK: ") << arg.str() << endl;
			#endif
            Donnie->speak(arg.str());
            break;
          }

          case WAIT:
          {
			if(tree->getToken(getChild(tree,0))->type != STRINGE){
				int wait = run(getChild(tree,0));
				#ifndef NDEBUG
				cout << translate("WAIT: ") << wait << endl;
				#endif
				sleep(wait);
				Donnie->speak(string(translate("esperando ")) +  to_string (wait) + string(translate(" segundos.")));
			}
            break;
          }


          case FORE:
          {

            for_itFlag = WAITING;

            run(getChild(tree,0));                              // Executa condição inicial

            if(for_itFlag == EXIST)
              break;

            int a = run(getChild(tree,1));                      // Retorna o valor das variáveis na condição
            int b = run(getChild(tree,3));                      // #
            string c = (string)getText(getChild(tree,2));       // Retorna operador de condição

            bool ok = compare(a,b,c);       // Compara variaveis

            while (ok == true)
            {
              run(getChild(tree,5));                                        //Executa bloco
              run(getChild(tree,4));                                        //Executa operação de incremento
              ok = compare(run(getChild(tree,1)),run(getChild(tree,3)),c);  //Realiza comparação novamente
            }

            if(for_itFlag == CREATED)
              DonnieMemory::getInstance()->purgeForVar();
            for_itFlag = IDLE;

            break;
          }

          case WHILEE:
          {
			#ifndef NDEBUG
            cout << translate("while") << endl;
			#endif
            int a = run(getChild(tree,0));                      // Retorna o valor das variáveis na condição
            int b = run(getChild(tree,2));                      // #
            string c = (string)getText(getChild(tree,1));       // Retorna operador de condição

            bool ok = compare(a,b,c);       // Compara variaveis

            while (ok == true)
            {
              run(getChild(tree,3));                                        //Executa bloco
              ok = compare(run(getChild(tree,0)),run(getChild(tree,2)),c);  //Realiza comparação novamente
            }

            break;
          }

          case REPEAT:
          {
            for (int r = 0; r < run(getChild(tree,0)); r++)
            {
              run(getChild(tree,1));
            }
            break;
          }

          case IFE:
          {
            int a = run(getChild(tree,0));                  // Retorna o valor das variáveis na condição
            int b = run(getChild(tree,2));                  // #
            string c = (string)getText(getChild(tree,1));   // Retorna operador de condição

            bool ok = compare(a,b,c);           // Compara variaveis

            if (ok == true)
            {
              run(getChild(tree,3));            // Caso verdadeiro executa bloco THEN
              break;
            }
            if (tree->getChildCount(tree) == 5) // Caso exista e condição for falsa executa bloco ELSE
            {
              if (ok == false)
              {
                run(getChild(tree,4));
              }
            }
            break;
          }

          case THEN:
          case ELSEE:
          case DOIT:
          case REPTB:
          {
			#ifndef NDEBUG
            cout << "N Fi: " << tree->getChildCount(tree) << endl;
            #endif
            for (int f = 0; f < tree->getChildCount(tree); f++)
              {
				#ifndef NDEBUG
                cout << getText(getChild(tree,f)) << endl;
                #endif
                run(getChild(tree,f));
              }
            break;
          }

          case MAKE:
          {

			char* var = (char*)getText(getChild(tree,0));
			if (reserved(var)){
				//cout << "nome da var: " << var << endl;
				sayStr << translate("Nome da variável não é válido");
				Donnie->speak(sayStr.str());
				break;
			}
			
            int val;
            if (tree->getChildCount(tree) < 2)
            {
              val = 0;                           // Se não for atribuido valor, variável começa com zero
            }
            else
              val = run(getChild(tree,1));

            if(for_itFlag == WAITING)
              for_itFlag = DonnieMemory::getInstance()->addForVar(var,val);
            else{
			  //cout << "Estou criando a var ... " << endl;
              DonnieMemory::getInstance()->addVar(var,val);
		    }

            break;
            }

          case PROCINV:
          {
            char* name = (char*)getText(getChild(tree,0));
            mem local;                                      // Inicia dicionário local

            procDec procedure = DonnieMemory::getInstance()->getProc(name);

            if(procedure.argNum == tree->getChildCount(tree)-1)
            {
              for (int f = 1; f <= procedure.argNum ; f++)
                local.memory[procedure.args[f-1]] = run(getChild(tree,f));   // Atribui as variaveis de argumento no dicionário

              DonnieMemory::getInstance()->stackMemory(local);                      // Empilha dicionário

              run(procedure.node);                      // Executa procedimento

              DonnieMemory::getInstance()->unstackMemory();                            // Desempilha dicionário
            }
            else
            {
              sayStr << translate("Número de argumentos invalido.");
              Donnie->speak(sayStr.str());
              cout << sayStr.str() << endl;

            }

            break;
          }

          case PROCDEC:
          {
            char* name = (char*)getText(getChild(tree,0));

            int childNum = tree->getChildCount(tree);

            procDec procedure;

            if(childNum < 3)
            {
              procedure.argNum = 0;                    // Caso procedimento não apresente parametros salva bloco do procedimento
              procedure.node = getChild(tree,1);
            }
            else
            {
              for(int f = 1; f < childNum - 1; f++)
                procedure.args.push_back(getText(getChild(tree,f)));   // Caso tenha parametros salva numa lista

              procedure.argNum = childNum - 2;
              procedure.node = getChild(tree,childNum - 1);      // Salva bloco do procedimento
            }

            DonnieMemory::getInstance()->addProc(name, procedure);
            break;
          }

          case HIST:
          {
				//TODO (amory): implementar um clear para o historico
				//TODO amory. usar TTS o histórico
				cout << History->show();
				break;
		  }

          case STATE:
          {
			  std::ostringstream output;

			  output.imbue(loc);

			  if (History->size()>0){
				output << History->getLast();
			  }else{
				output << translate("Nenhum comando executado, posição [")
				       << Donnie->GetPos("body",0)  <<  //POSITION_X in steps
				   "," << Donnie->GetPos("body",1)  <<  //POSITION_Y in steps
				   "," << Donnie->GetPos("body",2) << "]"; //POSITION_YAW in degrees
				   // dont add newline here !
			  }
				
				//cout << output.str() << endl;
				Donnie->speak(output.str());
				break;
		  }

          case SEMICOLON:
          {

              //int fi = tree->getChildCount(tree);
              //cout << "COMMENT: " << fi << std::endl;
              break;
          }

          case WS:
          {

              //int fi = tree->getChildCount(tree);
              //cout << "WS: " << fi << std::endl;
              break;
          }

          case COMP:
          {

              //return getText(tree);
              break;
          }

          case QUIT:
          {
            #ifndef NDEBUG
            cout << translate("EXIT") << endl;
            #endif
            //fclose(log);
            done = 1;
            break;
          }

          case EQ:
          {

            char* var = (char*)getText(getChild(tree,0));
            int val = run(getChild(tree,1));

            DonnieMemory::getInstance()->assignVar(var,val);
            break;
          }

          default:
              //cout << "Unhandled token: #" << tok->type << '\n';
               throw sintaxeException(translate("Sintaxe não conhecida\n"));
               break;
        }
    }
    else {
        int k = tree->getChildCount(tree);
        int r = 0;
        for(int i = 0; i < k; i++) {
            r = run(getChild(tree, i));
        }
        return r;
    }
}

void ExprTreeEvaluator::speak(string text)
{
	Donnie->speak(text);
}

void ExprTreeEvaluator::muteTTS(bool m)
{
	Donnie->muteTTS(m);
}

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i)
{

    //assert(i < tree->getChildCount(tree));
    //if(!(i < tree->getChildCount(tree))) throw sintaxeException();

    return (pANTLR3_BASE_TREE) tree->getChild(tree, i);
}

const char* getText(pANTLR3_BASE_TREE tree)
{
    return (const char*) tree->getText(tree)->chars;
}

bool compare (int a, int b, string comp)
{
  if (comp == "<")
  {
    return (a < b);
  }
  else if (comp == "<=")
  {
    return (a <= b);
  }
  else if (comp == ">")
  {
    return (a > b);
  }
  else if (comp == ">=")
  {
    return (a >= b);
  }
  else if (comp == "==")
  {
    return (a == b);
  }
  else if (comp == "<>")
  {
    return (a != b);
  }
  else
  {
    cout << translate("null comp") << endl;
    //TODO: (amory) nao era p gerar uma excecao ?
    return false;
  }
}

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss;

    std::string item, aux = s;

	// remove extra spaces
	aux.erase(std::unique(aux.begin(), aux.end(),
			[](char a, char b) { return a == ' ' && b == ' '; } ), aux.end() );
	// tolower
	transform(aux.begin(), aux.end(), aux.begin(), ::tolower);
    ss.str(aux);
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


string cleanAccents(string code){

	//this function does not replace character if it is within double quotes, like in the Speak command
	// note that acented character uses 2 bytes, thus, extra space is required for each accented character
    size_t index = 0, index_aux1 = 0, index_aux2 = 0;
    string aux = code;

    //remove all sentences between double quotes from aux string
    while ((index_aux1 = aux.find('"', index_aux2)) != string::npos){
        if ((index_aux2 = aux.find('"', index_aux1+1)) != string::npos){
			aux.replace(index_aux1, (index_aux2-index_aux1+1), (index_aux2-index_aux1+1), '-');
			index_aux2++;
		}
	}

	int i=0, count_replaces=0;
	//search in the aux string for the commands with accented characters and replace them for not accented characters in the code string
    while((index = aux.find_first_of("âãáçóÂÃÁÇÓ", index)) != string::npos){
		i = index-count_replaces; //this line is important to match the indexes of aux and code strings, because accented characters occupy
								  //2 bytes insted of 1 byte, so replacing them makes the indexes to not match
    	if((code.compare(i,2,"â") == 0) || (code.compare(i,2,"ã") == 0) || (code.compare(i,2,"á") == 0))
    		code.replace(i,2,"a");
    	else if((code.compare(i,2,"Â") == 0) || (code.compare(i,2,"Ã") == 0) || (code.compare(i,2,"Á") == 0))
    		code.replace(i,2,"A");
    	else if(code.compare(i,2,"ç") == 0)
    		code.replace(i,2,"c");
    	else if(code.compare(i,2,"Ç") == 0)
    		code.replace(i,2,"C");
    	else if(code.compare(i,2,"ó") == 0)
    		code.replace(i,2,"o");
    	else if(code.compare(i,2,"Ó") == 0)
    		code.replace(i,2,"O");
    	index+=2; //accented characters have 2 bytes
    	count_replaces++;
    }

	return code;
}
