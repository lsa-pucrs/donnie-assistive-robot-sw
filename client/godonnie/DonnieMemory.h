#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <utility>
#include <algorithm>

using namespace std;

struct mem
{
   map<string,int> memory;
};

class DonnieMemory
{
private:
	stack<mem> Memory;
	map<string,int>::iterator it;
	stack<string> tempVar;

	static DonnieMemory *singleton;
public:
	DonnieMemory();
	~DonnieMemory();

	// create instante of singleton
	static DonnieMemory *getInstance();

	// delete singleton
	static void ResetInstance();

	int addVar(string name, int value);
	int addVar(string name);
	int assignVar(string name, int value);
	int getVar(string name);
	void stackMemory(mem local);
	void unstackMemory();
	int addForVar(string name, int value);
	void purgeForVar();
};