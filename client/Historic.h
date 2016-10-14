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
		//Bidiretional Iterator
		list<Command>::iterator iterator;
		bool resetIterator();
		
		//Variable to hold Singleton 
		static Historic singleton;
		
		//Block the construction and destruction of the object
		Historic();
		~Historic();
			
	public:
		//Method to get the Singleton
		static Historic& getHistoric();
		
		
		bool addCommand(string name, string feedback);
		int size();
		string getLast();
		string getLine(int line);
		string previous();
		string next();
};
