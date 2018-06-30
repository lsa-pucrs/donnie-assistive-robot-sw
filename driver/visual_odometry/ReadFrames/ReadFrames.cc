/*!
* Desc: Player-based read frames driver.
* Author: Joice Marek
* Date: January 2017
* Laboratório de Sistemas Autônomos
* - https://lsa.pucrs.br/
* - https://github.com/lsa-pucrs
* Faculdade de Informática - PUCRS
* - www.inf.pucrs.br
*/

/*! ReadFrames configuration file syntax:

driver
(
	name "ReadFrames"
	plugin "/opt/player-stage/src/player-3.0.2/build/examples/plugins/ReadFrames/libReadFrames.so"
	provides ["camera:0"]
    #Specify the height (in pixels) of the camera image. Default value is 480.
    height 480
    #Specify the width (in pixels) of the camera image. Default value is 640.
    width 640
    #Specify the number of channels for the camera. Value 1 is a greyscale image and 3 a RGB image.
    channels 1
    #Specify the step for reading. This can decrease the trajectory capture FPS. FPS = FPS/step.
	step 1
    #Specify the maximun number of frames. Default 100
	max_frames 350
    #Specify the frame to start reading. Default is 0
	init_frame 0
    #Specify the path to the list of frames
	path "/home/user/frames"

)
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
/*! This driver reads frames from a list of images and publishes them, so a client or other driver can receive it. */
class ReadFrames : public ThreadedDriver
{
  public:
    
    //! Constructor
    ReadFrames(ConfigFile* cf, int section);

    //!This method will be invoked on each incoming message
    virtual int ProcessMessage(QueuePointer &resp_queue, 
                               player_msghdr * hdr,
                               void * data);

  private:

    //! Main function for device thread.
    virtual void Main();
    //! Runs when a client connects to this driver.
    virtual int MainSetup();
    //! Runs when a client disconnects from this driver.
    virtual void MainQuit();
    
    int readframe();

    //! Camera interface
    player_devaddr_t camera_addr;
    //! Camera data
    player_camera_data_t * data;
    
    //! Frame to be publish
    Mat frame;
    //! Frame characteristcs. 
    int height, width, channels;
    //! Frame to start reading
    int init_frame;
    //! Maximun number of frames
    int max_frames;
    //! Step for reading. FPS = FPS/step
    int step;
    //! Count frames
    int cont;
    //! Path to the list of frames
    const char* path;
    //! Path to each frame
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
  max_frames = cf->ReadInt(section, "max_frames", 100);
  init_frame = cf->ReadInt(section, "init_frame", 0) ;
  step = cf->ReadInt(section, "step", 1);
  path = cf->ReadString(section, "path", "/home/user/frames");
	    
	cont = init_frame;

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
    //! Get image from path
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
    
    //! Changing channels to the specified in configuration file
    if(frame.channels() == 3 && channels == 1)
      cvtColor(frame, frame, CV_BGR2GRAY, 1);
    if(frame.channels() == 1 && channels == 3)
      cvtColor(frame, frame, CV_GRAY2BGR, 3);
    //! Resize frame to the specified size in the configuration file
    Size size(width,height);
    resize(frame,frame,size);

    //cout << "frame channels " << frame.channels() << endl;
    //cout << "height " << frame.size().height << endl;
    //cout << "width " << frame.size().width << endl;
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

    //! Read untill max number of frames specified in the configuration file
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
    puts("ReadFrames driver initializing  " );
    ReadFrames_Register(table);
    puts("ReadFrames driver done");
    return(0);
  }
}

