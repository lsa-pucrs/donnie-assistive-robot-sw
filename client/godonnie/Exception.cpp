#include "Exception.h"
#include <exception>
#include <iostream>

using namespace std;

parametroException::parametroException(string error) {
    this->m_msg = error;
}
parametroException::parametroException() {
    this->m_msg = "Parametro inválido\n";
}
const char* parametroException::what() const throw() {
    return this->m_msg.c_str();
}

sintaxeException::sintaxeException(string error) {
    this->m_msg = error;
}
sintaxeException::sintaxeException() {
    this->m_msg = "Erro de Sintaxe\n";
}
const char* sintaxeException::what() const throw() {
    return this->m_msg.c_str();
}

variavelException::variavelException(string error) {
    this->m_msg = error;
}
variavelException::variavelException() {
    this->m_msg = "Erro Variável Inexistente\n";
}
const char* variavelException::what() const throw() {
    return this->m_msg.c_str();
}

invalidValueException::invalidValueException(string error) {
    this->m_msg = error;
}
invalidValueException::invalidValueException() {
    this->m_msg = "Valor Invalido para o Parametro\n";
}
const char* invalidValueException::what() const throw() {
    return this->m_msg.c_str();
}
