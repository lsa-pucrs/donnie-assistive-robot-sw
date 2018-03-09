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
	temp.posx = (int)DonnieClient::getInstance()->GetPos("body",0);
	temp.posy = (int)DonnieClient::getInstance()->GetPos("body",1);
	temp.degree = (int)DonnieClient::getInstance()->GetPos("body",2);
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
	
	generator gen;
	gen.add_messages_path(string(getenv("DONNIE_SOURCE_PATH")) + "/loc");
	gen.add_messages_domain("Historic");
	locale loc = gen(string(getenv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);
	history.imbue(loc);

	history << translate("Histórico possui ") << commandsList.size() << translate(" comandos") << endl;
	for (iterator = this->commandsList.begin(); iterator != this->commandsList.end(); ++iterator) {
		history << translate("Comando ") << cmdNum << translate(" foi ") << (*iterator).name << ", " <<  (*iterator).feedback << 
		translate(", posição [") <<  (*iterator).posx  << ", " << (*iterator).posy  << 
		", " << (*iterator).degree << "]" << endl ;
		cmdNum++;
	}

	return history.str();
}

string Historic::getLast()
{
	stringstream history;
	Command last;
	
	generator gen;
	gen.add_messages_path(string(getenv("DONNIE_SOURCE_PATH")) + "/loc");
	gen.add_messages_domain("Historic");
	locale loc = gen(string(getenv("DONNIE_LANG")) + ".UTF-8");
	locale::global(loc);
	cout.imbue(loc);
	cerr.imbue(loc);

	if (commandsList.size() > 0){
		last = commandsList.back();
		history << translate("Comando ") << commandsList.size()-1 << translate(" foi ") << last.name << ", " <<  last.feedback << 
		translate(", posição [") <<  last.posx  << ", " << last.posy  << 
		", " << last.degree << "]"; // dont add newline here !
	}

	return history.str();
}

void Historic::clear()
{
	this->commandsList.clear();
}
