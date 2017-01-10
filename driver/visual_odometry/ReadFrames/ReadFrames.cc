/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2003  
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
 * A simple example of how to write a driver that will be built as a
 * shared object.
 */

// ONLY if you need something that was #define'd as a result of configure 
// (e.g., HAVE_CFMAKERAW), then #include <config.h>, like so:
/*
#include <config.h>
*/

#if !defined (WIN32)
  #include <unistd.h>
#endif
#include <string.h>
#include <iostream>
#include <assert.h>

#include <libplayercore/playercore.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// The class for the driver
class ReadFrames : public ThreadedDriver
{
  public:
    
    // Constructor; need that
    ReadFrames(ConfigFile* cf, int section);

    //This method will be invoked on each incoming message
    virtual int ProcessMessage(QueuePointer &resp_queue, 
                               player_msghdr * hdr,
                               void * data);

  private:

    // Main function for device thread.
    virtual void Main();
    virtual int MainSetup();
    virtual void MainQuit();
    int readframe();

    VideoCapture capture;
    player_devaddr_t camera_addr; // camera a ser enviada
    Mat frame;
    player_camera_data_t * data;
    int height, width, channels;
    int cont, max_frames, init_frame;
    int step;

    const char* path;
    char imgpath[80];
};

// A factory creation function, declared outside of the class so that it
// can be invoked without any object context (alternatively, you can
// declare it static in the class).  In this function, we create and return
// (as a generic Driver*) a pointer to a new instance of this driver.
Driver* 
ReadFrames_Init(ConfigFile* cf, int section)
{
  // Create and return a new instance of this driver
  return((Driver*)(new ReadFrames(cf, section)));
}

// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void ReadFrames_Register(DriverTable* table)
{
  table->AddDriver("ReadFrames", ReadFrames_Init);
}

////////////////////////////////////////////////////////////////////////////////
// Constructor.  Retrieve options from the configuration file and do any
// pre-Setup() setup.
ReadFrames::ReadFrames(ConfigFile* cf, int section)
    : ThreadedDriver(cf, section, false, PLAYER_MSGQUEUE_DEFAULT_MAXLEN) 
{
  memset(&(this->camera_addr), 0, sizeof (player_devaddr_t)); 
  memset(&(this->data), 0, sizeof (player_camera_data_t)); 
  
  if (cf->ReadDeviceAddr(&(this->camera_addr), section, "provides", PLAYER_CAMERA_CODE, -1, NULL)){
    this->SetError(-1);
    return;
  }
  if (this->AddInterface(this->camera_addr)){
    this->SetError(-1);
    return;
  }
  height = cf->ReadInt(section, "height", 480);
  width = cf->ReadInt(section, "width", 640);
  channels = cf->ReadInt(section, "channels", 3);
  max_frames = cf->ReadInt(section, "max_frames", 1);
  init_frame = cf->ReadInt(section, "init_frame", 0);
  cont = init_frame; 

  step = cf->ReadInt(section, "step", 1);
  path = cf->ReadString(section, "path", "/home/lsa/Desktop/frames");
  cout << "path: " << path << endl;

  return;
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int ReadFrames::MainSetup() 
{   
  puts("ReadFrames initializing! ");

  // Here you do whatever is necessary to setup the device, like open and
  // configure a serial port.
  
  puts("ReadFrames ready");

  return(0);
}



////////////////////////////////////////////////////////////////////////////////
// Shutdown the device
void ReadFrames::MainQuit() // este método serve para fechar tudo que ficou aberto. fechar a raspicam e o opencv.
{
  puts("Shutting ReadFrames driver down");

  // Here you would shut the device down by, for example, closing a
  // serial port.
  frame.release();

  puts("ReadFrames driver has been shutdown");
}



int ReadFrames::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data)
{
  assert(hdr);

 // Process messages here.  Send a response if necessary, using Publish().
 // If you handle the message successfully, return 0.  Otherwise,
 // return -1, and a NACK will be sent for you, if a response is required.
  return(0);
}

int ReadFrames::readframe()
{
    int i;
    //sleep(delay);
    //get image from path
    sprintf(imgpath, "%s/%06d.png", path, cont);
    frame = imread(imgpath);

    if (frame.depth() != CV_8U)
    {
      PLAYER_ERROR1("Unsupported depth %d", frame.depth());
      return -1;
    }
    data = reinterpret_cast<player_camera_data_t *>(calloc(1,sizeof(player_camera_data_t)));  
    if (!data)
    {
      PLAYER_ERROR("Out of memory - data");
      return -1;
    }
    
    //alteração de channels conforme .cfg
    if(frame.channels() == 3 && channels == 1)
      cvtColor(frame, frame, CV_BGR2GRAY, 1);
    if(frame.channels() == 1 && channels == 3)
      cvtColor(frame, frame, CV_GRAY2BGR, 3);

    Size size(width,height);
    resize(frame,frame,size);

    cout << "frame channels " << frame.channels() << endl;
    cout << "height " << frame.size().height << endl;
    cout << "width " << frame.size().width << endl;
    assert((frame.channels()) > 0);
    data->image = reinterpret_cast<unsigned char *>(calloc(height*width,sizeof(uint8_t)*channels));

    data->image_count = width * height * channels;
    assert(data->image_count > 0);
    
    if (!(data->image))
    {
      PLAYER_ERROR("Out of memory -  image");
      return -1;
    }
    data->width = width;
    data->height = height;
    data->bpp = channels * 8;
    data->fdiv = 0;
    memcpy(data->image, frame.ptr(), data->image_count);
    frame.release();
    switch (data->bpp)
    {
      case 8:
        data->format = PLAYER_CAMERA_FORMAT_MONO8;
        break;
      case 24:
        data->format = PLAYER_CAMERA_FORMAT_RGB888;
        break;
      case 32:
        data->format = PLAYER_CAMERA_FORMAT_RGB888;
      default:
        PLAYER_ERROR1("Unsupported image depth %d", data->bpp);
        data->bpp = 0;
    }
    assert(data->bpp > 0);
    data->compression = PLAYER_CAMERA_COMPRESS_RAW;

    this->Publish(device_addr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, reinterpret_cast<void *>(data), 0, NULL, false);
    // cout << "cont " << cont << endl;
    cont += step;
  return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void ReadFrames::Main() 
{
  // The main loop; interact with the device here
  for(;;)
  {
    // test if we are supposed to cancel
    pthread_testcancel();

    // Process incoming messages.  ExampleDriver::ProcessMessage() is
    // called on each message.
    ProcessMessages();
    // Interact with the device, and push out the resulting data, using
    // Driver::Publish()

    //lê um numero max de frames especificado no .cfg
    if(cont < max_frames)
      readframe();
    else{
      break;
    }

    // Sleep (you might, for example, block on a read() instead)
    usleep(100000);
  }
}

////////////////////////////////////////////////////////////////////////////////
// Extra stuff for building a shared object.

/* need the extern to avoid C++ name-mangling  */
extern "C" {
  int player_driver_init(DriverTable* table)
  {
    puts("Example driver initializing  " );
    ReadFrames_Register(table);
    puts("Example driver done");
    return(0);
  }
}

