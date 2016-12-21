#include "DonnieMemory.h"
#include "utils.h"
#include <stdio.h>

using namespace std;


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
	cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
	{
		cout << "Variavel ja existe." << endl;
	}
	else
	{
		#ifndef NDEBUG
        	cout << "MAKE: " << var << " = " << value << endl;
        #endif
		Memory.top().memory[var] = value;
		return value;
	}
}

int DonnieMemory::addVar(string name)
{
	addVar(name,0);
}

int DonnieMemory::assignVar(string name, int value)
{
	string var = toLowerCase(name);
	cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
    {
      Memory.top().memory[var] = value;
      return value;
    }
    else
    	cout << "Variável não existe" << endl;
}

int DonnieMemory::getVar(string name)
{
	string var = toLowerCase(name);
	cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
	{
	  	return Memory.top().memory[var]; // Variável local do primeiro item da stack
	}
	else
	  cout << "Variável não existe" << endl;
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
	cout << "variavel " << var << endl;
	if(Memory.top().memory.find(var) != Memory.top().memory.end())
		cout << "Variavel for ja existe" << endl;
	else
	{
		addVar(var, value);
		tempVar.push(var);
	}

}

void DonnieMemory::purgeForVar()
{
	Memory.top().memory.erase(tempVar.top());
	tempVar.pop();
}
