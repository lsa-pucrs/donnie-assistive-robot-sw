/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2003  
 *     Brian Gerkey, Andrew Howard
 *                      
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/*
 * Desc: A simple example of how to write a driver that supports multiple interface.
 * Also demonstrates use of a driver as a loadable object.
 * Author: Andrew Howard
 * Date: 25 July 2004
 * CVS: $Id: Donnie.cc 8000 2009-07-12 10:53:35Z gbiggs $
 */


// ONLY if you need something that was #define'd as a result of configure 
// (e.g., HAVE_CFMAKERAW), then #include <config.h>, like so:
/*
#include <config.h>
*/

/*
* Desc: Donnie's Player-based driver.
* Author: Guilherme Marques - 
* Date:  August 2016
* Laboratório de Sistemas Autônomos 
*  - https://lsa.pucrs.br/
*  - https://github.com/lsa-pucrs
* Faculdade de Informática - PUCRS  
*  - www.inf.pucrs.br
*/


/*
cfg example:

TODO:  colocar exemplo c todos os parametros

*/

// TODO: separar a definicao da classe em um .h para gerar documentacao automatica com doxygen

#include "donnie.h"

#include <unistd.h>
#include <netinet/in.h>
#include <cmath>


// A factory creation function, declared outside of the class so that it
// can be invoked without any object context (alternatively, you can
// declare it static in the class).  In this function, we create and return
// (as a generic Driver*) a pointer to a new instance of this driver.
Driver* Donnie_Init(ConfigFile* cf, int section){
	// Create and return a new instance of this driver
	return ((Driver*) (new Donnie(cf, section)));
}

// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void Donnie_Register(DriverTable* table){
	table->AddDriver("donnie", Donnie_Init);
}

////////////////////////////////////////////////////////////////////////////////
// Extra stuff for building a shared object.

/* need the extern to avoid C++ name-mangling  */
extern "C"{
	int player_driver_init(DriverTable* table)
	{
		puts("Donnie driver initiallized");
		Donnie_Register(table);
		puts("waiting for client startup...");
		return(0);
	}
}
////////////////////////////////////////////////////////////////////////////////

