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


ExprTreeEvaluator::ExprTreeEvaluator()
{

	Donnie = DonnieClient::getInstance();
	History = Historic::getInstance();

  	memFlag = 0;
  	for_itFlag = 0;
    done = 0;
}

ExprTreeEvaluator::~ExprTreeEvaluator()
{
	DonnieClient::ResetInstance();
	Donnie = NULL;
	
	Historic::ResetInstance();
	History = NULL;
}

int ExprTreeEvaluator::parser(pANTLR3_INPUT_STREAM input)
{
  	if (input == NULL)
  	{
  	  cout << "Input file error" << endl;
  	  return -1;
  	}

  	pGoDonnieLexer lex = GoDonnieLexerNew(input);
  	if (lex == NULL)
  	{
      cout << "Unable to create the lexer due to malloc() failure1\n";
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
		// TODO: esta parte gera uma excecao qnd tem um comando invalido 
		// que nao eh capturada pelo catch abaixo. teria q capturar para 
		// evitar de executar o programa
		r = parser->start_rule(parser);
		//pANTLR3_BASE_TREE tree = r.tree;
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
		//cout << parser->pParser->rec->getNumberOfSyntaxErrors(parser->pParser->rec) + lex->pLexer->rec->getNumberOfSyntaxErrors(lex->pLexer->rec) << " errors. tree walking aborted." << endl;
		Donnie->speak(to_string (errors) + " erro foi encontrado.");
		// será q isso funciona p pegar a linha ? http://puredanger.github.io/tech.puredanger.com/2007/02/01/recovering-line-and-column-numbers-in-your-antlr-ast/
		// este exemplo tb extende a classe token, para incluir informacoes uteis p msg de erro
		// http://www.milk.com/kodebase/antlr-tutorial/
		// http://www.milk.com/kodebase/antlr-tutorial/ExtentToken.java
		// http://www.milk.com/kodebase/antlr-tutorial/ErrorFormatter.java
		//cout << "Error in line " << parser->pParser->rec->state->tokenStartLine << " near " << parser->pParser->rec->state->text << endl;
    } else if (errors > 1)
    {
		//cout << parser->pParser->rec->getNumberOfSyntaxErrors(parser->pParser->rec) + lex->pLexer->rec->getNumberOfSyntaxErrors(lex->pLexer->rec) << " errors. tree walking aborted." << endl;
		Donnie->speak(to_string (errors) + " erros foram encontrados.");
 
    }else{
		//if all tests passed, try to run the GoDonnie code
		try{
			this->run(r.tree);
		}
		catch(exception& e)
		{
			//cout << e.what();
			Donnie->speak(e.what());
			
		}
	}
	
  	parser->free(parser);
  	tokens->free(tokens);
  	lex->free(lex);
  	input->close(input);
  	
  	return 1;
}

int ExprTreeEvaluator::terminalMode(char* textIn)
{
  uint8_t* bufferData = (uint8_t*)textIn;
    
  uint32_t bufferSize = strlen(textIn);
    
  pANTLR3_UINT8 bufferName = (pANTLR3_UINT8)"INPUT text";
        
  pANTLR3_INPUT_STREAM input = antlr3NewAsciiStringInPlaceStream( bufferData, bufferSize, bufferName);

  this->parser(input);

  return done;

}

