#include "floor_desc.h"
#include <termios.h>

#define KEY_UP 		56 // # 8 in the numeric keyboard
#define KEY_DOWN 	50 // # 2
#define KEY_LEFT 	52 // # 4
#define KEY_RIGHT 	54 // # 6
#define KEY_ENTER 	10 // real enter code
#define KEY_POS 	53 // # 5
#define KEY_HOME    48 // # 0
#define KEY_ESC     27 // real ESC code


FloorClient::~FloorClient()
{
	delete robot;
	delete p2d;
	delete speech;
}


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
    
    // set to true to stay silent
    // TODO read parameter to set muted
    muted = false;

	// setup yaml
	cout << "loading YAML parser" << endl;
	doc = YAML::LoadFile("house_donnie.yaml");
	
	// dump all YAML file
	//cout << doc;

	// main refs used to learn how to use it
	// https://github.com/jbeder/yaml-cpp/wiki/How-To-Parse-A-Document-(Old-API)
	// https://www.gamedev.net/articles/programming/engines-and-middleware/yaml-basics-and-parsing-with-yaml-cpp-r3508/
	cout << "loading YAML into memory" << endl;
	doc >> this->floorplan;
	cout << "---- " << this->floorplan.name << " " << this->floorplan.description  << " " << this->floorplan.area << endl;
	cout << this->floorplan.bl_pos.x << " " << this->floorplan.bl_pos.y << endl;
	cout << "1=====" << endl;
	cout << floorplan.bl_pos << endl;
	cout << "2=====" << endl;
	cout << floorplan.rooms[0] << endl;
	cout << "3=====" << endl;
	cout << floorplan << endl;
	cout << "=====" << endl;
	
/*
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
	current_robot_pos.x = p2d->GetXPos();
	current_robot_pos.y = p2d->GetYPos();
	cout << "Posicao: " << current_robot_pos << endl;
}


std::ostream& operator<<(std::ostream& os, const PosXY& m)
{
    return os << "[ " <<  m.x << ", " << m.y << " ]"; 
}


std::ostream& operator<<(std::ostream& os, const Room& m)
{
    os <<
    "name: " <<  m.name << endl << 
    "description: " <<  m.description << endl << 
    "area: " <<  m.area << endl << 
    "bl_pos: " <<  m.bl_pos << endl << 
    "tr_pos: " <<  m.tr_pos << endl << 
    "objects: " << endl ;
    for(unsigned i=0;i<m.objects.size();i++)
		os << " - " << m.objects[i] << endl;
	os << "doors:" << endl;
    for(unsigned i=0;i<m.doors.size();i++)
		os << " - " << m.doors[i] << endl;
    
    return os;
}

std::ostream& operator<<(std::ostream& os, const Floorplan& m)
{
    os <<
    "name: " <<  m.name << endl << 
    "description: " <<  m.description << endl << 
    "area: " <<  m.area << endl << 
    "bl_pos: " <<  m.bl_pos << endl << 
    "tr_pos: " <<  m.tr_pos << endl << 
	"rooms:" << endl;
    for(unsigned i=0;i<m.rooms.size();i++)
		os << i << ")" << endl << m.rooms[i];
    
    return os;
}


void operator >> (const YAML::Node& node, PosXY& v) {
   v.x = node[0].as<float>();
   v.y = node[1].as<float>();
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
	
   r.name = node["name"].as<string>();
   r.description = node["description"].as<string>();
   r.area = node["area"].as<float>();
   node["bl_pos"] >> r.bl_pos;
   node["tr_pos"] >> r.tr_pos;
   //node["rooms"] >> r.rooms;
   const YAML::Node& rooms = node["rooms"];
   for(unsigned i=0;i<rooms.size();i++) {
      Room room;
      rooms[i] >> room;
      r.rooms.push_back(room);
   } 
}

void FloorClient::say(const char *str){
	if (muted)
		cout << str << endl;
	else
		speech->Say(str);
}


void FloorClient::up(){
	cout << endl << "Up" << endl;//key up
	say("up");
}

void FloorClient::down(){
	cout << endl << "Down" << endl;   // key down
	say("down");
}

void FloorClient::child(){
	cout << endl << "Right - child" << endl;  // key right
	say("Right - child");
}

void FloorClient::parent(){
	cout << endl << "Left - parent" << endl;  // key left
	say("Left - parent");
}


int getch(void)
{
 int ch;
 struct termios oldt;
 struct termios newt;
 tcgetattr(STDIN_FILENO, &oldt); /*store old settings */
 newt = oldt; /* copy old settings to new settings */
 newt.c_lflag &= ~(ICANON | ECHO); /* make one change to old settings in new settings */
 tcsetattr(STDIN_FILENO, TCSANOW, &newt); /*apply the new settings immediatly */
 ch = getchar(); /* standard getchar call */
 tcsetattr(STDIN_FILENO, TCSANOW, &oldt); /*reapply the old settings */
 return ch; /*return received char */
}


int main(int argc, char *argv[]){
    FloorClient *donnie1 = new FloorClient();
	int c = 0;
    
    while(1){
        usleep(100); //little delay

		// get robot position
		donnie1->getPos();

		switch(c=getch()) {
		case KEY_UP:    // go up in the same hier level
			donnie1->up();
			break;
		case KEY_DOWN:  // go down in the same hier level
			donnie1->down();
			break;
		case KEY_LEFT:  // go up into hierachy, up to the parent item. 
			donnie1->parent();
			break;
		case KEY_RIGHT: // enter in the subitem, the child item
			donnie1->child();
			break;
		case KEY_POS: // describe the room based on the current robot position
			donnie1->say("position");
			cout << "position" << endl;
			break;
		case KEY_ENTER: // say the current position
			donnie1->say("enter");
			cout << "enter" << endl;
			break;
		case KEY_HOME: // go to the initial position
			donnie1->say("home");
			cout << "home" << endl;
			break;
		case KEY_ESC: // end of the program
			donnie1->say("end");
			cout << "end" << endl;
			usleep(100); //give some time to finish to say 
			delete donnie1;
			return 0;
		default:
			donnie1->say("invalid key");
			cout << endl << "in " << c << endl;  // not arrow
		}
		//this->speak();


    }
    return 0;
}
