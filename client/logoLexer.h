/** \file
 *  This C header file was generated by $ANTLR version 3.2 Sep 23, 2009 12:02:23
 *
 *     -  From the grammar source file : logo.g
 *     -                            On : 2016-08-05 17:19:00
 *     -                 for the lexer : logoLexerLexer *
 * Editing it, at least manually, is not wise. 
 *
 * C language generator and runtime by Jim Idle, jimi|hereisanat|idle|dotgoeshere|ws.
 *
 *
 * The lexer logoLexer has the callable functions (rules) shown below,
 * which will invoke the code for the associated rule in the source grammar
 * assuming that the input stream is pointing to a token/text stream that could begin
 * this rule.
 * 
 * For instance if you call the first (topmost) rule in a parser grammar, you will
 * get the results of a full parse, but calling a rule half way through the grammar will
 * allow you to pass part of a full token stream to the parser, such as for syntax checking
 * in editors and so on.
 *
 * The parser entry points are called indirectly (by function pointer to function) via
 * a parser context typedef plogoLexer, which is returned from a call to logoLexerNew().
 *
 * As this is a generated lexer, it is unlikely you will call it 'manually'. However
 * the methods are provided anyway.
 * * The methods in plogoLexer are  as follows:
 *
 *  -  void      plogoLexer->T__45(plogoLexer)
 *  -  void      plogoLexer->T__46(plogoLexer)
 *  -  void      plogoLexer->T__47(plogoLexer)
 *  -  void      plogoLexer->PROCINV(plogoLexer)
 *  -  void      plogoLexer->PROCDEC(plogoLexer)
 *  -  void      plogoLexer->PROCB(plogoLexer)
 *  -  void      plogoLexer->FIMPROC(plogoLexer)
 *  -  void      plogoLexer->FORE(plogoLexer)
 *  -  void      plogoLexer->FORB(plogoLexer)
 *  -  void      plogoLexer->FIMFOR(plogoLexer)
 *  -  void      plogoLexer->PRINTE(plogoLexer)
 *  -  void      plogoLexer->HIST(plogoLexer)
 *  -  void      plogoLexer->QUIT(plogoLexer)
 *  -  void      plogoLexer->PF(plogoLexer)
 *  -  void      plogoLexer->PT(plogoLexer)
 *  -  void      plogoLexer->PD(plogoLexer)
 *  -  void      plogoLexer->PE(plogoLexer)
 *  -  void      plogoLexer->SCAN(plogoLexer)
 *  -  void      plogoLexer->RANGER(plogoLexer)
 *  -  void      plogoLexer->INFO(plogoLexer)
 *  -  void      plogoLexer->POS(plogoLexer)
 *  -  void      plogoLexer->THEN(plogoLexer)
 *  -  void      plogoLexer->IFE(plogoLexer)
 *  -  void      plogoLexer->FIMIF(plogoLexer)
 *  -  void      plogoLexer->ELSEE(plogoLexer)
 *  -  void      plogoLexer->REPEAT(plogoLexer)
 *  -  void      plogoLexer->REPTB(plogoLexer)
 *  -  void      plogoLexer->FIMREPT(plogoLexer)
 *  -  void      plogoLexer->ESPERA(plogoLexer)
 *  -  void      plogoLexer->MAKE(plogoLexer)
 *  -  void      plogoLexer->Latin1_Supplement(plogoLexer)
 *  -  void      plogoLexer->ID(plogoLexer)
 *  -  void      plogoLexer->STRINGE(plogoLexer)
 *  -  void      plogoLexer->COMENT(plogoLexer)
 *  -  void      plogoLexer->MINUS(plogoLexer)
 *  -  void      plogoLexer->MULT(plogoLexer)
 *  -  void      plogoLexer->DIV(plogoLexer)
 *  -  void      plogoLexer->PLUS(plogoLexer)
 *  -  void      plogoLexer->EQ(plogoLexer)
 *  -  void      plogoLexer->COMP(plogoLexer)
 *  -  void      plogoLexer->SEMICOLON(plogoLexer)
 *  -  void      plogoLexer->INT(plogoLexer)
 *  -  void      plogoLexer->EOL(plogoLexer)
 *  -  void      plogoLexer->WS(plogoLexer)
 *  -  void      plogoLexer->Tokens(plogoLexer)
 *
 * The return type for any particular rule is of course determined by the source
 * grammar file.
 */
