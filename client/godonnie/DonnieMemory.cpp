#include "DonnieMemory.h"
#include "utils.h"
#include <stdio.h>

using namespace std;

/// definition of variable return tokens
#define WAITING 3
#define ASSIGNED 2
#define CREATED 1
#define NEXIST 0
#define EXIST -1


DonnieMemory::DonnieMemory()
{
	mem global;
	Memory.push(global);
	Donnie = DonnieClient::getInstance();
}

DonnieMemory::~DonnieMemory()
{

}

DonnieMemory *DonnieMemory::getInstance()
{
   if (!singleton)   // Only allow one instance of class to be generated.
      singleton = new DonnieMemory();
   return singleton;
}

void DonnieMemory::ResetInstance()
{
      delete singleton; 
      singleton = NULL; 
}

int DonnieMemory::addVar(string name, int value)
{
	std::ostringstream sayStr;
	string var = toLowerCase(name);
	//cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
	{
		sayStr << "Variável \"" << name << "\" já existe.";
		Donnie->speak(sayStr.str());
		cout << sayStr.str() << endl;
		return EXIST;
	}
	else
	{
		#ifndef NDEBUG
        	cout << "MAKE: " << var << " = " << value << endl;
        #endif
		Memory.top().memory[var] = value;
		return CREATED;
	}
}

int DonnieMemory::addVar(string name)
{
	addVar(name,0);
}

int DonnieMemory::assignVar(string name, int value)
{
	std::ostringstream sayStr;
	string var = toLowerCase(name);
	//cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
    {
      Memory.top().memory[var] = value;
      return ASSIGNED;
    }
    else
    {
    	sayStr << "Variável \"" << name << "\" não existe.";
    	Donnie->speak(sayStr.str());
		cout << sayStr.str() << endl;
    }
    return NEXIST;
}

int DonnieMemory::getVar(string name)
{
	std::ostringstream sayStr;
	string var = toLowerCase(name);
	//cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
	{
	  	return Memory.top().memory[var]; // Variável local do primeiro item da stack
	}
	else
	{
		sayStr << "Variável \"" << name << "\" não existe.";
		Donnie->speak(sayStr.str());
		cout << sayStr.str() << endl;
	}
	return NEXIST;
}

void DonnieMemory::stackMemory(mem local)
{
	Memory.push(local);
}

void DonnieMemory::unstackMemory()
{
	Memory.pop();
}

int DonnieMemory::addForVar(string name, int value)
{
	std::ostringstream sayStr;
	string var = toLowerCase(name);
	//cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
	{
		sayStr << "Variável PARA \"" << name << "\" já existe.";
		Donnie->speak(sayStr.str());
		cout << sayStr.str() << endl;
	}
	else
	{
		addVar(var, value);
		tempVar.push(var);
		return CREATED;
	}
	return EXIST;

}

void DonnieMemory::purgeForVar()
{
	Memory.top().memory.erase(tempVar.top());
	tempVar.pop();
}


void DonnieMemory::addProc(string name, procDec procedure)
{
	std::ostringstream sayStr;
	string var = toLowerCase(name);
	if(proc.find(var) != proc.end())
	{
		sayStr << "Procedimento \"" << name << "\" já foi declarado.";
		Donnie->speak(sayStr.str());
		cout << sayStr.str() << endl;
	}
	else
		proc[var] = procedure;
}

procDec DonnieMemory::getProc(string name)
{
	std::ostringstream sayStr;
	string var = toLowerCase(name);
	if(proc.find(var) != proc.end())
	{
		return proc[var];
	}
	else
	{
		sayStr << "Procedimento \"" << name << "\" não existe.";
		Donnie->speak(sayStr.str());
		cout << sayStr.str() << endl;
	}
}