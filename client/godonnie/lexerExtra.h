#include <antlr3baserecognizer.h>
#include <antlr3defs.h>
#include <antlr3lexer.h>

#ifndef def_displayRecognitionErrorNewLexer
#define def_displayRecognitionErrorNewLexer
//static void displayRecognitionErrorNewLexer(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames);
static void displayRecognitionErrorNewLexer(pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8* tokenNames) {
    pANTLR3_LEXER lexer;
    pANTLR3_EXCEPTION ex;
    pANTLR3_STRING ftext;
    //pANTLR3_COMMON_TOKEN    theToken;

    lexer = (pANTLR3_LEXER)(recognizer->super);
    ex = lexer->rec->state->exception;

    // See if there is a 'filename' we can use
    //
    if (ex->name == NULL) {
	//ANTLR3_FPRINTF(stderr, "-unknown source-(");
    } else {
	//ftext = ex->streamName->to8(ex->streamName);
	//ANTLR3_FPRINTF(stderr, "\%s(", ftext->chars);
    }

    ANTLR3_FPRINTF(stderr, "Erro na linha %d ", recognizer->state->exception->line);
    ANTLR3_FPRINTF(stderr, "e na coluna %d. ", (recognizer->state->exception->charPositionInLine));
    ANTLR3_FPRINTF(stderr, " Palavra invalida \n");

    //ANTLR3_FPRINTF(stderr, "Verificar comando %s \n", ttext == NULL ? (pANTLR3_UINT8)"<palavra não identificada>" : ttext2->chars);
    if (ex->c == (char)ANTLR3_TOKEN_EOF) {
	//ANTLR3_FPRINTF(stderr, "Verificar fim da linha.\n");
    } else {
	//ANTLR3_FPRINTF(stderr, "Verificar letra %d %c \n", (unsigned char)ex->c, ex->c);
    }
}
#endif