// [The "BSD licence"]
// Copyright (c) 2005-2009 Jim Idle, Temporal Wave LLC
// http://www.temporal-wave.com
// http://www.linkedin.com/in/jimidle
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef	_logoLexer_H
#define _logoLexer_H
/* =============================================================================
 * Standard antlr3 C runtime definitions
 */
#include    <antlr3.h>

/* End of standard antlr 3 runtime definitions
 * =============================================================================
 */
 
#ifdef __cplusplus
extern "C" {
#endif

// Forward declare the context typedef so that we can use it before it is
// properly defined. Delegators and delegates (from import statements) are
// interdependent and their context structures contain pointers to each other
// C only allows such things to be declared if you pre-declare the typedef.
//
typedef struct logoLexer_Ctx_struct logoLexer, * plogoLexer;



#ifdef	ANTLR3_WINDOWS
// Disable: Unreferenced parameter,							- Rules with parameters that are not used
//          constant conditional,							- ANTLR realizes that a prediction is always true (synpred usually)
//          initialized but unused variable					- tree rewrite variables declared but not needed
//          Unreferenced local variable						- lexer rule declares but does not always use _type
//          potentially unitialized variable used			- retval always returned from a rule 
//			unreferenced local function has been removed	- susually getTokenNames or freeScope, they can go without warnigns
//
// These are only really displayed at warning level /W4 but that is the code ideal I am aiming at
// and the codegen must generate some of these warnings by necessity, apart from 4100, which is
// usually generated when a parser rule is given a parameter that it does not use. Mostly though
// this is a matter of orthogonality hence I disable that one.
//
#pragma warning( disable : 4100 )
#pragma warning( disable : 4101 )
#pragma warning( disable : 4127 )
#pragma warning( disable : 4189 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4701 )
#endif

/** Context tracking structure for logoLexer
 */
struct logoLexer_Ctx_struct
{
    /** Built in ANTLR3 context tracker contains all the generic elements
     *  required for context tracking.
     */
    pANTLR3_LEXER    pLexer;


     void (*mT__45)	(struct logoLexer_Ctx_struct * ctx);
     void (*mT__46)	(struct logoLexer_Ctx_struct * ctx);
     void (*mT__47)	(struct logoLexer_Ctx_struct * ctx);
     void (*mPROCINV)	(struct logoLexer_Ctx_struct * ctx);
     void (*mPROCDEC)	(struct logoLexer_Ctx_struct * ctx);
     void (*mPROCB)	(struct logoLexer_Ctx_struct * ctx);
     void (*mFIMPROC)	(struct logoLexer_Ctx_struct * ctx);
     void (*mFORE)	(struct logoLexer_Ctx_struct * ctx);
     void (*mFORB)	(struct logoLexer_Ctx_struct * ctx);
     void (*mFIMFOR)	(struct logoLexer_Ctx_struct * ctx);
     void (*mPRINTE)	(struct logoLexer_Ctx_struct * ctx);
     void (*mHIST)	(struct logoLexer_Ctx_struct * ctx);
     void (*mQUIT)	(struct logoLexer_Ctx_struct * ctx);
     void (*mPF)	(struct logoLexer_Ctx_struct * ctx);
     void (*mPT)	(struct logoLexer_Ctx_struct * ctx);
     void (*mPD)	(struct logoLexer_Ctx_struct * ctx);
     void (*mPE)	(struct logoLexer_Ctx_struct * ctx);
     void (*mSCAN)	(struct logoLexer_Ctx_struct * ctx);
     void (*mRANGER)	(struct logoLexer_Ctx_struct * ctx);
     void (*mINFO)	(struct logoLexer_Ctx_struct * ctx);
     void (*mPOS)	(struct logoLexer_Ctx_struct * ctx);
     void (*mTHEN)	(struct logoLexer_Ctx_struct * ctx);
     void (*mIFE)	(struct logoLexer_Ctx_struct * ctx);
     void (*mFIMIF)	(struct logoLexer_Ctx_struct * ctx);
     void (*mELSEE)	(struct logoLexer_Ctx_struct * ctx);
     void (*mREPEAT)	(struct logoLexer_Ctx_struct * ctx);
     void (*mREPTB)	(struct logoLexer_Ctx_struct * ctx);
     void (*mFIMREPT)	(struct logoLexer_Ctx_struct * ctx);
     void (*mESPERA)	(struct logoLexer_Ctx_struct * ctx);
     void (*mMAKE)	(struct logoLexer_Ctx_struct * ctx);
     void (*mLatin1_Supplement)	(struct logoLexer_Ctx_struct * ctx);
     void (*mID)	(struct logoLexer_Ctx_struct * ctx);
     void (*mSTRINGE)	(struct logoLexer_Ctx_struct * ctx);
     void (*mCOMENT)	(struct logoLexer_Ctx_struct * ctx);
     void (*mMINUS)	(struct logoLexer_Ctx_struct * ctx);
     void (*mMULT)	(struct logoLexer_Ctx_struct * ctx);
     void (*mDIV)	(struct logoLexer_Ctx_struct * ctx);
     void (*mPLUS)	(struct logoLexer_Ctx_struct * ctx);
     void (*mEQ)	(struct logoLexer_Ctx_struct * ctx);
     void (*mCOMP)	(struct logoLexer_Ctx_struct * ctx);
     void (*mSEMICOLON)	(struct logoLexer_Ctx_struct * ctx);
     void (*mINT)	(struct logoLexer_Ctx_struct * ctx);
     void (*mEOL)	(struct logoLexer_Ctx_struct * ctx);
     void (*mWS)	(struct logoLexer_Ctx_struct * ctx);
     void (*mTokens)	(struct logoLexer_Ctx_struct * ctx);    const char * (*getGrammarFileName)();
    void	    (*free)   (struct logoLexer_Ctx_struct * ctx);
        
};

// Function protoypes for the constructor functions that external translation units
// such as delegators and delegates may wish to call.
//
ANTLR3_API plogoLexer logoLexerNew         (pANTLR3_INPUT_STREAM instream);
ANTLR3_API plogoLexer logoLexerNewSSD      (pANTLR3_INPUT_STREAM instream, pANTLR3_RECOGNIZER_SHARED_STATE state);

/** Symbolic definitions of all the tokens that the lexer will work with.
 * \{
 *
 * Antlr will define EOF, but we can't use that as it it is too common in
 * in C header files and that would be confusing. There is no way to filter this out at the moment
 * so we just undef it here for now. That isn't the value we get back from C recognizers
 * anyway. We are looking for ANTLR3_TOKEN_EOF.
 */
#ifdef	EOF
#undef	EOF
#endif
#ifdef	Tokens
#undef	Tokens
#endif 
#define HIST      42
#define PT      7
#define FIMREPT      26
#define INFO      12
#define MINUS      17
#define REPTB      25
#define COMP      20
#define MULT      18
#define PROCDEC      39
#define SEMICOLON      4
#define FORB      23
#define ID      34
#define COMENT      38
#define FORE      31
#define PROCB      27
#define FIMPROC      28
#define EQ      35
#define Latin1_Supplement      43
#define POS      13
#define THEN      21
#define PLUS      16
#define REPEAT      32
#define IFE      29
#define MAKE      33
#define INT      41
#define SCAN      11
#define WS      5
#define ELSEE      22
#define EOF      -1
#define FIMFOR      24
#define PROCINV      40
#define EOL      44
#define ESPERA      10
#define QUIT      15
#define RANGER      14
#define DIV      19
#define PRINTE      36
#define PD      8
#define PE      9
#define PF      6
#define STRINGE      37
#define T__45      45
#define T__46      46
#define T__47      47
#define FIMIF      30
#ifdef	EOF
#undef	EOF
#define	EOF	ANTLR3_TOKEN_EOF
#endif

#ifndef TOKENSOURCE
#define TOKENSOURCE(lxr) lxr->pLexer->rec->state->tokSource
#endif

/* End of token definitions for logoLexer
 * =============================================================================
 */
/** \} */

#ifdef __cplusplus
}
#endif

#endif

/* END - Note:Keep extra line feed to satisfy UNIX systems */
