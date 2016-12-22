/*
 * Desc: Class to Keep the Commands History
 * Author: Henry Nunes e Alexandre Amory
 * Date:  November 2016
 * Laboratório de Sistemas Autônomos 
 *  - https://lsa.pucrs.br/
 *  - https://github.com/lsa-pucrs
 * Faculdade de Informática - PUCRS  
 *  - www.inf.pucrs.br
 */

#pragma once
#ifndef HISTORIC_H
#define HISTORIC_H

#include <string>
#include <list>

using namespace std;

///Class to Keep the Commands History
class Historic
{	
	private:
		///Struct to save commands
		struct Command {
			string name;     ///name of command including parameters
			string feedback; ///say if it bumped or not during the movement
			int posx;        /// in steps
			int posy;        /// in steps
			int degree;   /// in degrees
			//TODO: implement << operator
		};
		/// list of commands
		list<Command> commandsList;
		
		/*
		string getLine(int line);
		string previous();
		string next();
		void resetIterator();
		*/		
		
		//Variable to hold Singleton 
		static Historic *singleton;
		
		//Block the construction and destruction of the object
		Historic();
		~Historic();
			
	public:
		/** method to get the Singleton */
		static Historic *getInstance();
		/** delete singleton */
		static void ResetInstance();		
		
		/** add command to the history list*/
		bool addCommand(string name, string feedback);
		/** number of commands in the history list*/
		int size();
		/** return the history of commands on a string*/
		string show();
		/** clear the history list*/
		void clear();
		/** the the last command in the list. used in command Estado*/
		string getLast();

};

#endif
