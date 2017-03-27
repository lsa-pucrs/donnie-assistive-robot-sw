grammar GoDonnie;

options
{
	language = C;
	output = AST;
   	ASTLabelType=pANTLR3_BASE_TREE;
}

// MUdando para a nova exception



// Header 
@parser::header{
#include <antlr3baserecognizer.h>

#ifndef def_displayRecognitionErrorNew
#define def_displayRecognitionErrorNew
static void	displayRecognitionErrorNew(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames);
static void *getMissingSymbolNew(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_INT_STREAM	istream, pANTLR3_EXCEPTION	e, ANTLR3_UINT32 expectedTokenType, pANTLR3_BITSET_LIST follow);
#endif


}

//declaration of global variable in GoDonnieParser.c
@parser::postinclude{
char messageError[2000];
}

@lexer::header{
#include "lexerExtra.h"
}

@lexer::apifuncs {
  //rec->displayRecognitionError = displayRecognitionErrorNew;
  RECOGNIZER->displayRecognitionError = displayRecognitionErrorNewLexer;
}


@parser::members{
	
	static void displayRecognitionErrorNew(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
	{
		char messageAux[100];
		
		//ANTLR3_FPRINTF(stderr, "TESTE DE ERRO");
		
		pANTLR3_PARSER			parser;
		pANTLR3_TREE_PARSER	    tparser;
		pANTLR3_INT_STREAM	    is;
		pANTLR3_STRING			ttext;
		pANTLR3_STRING			ftext;
		pANTLR3_EXCEPTION	    ex;
		pANTLR3_COMMON_TOKEN    theToken;
		pANTLR3_BASE_TREE	    theBaseTree;
		pANTLR3_COMMON_TREE	    theCommonTree;

		// Retrieve some info for easy reading.
		//
		ex	    =		recognizer->state->exception;
		ttext   =		NULL;

		// See if there is a 'filename' we can use
		//
		if	(ex->streamName == NULL)
		{
			if	(((pANTLR3_COMMON_TOKEN)(ex->token))->type == ANTLR3_TOKEN_EOF)
			{
				//ANTLR3_FPRINTF(stderr, "-end of input-(");
			}
			else
			{
				//ANTLR3_FPRINTF(stderr, "-unknown source-(");
			}
		}
		else
		{
			ftext = ex->streamName->to8(ex->streamName);
			//ANTLR3_FPRINTF(stderr, "\%s(", ftext->chars);
		}

		// Next comes the line number
		//
		
		sprintf(messageAux, "Erro na linha \%d ", recognizer->state->exception->line);
		strcat(messageError, messageAux);
		//ANTLR3_FPRINTF(stderr, "Erro na linha \%d ", recognizer->state->exception->line);
		//ANTLR3_FPRINTF(stderr, " : error \%d : \%s", recognizer->state->exception->type, (pANTLR3_UINT8)(recognizer->state->exception->message));


		// How we determine the next piece is dependent on which thing raised the
		// error.
		//
		switch	(recognizer->type)
		{
		case	ANTLR3_TYPE_PARSER:

			// Prepare the knowledge we know we have
			//
			parser	    = (pANTLR3_PARSER) (recognizer->super);
			tparser	    = NULL;
			is			= parser->tstream->istream;
			theToken    = (pANTLR3_COMMON_TOKEN)(recognizer->state->exception->token);
			ttext	    = theToken->toString(theToken);

			//ANTLR3_FPRINTF(stderr, ", at offset \%d", recognizer->state->exception->charPositionInLine);
			//ANTLR3_FPRINTF(stderr, "e na coluna \%d. ", (recognizer->state->exception->charPositionInLine+1));
			sprintf(messageAux, "e na coluna \%d. ", recognizer->state->exception->charPositionInLine+1);
			strcat(messageError, messageAux);
			if  (theToken != NULL)
			{
				if (theToken->type == ANTLR3_TOKEN_EOF)
				{
					//ANTLR3_FPRINTF(stderr, ", at <EOF>");
					//ANTLR3_FPRINTF(stderr, " no fim do arquivo.");
					sprintf(messageAux, "no fim do arquivo.");
					strcat(messageError, messageAux);
				}
				else
				{
					// Guard against null text in a token
					//
					//ANTLR3_FPRINTF(stderr, "\n    near \%s\n    ", ttext == NULL ? (pANTLR3_UINT8)"<no text for the token>" : ttext->chars);
					pANTLR3_UINT8 temp = (pANTLR3_UINT8)"Sem token identificado";
					
					pANTLR3_STRING ttext2 = theToken->getText(theToken);
					
					//ANTLR3_FPRINTF(stderr, "\n\n \%s \n\n", ttext->chars);
					//ANTLR3_FPRINTF(stderr, "Verificar comando \%s", ttext == NULL ? (pANTLR3_UINT8)"<palavra não identificada>" : ttext2->chars);
					sprintf(messageAux, "Verificar comando \%s", ttext == NULL ? (pANTLR3_UINT8)"<palavra não identificada>" : ttext2->chars);
					strcat(messageError, messageAux);
				}
			}
			break;

		case	ANTLR3_TYPE_TREE_PARSER:

			tparser		= (pANTLR3_TREE_PARSER) (recognizer->super);
			parser		= NULL;
			is			= tparser->ctnstream->tnstream->istream;
			theBaseTree	= (pANTLR3_BASE_TREE)(recognizer->state->exception->token);
			ttext		= theBaseTree->toStringTree(theBaseTree);

			if  (theBaseTree != NULL)
			{
				theCommonTree	= (pANTLR3_COMMON_TREE)	    theBaseTree->super;

				if	(theCommonTree != NULL)
				{
					theToken	= (pANTLR3_COMMON_TOKEN)    theBaseTree->getToken(theBaseTree);
				}
				//ANTLR3_FPRINTF(stderr, ", na coluna \%d", (theBaseTree->getCharPositionInLine(theBaseTree)+1));
				sprintf(messageAux, ", na coluna \%d", (theBaseTree->getCharPositionInLine(theBaseTree)+1));
				strcat(messageError, messageAux);
				
				//ANTLR3_FPRINTF(stderr, ", na palavra \%s", ttext->chars);
				sprintf(messageAux, ", na palavra \%s", ttext->chars);
				strcat(messageError, messageAux);
			}
			break;

		default:

			//ANTLR3_FPRINTF(stderr, "Base recognizer function displayRecognitionError called by unknown parser type - provide override for this function\n");
			return;
			break;
		}

		// Although this function should generally be provided by the implementation, this one
		// should be as helpful as possible for grammar developers and serve as an example
		// of what you can do with each exception type. In general, when you make up your
		// 'real' handler, you should debug the routine with all possible errors you expect
		// which will then let you be as specific as possible about all circumstances.
		//
		// Note that in the general case, errors thrown by tree parsers indicate a problem
		// with the output of the parser or with the tree grammar itself. The job of the parser
		// is to produce a perfect (in traversal terms) syntactically correct tree, so errors
		// at that stage should really be semantic errors that your own code determines and handles
		// in whatever way is appropriate.
		//
		switch  (ex->type)
		{
		case	ANTLR3_UNWANTED_TOKEN_EXCEPTION:

			// Indicates that the recognizer was fed a token which seesm to be
			// spurious input. We can detect this when the token that follows
			// this unwanted token would normally be part of the syntactically
			// correct stream. Then we can see that the token we are looking at
			// is just something that should not be there and throw this exception.
			//
			if	(tokenNames == NULL)
			{
				//ANTLR3_FPRINTF(stderr, " : Extraneous input...");
				//ANTLR3_FPRINTF(stderr, " : Entrada desconhecida...");
				sprintf(messageAux, " : Entrada desconhecida...");
				strcat(messageError, messageAux);
				
			}
			else
			{
				if	(ex->expecting == ANTLR3_TOKEN_EOF)
				{
					//ANTLR3_FPRINTF(stderr, " : Extraneous input - expected <EOF>\n");
					//ANTLR3_FPRINTF(stderr, " : Entrada desconhecida - Esperava o fim do arquivo\n");
					sprintf(messageAux, " : Entrada desconhecida - Esperava o fim do arquivo\n");
					strcat(messageError, messageAux);
				}
				else
				{
					//ANTLR3_FPRINTF(stderr, " : Extraneous input - expected \%s ...\n", tokenNames[ex->expecting]);
					//ANTLR3_FPRINTF(stderr, " : Entrada desconhecida - esperando \%s ...\n", tokenNames[ex->expecting]);
					sprintf(messageAux, " : Entrada desconhecida - esperando \%s ...\n", tokenNames[ex->expecting]);
					strcat(messageError, messageAux);
				}
			}
			break;

		case	ANTLR3_MISSING_TOKEN_EXCEPTION:

			// Indicates that the recognizer detected that the token we just
			// hit would be valid syntactically if preceeded by a particular 
			// token. Perhaps a missing ';' at line end or a missing ',' in an
			// expression list, and such like.
			//
			if	(tokenNames == NULL)
			{
				//ANTLR3_FPRINTF(stderr, " : Missing token (\%d)...\n", ex->expecting);
				//ANTLR3_FPRINTF(stderr, " : token faltando (\%d)...\n", ex->expecting);
				sprintf(messageAux, " : token faltando (\%d)...\n", ex->expecting);
				strcat(messageError, messageAux);
			}
			else
			{
				if	(ex->expecting == ANTLR3_TOKEN_EOF)
				{
					//ANTLR3_FPRINTF(stderr, " : Missing <EOF>\n");
					//ANTLR3_FPRINTF(stderr, " : Faltando fim do aquivo\n");
					sprintf(messageAux, " : Faltando fim do aquivo\n");
					strcat(messageError, messageAux);
				}
				else
				{
					//ANTLR3_FPRINTF(stderr, " : Missing \%s \n", tokenNames[ex->expecting]);
					//ANTLR3_FPRINTF(stderr, " : Faltandoaa \%d \n", ex->expecting);
					//ANTLR3_FPRINTF(stderr, "\n");
					sprintf(messageAux, "\n");
					strcat(messageError, messageAux);
				}
			}
			break;

		case	ANTLR3_RECOGNITION_EXCEPTION:

			// Indicates that the recognizer received a token
			// in the input that was not predicted. This is the basic exception type 
			// from which all others are derived. So we assume it was a syntax error.
			// You may get this if there are not more tokens and more are needed
			// to complete a parse for instance.
			//
			//ANTLR3_FPRINTF(stderr, " : syntax error...\n");  
			//ANTLR3_FPRINTF(stderr, " : Erro de sintaxe...\n");   
			sprintf(messageAux, " : Erro de sintaxe...\n");   
			strcat(messageError, messageAux);
			break;

		case    ANTLR3_MISMATCHED_TOKEN_EXCEPTION:

			// We were expecting to see one thing and got another. This is the
			// most common error if we coudl not detect a missing or unwanted token.
			// Here you can spend your efforts to
			// derive more useful error messages based on the expected
			// token set and the last token and so on. The error following
			// bitmaps do a good job of reducing the set that we were looking
			// for down to something small. Knowing what you are parsing may be
			// able to allow you to be even more specific about an error.
			//
			if	(tokenNames == NULL)
			{
				//ANTLR3_FPRINTF(stderr, " : syntax error...\n");
				//ANTLR3_FPRINTF(stderr, " : Erro de sintaxe...\n");
				sprintf(messageAux, " : Erro de sintaxe...\n");   
				strcat(messageError, messageAux);
			}
			else
			{
				if	(ex->expecting == ANTLR3_TOKEN_EOF)
				{
					//ANTLR3_FPRINTF(stderr, " : expected <EOF>\n");
					//ANTLR3_FPRINTF(stderr, " : esperando fim do arquivo\n");
					sprintf(messageAux, " : esperando fim do arquivo\n");
					strcat(messageError, messageAux);
				}
				else
				{
					//ANTLR3_FPRINTF(stderr, " : expected \%s ...\n", tokenNames[ex->expecting]);
					//ANTLR3_FPRINTF(stderr, " : esperando \%s ...\n", tokenNames[ex->expecting]);
					sprintf(messageAux, " : esperando \%s ...\n", tokenNames[ex->expecting]);
					strcat(messageError, messageAux);
				}
			}
			break;

		case	ANTLR3_NO_VIABLE_ALT_EXCEPTION:

			// We could not pick any alt decision from the input given
			// so god knows what happened - however when you examine your grammar,
			// you should. It means that at the point where the current token occurred
			// that the DFA indicates nowhere to go from here.
			//
			//ANTLR3_FPRINTF(stderr, " : entrada desconhecida...\n");
			//ANTLR3_FPRINTF(stderr, "\n");
			sprintf(messageAux, "\n");
			strcat(messageError, messageAux);
			
			break;

		case	ANTLR3_MISMATCHED_SET_EXCEPTION:

			{
				ANTLR3_UINT32	  count;
				ANTLR3_UINT32	  bit;
				ANTLR3_UINT32	  size;
				ANTLR3_UINT32	  numbits;
				pANTLR3_BITSET	  errBits;

				// This means we were able to deal with one of a set of
				// possible tokens at this point, but we did not see any
				// member of that set.
				//
				//ANTLR3_FPRINTF(stderr, " : unexpected input...\n  expected one of : ");
				//ANTLR3_FPRINTF(stderr, " : entrada desconhecida...\n  esperava um desses : ");

				// What tokens could we have accepted at this point in the
				// parse?
				//
				count   = 0;
				errBits = antlr3BitsetLoad		(ex->expectingSet);
				numbits = errBits->numBits		(errBits);
				size    = errBits->size			(errBits);

				if  (size > 0)
				{
					// However many tokens we could have dealt with here, it is usually
					// not useful to print ALL of the set here. I arbitrarily chose 8
					// here, but you should do whatever makes sense for you of course.
					// No token number 0, so look for bit 1 and on.
					//
					for	(bit = 1; bit < numbits && count < 8 && count < size; bit++)
					{
						// TODO: This doesn;t look right - should be asking if the bit is set!!
						//
						if  (tokenNames[bit])
						{
							//ANTLR3_FPRINTF(stderr, "\%s\%s", count > 0 ? ", " : "", tokenNames[bit]); 
							count++;
						}
					}
					ANTLR3_FPRINTF(stderr, "\n");
				}
				else
				{
					//ANTLR3_FPRINTF(stderr, "Actually dude, we didn't seem to be expecting anything here, or at least\n");
					//ANTLR3_FPRINTF(stderr, "I could not work out what I was expecting, like so many of us these days!\n");
				}
			}
			break;

		case	ANTLR3_EARLY_EXIT_EXCEPTION:

			// We entered a loop requiring a number of token sequences
			// but found a token that ended that sequence earlier than
			// we should have done.
			//
			//ANTLR3_FPRINTF(stderr, " : missing elements...\n");
			//ANTLR3_FPRINTF(stderr, " :  faltando elementos...\n");
			sprintf(messageAux, " :  faltando elementos...\n");
			strcat(messageError, messageAux);
			break;

		default:

			// We don't handle any other exceptions here, but you can
			// if you wish. If we get an exception that hits this point
			// then we are just going to report what we know about the
			// token.
			//
			//ANTLR3_FPRINTF(stderr, " : syntax not recognized...\n");
			//ANTLR3_FPRINTF(stderr, " : sintaxe não reconhecida...\n");
			sprintf(messageAux, " : sintaxe não reconhecida...\n");
			strcat(messageError, messageAux);
			break;
		}

		// Here you have the token that was in error which if this is
		// the standard implementation will tell you the line and offset
		// and also record the address of the start of the line in the
		// input stream. You could therefore print the source line and so on.
		// Generally though, I would expect that your lexer/parser will keep
		// its own map of lines and source pointers or whatever as there
		// are a lot of specific things you need to know about the input
		// to do something like that.
		// Here is where you do it though :-).
		//

	}
	
	
	
	static void *getMissingSymbolNew(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_INT_STREAM	istream, pANTLR3_EXCEPTION	e, ANTLR3_UINT32 expectedTokenType, pANTLR3_BITSET_LIST follow)
	{
	pANTLR3_TOKEN_STREAM			ts;
	pANTLR3_COMMON_TOKEN_STREAM		cts;
	pANTLR3_COMMON_TOKEN			token;
	pANTLR3_COMMON_TOKEN			current;
	pANTLR3_STRING					text;

	// Dereference the standard pointers
	//
	ts		= (pANTLR3_TOKEN_STREAM)istream->super;
	cts		= (pANTLR3_COMMON_TOKEN_STREAM)ts->super;
	
	// Work out what to use as the current symbol to make a line and offset etc
	// If we are at EOF, we use the token before EOF
	//
	current	= ts->_LT(ts, 1);
	if	(current->getType(current) == ANTLR3_TOKEN_EOF)
	{
		current = ts->_LT(ts, -1);
	}
	
	// Finally return the pointer to our new token
	//
	return	current;
	}
	
	
}

@parser::apifuncs {
  RECOGNIZER->displayRecognitionError = displayRecognitionErrorNew;
  RECOGNIZER->getMissingSymbol            = getMissingSymbolNew;
}





start_rule	:	prog;

prog	:	line+;
			

line	:	(cmd SEMICOLON!?) ;


cmd	:	arg
		|narg
		|make
		|ife
		|fore
		|repeat
		|whilee
		|assign
		|speake
		|commente
		|procedec
		|proceinv
		;
		
		
//block	:	cmd ( SEMICOLON!? cmd)* SEMICOLON!?;

		
arg	:	(FW^| BW^| RTURN^| LTURN^| WAIT^) expression;


narg	:	(SCAN | POS | RANGER | QUIT | COLOR | SOUND | HIST | STATE);
 
		
signEXP	:	(PLUS^ | MINUS^)? atom;


multEXP	:	signEXP ((MULT^ | DIV^) signEXP)*;


expression	:	multEXP ((PLUS^ | MINUS^) multEXP)*;


comparison	:	expression COMP expression;


then	:	THEN^ prog;


elsee	:	ELSEE^ prog;


forblock	:	DOIT^ cmd ( SEMICOLON!? cmd)* SEMICOLON!? ENDFOR!;


whileblock	:	DOIT^ cmd ( SEMICOLON!? cmd)* SEMICOLON!? ENDWHILE!;


reptblock	:	REPTB^ cmd ( SEMICOLON!? cmd)* SEMICOLON!? ENDREPT!;


procblock	:	DOIT^ cmd ( SEMICOLON!? cmd)* SEMICOLON!? ENDPROC!;


ife	:	IFE^  comparison then elsee? ENDIF!;


fore	:	FORE^ (make|assign) SEMICOLON! comparison SEMICOLON! assign forblock;


whilee	:	WHILEE^ comparison whileblock ;


repeat	:	REPEAT^ expression reptblock;


make	:	MAKE^ ID (EQ! expression)?;


assign	:	ID EQ^ expression;


speake	:	SPEAKE^ (expression | STRINGE) ;


commente	:	COMENT;


procedec	:	PROCDEC^ ID (':'! (ID) (','! (ID))*)? procblock;


proceinv	:	ID PROCINV^ (expression (','! expression)*)? ']'!;

	
atom	:	POS
		|RANGER
		|COLOR
		|INT
		|ID
		;


//BLOCK	:	'[';



PROCINV	:	'[';	


PROCDEC	:	(('A'|'a')('P'|'p')('R'|'r')('E'|'e')('N'|'n')('D'|'d')('E'|'e')('R'|'r'))
		;
		
		
ENDPROC	:	(('F'|'f')('I'|'i')('M'|'m')(' ')+('A'|'a')('P'|'p')('R'|'r')('E'|'e')('N'|'n')('D'|'d')('E'|'e')('R'|'r'))
		;


FORE	:	(('P'|'p')('A'|'a')('R'|'r')('A'|'a')(' ')?)
		;
		
DOIT	:	(('F'|'f')('A'|'a')('Ç'|'ç'|'C'|'c')('A'|'a'))
		;

ENDFOR	:	(('F'|'f')('I'|'i')('M'|'m')(' ')+('P'|'p')('A'|'a')('R'|'r')('A'|'a'))
		;
		
WHILEE	:	(('E'|'e')('N'|'n')('Q'|'q')('U'|'u')('A'|'a')('N'|'n')('T'|'t')('O'|'o'))
		;
		
ENDWHILE	:	(('F'|'f')('I'|'i')('M'|'m')(' ')+('E'|'e')('N'|'n')('Q'|'q')('U'|'u')('A'|'a')('N'|'n')('T'|'t')('O'|'o'))
		;
		

SPEAKE	:	(('F'|'f')('A'|'a')('L'|'l')('A'|'a')('R'|'r'))
		;
		
		
HIST	:	(('H'|'h')('I'|'i')('S'|'s')('T'|'t')('Ó'|'ó'|'O'|'o')('R'|'r')('I'|'i')('C'|'c')('O'|'o'))
		;

STATE	:	(('E'|'e')('S'|'s')('T'|'t')('A'|'a')('D'|'d')('O'|'o'))
		;

		
QUIT	:	(('S'|'s')('A'|'a')('I'|'i')('R'|'r'))
		;

/*forward*/ 
FW	:	(('P'|'p')('F'|'f'))
		|(('P'|'p')('A'|'a')('R'|'r')('A'|'a') (' ')+ ('F'|'f')('R'|'r')('E'|'e')('N'|'n')('T'|'t')('E'|'e'))
		;

/*backward*/
BW	:	(('P'|'p')('T'|'t'))
		|(('P'|'p')('A'|'a')('R'|'r')('A'|'a') (' ')+ ('T'|'t')('R'|'r')('A'|'a'|'Á'|'á')('S'|'s'))
		;

/*right turn*/
RTURN	:	(('G'|'g')('D'|'d'))
		|(('G'|'g')('I'|'i')('R'|'r')('A'|'a')('R'|'r') (' ')+ ('D'|'d')('I'|'i')('R'|'r')('E'|'e')('I'|'i')('T'|'t')('A'|'a'))
		;

/*left turn*/	
LTURN	:	(('G'|'g')('E'|'e'))
		|(('G'|'g')('I'|'i')('R'|'r')('A'|'a')('R'|'r') (' ')+ ('E'|'e')('S'|'s')('Q'|'q')('U'|'u')('E'|'e')('R'|'r')('D'|'d')('A'|'a'))
		;
		
SCAN	:	(('E'|'e')('S'|'s')('P'|'p')('I'|'i')('A'|'a')('R'|'r'));

RANGER	
:	(('D'|'d')('I'|'i')('S'|'s')('T'|'t')('â'|'Â'|'A'|'a')('N'|'n')('C'|'c')('I'|'i')('A'|'a'))
		(' ')+ (('F'|'f')|('FE'|'fe'|'Fe'|'fE')|('FD'|'fd'|'Fd'|'fD')|('T'|'t')|('TE'|'te'|'Te'|'tE')|('TD'|'td'|'Td'|'tD'))
		;
		
POS	:	(('P'|'p')('O'|'o')('S'|'s')(('I'|'i')('Ç'|'ç'|'C'|'c')('Ã'|'ã'|'A'|'a')('O'|'o'))?) (' ')+ (('X'|'x') | ('Y'|'y') | ('A'|'a'))
		;

COLOR	:	(('C'|'c')('O'|'o')('R'|'r')) (' ')+ ((('A'|'a')('Z'|'z')('U'|'u')('L'|'l')) | (('V'|'v')('E'|'e')('R'|'r')('D'|'d')('E'|'e')) | (('V'|'v')('E'|'e')('R'|'r')('M'|'m')('E'|'e')('L'|'l')('H'|'h')('O'|'o')))
		;

SOUND	:	(('S'|'s')('O'|'o')('M'|'m')) (' ')+ ((('L'|'l')('I'|'i')('G'|'g')('A'|'a')('D'|'d')('O'|'o')) | (('D'|'d')('E'|'e')('S'|'s')('L'|'l')('I'|'i')('G'|'g')('A'|'a')('D'|'d')('O'|'o')))
		;
		
		
THEN	:	(('E'|'e')('N'|'n')('T'|'t')('Ã'|'ã'|'A'|'a')('O'|'o'))
		;
		
IFE	:	(('S'|'s')('E'|'e'))	
		;
		
ENDIF	:	(('F'|'f')('I'|'i')('M'|'m')(' ')+('S'|'s')('E'|'e'))
		;
		
ELSEE	:	(('S'|'s')('E'|'e')('N'|'n')('Ã'|'ã'|'A'|'a')('O'|'o'))
		;
		
REPEAT	:	(('R'|'r')('E'|'e')('P'|'p')('I'|'i')('T'|'t')('A'|'a'))
		;


REPTB	:	(('V'|'v')('E'|'e')('Z'|'z')('E'|'e')('S'|'s'))
		;
		
ENDREPT	:	(('F'|'f')('I'|'i')('M'|'m')(' ')+('R'|'r')('E'|'e')('P'|'p')('I'|'i')('T'|'t')('A'|'a'))
		;

WAIT	:	(('E'|'e')('S'|'s')('P'|'p')('E'|'e')('R'|'r')('A'|'a')('R'|'r'))
		;

MAKE	:	(('C'|'c')('R'|'r')('I'|'i')('A'|'a')('R'|'r'))
		;
	
/*
fragment
Latin1_Supplement                   :   '\u00A0' .. '\u00FF' ;

	
ID	:	('A'..'Z'|'a'..'z'|Latin1_Supplement)('a'..'z'|'A'..'Z'|Latin1_Supplement|'0'..'9'|'_')*;
*/			

ID	:	('A'..'Z'|'a'..'z')('a'..'z'|'A'..'Z'|'0'..'9'|'_')*;

STRINGE	:	'"' (~('"'))+ '"';


COMENT	:	'--' (~('\r'|'\n'))* ('\r\n'|'\r'|'\n'|EOF);
		
		
MINUS	:	'-';

MULT	:	'*';

DIV	:	'/';		
		
PLUS	:	'+';

EQ	:	'=';
		
COMP	:	'<' | '>' | '==' | '>='| '<=' | '<>';
		
SEMICOLON	:	';';

INT	:	'0'..'9'+;

EOL	:	('\r'
		| '\n'){$channel=HIDDEN;};

/*WS	:	( ' ' | '\t')+;*/
WS	:	( ' ' | '\t')+ {$channel=HIDDEN;};



