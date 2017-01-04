/*
 * Desc: Exception classes to treat parse errors
 * Author: Henry Nunes
 * Date:  November 2016
 * Laboratório de Sistemas Autônomos 
 *  - https://lsa.pucrs.br/
 *  - https://github.com/lsa-pucrs
 * Faculdade de Informática - PUCRS  
 *  - www.inf.pucrs.br
 * 
 * follow tips in to change the exception behavior of the C runtime library
 * http://stackoverflow.com/questions/4751699/what-is-the-antlr3-c-runtime-equivalent-of-throwing-an-exception-for-error-repor
 * 
 */

#pragma once
#ifndef EXCEPTION_H
#define EXCEPTION_H

using namespace std;

//TODO (amory). mudar nome das classes para ingles
class parametroException: public exception
{
		string m_msg;
	public:
		parametroException(string error);
		parametroException();
		const char* what() const throw();
};
class sintaxeException: public exception
{
		string m_msg;
	public:
		sintaxeException(string error);
		sintaxeException();
		virtual const char* what() const throw();
};
class variavelException: public exception
{
		string m_msg;
	public:
		variavelException(string error);
		variavelException();
		virtual const char* what() const throw();
};

class invalidValueException: public exception
{
		string m_msg;
	public:
		invalidValueException(string error);
		invalidValueException();
		virtual const char* what() const throw();
};

#endif
