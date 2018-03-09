#include <stdlib.h>
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
	generator gen;
	gen.add_messages_path(string(getenv("DONNIE_SOURCE_PATH")) + "/loc");
	gen.add_messages_domain("Exception");
	locale loc = gen(string(getenv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);
	this->m_msg = translate("Parametro inválido\n");
}
const char* parametroException::what() const throw()
{
	return this->m_msg.c_str();
}


sintaxeException::sintaxeException(string error)
{
	this->m_msg = error;
}
sintaxeException::sintaxeException()
{
	generator gen;
	gen.add_messages_path(string(getenv("DONNIE_SOURCE_PATH")) + "/loc");
	gen.add_messages_domain("Exception");
	locale loc = gen(string(getenv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);
	this->m_msg = translate("Erro de Sintaxe\n");
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
	generator gen;
	gen.add_messages_path(string(getenv("DONNIE_SOURCE_PATH")) + "/loc");
	gen.add_messages_domain("Exception");
	locale loc = gen(string(getenv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);
	this->m_msg = translate("Erro Variável Inexistente\n");
}
const char* variavelException::what() const throw()
{
	return this->m_msg.c_str();
}

invalidValueException::invalidValueException(string error)
{
	this->m_msg = error;
}
invalidValueException::invalidValueException()
{
	generator gen;
	gen.add_messages_path(string(getenv("DONNIE_SOURCE_PATH")) + "/loc");
	gen.add_messages_domain("Exception");
	locale loc = gen(string(getenv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);
	this->m_msg = translate("Valor Invalido para o Parametro\n");
}
const char* invalidValueException::what() const throw()
{
	return this->m_msg.c_str();
}


