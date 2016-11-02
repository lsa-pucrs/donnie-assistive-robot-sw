using namespace std;

class parametroException: public exception
{
		string m_msg;
	public:
		parametroException(string error);
		parametroException();
		const char* what() const throw();
};
class sintaxeException: public exception
{
		string m_msg;
	public:
		sintaxeException(string error);
		sintaxeException();
		virtual const char* what() const throw();
};
class variavelException: public exception
{
		string m_msg;
	public:
		variavelException(string error);
		variavelException();
		virtual const char* what() const throw();
};
