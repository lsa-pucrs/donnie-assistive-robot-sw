#include "DonnieClient.h"
#include "Historic.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iomanip>

using namespace std;

//Historic
Historic *Historic::singleton = NULL;
Historic::Historic()
{
	 this->commandsList = list<Command>(); 
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

string Historic::show()
{
	stringstream history;
	list<Command>::iterator iterator;
	int cmdNum=0;
	
	history << "Histórico possui " << commandsList.size() << " comandos" << endl;
	for (iterator = this->commandsList.begin(); iterator != this->commandsList.end(); ++iterator) {
		history << "Comando " << cmdNum << " foi " << (*iterator).name << ", " <<  (*iterator).feedback << 
		", posição [" << std::fixed << std::setprecision(2)  <<  (*iterator).posx  << 
		"," << std::fixed << std::setprecision(2) <<  (*iterator).posy  << 
		"," << std::fixed << std::setprecision(2) <<  (*iterator).degree << "]" << endl ;
		cmdNum++;
	}

	return history.str();
}

string Historic::getLast()
{
	stringstream history;
	Command last;
	
	if (commandsList.size() > 0){
		last = commandsList.back();
		history << "Comando " << commandsList.size()-1 << " foi " << last.name << ", " <<  last.feedback << 
		", posição [" << std::fixed << std::setprecision(2)  <<  last.posx  << 
		"," << std::fixed << std::setprecision(2) <<  last.posy  << 
		"," << std::fixed << std::setprecision(2) <<  last.degree << "]" << endl ;
	}

	return history.str();
}

void Historic::clear()
{
	this->commandsList.clear();
}
