#include <iostream>
#include <exception>
#include "Exception.h"

using namespace std;

parametroException::parametroException(string error)
{
	this->m_msg = error;
}
parametroException::parametroException()
{
	this->m_msg = "Parametro invalido\n";
}
const char* parametroException::what() const throw()
{
	return "Parametro invalido\n";
}


sintaxeException::sintaxeException(string error)
{
	this->m_msg = error;
}
sintaxeException::sintaxeException()
{
	this->m_msg = "Erro de Sintaxe\n";
}
const char* sintaxeException::what() const throw()
{
	return this->m_msg.c_str();
}


variavelException::variavelException(string error)
{
	this->m_msg = error;
}
variavelException::variavelException()
{
	this->m_msg = "Erro Variavel Inexistente\n";
}
const char* variavelException::what() const throw()
{
	return this->m_msg.c_str();
}

