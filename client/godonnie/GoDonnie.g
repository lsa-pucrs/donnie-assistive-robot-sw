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

