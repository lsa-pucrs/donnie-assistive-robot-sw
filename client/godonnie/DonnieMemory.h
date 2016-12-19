#include <iostream>
#include <map>
#include <stack>

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
	int getVar(string name);
	void stackMemory();
	void unstackMemory();
};
