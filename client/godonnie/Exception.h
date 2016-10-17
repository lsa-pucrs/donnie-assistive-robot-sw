using namespace std;

class tipoException: public exception
{
	public:
		const char* what() const throw();
};
class sintaxeException: public exception
{
	public:
		virtual const char* what() const throw();
};
