#include <iostream>
#include <exception>
#include "Exception.h"

using namespace std;


const char* tipoException::what() const throw()
{
	return "Erro de tipo invalido\n";
}
const char* sintaxeException::what() const throw()
{
	return "Erro Sintaxe invalida\n";
}
const char* variavelException::what() const throw()
{
	return "Erro Variavel Inexistente\n";
}