// Constructor.  Retrieve options from the configuration file and do any
// pre-Setup() setup.
Donnie::Donnie(ConfigFile* cf, int section) : ThreadedDriver(cf, section){
	// zero ids, so that we'll know later which interfaces were
	memset (&m_pos_data, 0, sizeof(player_position2d_data_t));  //descobrir o porque disso //g
	memset (&this->power_addr, 0, sizeof (player_devaddr_t));
	memset (&this->bumper_addr, 0, sizeof (player_devaddr_t));
	memset (&this->m_position_addr, 0, sizeof(player_devaddr_t));


	// Create dio interface
	 if (cf->ReadDeviceAddr(&(this->m_dio_addr), section, "provides", PLAYER_DIO_CODE, -1, NULL)){
			PLAYER_ERROR("Could not read dio ID ");
			SetError(-1);
			return;
	 }
	 if (AddInterface(this->m_dio_addr)){
			PLAYER_ERROR("Could not add dio interface ");
			SetError(-1);
			return;
	 }
	 // Create my ranger interface
	 if (cf->ReadDeviceAddr(&(this->m_ranger_addr), section, "provides", PLAYER_RANGER_CODE, -1, NULL)){
			PLAYER_ERROR("Could not read ranger ID ");
			SetError(-1);
			return;
	 }
	 if (AddInterface(this->m_ranger_addr)){
			PLAYER_ERROR("Could not add ranger interface ");
			SetError(-1);
			return;
	 }
	 	// Create my neck position interface
	 if (cf->ReadDeviceAddr(&(this->m_neck_position_addr), section, "provides", PLAYER_POSITION2D_CODE, -1, "neck")){
			PLAYER_ERROR("Could not read position2d ID ");
			SetError(-1);
			return;
	 }
	 if (AddInterface(this->m_neck_position_addr)){
			PLAYER_ERROR("Could not add position2d interface ");
			SetError(-1);    
			return;
	 }
		// Create my base position interface
	 if (cf->ReadDeviceAddr(&(this->m_position_addr), section, "provides", PLAYER_POSITION2D_CODE, -1, "base")){
			PLAYER_ERROR("Could not read position2d ID ");
			SetError(-1);
			return;
	 }
	 if (AddInterface(this->m_position_addr)){
			PLAYER_ERROR("Could not add position2d interface ");
			SetError(-1);    
			return;
	 }

	 // Create my bumper interface
		if (cf->ReadDeviceAddr (&(this->bumper_addr),section,"provides",PLAYER_BUMPER_CODE, -1, NULL)){
			PLAYER_ERROR("Could not read bumper interface ");
			SetError (-1);
		return;
		}
		if (AddInterface (this->bumper_addr) != 0){
			PLAYER_ERROR("Could not add bumper interface ");
			SetError (-1);
		return;
		}
		// Create my power interface
		if (cf->ReadDeviceAddr (&(this->power_addr),section,"provides",PLAYER_POWER_CODE,-1, NULL)){
      PLAYER_ERROR("Could not read power interface ");
      this->SetError (-1);
	  	return;
    }
    if (this->AddInterface (this->power_addr) != 0){
    	PLAYER_ERROR("Could not add power interface ");
	  	this->SetError (-1);
	  	return;
	}
	/*// Create my beep interface
	if (cf->ReadDeviceAddr(&(this->m_beep_addr), section, "provides", PLAYER_BEEP_CODE, -1, NULL)){
			PLAYER_ERROR("Could not read BEEP ");
			SetError(-1);
			return;
	 }
	 if (AddInterface(this->m_beep_addr)){
			PLAYER_ERROR("Could not add dio interface ");
			SetError(-1);
			return;
	 }*/
	 
	port = cf->ReadString (section, "port", "/dev/ttyACM0");
	robot_width = cf->ReadFloat(section, "width", 0.2);    // [m]
	robot_length = cf->ReadFloat(section, "length", 0.2);  // [m]
	robot_height = cf->ReadFloat(section, "height", 0.1);  // [m]

	linear_max_vel = cf->ReadFloat(section, "linear_max_vel", 0.10); //[m/s] (0.10m/s -> 10cm/s)
	ang_max_vel = cf->ReadFloat(section, "ang_max_vel", 0.10); //[rad/s]

	pid_kp = cf->ReadFloat(section, "pid_kp", 0); 
	pid_ki = cf->ReadFloat(section, "pid_ki", 0); 
	pid_kd = cf->ReadFloat(section, "pid_kd", 0); 
	pid_setpoint = cf->ReadFloat(section, "pid_setpoint", 0.);
	pid_offset_r = cf->ReadInt(section, "pid_offset_r", 0);
	pid_offset_l = cf->ReadInt(section, "pid_offset_l", 0);

	step_length = cf->ReadFloat(section, "step_length", 0);

	//odometry
	wheel_radius = cf->ReadFloat(section, "wheel_radius", DEFAULT_WHEEL_RADIUS);
	axle_length = cf->ReadFloat(section, "axle_length", DEFAULT_AXLE_LENGTH);
	cpr = cf->ReadFloat(section, "encoder_cpr", DEFAULT_CPR);

	//!Debug Messages Level
	/* 0 Print no Messages 
	** 1 or more print debug messages or some expecific messages
	*/
	print_debug_messages = cf->ReadInt(section, "print_debug_messages", 0); 

	

	//angular_offset = cf->ReadFloat(section, "angular_offset", 0);


	//this->RegisterProperty ("port", &this->port, cf, section);
	/*
	m_baud_rate = cf->ReadInt(section, "baud", 38400);
	m_quiet = cf->ReadBool(section, "quiet", TRUE);
	robot_geom.size.sw = cf->ReadTupleFloat(section, "robot_geometry", 0, 0.01);
	robot_geom.size.sl = cf->ReadTupleFloat(section, "robot_geometry", 1, 0.01);
	*/
	arduino = new Serial(port.c_str());  //c_str() convert string to const char*
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device(ONLY WHEN A CLIENT CONECTS TO DRIVER).  Return 0 if things go well, and -1 otherwise.
int Donnie::MainSetup(){   
	puts("MainSetup driver initialising");
	//reset variables
	last_posLeft = 0;
	last_posRight = 0;
	ticksR = 0;
	ticksL = 0;
	//odometry
	x=0;
	y=0;
	th=0;
	lasttickR=0;
	lasttickL=0;

	if(arduino==NULL) 
		arduino = new Serial(port.c_str());  //c_str() convert string to const char*

	puts("MainSetup driver ready");
	return(0);
}


////////////////////////////////////////////////////////////////////////////////
//Shutdown the devic (each client shutdown)
void Donnie::MainQuit(){
	PLAYER_MSG0(0, "Donnie client has been shutdown...\n");
	PLAYER_MSG0(0, "Shutting Donnie driver down...");

	 //Stop and join the driver thread
	 //StopThread();

	// delete the interface library
	 if (arduino)
	 {
			MotorsOff();
	 		//Beep(1000,1000);
			arduino->closePort(); // it will reset arduino after each client disconect the robot
			arduino = NULL;
	 }

	 PLAYER_MSG0(0, "Done.\n");
	 //return 0; //close server
}


	double processIncomingDataLag;
int Donnie::processIncomingData(){
	//uint8_t i;
	if(arduino->readData(rx_data,&rx_data_count)){
		//processIncomingDataLag = tmr.elapsedus() - processIncomingDataLag ;
		//printf("IncomingDataTS:%.2lf us, PACK:%x.\n",processIncomingDataLag,rx_data[0]);
		if(rx_data[0]==DIOPACK) ProcessDioData();
		else if(rx_data[0]==RANGERPACK) ProcessRangerData();
		else if(rx_data[0]==BUMPERPACK) ProcessBumperData();
		else if(rx_data[0]==POWERPACK) ProcessPowerData();
		else if(rx_data[0]==SYSTEMMESSAGEPACK) ProcessSystemMessageData();
		else if(rx_data[0]==REQUESTCONFIGPACK) ProcessRequestConfig();
		else if(rx_data[0]==PINGPACK) ProcessRequestPing();
		else if(rx_data[0]==ENCODERPACK) ProcessEncoderData();
		else printf("unknown message, protocol type: %.2X\n\n",rx_data[0]);
		//processIncomingDataLag = tmr.elapsedus();
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void Donnie::Main(){
	// The main loop; interact with the device here
		//double mainLoopLag,mainLoopLagLast;
	for(;;){
		//mainLoopLag = tmr.elapsedus(); //to rimestamp main loop
		// test if we are supposed to cancel
		pthread_testcancel();

		
		processIncomingData(); //deal with the incoming data from driver. Determines what to do acording the message type

		// Process incoming messages.  Calls ProcessMessage() on each pending message.
		ProcessMessages();

		//give robot a chance to change state May this can lag the sonar update
		usleep(10); //Warning: This can lag sonar's readings
		//mainLoopLag = tmr.elapsedus() - mainLoopLag;
		//if(mainLoopLag!=mainLoopLagLast) printf("MainLoopLag:%.2lf us.\n",mainLoopLag);
		//mainLoopLagLast = mainLoopLag; 
	}
	return;
}


int Donnie::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data){
	 // Handle new data comming from client
	 //PLAYER_WARN("New message received from client");
	 if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_DIO_CMD_VALUES, m_dio_addr)){
			ProcessDioCommand(hdr, *reinterpret_cast<player_dio_cmd_t *>(data));
			
			return(0);
	 }
	 else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_DIO_DATA_VALUES, m_dio_addr)){
			//ProcessDioData(hdr, *reinterpret_cast<player_dio_data_t *>(data));
			if(print_debug_messages==1) PLAYER_WARN("Dio data received");
			return(0);
	 }
	 //neck position 2D
	 else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_VEL, m_neck_position_addr)){
			//to use foo.SetSpeed (double aXSpeed, double aYawSpeed)
			if(print_debug_messages==1) PLAYER_WARN("NECK position2d vel cmd received");
			assert(hdr->size == sizeof(player_position2d_cmd_vel_t));
			ProcessNeckPos2dVelCmd(hdr, *reinterpret_cast<player_position2d_cmd_vel_t *>(data));
			return(0);
	 }
	 //base position 2D
	 else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_POS, m_position_addr)){
			//to use foo.GoTo (player_pose2d_t pos, player_pose2d_t vel)
			if(print_debug_messages==1) PLAYER_WARN("position2d goto cmd received");
			assert(hdr->size == sizeof(player_position2d_cmd_pos_t)); //g if this is false then call eception error
			ProcessPos2dPosCmd(hdr, *reinterpret_cast<player_position2d_cmd_pos_t *>(data));
			return(0);
	 }
	 else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_POSITION2D_CMD_VEL, m_position_addr)){
			//to use foo.SetSpeed (double aXSpeed, double aYawSpeed)
			//PLAYER_WARN("position2d vel cmd received");
			assert(hdr->size == sizeof(player_position2d_cmd_vel_t));
			ProcessPos2dVelCmd(hdr, *reinterpret_cast<player_position2d_cmd_vel_t *>(data));
			return(0);
	 }
	 else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_MOTOR_POWER, m_position_addr)){
			//to use foo.SetMotorEnable(bool enable)
			if(print_debug_messages==1) PLAYER_WARN("position2d motor power cmd received");
			this->Publish(m_position_addr, resp_queue, PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_MOTOR_POWER);
			return 0;
	 }
	 else if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_GET_GEOM, m_position_addr)){
			//to use foo.RequestGeom()
			if(print_debug_messages==1) PLAYER_WARN("position2d update geometry request received");
			ProcessPos2dGeomReq(hdr);
			return(0);
	 }
	 /*if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD, PLAYER_BEEP_CMD_VALUES, m_beep_addr)){
			ProcessBeepCmd(hdr, *reinterpret_cast<player_beep_cmd_t*>(data));
			

			return 0;
	 }*/

	

	// Tell the caller that you don't know how to handle this message
	return(-1);
}

