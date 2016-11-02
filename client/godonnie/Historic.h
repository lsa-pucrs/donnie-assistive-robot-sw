//Class to Keep the Commands History
class Historic
{	
	private:
		//Struct to save commands, no external access alowed
		struct Command {
			string name;
			string feedback;
			double posx;
			double posy;
			double degree;
		};
		list<Command> commandsList;
		list<Command>::iterator iterator;
		/*
		string getLast();
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

};
