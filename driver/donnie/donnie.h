#ifndef donnie_h
#define donnie_h

#include <libplayercore/playercore.h>
#include <sys/time.h>
#include <string.h>
#include <iostream>
#include "protocol.cc"
#include "motors.h"

#define BUFFER_SIZE 200

//!Robot Defines
#define PI 3.141592653
#define DIAMETER 0.44 //[m]
#define RADIUS (DONNIE_DIAMETER * 0.5) //[m]
#define CIRCUMFERENCE (2 * PI * DONNIE_RADIUS) //[m]
#define CENTRE_TO_WHEEL 0.135 //[m]
#define PULSE_TO_RPM 1.83  //[rpm] (SEC_PR_MIN*MSEC_PR_SEC) / GEAR_RATIO / PULSES_PR_REV

#define WHEEL_RADIUS  2.1 // [mm]0.04 //[m]
#define WHEEL_DIAMETER (WHEEL_RADIUS*2)
#define DEFAULT_AXLE_LENGTH	0.083
//#define DEFAULT_AXLE_LENGTH	0.301


//! Timer to count time in seconds. 
/*! Used to count time for ping messages */
class Timer
{
public:
    Timer() { clock_gettime(CLOCK_REALTIME, &beg_); }

    double elapsed() {
        clock_gettime(CLOCK_REALTIME, &end_);
        return end_.tv_sec - beg_.tv_sec +
            (end_.tv_nsec - beg_.tv_nsec) / 1000000000.;
    }

    void reset() { clock_gettime(CLOCK_REALTIME, &beg_); }

private:
    timespec beg_, end_;
};

////////////////////////////////////////////////////////////////////////////////
//! Class to controls Donnie robot
/*! This class process the packages from the robot's microcontroller and 
publish it in the Player's environment. This data can be acessed by clients conected in the player server
Also this class process the commands from clients and can sent it to the microcontroller.
*/
class Donnie : public ThreadedDriver{
	public:

		//! Constructor.
		//! Runs when player server was launched
		Donnie(ConfigFile* cf, int section);

		//! This method will be invoked on each incoming message
		virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data);

	private:
		//! Main function for device thread.
		virtual void Main();
		//! Runs when a client connects to this driver
		virtual int MainSetup();
		//! Runs when a client disconnects from this driver 
		virtual void MainQuit();

		//! Verify the message type of a message package and branch to the respective procedure
		int processIncomingData(); 

		//! Procedure that deals with Dio Package Data
		void ProcessDioData();
		//! Procedure that deals with Ranger Package Data
		void ProcessRangerData();
		//! Procedure that deals with Bumper Package Data
		void ProcessBumperData();
		//! Procedure that deals with Power Package Data
		void ProcessPowerData();
		//! Procedure that deals with System Message Package Data
		void ProcessSystemMessageData();
		//! Procedure that deals with Config Package 
		void ProcessRequestConfig();
		//! Procedure that deals with Ping Package 
		void ProcessRequestPing();
		//! Procedure that deals with Encoder Package Data
		void ProcessEncoderData();

		//!Deals with commands from Client
		/*! Get the client inputs to manipulate digital pins in the robot microcontroller.
			\param hdr not used.
			\param data pointer to player_dio_cmd_t struct. That can be acessed to know which pins have to be changed.
			\return none
		*/
		void ProcessDioCommand(player_msghdr_t* hdr, player_dio_cmd_t &data);   
		//!Deals with commands from Client
		void ProcessNeckPos2dVelCmd(player_msghdr_t* hdr, player_position2d_cmd_vel_t &data);
		//!Deals with commands from Client
		void ProcessPos2dVelCmd(player_msghdr_t* hdr, player_position2d_cmd_vel_t &data);
		//!Deals with commands from Client
		void ProcessPos2dPosCmd(player_msghdr_t* hdr, player_position2d_cmd_pos_t &data);
		//!Deals with commands from Client
		void ProcessPos2dGeomReq(player_msghdr_t* hdr);

		//!Calculates Odometry by the wheel's encoder 
		void Odometry();
		//!Convert float in 4 Bytes
		void FloatToBytes(float value, uint8_t *data);
		//!Convert Uint16_t in 2 Bytes
		void Uint16_tToBytes(uint16_t value, uint8_t *data);
		//!Convert Uint32_t in 4 Bytes
		void Uint32_tToBytes(uint32_t value, uint8_t *data);
		//!Deals with BeepProxy commands from Client
		//!void ProcessBeepCmd(player_msghdr_t* hdr, player_beep_cmd_t &data);
		//!Send a beep command to play tones in a onboard buzzer
		//!void Beep(uint16_t frequency,uint32_t duration);
		//!Send speed zero to motors
		void MotorsOff();

		//!to get the execution time
		Timer tmr;

		//! COM port where Arduino is connected
		std::string port;

		//! pointer that deals with protocol messages. Read/Write packages from Arduino
		Serial *arduino;
		//! Serial buffers
		uint8_t rx_data[BUFFER_SIZE], tx_data[BUFFER_SIZE]; 
		unsigned int rx_data_count, tx_data_count;
 

		//! Dio interface
		player_devaddr_t m_dio_addr;  
		//! Ranger interface
		player_devaddr_t m_ranger_addr;
		//! Neck position interface
		player_devaddr_t m_neck_position_addr;
		//! Odometry interface
		player_devaddr_t m_position_addr;
		//! Bumper interface
		player_devaddr_t bumper_addr;
		//! Power interface
		player_devaddr_t power_addr;
		//! Odometry interface
		//!player_devaddr_t m_beep_addr;
		//! Beep interface

		//!Odometry data
		player_position2d_data_t m_pos_data;


		//! Odometry parameters
		int32_t last_posLeft;
		int32_t last_posRight;
		int16_t ticksR; 
		int16_t ticksL;

		//!Robot Geometry parameters
		double robot_width;
		double robot_length;
		double robot_height;

		//!Speed parameters 
		double linear_max_vel;
		double ang_max_vel;

		//!PID controller parameters
		double pid_kp; 
		double pid_ki; 
		double pid_kd; 
		double pid_setpoint;
		int pid_offset_r;
		int pid_offset_l;
		//!double angular_offset;

		//!Debug parameters
		int print_debug_messages;

		//!Others parameters
		double step_length;
};

#endif