void Donnie::ProcessDioCommand(player_msghdr_t* hdr, player_dio_cmd_t &data){
	 if(print_debug_messages==1) PLAYER_WARN("Message ProcessDioCommand");

	 std::cout << "Dio count:" << data.count << std::endl; //bits qnt
	 std::cout << "Dio digout:" << std::hex << data.digout << std::endl; //decimal value

	 tx_data_count=2;
	 tx_data[0]=DIOPACK;
	 tx_data[1]=(uint8_t)data.digout;
	 arduino->writeData(tx_data,tx_data_count);

	 //Publish a message via one of this driver's interfaces. This publish data to subscribed clients.
	 //This form of Publish will assemble the message header for you. The message is broadcast to all interested parties
	 Publish(m_dio_addr, PLAYER_MSGTYPE_DATA, PLAYER_DIO_CMD_VALUES, reinterpret_cast<void*>(&data), sizeof(data), NULL); //the NULL is the Timestamp and meens that the current time will be filled in) 
}

//NECK
//foo.SetSpeed(double x, double pa)
void Donnie::ProcessNeckPos2dVelCmd(player_msghdr_t* hdr, 
						player_position2d_cmd_vel_t &data){
	std::cout << "Pos2DVelCmd vel.px:" << data.vel.px << " vel.py:" << data.vel.py << " vel.pa:"<< data.vel.pa << std::endl; //bits qnt
	std::cout << "Pos2DVelCmd state:" << std::hex << data.state << std::endl;
	std::cout << std::endl;

	tx_data_count=3;
	tx_data[0]=SERVOPACK;
	tx_data[1]=0x10;
	tx_data[2]=(uint8_t)data.vel.pa;

	arduino->writeData(tx_data,tx_data_count);
}

