#include "Compiler.h"
#include "Exception.h"

using std::map;
using std::vector;
using std::stack;
using std::string;
using std::cout;
using std::endl;

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
	assert(input != NULL);
                                   
  	pGoDonnieLexer lex = GoDonnieLexerNew(input);
  	assert(lex != NULL);
  	pANTLR3_COMMON_TOKEN_STREAM tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
  	assert(tokens != NULL);

  	pGoDonnieParser parser = GoDonnieParserNew(tokens);
  	assert(parser != NULL);

  	GoDonnieParser_prog_return r = parser->prog(parser);

  	pANTLR3_BASE_TREE tree = r.tree;

  	if (tree == NULL)
  	{
  	  cout << "error" << endl;
  	  return -1;
  	}

	//Tentando rodar o programa
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
	string command="";
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    if(tok) {
        switch(tok->type) {

          case INT:
          {
            return atoi(getText(tree));
          }

          case ID:
          {            
            if (!memFlag)         // Se flag for zero retorna o valor da variável global caso contrario o valor da variável local  
            {
              if(memory.find(getText(tree)) != memory.end())
              {
                return memory[getText(tree)];
              }
              else
                //cout << "Variavel " << getText(tree) << " global não existe" << endl;
                throw variavelException();
            }
            else
            {
              if(localMem.top().memory.find(getText(tree)) != localMem.top().memory.end())
              {
                return localMem.top().memory[getText(tree)]; // Variável local do primeiro item da stack
              }
              else
                //cout << "Variavel " << getText(tree) << " local não existe" << endl;
                throw variavelException();
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

            //int fi = tree->getChildCount(tree);
            //cout << "MULT: " << fi << std::endl;
            return run(getChild(tree,0)) * run(getChild(tree,1));
          }

          case DIV:
          {

            //int fi = tree->getChildCount(tree);
            //cout << "DIV: " << fi << std::endl;
            return run(getChild(tree,0)) / run(getChild(tree,1));
          }

          case PF:
          {
			#ifndef NDEBUG
            cout << "PF: " << run(getChild(tree,0)) << endl;
            #endif
            // run the command
            float distance = (float)run(getChild(tree,0));
            Donnie->ParaFrente(distance);
			// save into history
            std::ostringstream distanceStr;
			distanceStr << distance;
            command = string(getText(tree)) + " " + distanceStr.str();
            //command = string(getText(tree)) + " " + to_string(distance);
            History->addCommand(command,"");
            //Para_Frente(run(getChild(tree,0)),&robot,&p2dProxy,sonarProxy,front_bumper,back_bumper,&speech,&p2d_headProxy);
            break;
          }

          case PT:
          {
			#ifndef NDEBUG
            cout << "PT: " << run(getChild(tree,0)) << endl;
            #endif
            // run the command
            float distance = (float)run(getChild(tree,0));
            Donnie->ParaTras(distance);
			// save into history
            std::ostringstream distanceStr;
			distanceStr << distance;
            command = string(getText(tree)) + " " + distanceStr.str();
            //command = string(getText(tree)) + " " + to_string(distance);
            History->addCommand(command,"");            
            //Para_Tras(run(getChild(tree,0)),&robot,&p2dProxy,sonarProxy,front_bumper,back_bumper,&speech,&p2d_headProxy);
            break;
          }

          case PD:
          {
			#ifndef NDEBUG
            cout << "PD: " << run(getChild(tree,0)) << endl;
            #endif
            // run the command
            float distance = (float)run(getChild(tree,0));
            Donnie->ParaDireita(distance);
			// save into history
            std::ostringstream distanceStr;
			distanceStr << distance;
            command = string(getText(tree)) + " " + distanceStr.str();
            //command = string(getText(tree)) + " " + to_string(distance);
            History->addCommand(command,"");            
            //Para_Direita(run(getChild(tree,0)),&robot,&p2dProxy,sonarProxy,front_bumper,back_bumper,&speech,&p2d_headProxy);
            break;
          }

          case PE:
          {
			#ifndef NDEBUG
            cout << "PE: " << run(getChild(tree,0)) << endl;
            #endif
            // run the command
            float distance = (float)run(getChild(tree,0));
            Donnie->ParaEsquerda(distance);
			// save into history
            std::ostringstream distanceStr;
			distanceStr << distance;
            command = string(getText(tree)) + " " + distanceStr.str();
            //command = string(getText(tree)) + " " + to_string(distance);
            History->addCommand(command,"");            
            //Para_Esquerda(run(getChild(tree,0)),&robot,&p2dProxy,sonarProxy,front_bumper,back_bumper,&speech,&p2d_headProxy);
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

            char* tkn = strtok (cmdTxt," ");

            tkn = strtok (NULL, " ");

            int arg;

            if(tkn[0] == 'f' or tkn[0] == 'F')
            {
              if(tkn[1] == 101 or tkn[1] == 69)
                arg = 2;
              else if(tkn[1] == 100 or tkn[1] == 68)
                arg = 3;
              else
                arg = 0;
            }
            else if(tkn[0] == 116 or tkn[0] == 84)
            {
              if(tkn[1] == 101 or tkn[1] == 69)
                arg = 4;
              else if(tkn[1] == 100 or tkn[1] == 68)
                arg = 5;
              else
                arg = 1;
            }
            else
            {
              arg = 6;
            } 
			#ifndef NDEBUG
            cout << "RANGER: " << arg << endl;
            #endif
            return (int)Donnie->GetRange(arg);
          }

          case POS:
          {
            char* cmdTxt= (char*)getText(tree);

            char* tkn = strtok (cmdTxt," ");

            tkn = strtok (NULL, " ");

            int arg;

            if(tkn[0] == 120)
              arg = 0;
            else if(tkn[0] == 121)
              arg = 1;
            else
              arg = 2;
			#ifndef NDEBUG
            cout << "POS: " << getText(tree) << endl;
            #endif
            return (int)Donnie->GetPos(arg);

          }

          case COMENT:
          {
            //cout << "COMMENT: " << endl; 
            //cout << getText(tree) << endl;
            break;
          }

          case PRINTE:
          {
            //cout << "PRINT: " << endl; 
            //cout << "TIPE: " << tree->getToken(getChild(tree,0))->type << endl;
            if(tree->getToken(getChild(tree,0))->type == STRINGE)         // Caso seja string informa texto do filho caso contrario executa o filho
              cout << getText(getChild(tree,0)) << endl;
            else
              cout << run(getChild(tree,0)) << endl;
            break;
          }

          case ESPERA:
          {
            #ifndef NDEBUG
				if(tree->getToken(getChild(tree,0))->type == STRINGE)
					cout << "ESPERAR: " << getText(getChild(tree,0)) << endl;
			#endif
			if(tree->getToken(getChild(tree,0))->type != STRINGE)
				sleep(run(getChild(tree,0)));
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
          case FACA:
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
            //Mix_CloseAudio();
            //SDL_Quit();
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


