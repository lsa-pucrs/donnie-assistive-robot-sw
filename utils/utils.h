#ifndef UTILS_H_
#define UTILS_H_

#include <libplayerc++/playerc++.h>
#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;

// Message macros
#ifndef NDEBUG
//! Debug messages. They are not showed when NDEBUG are not defined
/*! Especify the parameters by << separator \n
	Example: DEBUG_MSG("test" << var << "Kmh")*/
#define DEBUG_MSG(msg) cout << msg << endl << flush
#else  //avoid errors if NDEBUG are not defined and someone use the defines with parameters
#define DEBUG_MSG(msg) //void
#endif


double radTOrad(double rad);

double radTOdeg(double rad);

double degTOrad(double deg);

string toLowerCase(string input);

//! Allow to use environments variables
/*!	Used to avoid the need to recompile all code for little changes.\n
	Examples: \n
	To test if the variable exists: \n
	if(GetEnv("IP").size()==0) \n
	or \n
	if(GetEnv("IP")=="") \n
	To set variable you can use "$export IP=localhost"
	\param string var with the required environment
	\return string with the value in the environment parameter. Returns "" with non initialized param.*/
string GetEnv( const string & var );	

#endif