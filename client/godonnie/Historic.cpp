#include "DonnieClient.h"
#include "Historic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iomanip>


using namespace std;

//Historic
Historic *Historic::singleton = NULL;
Historic::Historic()
{
	 this->commandsList = list<Command>(); 
	 this->iterator = commandsList.begin();
}

Historic::~Historic()
{
	commandsList.clear();
}

Historic *Historic::getInstance()
{
	if (!singleton)   // Only allow one instance of class to be generated.
      singleton = new Historic();
	return singleton;
}

void Historic::ResetInstance()
{
      delete singleton; 
      singleton = NULL; 
}

bool Historic::addCommand(string name, string feedback)
{
	Command temp = Command();
	temp.name = name;
	temp.feedback = feedback;
	temp.posx = DonnieClient::getInstance()->GetPos("body",0);
	temp.posy = DonnieClient::getInstance()->GetPos("body",1);
	temp.degree = DonnieClient::getInstance()->GetPos("body",2);
	this->commandsList.push_back(temp);
	return true;
}
int Historic::size()
{
	return this->commandsList.size();
}

/*
 * 
void Historic::resetIterator()
{
	if(this->size() != 0)
	{
		this->iterator = commandsList.begin();
	}
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
*/
string Historic::show()
{
	stringstream history;
	int cmdNum=0;
	
	history << "Historico possui " << commandsList.size() << " comandos" << endl;
	for (this->iterator = this->commandsList.begin(); this->iterator != this->commandsList.end(); ++iterator) {
		history << "Comando " << cmdNum << " foi " << (*iterator).name << ", " <<  (*iterator).feedback << 
		", posicao [" << std::fixed << std::setprecision(2)  <<  (*iterator).posx  << 
		"," << std::fixed << std::setprecision(2) <<  (*iterator).posy  << 
		"," << std::fixed << std::setprecision(2) <<  (*iterator).degree << "]" << endl ;
		cmdNum++;
	}

	return history.str();
}

void Historic::clear()
{
	this->commandsList.clear();
}