void Donnie::ProcessPos2dPosCmd(player_msghdr_t* hdr,
																			player_position2d_cmd_pos_t &data){
	std::cout << "Pos2DPosCmd pos.px:" << data.pos.px << " pos.py:" << data.pos.py << " pos.pa:"<< data.pos.pa << std::endl; //bits qnt
	std::cout << "Pos2DPosCmd vel.px:" << data.vel.px << " vel.py:" << data.vel.py << " vel.pa:"<< data.vel.pa << std::endl; //bits qnt
	std::cout << "Pos2DVelCmd state:" << data.state << std::endl;
	std::cout << std::endl;


}


double map(double x, double in_min, double in_max, double out_min, double out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


//foo.SetSpeed(double x, double pa)
void Donnie::ProcessPos2dVelCmd(player_msghdr_t* hdr, 
						player_position2d_cmd_vel_t &data){
	//double linear_r, angular_r;
	double right_aux, left_aux,right_pwm, left_pwm;


	//std::cout << "Pos2DVelCmd vel.px:" << data.vel.px << " vel.py:" << data.vel.py << " vel.pa:"<< data.vel.pa << std::endl; //bits qnt
	//std::cout << "Pos2DVelCmd state:" << std::hex << data.state << std::endl;
	//std::cout << std::endl;

	//linear_r = data.vel.px*linear_max_vel;
	//angular_r = data.vel.pa*ang_max_vel;

	//if(data.vel.px>maxSpeed) std::cout << "Valor acima da velocidade maxima: " << data.vel.px << std::endl;
	
	
	right_aux = data.vel.px + data.vel.pa;
	left_aux  = data.vel.px - data.vel.pa;

	right_pwm = map(right_aux,0,linear_max_vel,0,255);
	left_pwm = map(left_aux,0,linear_max_vel,0,255);


	//std::cout << "right_pwm: " << right_pwm << std::endl;
    // std::cout << "left_pwm: " << left_pwm << std::endl;

	tx_data_count=5;
	tx_data[0]=MOTORPACK;
	if(right_pwm>0){
		tx_data[1]=0x0f;
		tx_data[2]=right_pwm;
	}
	else{
		tx_data[1]=(uint8_t)0xf0;
		tx_data[2]=(uint8_t)(-1*right_pwm);
	}
	if(left_pwm>0){
		tx_data[3]=0xf0;
		tx_data[4]=left_pwm;
	}
	else{
		tx_data[3]=(uint8_t)0x0f;
		tx_data[4]=(uint8_t)(-1*left_pwm);
	}

	//std::cout << "right_pwm: " << std::dec << (int)tx_data[2] << "dir: " << std::hex << tx_data[1] << std::endl;
	//std::cout << "left_pwm: " << std::dec << (int)tx_data[4] << "dir: " << std::hex << tx_data[3] << std::endl << std::endl;


	arduino->writeData(tx_data,tx_data_count);
	/*
	this->m_pos_data.pos.px = x_;
	this->m_pos_data.pos.py = y_;
	this->m_pos_data.pos.pa = th_;*/
	this->m_pos_data.vel.px = data.vel.px;
	//this->m_pos_data.vel.py = speedL;
	this->m_pos_data.vel.pa = data.vel.pa;

	//update odometry data (code source: server/drivers/mixed/wbr/914/wbr914.cc)
    this->Publish(this->m_position_addr,
		  PLAYER_MSGTYPE_DATA,PLAYER_POSITION2D_DATA_STATE,
		  (void*)&(this->m_pos_data), sizeof(this->m_pos_data), NULL); //sizeof(player_position2d_data_t), NULL);

	Odometry();

}

void Donnie::ProcessPos2dGeomReq(player_msghdr_t* hdr){
	// this function updates the geometry and its data can be acessed by foo.GetOffset()
	player_position2d_geom_t geom;

	
	geom.pose.px = m_pos_data.pos.px;                                           // [m]
	geom.pose.py = m_pos_data.pos.py;                                           // [m]
	geom.pose.pz = m_pos_data.pos.pa; //marques                  // [rad]
	geom.size.sl = robot_length;                                                    // [m]
	geom.size.sw = robot_width;                                                     // [m]
	geom.size.sh = robot_height;                                                    // [m]

	/*std::cout << "Pos2dGeom pa:" << m_pos_data.pos.pa << std::endl;
	std::cout << std::endl;*/

	Publish(m_position_addr, 
				 PLAYER_MSGTYPE_RESP_ACK, PLAYER_POSITION2D_REQ_GET_GEOM, 
				 &geom, sizeof(geom), NULL);

}


void Donnie::ProcessDioData(){
	/*printf("DIOPACK:");

	for(int i=rx_data[1]-1;i>=0;i--) printf("%u",(rx_data[2] >> i) & 1); //show value as binary
	printf("\n\n");            */     

	player_dio_data_t diodata;
	diodata.count=rx_data[1]; 
	diodata.bits=rx_data[2]; //bitfield

	Publish(m_dio_addr, PLAYER_MSGTYPE_DATA, PLAYER_DIO_DATA_VALUES,
									 reinterpret_cast<void *>(&diodata), sizeof(diodata), NULL); 
}


void Donnie::ProcessRangerData(){
	uint8_t i;
	/*
	printf("RANGERPACK:");
	for(i=0;i<rx_data_count-1;i++){
		printf("%.2X",rx_data[i+1]); //+1 devido a prosicao zero ser o typo da mensagem
	}
	printf("\n\n");*/

	player_ranger_data_range_t rangerdata;
	memset( &rangerdata, 0, sizeof(rangerdata) );

	// a sonar/IR type with one range per beam origin 
	rangerdata.ranges_count = rx_data[1]; //quantity of sensors in this package
	rangerdata.ranges = new double[rx_data[1]]; //alocate memory for the rangers
	for(i=0;i<rx_data[1];i++){
		rangerdata.ranges[i] = rx_data[i+2]/100.0;
	}


	Publish(m_ranger_addr, PLAYER_MSGTYPE_DATA, PLAYER_RANGER_DATA_RANGE,
		 reinterpret_cast<void *>(&rangerdata), sizeof(rangerdata), NULL); 
}

void Donnie::ProcessBumperData(){
	uint8_t i;
	/*
	printf("BUMPERPACK:");
	for(i=0;i<rx_data_count-1;i++){
		printf("%.2X",rx_data[i+1]); //+1 devido a prosicao zero ser o typo da mensagem
	}
	printf("\n\n");*/

	player_bumper_data_t bumperdata;
	memset(&bumperdata,0,sizeof(bumperdata));

	// Update bumper data
	bumperdata.bumpers_count = rx_data[1]; //quantity of bumpers in this package
	bumperdata.bumpers = new uint8_t[rx_data[1]];
	for(i=0;i<rx_data[1];i++){
		bumperdata.bumpers[i] = rx_data[i+2];
	}


	this->Publish(bumper_addr,
		PLAYER_MSGTYPE_DATA, PLAYER_BUMPER_DATA_STATE,
		(void*)&bumperdata, sizeof(bumperdata), NULL);


	delete bumperdata.bumpers;
}

void Donnie::ProcessPowerData(){
	uint8_t i;
	/*
	printf("POWERPACK:");
	for(i=0;i<rx_data_count-1;i++){
		printf("%.2X",rx_data[i+1]); //+1 devido a prosicao zero ser o typo da mensagem
	}
	printf("\n\n");*/


	player_power_data_t powerdata;
  memset(&powerdata,0,sizeof(powerdata));


  int16_t aux = 0;
  aux = aux ^ rx_data[2]; //0000 & 0001 = 0001
  aux = (aux << 8) ^ rx_data[1];   //0001 & 0100 = 0101




  // Update power data
	powerdata.volts = (float)aux/1000; // rx_data[1];  //float [V]
	//powerdata.watts = voltage * current;
	powerdata.valid = (PLAYER_POWER_MASK_VOLTS | PLAYER_POWER_MASK_WATTS); //sem essa mascara o valor do proxy nao atualiza

  //powerdata.percent =;
  //powerdata.charging =;

	this->Publish(power_addr,
		PLAYER_MSGTYPE_DATA, PLAYER_POWER_DATA_STATE,
		(void*)&powerdata, sizeof(powerdata), NULL);
}

void Donnie::ProcessSystemMessageData(){
	uint8_t i;
	//printf("SYSTEM MESSAGE:");
	for(i=0;i<rx_data_count-1;i++){
		printf("%c",rx_data[i+1]); //+1 devido a prosicao zero ser o typo da mensagem
	}
	//printf("\n\n");

}

void Donnie::ProcessRequestConfig(){
	uint8_t i;
	/*
	printf("RECEIVED REQUEST CONFIG:");
	for(i=0;i<rx_data_count-1;i++){
		printf("%.2X",rx_data[i+1]); //+1 devido a prosicao zero ser o typo da mensagem
	}
	printf("\n\n");*/

	if(print_debug_messages==1) printf("RECEIVED REQUEST CONFIG. Sending Arduino Config...\n\n");

	// update arduino config variables
	uint8_t converted[10]; //10 valor que caiba as conversoes
	tx_data[0] = CONFIGPACK;

	FloatToBytes(pid_kp,converted);
	tx_data[1] = converted[0];
	tx_data[2] = converted[1];
	tx_data[3] = converted[2];
	tx_data[4] = converted[3];

	FloatToBytes(pid_ki,converted);
	tx_data[5] = converted[0];
	tx_data[6] = converted[1];
	tx_data[7] = converted[2];
	tx_data[8] = converted[3];

	FloatToBytes(pid_kd,converted);
	tx_data[9] = converted[0];
	tx_data[10] = converted[1];
	tx_data[11] = converted[2];
	tx_data[12] = converted[3];

	FloatToBytes(pid_setpoint,converted);
	tx_data[13] = converted[0];
	tx_data[14] = converted[1];
	tx_data[15] = converted[2];
	tx_data[16] = converted[3];

	tx_data[17] = (int8_t)pid_offset_r;
	tx_data[18] = (int8_t)pid_offset_l;

	tx_data_count=19;
	arduino->writeData(tx_data,tx_data_count);

}


void Donnie::ProcessRequestPing(){
	/*uint8_t i;
	printf("RECEIVED REQUEST PING:");
		for(i=0;i<rx_data_count-1;i++){
		printf("%.2X",rx_data[i+1]); //+1 devido a prosicao zero ser o typo da mensagem
	}

	double elapsed = tmr.elapsed();
    printf(", Time elapsed:%.2lf sec.\n",elapsed);


	

	puts("Sending ping...");*/
	tx_data_count=2;
	tx_data[0]=PINGPACK;
	tx_data[1]=43;
	arduino->writeData(tx_data,tx_data_count);
	if(print_debug_messages==1) printf(".\n");
}






	int32_t change_left = 0;
	int32_t change_right = 0;
	int32_t transchange = 0;
	int32_t rotchange = 0;
	double encoderPackLag;
	
void Donnie::ProcessEncoderData(){
	//encoderPackLag = tmr.elapsedus() - encoderPackLag ;
	//printf("EncoderPackTS:%.2lf us.\n",encoderPackLag);
	//encoderPackLag = tmr.elapsedus();
	uint8_t i;
	/*
	printf("ENCODERPACK:");
	for(i=0;i<rx_data_count-1;i++){
		printf("%.2X",rx_data[i+1]); //+1 devido a prosicao zero ser o typo da mensagem
	}
	printf("\n\n");
	*/	
	int16_t auxTicksR = 0; 
	int16_t auxTicksL = 0;
	int16_t speedR = 0; 
	int16_t speedL = 0;

 	auxTicksR = auxTicksR ^ rx_data[1];
	auxTicksR = (auxTicksR << 8) ^ rx_data[2];  
 	auxTicksL = auxTicksL ^ rx_data[3]; 
 	auxTicksL = (auxTicksL << 8) ^ rx_data[4];  
 	auxTicksR = (int16_t)auxTicksR;
 	auxTicksL = (int16_t)auxTicksL;

 	
 	speedR = speedR ^ rx_data[5];
	speedR = (speedR << 8) ^ rx_data[6];  
 	speedL = speedL ^ rx_data[7]; 
 	speedL = (speedL << 8) ^ rx_data[8];  
 	speedR = (int16_t)speedR;
 	speedL = (int16_t)speedL;


 	this->ticksR = auxTicksR;
 	this->ticksL = auxTicksL;
 	Odometry();

 	//if(print_debug_encoder) printf("Encoder L:%d|R:%d\n",auxTicksL,auxTicksR);

 	/*
 	ou um ou outro
 	speedR = (int16_t) rx_data[5];
 	speedL = (int16_t) rx_data[6];
 	*/


	
	//printf("ticksR:%d, ticksL:%d\n",ticksR,ticksL);
	//rx_data[1]; //ticksR
	//rx_data[2]; //ticksL

	/*
	this->m_pos_data.pos.px = ticksR;
	this->m_pos_data.pos.py = ticksL;
	this->m_pos_data.pos.pa = 42;
	this->m_pos_data.vel.px = speedR;
	this->m_pos_data.vel.py = speedL;
	this->m_pos_data.vel.pa = 42;

	//update odometry data (code source: server/drivers/mixed/wbr/914/wbr914.cc)
    this->Publish(this->m_position_addr,
		  PLAYER_MSGTYPE_DATA,PLAYER_POSITION2D_DATA_STATE,
		  (void*)&(this->m_pos_data), sizeof(this->m_pos_data), NULL); //sizeof(player_position2d_data_t), NULL);
*/
}

void Donnie::Odometry(){
	//M_PI is defined in math.h
	Dr=(2* M_PI* wheel_radius)/cpr * (ticksR-lasttickR); //(meters per ticks) * deltaTick 
	Dl=(2* M_PI* wheel_radius)/cpr * (ticksL-lasttickL); //(meters per ticks) * deltaTick 
	Dc= (Dr+Dl)/2.0; // robot center translation
	th = th + (Dr-Dl)/axle_length; // affects the odometry directly

	// force th to be in the range 0 to 6.28 pi
	if (th > 2.0 * M_PI) th = th - (2.0 * M_PI); //marques
	if (th < 0.0)      th = th + (2.0 * M_PI); //marques

	x= x + Dc *cos(th);
	y= y + Dc *sin(th);

	this->m_pos_data.pos.px = x;
	this->m_pos_data.pos.py = y;
	this->m_pos_data.pos.pa = th;

	// Print only messages of encoderpack (81)
	if(print_debug_messages==ENCODERPACK){
		if(abs(ticksR-lasttickR)!=0 || abs(ticksL-lasttickL)!=0){ //marques
			std::cout 	<< "x:" << this->m_pos_data.pos.px << ","
					<< "y:" << this->m_pos_data.pos.py << ","
					<< "th:" << this->m_pos_data.pos.pa << ","
					<< "ts:" << tmr.elapsedus()
					<< std::endl; //toPlot
		}
	}

	lasttickR=ticksR;
	lasttickL=ticksL;

	//update odometry data (code source: server/drivers/mixed/wbr/914/wbr914.cc)
    this->Publish(this->m_position_addr,
		  PLAYER_MSGTYPE_DATA,PLAYER_POSITION2D_DATA_STATE,
		  (void*)&(this->m_pos_data), sizeof(this->m_pos_data), NULL);
}


void Donnie::FloatToBytes(float value, uint8_t *data){
	memset (data, 0, sizeof(data));
    //This creates a new union type in wich all its member elements occupy the same physical space in memory
	union {
	  float f;
	  uint8_t b[4]; //u.b[3] = b0;
	} u;

	u.f = value;

    data[0] = u.b[3];
    data[1] = u.b[2];
    data[2] = u.b[1];
    data[3] = u.b[0];
}

void Donnie::Uint16_tToBytes(uint16_t value, uint8_t *data){
	memset (data, 0, sizeof(data));
    //This creates a new union type in wich all its member elements occupy the same physical space in memory
	union {
	  uint16_t v;
	  uint8_t b[2]; //u.b[1] = b0;
	} u;

	u.v = value;

    data[0] = u.b[1];
    data[1] = u.b[0];
}

void Donnie::Uint32_tToBytes(uint32_t value, uint8_t *data){
	memset (data, 0, sizeof(data));
    //This creates a new union type in wich all its member elements occupy the same physical space in memory
	union {
	  uint32_t v;
	  uint8_t b[4]; //u.b[3] = b0;
	} u;

	u.v = value;

    data[0] = u.b[3];
    data[1] = u.b[2];
    data[2] = u.b[1];
    data[3] = u.b[0];
}

/*void Donnie::ProcessBeepCmd(player_msghdr_t* hdr, player_beep_cmd_t &data){

	uint16_t frequency = data.frequency;
	uint32_t duration = data.duration;
	//std::cout << "Freq:" << data.frequency << std::endl;
	//std::cout << "Dur:" << data.duration << std::endl << std::endl;
	//PLAYER_MSG0(0, "Beep sending..");
	uint8_t converted[10]; //10 valor que caiba as conversoes
 	tx_data_count=7;
	tx_data[0]=BEEPPACK;
	Uint16_tToBytes(frequency,converted);
	tx_data[1]=converted[0];
	tx_data[2]=converted[1];
	Uint32_tToBytes(duration,converted);
	tx_data[3]=converted[0];
	tx_data[4]=converted[1];
	tx_data[5]=converted[2];
	tx_data[6]=converted[3];
	arduino->writeData(tx_data,tx_data_count);
	//PLAYER_MSG0(0, "Beep sent");
}*/

/*void Donnie::Beep(uint16_t frequency,uint32_t duration){
	player_beep_cmd_t data;
	data.frequency = frequency;
	data.duration = duration;

	ProcessBeepCmd(NULL,data);
}*/

void Donnie::MotorsOff(){
	tx_data_count=5;
	tx_data[0]=MOTORPACK;
	tx_data[1]=0;
	tx_data[2]=0;
	tx_data[3]=0;
	tx_data[4]=0;
	arduino->writeData(tx_data,tx_data_count);
}
