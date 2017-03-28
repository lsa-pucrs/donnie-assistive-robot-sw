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
	string var = toLowerCase(name);
	//cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
	{
		cout << "Variável \"" << name << "\" já existe." << endl;
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
	string var = toLowerCase(name);
	//cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
    {
      Memory.top().memory[var] = value;
      return ASSIGNED;
    }
    else
    	cout << "Variável \"" << name << "\" não existe" << endl;
    return NEXIST;
}

int DonnieMemory::getVar(string name)
{
	string var = toLowerCase(name);
	//cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
	{
	  	return Memory.top().memory[var]; // Variável local do primeiro item da stack
	}
	else
	  cout << "Variável \"" << name << "\" não existe" << endl;
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
	string var = toLowerCase(name);
	//cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
		cout << "Variável PARA \"" << name << "\" já existe" << endl;
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
	string var = toLowerCase(name);
	if(proc.find(var) != proc.end())
	{
		cout << "Procedimento \"" << name << "\" já foi declarado " << endl;
	}
	else
		proc[var] = procedure;
}

procDec DonnieMemory::getProc(string name)
{
	string var = toLowerCase(name);
	if(proc.find(var) != proc.end())
	{
		return proc[var];
	}
	else
		cout << "Procedimento \"" << name << "\" não existe " << endl;
}