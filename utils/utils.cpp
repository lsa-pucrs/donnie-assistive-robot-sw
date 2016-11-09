#include "utils.h"

using namespace PlayerCc;


//////////////Converte radianos negativos ou positivos em radianos positivos EX. -1,57079 = 4,71238/////////////////////
double radTOrad(double rad)
{
	if(rad < 0)
	{
		rad = 2*M_PI + rad;
	}
	return rad;
}

////////////////////Converte radianos negativos ou positivos em graus positivos EX. -1,57079 = 270///////////////////////
double radTOdeg(double rad)
{
	if(rad < 0)
		rad = 360 + rtod(rad);
	else
		rad = rtod(rad);
	return rad;
}

///////////////////Converte graus negativos ou positivos em radianos positivos EX. -60 = 5,23598/////////////////////////

double degTOrad(double deg)
{
	if(deg < 0)
		deg = 2*M_PI + dtor(deg);
	else
		deg = dtor(deg);
	return deg;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string GetEnv( const string & var ) 
{
    const char * val = ::getenv( var.c_str() );
    if ( val == 0 ) {
       DEBUG_MSG("\033[41mWarning!\033[0m " << var.c_str() << " environment variable are not set!"); //TODO BUG -> Essa macro nesse arquivo nunca executa pois o NDEBUG nunca esta setado. Aparentemente o utils.h compila antes do main.cc nesse meu makefile
       return "";
    }
    else {
       return val;
    }
}
