#include <iostream>
#include <exception>
#include "Exception.h"

using namespace std;


const char* tipoException::what() const throw()
{
	return "Erro de tipo invalido";
}
const char* sintaxeException::what() const throw()
{
	return "Erro sintaxe invalida";
}

