#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <utility>
#include <algorithm>
#include <vector>

#include "GoDonnieLexer.h"
#include "DonnieClient.h"

using namespace std;

struct mem
{
   map<string,int> memory;
};

typedef struct
{
  pANTLR3_BASE_TREE node;
  int argNum;
  vector<string> args;
}procDec;

class DonnieMemory
{
private:
	stack<mem> Memory;
	stack<string> tempVar;
	map<string,procDec> proc;
	DonnieClient *Donnie;

	static DonnieMemory *singleton;
public:
	DonnieMemory();
	~DonnieMemory();

	// create instante of singleton
	static DonnieMemory *getInstance();

	// delete singleton
	static void ResetInstance();

///////////Variables///////////
	int addVar(string name, int value);
	int addVar(string name);
	int assignVar(string name, int value);
	int getVar(string name);
	void stackMemory(mem local);
	void unstackMemory();
	int addForVar(string name, int value);
	void purgeForVar();

//////////Procedure////////////
	void addProc(string name, procDec procedure);
	procDec getProc(string name);
};
