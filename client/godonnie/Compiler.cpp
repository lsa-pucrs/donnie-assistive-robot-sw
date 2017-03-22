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

/// definition of the color tokens in Portuguese
#define COLOR_BLUE "azul"
#define COLOR_RED "vermelho"
#define COLOR_GREEN "verde"

/// definition of the on/off tokens in Portuguese
#define SOUND_ON "ligado"
#define SOUND_OFF "desligado"

/// definition of variable return tokens
#define IDLE 4
#define WAITING 3
#define ASSIGNED 2
#define CREATED 1
#define NEXIST 0
#define EXIST -1

DonnieMemory *DonnieMemory::singleton = 0;


ExprTreeEvaluator::ExprTreeEvaluator()
{

	Donnie = DonnieClient::getInstance();
	History = Historic::getInstance();

  	for_itFlag = IDLE;
    done = 0;
    log = NULL;
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

int ExprTreeEvaluator::parseGD(char* textIn, bool enable_log)
{
  // remove accented characters
  string code = cleanAccents(string(textIn));
  uint8_t* bufferData = (uint8_t*)code.c_str();
  uint32_t bufferSize = strlen(code.c_str());
  //uint8_t* bufferData = (uint8_t*)textIn;
  //uint32_t bufferSize = strlen(textIn);
  pANTLR3_UINT8 bufferName = (pANTLR3_UINT8)"INPUT text";
  pANTLR3_INPUT_STREAM input = antlr3NewAsciiStringInPlaceStream( bufferData, bufferSize, bufferName);
  if (input == NULL) {
      cout << "Out of memory trying to allocate ANTLR input stream\n";
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
      cout << "Unable to create the lexer due to malloc() failure\n";
	  exit(ANTLR3_ERR_NOMEM);
  	  
  	} 

  	pANTLR3_COMMON_TOKEN_STREAM tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
  	if (tokens == NULL)
  	{
      cout << "Out of memory trying to allocate token stream\n";
	  exit(ANTLR3_ERR_NOMEM);  	  
  	}

  	// Finally, now that we have our lexer constructed, we can create the parser
  	pGoDonnieParser parser = GoDonnieParserNew(tokens);
  	if (parser == NULL)
  	{
  	  cout << "Out of memory trying to allocate parser\n";
	  exit(ANTLR3_ERR_NOMEM);
  	}


	//try to parse the GoDonnie code
	GoDonnieParser_start_rule_return r;
	try{
		// parsing
		r = parser->start_rule(parser);
		if (r.tree == NULL)
		{
		  cout << "Parse error" << endl;
		  exit(ANTLR3_ERR_NOMEM);
		}		
		// print the parse tree
		#ifndef NDEBUG
			cout << "Tree : " << r.tree->toStringTree(r.tree)->chars << endl;
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
    if (errors == 1)
    {
		Donnie->speak(to_string (errors) + " erro foi encontrado.");
		// será q isso funciona p pegar a linha ? http://puredanger.github.io/tech.puredanger.com/2007/02/01/recovering-line-and-column-numbers-in-your-antlr-ast/
		// este exemplo tb extende a classe token, para incluir informacoes uteis p msg de erro
		// http://www.milk.com/kodebase/antlr-tutorial/
		// http://www.milk.com/kodebase/antlr-tutorial/ExtentToken.java
		// http://www.milk.com/kodebase/antlr-tutorial/ErrorFormatter.java
		//cout << "Error in line " << parser->pParser->rec->state->tokenStartLine << " near " << parser->pParser->rec->state->text << endl;
    } else if (errors > 1)
    {
		Donnie->speak(to_string (errors) + " erros foram encontrados.");
 
    }else{
		// if text is parsed without error and logging is enabled, then save log
		// log is enabled only in terminal mode and not in the script mode
		if (enable_log){
		  if(log != NULL)
		  {
			if(fprintf(log, "%s", textIn) == EOF)
			  cout << "Erro ao salvar comando no log" << endl;
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

int ExprTreeEvaluator::run(pANTLR3_BASE_TREE tree)
{
	
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
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
			#ifndef NDEBUG
            cout << "PF: " << run(getChild(tree,0)) << endl;
            #endif
            // run the command
            int distance = (int)run(getChild(tree,0));
            int steps_taken = Donnie->moveForward(distance);
            // if less steps were taken, then report a bump
            std::ostringstream distanceStr;
            distanceStr << "andou " << steps_taken;
            if ((steps_taken < distance) || Donnie->bumped()){
              distanceStr << ", bateu";
            }else{
		      distanceStr << ", não bateu";
            }
			// save into history
            string command = string(getText(tree)) + " " + to_string(int(distance));
            History->addCommand(command,distanceStr.str());
            break;
          }

          case BW:
          { // backward
			#ifndef NDEBUG
            cout << "PT: " << run(getChild(tree,0)) << endl;
            #endif
            // run the command
            int distance = (int)run(getChild(tree,0));
            int steps_taken = Donnie->moveBackward(distance);
            // if less steps were taken, then report a bump
            std::ostringstream distanceStr;
            distanceStr << "andou " << steps_taken;
            if ((steps_taken < distance) || Donnie->bumped()){
              distanceStr << ", bateu";
            }else{
		      distanceStr << ", não bateu";
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
            cout << "PD: " << exp << endl;
            #endif
            // run the command
            float distance = (float)exp;
            Donnie->GotoTTS(-1*distance);
			// save into history
			// TODO: o comando pode ser interrompido por uma colizao.
			// assim, está errado assumir que a distancia pedida será a distancia percorrida
            string command = string(getText(tree)) + " " + to_string(int(distance));
            History->addCommand(command,(Donnie->bumped() ? "bateu" : "não bateu"));            
            break;
          }

          case LTURN:
          { // left turn
			int exp = run(getChild(tree,0));
			#ifndef NDEBUG
            cout << "PE: " << exp << endl;
            #endif
            // run the command
            float distance = (float)exp;
            Donnie->GotoTTS(distance);
			// save into history
			// TODO: o comando pode ser interrompido por uma colizao.
			// assim, está errado assumir que a distancia pedida será a distancia percorrida
            string command = string(getText(tree)) + " " + to_string(int(distance));
            History->addCommand(command,(Donnie->bumped() ? "bateu" : "não bateu"));            
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
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n"); 

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

          case POS:
          {
            int arg;
            float pos;
            vector<string> tokens;
            split((char*)getText(tree),' ',tokens);

			//if (tokens.size() != 2)
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n"); 

			// get the ranger id
			if (tokens[1] == POSITION_X)
				arg = 0;
			else if (tokens[1] == POSITION_Y)
				arg = 1;
			else if (tokens[1] == POSITION_YAW)
				arg = 2;
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
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");     

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
			if (tokens[1] == SOUND_ON)
				Donnie->muteTTS(false);
			else if (tokens[1] == SOUND_OFF)
				Donnie->muteTTS(true);
			//else 
				//throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");        

            return 0;
            break;
          }
                    
          case COMENT:
          {
			#ifndef NDEBUG
			cout << "COMMENT: " << getText(tree) << endl; 
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
			cout << "SPEAK: " << arg.str() << endl; 
			#endif
            Donnie->speak(arg.str());
            break;
          }

          case WAIT:
          {
			if(tree->getToken(getChild(tree,0))->type != STRINGE){
				int wait = run(getChild(tree,0));
				#ifndef NDEBUG
				cout << "WAIT: " << wait << endl;
				#endif				
				sleep(wait);
				Donnie->speak("esperando " +  to_string (wait) + " segundos.");
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
            cout << "while" << endl;
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

            int val;
            if (tree->getChildCount(tree) < 2)
            {
              val = 0;                           // Se não for atribuido valor, variável começa com zero 
            }
            else 
              val = run(getChild(tree,1));

            if(for_itFlag == WAITING)
              for_itFlag = DonnieMemory::getInstance()->addForVar(var,val);
            else
              DonnieMemory::getInstance()->addVar(var,val);

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
              cout << "Número de argumentos invalido" << endl;

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
			  
			  if (History->size()>0){
				output << History->getLast();
			  }else{
				output << "Nenhum comando executado, posição [" 
				       << Donnie->GetPos("body",0)  <<  //POSITION_X in steps
				   "," << Donnie->GetPos("body",1)  <<  //POSITION_Y in steps
				   "," << Donnie->GetPos("body",2) << "]"; //POSITION_YAW in degrees 
				   // dont add newline here !
			  }
				cout << output.str() << endl;
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
            cout << "EXIT" << endl;
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
               throw sintaxeException("Sintaxe não conhecida\n");
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
    cout << "null comp" << endl;
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
	
	//TODO does not replace character if it is within double quotes, like in the Speak command
	// note that acented character uses 2 bytes, thus, extra space is required for each accented character
    size_t index;
    while ((index = code.find("faça")) != string::npos)
        code.replace(index, 5, "faca ");	
    while ((index = code.find("posição")) != string::npos)
        code.replace(index, 9, "posicao  ");	
    while ((index = code.find("distância")) != string::npos)
        code.replace(index, 10, "distancia ");	
    while ((index = code.find("histórico")) != string::npos)
        code.replace(index, 10, "historico ");	
    while ((index = code.find("senão")) != string::npos)
        code.replace(index, 6, "senao ");	
    while ((index = code.find("então")) != string::npos)
        code.replace(index, 6, "entao ");	
    while ((index = code.find("trás")) != string::npos)
        code.replace(index, 5, "tras ");	
	
	return code;
}
