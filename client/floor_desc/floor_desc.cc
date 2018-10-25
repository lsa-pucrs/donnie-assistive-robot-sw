#include "floor_desc.h"

#include "yaml-cpp/node/type.h"

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
	
	
	cout << "1" << endl;
	//this->floorplan.name = doc["name"].as<string>();
	//this->floorplan.description = doc["description"].as<string>();
	//this->floorplan.area = doc["area"].as<float>();
	//cout << "---- " << this->floorplan.name << " " << this->floorplan.description  << " " << this->floorplan.area << endl;
	cout << "2" << endl;
	//YAML::Node ff = doc.begin();
	cout << "3" << endl;
	//cout << "---- " << ff["name"] << endl;
	cout << "4" << endl;
	doc >> this->floorplan;
	cout << "5" << endl;
	cout << "---- " << this->floorplan.name << " " << this->floorplan.description  << " " << this->floorplan.area << endl;
	cout << "6" << endl;
	cout << this->floorplan.bl_pos.x << " " << this->floorplan.bl_pos.y << endl;
	
	//cout << doc;
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
		key = it->first.as<std::string>();
        cout << "First: "  << key << endl;
        if (key == "name") {
			cout << "Second size: "  << it->second.size() << endl;
			cout << "Second: " << it->second.as<std::string>() << endl;; // can't do this until it's type is checked!!
		} else	if (key == "bl_pos") {
			cout << "Second size: "  << it->second.size() << endl;
			//cout << "Second: " << it->second.as<std::string>() << endl;; // can't do this until it's type is checked!!
		} else if (key == "rooms") {
			cout << "entrei no rooms" << endl;
			cout << "size1: "  << it->second.size() << endl;
			YAML::Node rooms = it->second;
			//cout << "Second size: "  << it->second.size() << endl;
			cout << "size2: "  << rooms.size() << endl;
			if (YAML::NodeType::Sequence == rooms.Type()) {
				cout << "eh sequence "  << endl;
				//for (std::size_t i_rooms = 0; i_rooms < rooms.size(); i_rooms++) {
				//  cout << "rooms 2 pos: " << rooms[i_rooms]->first.as<std::string>() << endl;
				//}				
				for (YAML::const_iterator it_rooms = rooms.begin(); it_rooms != rooms.end(); ++it_rooms){
					cout << "rooms pos: " << it_rooms->first.as<string>() << endl;
					//cout << "Second: " << it->second.as<std::string>() << endl;; // can't do this until it's type is checked!!
				}
			}else
				cout << "unespected node type for 'doors' key" << endl;
		}
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

void operator >> (const YAML::Node& node, PosXY& v) {
   v.x = node[0].as<float>();
   v.y = node[1].as<float>();
   //node[0] >> v.x;
   //node[1] >> v.y;
}

void operator >> (const YAML::Node& node, Room& r) {
   r.name = node["name"].as<string>();
   r.description = node["description"].as<string>();
   r.area = node["area"].as<float>();
   node["bl_pos"] >> r.bl_pos;
   node["tr_pos"] >> r.tr_pos;
   //node["objects"] >> r.objects;
   const YAML::Node& objects = node["objects"];
   cout << "OBJETOS:" << endl;
   for(unsigned i=0;i<objects.size();i++) {
      //string obj;
      //objects[i] >> obj;
      cout << objects[i].as<string>() << endl;
      r.objects.push_back(objects[i].as<string>());
   }
   //node["doors"] >> r.doors;
   const YAML::Node& doors = node["doors"];
   cout << "PORTAS:" << endl;
   for(unsigned i=0;i<doors.size();i++) {
      PosXY pos;
      doors[i] >> pos;
      cout << pos.x << ", " << pos.y << endl;
      r.doors.push_back(pos);
   } 
}

void operator >> (const YAML::Node& node, Floorplan& r) {
	
   //cout << it["name"].second.as<string>() << endl;
   //cout << node["name"].as<string>() << endl;
   r.name = node["name"].as<string>();
   r.description = node["description"].as<string>();
   r.area = node["area"].as<float>();
   node["bl_pos"] >> r.bl_pos;
   node["tr_pos"] >> r.tr_pos;
   /*
   node["name"] >> r.name;
   node["description"] >> r.description;
   node["bl_pos"] >> r.bl_pos;
   node["tr_pos"] >> r.tr_pos;
   node["area"] >> r.area;
   */    
   //node["rooms"] >> r.rooms;
   const YAML::Node& rooms = node["rooms"];
   for(unsigned i=0;i<rooms.size();i++) {
      Room room;
      rooms[i] >> room;
      r.rooms.push_back(room);
   } 
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
