#include "floor_desc.h"

// TODO: amory. criar include donnie_defs.h com todas as definicoes de tamanho do donnie
#define STEP_YAW 10 //gradianos
#define STEP_LENGHT 0.05
#define STEP_LENGHT_ERROR 0.03  //!Define o erro do ultimo passo. Caso o robo pare sem marcar o som de passo
#define SIDE_RANGER 0.05
#define FRONT_RANGER 0.06
#define BACK_RANGER 0.05
#define SCAN_YAW 30 //gradianos


FloorClient::FloorClient()
{
    string host = GetEnv("DONNIE_IP");
    string donnie_path = GetEnv("DONNIE_PATH");
    string donnie_lang = GetEnv("DONNIE_LANG");

    int port = atoi(GetEnv("DONNIE_PORT").c_str());
    if(host.size()==0) host = "localhost";
    if(port==0) port = 6665;
    if (donnie_path=="") {
		cerr << translate("variable DONNIE_PATH not defined. Please execute 'export DONNIE_PATH=<path-to-donnie>'") << endl;
		exit(1);
	}
    if (donnie_lang=="") {
		cerr << translate("variable DONNIE_LANG not defined. Please execute 'export DONNIE_LANG=$LANGUAGE'") << endl;
		exit(1);
	}
    // Set up language environment
    generator gen;
    gen.add_messages_path(donnie_path + "/resources/loc");
    gen.add_messages_domain("alerts");
    locale loc = gen(donnie_lang + ".UTF-8");
    locale::global(loc);
    cout.imbue(loc);
    cerr.imbue(loc);

	// setup yaml
	//fin = new ifstream(, std::ifstream::in)
	/*
	fin.open("house_donnie.yaml",  std::ifstream::in);
	if (fin.is_open())
		cout << "abriu o arquivo yaml" << endl;
	else
		cout << "nao abriu o arquivo yaml" << endl;

	
	//parser.Load(fin);
	*/
	
	cout << "loading parser" << endl;
	doc = YAML::LoadFile("house_donnie.yaml");
	cout << doc;
	cout << endl << endl << "loaded" << endl;
	cout << doc["name"] << endl;
	cout << doc["description"] << endl;
	cout << endl << endl << "loaded2" << endl;
	// https://github.com/jbeder/yaml-cpp/wiki/How-To-Parse-A-Document-(Old-API)
	// https://www.gamedev.net/articles/programming/engines-and-middleware/yaml-basics-and-parsing-with-yaml-cpp-r3508/
	
	/*
	if (parser.HandleNextDocument(doc))
		cout << "deu verdade" << endl;
	else
		cout << "deu false" << endl;
*/
/*
	std::string name, desc;
	config["name"] >> name;
	config["description"] >> desc;
	std::cout << "Found entry with name '" << name << "' and desc '" << desc << "'\n";
*/

	std::string key, value;
    for (YAML::const_iterator it = doc.begin(); it != doc.end(); ++it){
        std::cout << "First: " << it->first.as<std::string>() << "\n";
         // it->second.as<std::string>(); // can't do this until it's type is checked!!
    }	
	/*
	for(YAML::const_iterator it=doc.begin();it!=doc.end();++it) {
		it->first.as<std::string>() >> key;
		it->second.as<std::string>() >> value;
		std::cout << "Key: " << key << ", value: " << value << std::endl;
	}	
	*/
	/*
	std::string name, desc;
	doc["name"] >> name;
	doc["description"] >> desc;
	std::cout << "Found entry with name '" << name << "' and desc '" << desc << "'\n";
	*/
	
	//cout << "parsing ... " <<  parser.bool() << endl;
	cout << "player ... " << endl;

	// setup Player
	try{		
		robot = new PlayerClient(host,port);
		p2d = new Position2dProxy(robot,0);
		speech = new SpeechProxy(robot,0);
	}catch (PlayerError e){
		#ifndef NDEBUG
			cerr << e << endl;
		#endif
		cerr << translate("Não foi possível conectar no robô ") << endl;
		cerr << translate("Possivelmente o arquivo cfg está incorreto.") << endl;
		exit(1);
	}

    //setPos(p2d->GetXPos(),p2d->GetYPos(),p2d->GetYaw());
    robot->StartThread(); //create an robot->Read() in a separated thread
}

void FloorClient::getPos(){
	//p2d->GetXPos(),p2d->GetYPos()
    //return pos;
}


void FloorClient::up(){
}

void FloorClient::down(){
}

void FloorClient::child(){
}

void FloorClient::parent(){
}

void FloorClient::checkArrowKeys(){
}


int main(int argc, char *argv[]){
    FloorClient *donnie1 = new FloorClient();
    while(1){
        usleep(100); //little delay
        donnie1->checkArrowKeys();
    }
    return 0;
}
