#include "DonnieMemory.h"
#include <stdio.h>

using namespace std;

DonnieMemory *DonnieMemory::singleton = NULL;

DonnieMemory::DonnieMemory()
{

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
	if(Memory.top().memory.find(name) != Memory.top().memory.end())
	{
		return 0;
	}
	else
	{
		Memory.top().memory[name] = value;
		return 1;
	}
}

int DonnieMemory::addVar(string name)
{

}

int DonnieMemory::getVar(string name)
{

}

void stackMemory()
{

}

void unstackMemory()
{

}
