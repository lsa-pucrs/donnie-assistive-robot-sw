grammar GoDonnie;

options
{
	language = C;
	output = AST;
   	ASTLabelType=pANTLR3_BASE_TREE;
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
		|printe
		|comentario
		|procedec
		|proceinv
		|history
		;
		
		
//block	:	cmd ( SEMICOLON!? cmd)* SEMICOLON!?;

		
arg	:	(PF^| PT^| PD^| PE^| ESPERA^) expression;


narg	:	(SCAN | INFO | POS | RANGER | QUIT);
 
		
signEXP	:	(PLUS^ | MINUS^)? atom;


multEXP	:	signEXP ((MULT^ | DIV^) signEXP)*;


expression	:	multEXP ((PLUS^ | MINUS^) multEXP)*;


comparison	:	expression COMP expression;


then	:	THEN^ prog;


elsee	:	ELSEE^ prog;


forblock	:	FACA^ cmd ( SEMICOLON!? cmd)* SEMICOLON!? FIMFOR!;


whileblock	:	FACA^ cmd ( SEMICOLON!? cmd)* SEMICOLON!? FIMWHILE!;


reptblock	:	REPTB^ cmd ( SEMICOLON!? cmd)* SEMICOLON!? FIMREPT!;


procblock	:	PROCB^ cmd ( SEMICOLON!? cmd)* SEMICOLON!? FIMPROC!;


ife	:	IFE^  comparison then elsee? FIMIF!;


fore	:	FORE^ (make|assign) SEMICOLON! comparison SEMICOLON! assign forblock;


whilee	:	WHILEE^ comparison whileblock ;


repeat	:	REPEAT^ expression reptblock;


make	:	MAKE^ ID (EQ! expression)?;


assign	:	ID EQ^ expression;


printe	:	PRINTE^ (expression | STRINGE) ;


comentario	:	COMENT;

history     :   HIST;

procedec	:	PROCDEC^ ID (':'! (ID) (','! (ID))*)? procblock;


proceinv	:	ID PROCINV^ (expression (','! expression)*)? ']'!;

	
atom	:	POS
		|RANGER
		|INT
		|ID
		;


//BLOCK	:	'[';



PROCINV	:	'[';	


PROCDEC	:	(('A'|'a')('P'|'p')('R'|'r')('E'|'e')('N'|'n')('D'|'d')('E'|'e')('R'|'r'))
		;
		
PROCB	:	(('I'|'i')('N'|'n')('Í'|'í')('C'|'c')('I'|'i')('O'|'o'))
		;
		
FIMPROC	:	(('F'|'f')('I'|'i')('M'|'m')(' ')('A'|'a')('P'|'p')('R'|'r')('E'|'e')('N'|'n')('D'|'d')('E'|'e')('R'|'r'))
		;


FORE	:	(('P'|'p')('A'|'a')('R'|'r')('A'|'a')(' ')?)
		;
		
FACA	:	(('F'|'f')('A'|'a')('Ç'|'ç')('A'|'a'))
		;

FIMFOR	:	(('F'|'f')('I'|'i')('M'|'m')(' ')('P'|'p')('A'|'a')('R'|'r')('A'|'a'))
		;
		
WHILEE	:	(('E'|'e')('N'|'n')('Q'|'q')('U'|'u')('A'|'a')('N'|'n')('T'|'t')('O'|'o'))
		;
		
FIMWHILE	:	(('F'|'f')('I'|'i')('M'|'m')(' ')('E'|'e')('N'|'n')('Q'|'q')('U'|'u')('A'|'a')('N'|'n')('T'|'t')('O'|'o'))
		;
		

PRINTE	:	(('F'|'f')('A'|'a')('L'|'l')('A'|'a')('R'|'r'))
		;
		
		
HIST	:	(('H'|'h')('I'|'i')('S'|'s')('T'|'t')('O'|'o')('R'|'r')('I'|'i')('C'|'c')('O'|'o'))
		;

		
QUIT	:	(('S'|'s')('A'|'a')('I'|'i')('R'|'r'))
		;

PF	:	(('P'|'p')('F'|'f'))
		|(('P'|'p')('A'|'a')('R'|'r')('A'|'a') ' ' ('F'|'f')('R'|'r')('E'|'e')('N'|'n')('T'|'t')('E'|'e'))
		;

PT	:	(('P'|'p')('T'|'t'))
		|(('P'|'p')('A'|'a')('R'|'r')('A'|'a') ' ' ('T'|'t')('R'|'r')('A'|'a')('Z'|'z'))
		;
		
PD	:	(('G'|'g')('D'|'d'))
		|(('G'|'g')('I'|'i')('R'|'r')('A'|'a')('R'|'r') ' ' ('D'|'d')('I'|'i')('R'|'r')('E'|'e')('I'|'i')('T'|'t')('A'|'a'))
		;
		
PE	:	(('G'|'g')('E'|'e'))
		|(('G'|'g')('I'|'i')('R'|'r')('A'|'a')('R'|'r') ' ' ('E'|'e')('S'|'s')('Q'|'q')('U'|'u')('E'|'e')('R'|'r')('D'|'d')('A'|'a'))
		;
		
SCAN	:	(('E'|'e')('S'|'s')('P'|'p')('I'|'i')('A'|'a')('R'|'r'));

RANGER	:	(('D'|'d')('I'|'i')('S'|'s')('T'|'t')('Â'|'â')('N'|'n')('C'|'c')('I'|'i')('A'|'a'))
		' ' (('F'|'f')|('FE'|'fe')|('FD'|'fd')|('T'|'t')|('TE'|'te')|('TD'|'td')|('C'|'c'))
		;
		
INFO	:	(('E'|'e')('S'|'s')('T'|'t')('A'|'a')('D'|'d')('O'|'o'))
		;
		
POS	:	(('P'|'p')('O'|'o')('S'|'s')(('I'|'i')('Ç'|'ç')('Ã'|'ã')('O'|'o'))?) ' ' (('X'|'x') | ('Y'|'y') | ('A'|'a'))
		;
		
THEN	:	(('E'|'e')('N'|'n')('T'|'t')('Ã'|'ã')('O'|'o'))
		;
		
IFE	:	(('S'|'s')('E'|'e'))	
		;
		
FIMIF	:	(('F'|'f')('I'|'i')('M'|'m')(' ')('S'|'s')('E'|'e'))
		;
		
ELSEE	:	(('S'|'s')('E'|'e')('N'|'n')('Ã'|'ã')('O'|'o'))
		;
		
REPEAT	:	(('R'|'r')('E'|'e')('P'|'p')('I'|'i')('T'|'t')('A'|'a'))
		;


REPTB	:	(('V'|'v')('E'|'e')('Z'|'z')('E'|'e')('S'|'s'))
		;
		
FIMREPT	:	(('F'|'f')('I'|'i')('M'|'m')(' ')('R'|'r')('E'|'e')('P'|'p')('I'|'i')('T'|'t')('A'|'a'))
		;

ESPERA	:	(('E'|'e')('S'|'s')('P'|'p')('E'|'e')('R'|'r')('A'|'a')('R'|'r'))
		;

MAKE	:	(('C'|'c')('R'|'r')('I'|'i')('A'|'a')('R'|'r'))
		;
	
fragment
Latin1_Supplement                   :   '\u00A0' .. '\u00FF' ;

	
ID	:	('A'..'Z'|'a'..'z'|Latin1_Supplement)('a'..'z'|'A'..'Z'|Latin1_Supplement|'0'..'9'|'_')*;
			
		
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

WS	:	( ' '
		| '\t'
		 ){$channel=HIDDEN;};

