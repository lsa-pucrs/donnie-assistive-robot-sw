#include <algorithm> // remove
#include "Compiler.h"
#include "Exception.h"


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
#define RANGER_HEAD "c" // 6
#define POSITION_X "x"
#define POSITION_Y "y"
#define POSITION_YAW "a"

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
  	  cout << "Lexical error" << endl;
  	  return -1;
  	}   	
  	pANTLR3_COMMON_TOKEN_STREAM tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
  	if (tokens == NULL)
  	{
  	  cout << "Token error" << endl;
  	  return -1;
  	} 
  	pGoDonnieParser parser = GoDonnieParserNew(tokens);
  	if (parser == NULL)
  	{
  	  cout << "Parse error" << endl;
  	  return -1;
  	} 
	//try to parse the GoDonnie code
	GoDonnieParser_prog_return r;
	try{
		r = parser->prog(parser);
	}
	catch(exception& e)
	{
		cout << e.what();
	}
	
  	pANTLR3_BASE_TREE tree = r.tree;
  	if (tree == NULL)
  	{
  	  cout << "Parse error" << endl;
  	  return -1;
  	}

	//try to run the GoDonnie code
	try{
		this->run(tree);
	}
	catch(exception& e)
	{
		cout << e.what();
	}
	
  	parser->free(parser);
  	tokens->free(tokens);
  	lex->free(lex);
  	input->close(input);
  	
  	return 1;
}

int ExprTreeEvaluator::terminalMode(char* textIn)
{
  mode = TERMINAL;

  uint8_t* bufferData = (uint8_t*)textIn;
    
  uint32_t bufferSize = strlen(textIn);
    
  pANTLR3_UINT8 bufferName = (pANTLR3_UINT8)"INPUT text";
        
  pANTLR3_INPUT_STREAM input = antlr3NewAsciiStringInPlaceStream( bufferData, bufferSize, bufferName);

  this->parser(input);

  return done;

}

