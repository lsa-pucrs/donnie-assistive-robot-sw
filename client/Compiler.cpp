#include "Compiler.h"

using std::map;
using std::vector;
using std::stack;
using std::string;
using std::cout;
using std::endl;


ExprTreeEvaluator::ExprTreeEvaluator()
{

	Donnie = new DonnieClient();

  	memFlag = 0;
  	for_itFlag = 0;
    done = 1;
}

int ExprTreeEvaluator::parser(pANTLR3_INPUT_STREAM input)
{
	assert(input != NULL);
                                   
  	plogoLexer lex = logoLexerNew(input);
  	assert(lex != NULL);
  	pANTLR3_COMMON_TOKEN_STREAM tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
  	assert(tokens != NULL);

  	plogoParser parser = logoParserNew(tokens);
  	assert(parser != NULL);

  	logoParser_prog_return r = parser->prog(parser);

  	pANTLR3_BASE_TREE tree = r.tree;

  	if (tree == NULL)
  	{
  	  cout << "error" << endl;
  	  return -1;
  	}

  	this->run(tree);

  	parser->free(parser);
  	tokens->free(tokens);
  	lex->free(lex);
  	input->close(input);

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

  this->parser(input);

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
            if (!memFlag)         // Se flag for zero retorna o valor da variável global caso contrario o valor da variável local  
            {
              if(memory.find(getText(tree)) != memory.end())
              {
                return memory[getText(tree)];
              }
              else
                cout << "Variavel " << getText(tree) << " global não existe" << endl;
            }
            else
            {
              if(localMem.top().memory.find(getText(tree)) != localMem.top().memory.end())
              {
                return localMem.top().memory[getText(tree)]; // Variável local do primeiro item da stack
              }
              else
                cout << "Variavel " << getText(tree) << " local não existe" << endl;
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

            //cout << "PF: " << run(getChild(tree,0)) << endl;
            Donnie->ParaFrente((float)run(getChild(tree,0)));
            //Para_Frente(run(getChild(tree,0)),&robot,&p2dProxy,sonarProxy,front_bumper,back_bumper,&speech,&p2d_headProxy);
            break;
          }

          case PT:
          {

            //cout << "PT: " << run(getChild(tree,0)) << endl;
            Donnie->ParaTras((float)run(getChild(tree,0)));
            //Para_Tras(run(getChild(tree,0)),&robot,&p2dProxy,sonarProxy,front_bumper,back_bumper,&speech,&p2d_headProxy);
            break;
          }

          case PD:
          {

            //cout << "PD: " << run(getChild(tree,0)) << endl;
            Donnie->ParaDireita((float)run(getChild(tree,0)));
            //Para_Direita(run(getChild(tree,0)),&robot,&p2dProxy,sonarProxy,front_bumper,back_bumper,&speech,&p2d_headProxy);
            break;
          }

          case PE:
          {

            //cout << "PE: " << run(getChild(tree,0)) << endl;
            Donnie->ParaEsquerda((float)run(getChild(tree,0)));
            //Para_Esquerda(run(getChild(tree,0)),&robot,&p2dProxy,sonarProxy,front_bumper,back_bumper,&speech,&p2d_headProxy);
            break;

          }

          case SCAN:
          {

            cout << "SCAN"<< endl;
            //Scan(&head, &p2d_headProxy, &speech, SHProxy, BfinderProxy,&robot,&p2dProxy);
            break;
          }

          case STATUS:
          {
            cout << "STATUS"<< endl;
            //Mostra_Status(&speech);
            break;
          }

          case RANGER:
          {
            char* cmdTxt= (char*)getText(tree);

            char* tkn = strtok (cmdTxt," ");

            tkn = strtok (NULL, " ");

            int arg;

            if(tkn[0] == 102 or tkn[0] == 70)
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

            cout << "RANGER: " << arg << endl;
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

            cout << "POS: " << getText(tree) << endl;
            return (int)Donnie->GetPos(arg);

          }

          case COMENT:
          {
            cout << "COMMENT: " << endl; 
            cout << getText(tree) << endl;
            break;
          }

          case PRINTE:
          {
            cout << "PRINTE: " << endl; 
            //cout << "TIPE: " << tree->getToken(getChild(tree,0))->type << endl;
            if(tree->getToken(getChild(tree,0))->type == STRINGE)         // Caso seja string informa texto do filho caso contrario executa o filho
              cout << getText(getChild(tree,0)) << endl;
            else
              cout << run(getChild(tree,0)) << endl;
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

          //case THEN:
          //case ELSEE:
          case BLOCK:
          {
            for (int f = 0; f < tree->getChildCount(tree); f++)
              {
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
                cout << "MAKE: " << var << " = " << val << endl;
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
                cout << "MAKE: " << var << " = " << val << endl;
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
            break;
          }

          case QUIT:
          {
            cout << "EXIT" << endl;
            //Mix_CloseAudio();
            //SDL_Quit();
            done = 0;
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
                cout << "Variavel " << var << " local não existe" << endl;
            }
            break;            
          }
  
          default:
              cout << "Unhandled token: #" << tok->type << '\n';
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
    assert(i < tree->getChildCount(tree));
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
    return false;
  }
    
}


