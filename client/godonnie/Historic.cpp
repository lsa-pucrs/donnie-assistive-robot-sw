#include "DonnieClient.h"
#include "Historic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>


using namespace std;

//Historic
Historic Historic::singleton = Historic();
Historic::Historic()
{
	 this->commandsList = list<Command>(); 
	 this->iterator = commandsList.begin();
}
Historic::~Historic(){}
Historic& Historic::getHistoric()
{
	return singleton;
}
bool Historic::addCommand(string name, string feedback)
{
	Command temp = Command();
	temp.name = name;
	temp.feedback = feedback;
	temp.posx = DonnieClient::getInstance().GetPos(0); //Alterar para pegar a posição do DonnieClient
	temp.posy = DonnieClient::getInstance().GetPos(1); //Alterar para pegar a posição do DonnieClient
	temp.degree = DonnieClient::getInstance().GetPos(2); //Alterar para pegar a posição do DonnieClient
	this->commandsList.push_front(temp);
	return true;
}
int Historic::size()
{
	return this->commandsList.size();
}
bool Historic::resetIterator()
{
	this->iterator = commandsList.begin();
	return true;
}
string Historic::getLast()
{
	if(this->size() == 0)
	{
		string resposta = "Não há nenhum Comando";
		return resposta;
	}
	Command temp = this->commandsList.front();
	return temp.name;
}
string Historic::getLine(int line)
{
	if(line < 1 || line > this->size())
	{
		return string("Linha Invalida"); // Não sei se é bom retornar string assim
	}
	this->resetIterator();
	
	for(int i = 1; i < line; i++)// Need to do this because of the bidirectional nature of the iterator
	{	
		iterator++;
	}
	
	Command temp = *iterator;
	return temp.name;
}
string Historic::previous()
{
	string resposta = "";
	int iteratorIndex = distance(commandsList.begin(), iterator); //iterator - commandsList.begin();
	if(this->size() == 0)
	{
		resposta += "Não há nenhum Comando.";
		return resposta;
	}
	else if(iterator == commandsList.begin())
	{
		resposta += "Não há nenhum comando antes depois dessa linha.\n";
	}
	

	iterator++;
	iteratorIndex++;
	if(iterator == commandsList.end())
	{
		resposta += "Está é a última linha. Não há comandos depois desta linha.\n";	
	}
	if(iterator == commandsList.begin())
	{
		resposta += "Esta é a primeira linha. Não há comandos antes dessa linha.\n";
	}	
	Command temp = *iterator;
		resposta += temp.name;
		resposta += "\n";
	return resposta;
}

string Historic::next()
{
	string resposta = "";
	int iteratorIndex = distance(commandsList.begin(), iterator); //iterator - commandsList.begin();
	if(this->size() == 0)
	{
		resposta += "Não há nenhum Comando.";
		return resposta;
	}
	else if(iterator == commandsList.begin())
	{
		resposta += "Não há nenhum comando depois dessa linha.\n";
	}
	
	
	iterator--;
	iteratorIndex--;
	if(iterator == commandsList.end())
	{
		resposta += "Esta é a última linha. Não há comandos depois desta linha.\n";	
	}
	if(iterator == commandsList.begin())
	{
		resposta += "Esta é a primeira linha. Não há comandos antes dessa linha.\n";
	}	
	Command temp = *iterator;
		resposta += "O comando foi " + temp.name + "e o resultado foi " + temp.feedback + "\n";
	return resposta;
}