int ExprTreeEvaluator::scriptMode(char* fileIn)
{
  mode = SCRIPT;
  pANTLR3_INPUT_STREAM input = antlr3AsciiFileStreamNew((pANTLR3_UINT8)fileIn);       //  Utilizar para modo script

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
                throw variavelException("Variavel " + string(getText(tree)) + " global não existe");
            }
            else
            {
              if(localMem.top().memory.find(getText(tree)) != localMem.top().memory.end())
              {
                return localMem.top().memory[getText(tree)]; // Variável local do primeiro item da stack
              }
              else
                throw variavelException("Variavel " + string(getText(tree)) + " local não existe");
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
			// save into history
            std::ostringstream distanceStr;
			distanceStr << distance;
			string command;
            command = string(getText(tree)) + " " + distanceStr.str();
            // if less steps were taken, then report a bump
            distanceStr.str("");
            distanceStr.clear();
            distanceStr << "andou " << steps_taken;
            if (((float)steps_taken < (distance-1.0)) || Donnie->bumped())
				distanceStr << ", bateu";
			else
				distanceStr << ", nao bateu";
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
			// save into history
            std::ostringstream distanceStr;
			distanceStr << distance;
			string command;
            command = string(getText(tree)) + " " + distanceStr.str();
            // if less steps were taken, then report a bump
            distanceStr.str("");
            distanceStr.clear();
            distanceStr << "andou " << steps_taken;
            if (((float)steps_taken < (distance-1.0)) || Donnie->bumped())
				distanceStr << ", bateu";
			else
				distanceStr << ", nao bateu";
            History->addCommand(command,distanceStr.str());
            break;
          }

          case RTURN:
          { // right turn
			#ifndef NDEBUG
            cout << "PD: " << run(getChild(tree,0)) << endl;
            #endif
            // run the command
            float distance = (float)run(getChild(tree,0));
            Donnie->turnRight(distance);
			// save into history
            std::ostringstream distanceStr;
			distanceStr << distance;
            string command = string(getText(tree)) + " " + distanceStr.str();
            History->addCommand(command,(Donnie->bumped() ? "bateu" : "nao bateu"));            
            break;
          }

          case LTURN:
          { // left turn
			#ifndef NDEBUG
            cout << "PE: " << run(getChild(tree,0)) << endl;
            #endif
            // run the command
            float distance = (float)run(getChild(tree,0));
            Donnie->turnLeft(distance);
			// save into history
            std::ostringstream distanceStr;
			distanceStr << distance;
            string command = string(getText(tree)) + " " + distanceStr.str();
            History->addCommand(command,(Donnie->bumped() ? "bateu" : "nao bateu"));            
            break;

          }

          case SCAN:
          {
			#ifndef NDEBUG
            cout << "SCAN"<< endl;
            #endif
            cout << "Comando 'espiar' nao implementado" << endl;
            break;
          }

          case STATUS:
          {
			#ifndef NDEBUG
            cout << "STATUS"<< endl;
            #endif
            cout << "Comando 'STATUS' nao implementado" << endl;
            break;
          }

          case RANGER:
          {
            char* cmdTxt= (char*)getText(tree);
            string str;
            int arg;
            float range;
            vector<string> tokens;
            
            // remove extra space between tokens
            str = cmdTxt;
            str.erase(std::unique(str.begin(), str.end(),
					[](char a, char b) { return a == ' ' && b == ' '; } ), str.end() ); 
			// tolower
			transform(str.begin(), str.end(), str.begin(), ::tolower);
            // split into vector of tokens and get the 2nd token
            tokens = split(str,' ');

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
			else if (tokens[1] == RANGER_HEAD)
				arg = 6;
			else 
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n");        

			// TODO. leitura da cabeca nao funciona. ver bug #22 no github
            range  = Donnie->GetRange(arg);
			#ifndef NDEBUG
            cout << "RANGER: " << arg << " " << (int)range << endl;
            #endif
            return (int)range;
          }

          case POS:
          {
            char* cmdTxt= (char*)getText(tree);
            string str;
            int arg;
            float pos;
            vector<string> tokens;
            
            // remove extra space between tokens
            str = cmdTxt;
            str.erase(std::unique(str.begin(), str.end(),
					[](char a, char b) { return a == ' ' && b == ' '; } ), str.end() ); 
			// tolower
			transform(str.begin(), str.end(), str.begin(), ::tolower);
            // split into vector of tokens and get the 2nd token
            tokens = split(str,' ');

			// get the ranger id
			if (tokens[1] == POSITION_X)
				arg = 0;
			else if (tokens[1] == POSITION_Y)
				arg = 1;
			else if (tokens[1] == POSITION_YAW)
				arg = 2;
			else  
				throw sintaxeException("Sintaxe não conhecida para comando '"+tokens[0]+"'\n"); 
				
			pos = Donnie->GetPos(arg);             
			#ifndef NDEBUG
            cout << "POS: " << arg << " " << (int)pos << endl;
            #endif
            return (int)pos;
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
				int exp = run(getChild(tree,0));
				#ifndef NDEBUG
				cout << "WAIT: " << exp << endl;
				#endif				
				sleep(exp);
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
                cout << "Variavel " << var << " global já foi declarada" << endl;        // Se flag for zero e variável ainda não foi declarada cria-se uma variavel global 
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
                cout << "Variavel " << var << " local já foi declarada" << endl;
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
            if(mode == TERMINAL)
            {
              cout << "Procedimentos não são aceitos no modo terminal" << endl;
              break;
            }

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
            if(mode == TERMINAL)
            {
              cout << "Procedimentos não são aceitos no modo terminal" << endl;
              break;
            }

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

          case INFO:
          {
			#ifndef NDEBUG
              cout << "STATUS"<< endl;
            #endif
            cout << "Comando 'estado' nao implementado" << endl;
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
              else
                cout << "Variavel " << var << " global não existe" << endl;
                
            }
            else
            {
              if(localMem.top().memory.find(var) != localMem.top().memory.end())
              {
                localMem.top().memory[var] = val;
                return val;
              }
              else
                //cout << "Variavel " << var << " local não existe" << endl;
                ;
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
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}
