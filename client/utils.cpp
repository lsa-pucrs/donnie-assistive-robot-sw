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