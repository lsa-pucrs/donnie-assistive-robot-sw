/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2006 -
 *     Brian Gerkey
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
 *  Copyright (C) 2017
 *     LSA (https://lsa.pucrs.br/), Faculdade de Informática, PUCRS University
 *     Guilherme Marques, Alexandre Amory
 */

/** @ingroup drivers Drivers */
/** @{ */
/** @defgroup driver_soxplayer soxplayer
 @brief Player driver for LibSoX

The soxplayer driver receives the filename of an audio file supported by LibSoX 
and plays it. It can also apply sox effects such as speed, tempo, pitch. 
Refer to www.libsox.org to check the other available effects.

@par Compile-time dependencies

- libsox (sudo apt-get install libsox-dev)

@par Provides

- @ref interface_playsound

@par Requires

- none

@par Configuration requests

- none

@par Configuration file options

- speed (float)
  - Apply the 'speed' sox effect to control the sound speed
  - Default: 1
  - fast <1; slow >1 

- pitch (float)
  - Apply the 'pitch' sox effect to control the sound pitch
  - Default: ???
  - ????

- tempo (float)
 - Apply the 'tempo' sox effect to control the sound tempo
 - Default: 1
 - fast >1; slow <1  

@par Example

@verbatim
driver
(
	name "soxplayer"
	plugin "libsoxplayer"
	provides ["playsound:0"]
)
@endverbatim

@todo
- define the range for the configuration parameters

@author Guilherme Marques, Alexandre Amory

*/
/** @} */


#ifndef _SOXDEVICE_H
#define _SOXDEVICE_H


#include <libplayercore/playercore.h>

#include <iostream>
#include <sys/time.h>
#include <cmath>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //sleep
#include <fstream> //to check file exists

// Message levels
#define MESSAGE_ERROR	0
#define MESSAGE_INFO	1
#define MESSAGE_DEBUG	2

////////////////////////////////////////////////////////////////////////////////
// The class for the driver
// devices are used to communication between drivers
// interfaces used to communication between client and the driver
class Soxplayer : public ThreadedDriver{
	public:
		// Deals with assosiations comming by .cfg file
		Soxplayer(ConfigFile* cf, int section);
		~Soxplayer();
		// Deals with messages comming from the clients
		virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data);

	private:
		player_devaddr_t sound_addr;  //playsound interface

		//sox effects
		std::string speed_str;
		std::string pitch_str;
		std::string tempo_str;

		// Set up the device (ONLY WHEN A CLIENT CONECTS TO THE DRIVER).  Return 0 if things go well, and -1 otherwise.
		virtual int MainSetup();
		//Shutdown the device (occurs in each client shutdown)
		virtual void MainQuit();
		// Main function for device thread
		virtual void Main();
		// Executed when an incomming message is received and plays the audio file
		int  Play(char *fileAdr);
};


#endif