int ExprTreeEvaluator::scriptMode(char* fileIn)
{
  pANTLR3_INPUT_STREAM input = antlr3AsciiFileStreamNew((pANTLR3_UINT8)fileIn);       //  Utilizar para modo script

  if ( input == NULL )
  {
	cout << "Unable to open file " << fileIn << endl;
	exit(1);
  }

  this->parser(input);

  return 0;

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
            if (!memFlag)         // it tells whether the variable is global or local. recursive function is not supported  
            {
              if(memory.find(getText(tree)) != memory.end())
              {
                return memory[getText(tree)];
              }
              else
                throw variavelException("Variável global '" + string(getText(tree)) + "' não existe");
            }
            else
            {
              if(localMem.top().memory.find(getText(tree)) != localMem.top().memory.end())
              {
                return localMem.top().memory[getText(tree)]; // Variável local do primeiro item da stack
              }
              else
                throw variavelException("Variável local '" + string(getText(tree)) + "' não existe");
            }
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
            float distance = (float)run(getChild(tree,0));
            int steps_taken = Donnie->moveForward(distance);
            // if less steps were taken, then report a bump
            std::ostringstream distanceStr;
            distanceStr << "andou " << steps_taken;
            if (((float)steps_taken < (distance-1.0)) || Donnie->bumped()){
              distanceStr << ", bateu";
            }else{
		      distanceStr << ", nao bateu";
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
            float distance = (float)run(getChild(tree,0));
            int steps_taken = Donnie->moveBackward(distance);
            // if less steps were taken, then report a bump
            std::ostringstream distanceStr;
            distanceStr << "andou " << steps_taken;
            if (((float)steps_taken < (distance-1.0)) || Donnie->bumped()){
              distanceStr << ", bateu";
            }else{
		      distanceStr << ", nao bateu";
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
            History->addCommand(command,(Donnie->bumped() ? "bateu" : "nao bateu"));            
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
            History->addCommand(command,(Donnie->bumped() ? "bateu" : "nao bateu"));            
            break;

          }

          case SCAN:
          {
			// get 7 sonar and blob readings by moving the head from 0o to 180o
			// every 30 degrees
			float sonar_readings[7];
			int blobs_found[7];
			Donnie->Scan(sonar_readings,blobs_found);
            #ifndef NDEBUG
				int graus = 0;
				cout << "SCAN: "<< endl;
				for(int i=0; i<7; i++){
				   cout << blobs_found[i] << " objetos a " << int(sonar_readings[i]) << " passos no grau " << graus << endl; 
				   graus+=30;
				}              
				cout << endl;
            #endif
            break;
          }

/*
          case STATUS:
          {
			#ifndef NDEBUG
            cout << "STATUS"<< endl;
            #endif
            cout << "Comando 'STATUS' nao implementado" << endl;
            break;
          }
*/
          case RANGER:
          {
            int arg;
            float range;
            vector<string> tokens;
            split((char*)getText(tree),' ',tokens);
			if (tokens.size() != 2)
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n"); 

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
			else 
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");        

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

			if (tokens.size() != 2)
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n"); 

			// get the ranger id
			if (tokens[1] == POSITION_X)
				arg = 0;
			else if (tokens[1] == POSITION_Y)
				arg = 1;
			else if (tokens[1] == POSITION_YAW)
				arg = 2;
			else  
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n"); 
				
			pos = Donnie->GetPos("body",arg);             
			#ifndef NDEBUG
            cout << "POS: " << arg << " " << (int)pos << endl;
            #endif
            return (int)pos;
          }

          case COLOR:
          {
            int arg, blobs;
			// get 7 sonar and blob readings by moving the head from 0o to 180o
			// every 30 degrees. report only the blobs with requested color (r,g, or b)
			
            vector<string> tokens;
            split((char*)getText(tree),' ',tokens);
			if (tokens.size() != 2)
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n"); 

			// get the color id. color is encodedd in 0x00RRGGBB format
			if (tokens[1] == COLOR_BLUE)
				//arg = 0x000000FF;
				arg = 138;
			else if (tokens[1] == COLOR_GREEN)
				arg = 0x0000FF00;
			else if (tokens[1] == COLOR_RED)
				arg = 0x00FF0000;
			else 
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");        

			// only report when there are blobs with the selected color
			blobs = Donnie->Color(arg);
            return blobs;
            break;
          }
          
          case SOUND:
          {
            vector<string> tokens;
            split((char*)getText(tree),' ',tokens);
			if (tokens.size() != 2)
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n"); 

			// sound on or off
			if (tokens[1] == SOUND_ON)
				Donnie->muteTTS(false);
			else if (tokens[1] == SOUND_OFF)
				Donnie->muteTTS(true);
			else 
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");        

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
            bool it_flag = 0;
            char* it_var;

            for_itFlag = 1;

            run(getChild(tree,0));                              // Executa condição inicial

            if(!for_itFlag)
            {
              it_flag = 1;
              it_var = for_it;
            }
            for_itFlag = 0;

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

            if(it_flag)
            {
              if (!memFlag)
              {
                memory.erase(it_var);            
              }
              else
              {
                localMem.top().memory.erase(it_var);
              }
            }

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
          case PROCB:
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

            if(for_itFlag)
            {
              for_it = var;
              for_itFlag = 0;
            }

            int val;
            if (tree->getChildCount(tree) < 2)
            {
              val = 0;                           // Se não for atribuido valor, variável começa com zero 
            }
            else 
              val = run(getChild(tree,1));

            if (!memFlag)
            {
              if(memory.find(var) != memory.end())
              {
                //cout << "Variável " << var << " global já foi declarada" << endl;        // Se flag for zero e variável ainda não foi declarada cria-se uma variavel global 
                throw variavelException("Variável global '" + string(var) + "' já foi declarada");
              }
              else
              {
				#ifndef NDEBUG
                cout << "MAKE: " << var << " = " << val << endl;
                #endif
                memory[var] = val;
                return val;
              }
                
            }
              
            else                                                                            // Se flag for diferente de zero cria-se uma local
            {
              if(localMem.top().memory.find(var) != localMem.top().memory.end())
              {
                //cout << "Variável local '" << var << "' já foi declarada" << endl;
                throw variavelException("Variável local '" + string(var) + "' já foi declarada");
              }
              else
              {
                #ifndef NDEBUG
                cout << "MAKE: " << var << " = " << val << endl;
                #endif
                localMem.top().memory[var] = val;
                return val;
              }
                
            }
            break;
            }

          case PROCINV:
          {
            char* name = (char*)getText(getChild(tree,0));

            mem local;                                      // Inicia dicionário local

            for (int f = 1; f <= proc[name].argNum ; f++)
              local.memory[proc[name].args[f-1]] = run(getChild(tree,f));   // Atribui as variaveis de argumento no dicionário

            localMem.push(local);                      // Empilha dicionário 
            memFlag++;                                 // Incrementa flag de memória

            run(proc[name].node);                      // Executa procedimento 

            localMem.pop();                            // Desempilha dicionário
            memFlag--;                                 // Decrementa flag de memória

            break;
          }

          case PROCDEC:
          {
            char* name = (char*)getText(getChild(tree,0));
  
            int childNum = tree->getChildCount(tree);
  
            if(childNum < 3)
            {
              proc[name].argNum = 0;                    // Caso procedimento não apresente parametros salva bloco do procedimento
              proc[name].node = getChild(tree,1);
            }
            else
            {
              for(int f = 1; f < childNum - 1; f++)
                proc[name].args.push_back(getText(getChild(tree,f)));   // Caso tenha parametros salva numa lista 
  
              proc[name].argNum = childNum - 2;
              proc[name].node = getChild(tree,childNum - 1);      // Salva bloco do procedimento
            }
            break;
          }
          
          case HIST:
          {
				//TODO (amory): implementar um clear para o historico
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
				       << std::fixed << std::setprecision(2) <<  Donnie->GetPos("body",0)  <<  //POSITION_X
				   "," << std::fixed << std::setprecision(2) <<  Donnie->GetPos("body",1)  <<  //POSITION_Y
				   "," << std::fixed << std::setprecision(2) <<  Donnie->GetPos("body",2) << "]" << endl ; //POSITION_YAW
			  }
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
            done = 1;
            break;
          }

          case EQ:
          {
  
            char* var = (char*)getText(getChild(tree,0));
            int val = run(getChild(tree,1));

            if (!memFlag)
            {
              if(memory.find(var) != memory.end())
              {
                memory[var] = val;
                return val;
              }
              else{
                //cout << "Variável " << var << " global não existe" << endl;
                throw variavelException("Variável global " + string(getText(tree)) + " não existe");
			  }
            }
            else
            {
              if(localMem.top().memory.find(var) != localMem.top().memory.end())
              {
                localMem.top().memory[var] = val;
                return val;
              }
              else{
                //cout << "Variável " << var << " local não existe" << endl;
                throw variavelException("Variável global " + string(getText(tree)) + " não existe");
              }
            }
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
    if(!(i < tree->getChildCount(tree))) throw sintaxeException();
    
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
