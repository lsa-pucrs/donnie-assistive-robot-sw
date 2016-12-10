/*Raspicam_driver config file sintaxe example:
driver
(
	name "Raspicam_driver"
	plugin "/opt/player-stage/src/player-3.0.2/build/examples/plugins/Raspicam_driver/libRaspicam_driver.so"
    #Specify the numbers of FPS. Default value is 1
    fps 30
    #Specify the height (in pixels) of the camera image. Default value is 480
	height 480
    #Specify the width (in pixels) of the camera image. Default value is 640
	width  640
	#Specify the number os channels for the camera. Value 1 is a greyscale image and 3 a RGB image
	channels 3
	provides ["6665:camera:0"]

)

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
#include <ctime>

#include <libplayercore/playercore.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <raspicam/raspicam_cv.h>


using namespace cv;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// The class for the driver
//! A Class for a raspicam camera 
/*! This class uses a raspicam camera and publish its frames on the player format.*/
class Raspicam_driver : public ThreadedDriver
{
  public:
    
  //! Constructor.
  //! Runs when player server was launched.
    Raspicam_driver(ConfigFile* cf, int section);

  //! This method will be invoked on each incoming message.
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

    //! Configure raspicam.
    /*!
      \param FRAME_WIDTH width of the frames.
      \param FRAME_HEIGHT height of the frames.
      \param channels number of channes of the images. Can be 1(greyscale) or 3(RGB).
    */
    int configure(int FRAME_WIDTH, int FRAME_HEIGHT,int channels);
  
    //! Capture the next frame.
    Mat Raspi_Capture();
    //! Process and publish the current frame.
    int process_frame();


    //! Raspicam camera 
    raspicam::RaspiCam_Cv tCamera;

    //! Number of FPS. Can be used on the configuration file. Default value is 1.
    int fps;
    //! Used to assist the control of the number of frames per second.
    double delay;
	  double t1,t2;

    // ! Camera interface.
    player_devaddr_t camera_addr; 
    //! Camera data.
    player_camera_data_t * data;

    //! Frame to be published.
    Mat frame;
    // height of the frame. Can be used on the configuration file. Default value is 480.
    int height;
    // width of the frame. Can be used on the configuration file. Default value is 640.
    int width;
    // number of channels of the frame. Can be used on the configuration file. Default value is 3.
    int channels;

    //int cont;
    //int foop;
};

// A factory creation function, declared outside of the class so that it
// can be invoked without any object context (alternatively, you can
// declare it static in the class).  In this function, we create and return
// (as a generic Driver*) a pointer to a new instance of this driver.
Driver* 
Raspicam_driver_Init(ConfigFile* cf, int section)
{
  // Create and return a new instance of this driver
  return((Driver*)(new Raspicam_driver(cf, section)));
}


int Raspicam_driver::configure(int FRAME_WIDTH, int FRAME_HEIGHT,int channels)
{
 
 
tCamera.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH);
tCamera.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
tCamera.set(CV_CAP_PROP_BRIGHTNESS,50);
tCamera.set(CV_CAP_PROP_CONTRAST,50);
tCamera.set(CV_CAP_PROP_SATURATION,50);
tCamera.set(CV_CAP_PROP_GAIN,50);


 if(channels==1)
        tCamera.set(CV_CAP_PROP_FORMAT, CV_8UC1);
  else if(channels ==3)
        tCamera.set(CV_CAP_PROP_FORMAT, CV_8UC3);


  if(!tCamera.open())
    return -1;
  else
    return 0;
}

// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void Raspicam_driver_Register(DriverTable* table)
{
  table->AddDriver("Raspicam_driver", Raspicam_driver_Init);
}

////////////////////////////////////////////////////////////////////////////////
// Constructor.  Retrieve options from the configuration file and do any
// pre-Setup() setup.
Raspicam_driver::Raspicam_driver(ConfigFile* cf, int section)
    : ThreadedDriver(cf, section, false, PLAYER_MSGQUEUE_DEFAULT_MAXLEN) //pode colocar a interface que o driver proverá
{
  memset(&(this->camera_addr), 0, sizeof (player_devaddr_t)); 
  memset(&(this->data), 0, sizeof (player_camera_data_t)); 
  //cont = 0; 
  if (cf->ReadDeviceAddr(&(this->camera_addr), section, "provides", PLAYER_CAMERA_CODE, -1, NULL)){
    this->SetError(-1);
    return;
  }
  if (this->AddInterface(this->camera_addr)){
    this->SetError(-1);
    return;
  }

  fps = cf->ReadInt(section, "fps", 1);
  delay = 1.0 / fps;
  height = cf->ReadInt(section, "height", 480);
  width = cf->ReadInt(section, "width", 640);
  channels = cf->ReadInt(section, "channels", 3);
  t1=cv::getTickCount();
  
 if(configure(width,height,channels)==-1)
  {
    cout << "\tCamera not found or could not be opened" << endl;
    this->SetError(-1);
    //return -1;
  }

  return;
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int Raspicam_driver::MainSetup() 
{   
  puts("Raspicam_driver initializing! ");

  // Here you do whatever is necessary to setup the device, like open and
  // configure a serial port.

  //printf("Was foo option given in config file? %d\n", this->foop);
    
  puts("Raspicam_driver ready");

  return(0);
}



////////////////////////////////////////////////////////////////////////////////
// Shutdown the device
void Raspicam_driver::MainQuit()
{
  puts("Shutting example driver down");

  // Here you would shut the device down by, for example, closing a
  frame.release();
  tCamera.release();

  puts("Example driver has been shutdown");
}



int Raspicam_driver::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data)
{
  assert(hdr);

 // Process messages here.  Send a response if necessary, using Publish().
 // If you handle the message successfully, return 0.  Otherwise,
 // return -1, and a NACK will be sent for you, if a response is required.
  return(0);
}
Mat Raspicam_driver::Raspi_Capture()
{

    cv::Mat tCapturedImage;
    tCamera.grab();
    tCamera.retrieve(tCapturedImage);
    flip(tCapturedImage,tCapturedImage,-1);

    return tCapturedImage;
}
int Raspicam_driver::process_frame()
{

	t2=cv::getTickCount();
  if( (double ( t2-t1 ) /double ( cv::getTickFrequency())) >=delay)	 
  {

     frame=Raspi_Capture();

      Size size(height,width);
      resize(frame,frame,size); //use this resize because the configuration function of raspicam is not working properly
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
      assert((frame.channels()) > 0);
      data->image = reinterpret_cast<unsigned char *>(calloc(height*width,sizeof(uint8_t)*channels));

      data->image_count = frame.size().width * frame.size().height * frame.channels();
      assert(data->image_count > 0);


      if (!(data->image))
      {
        PLAYER_ERROR("Out of memory -  image");
        return -1;
      }
         

      data->width = frame.size().width;
      data->height = frame.size().height;
      data->bpp = frame.channels() * 8;
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
       Mat teste(data->height, data->width, CV_8UC3, data->image);

      assert(data->bpp > 0);
      data->compression = PLAYER_CAMERA_COMPRESS_RAW;

      this->Publish(device_addr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, reinterpret_cast<void *>(data), 0, NULL, false);

      t1 =cv::getTickCount();
  }
      return 0;
}


////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void Raspicam_driver::Main() 
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
      
      process_frame();

      // Sleep (you might, for example, block on a read() instead)
      usleep(100000); //IMPORTANTE: deve ser acrescentado um delay para a câmera
    }
}

////////////////////////////////////////////////////////////////////////////////
// Extra stuff for building a shared object.

/* need the extern to avoid C++ name-mangling  */
extern "C" {
  int player_driver_init(DriverTable* table)
  {
    puts("Example driver initializing  " );
    Raspicam_driver_Register(table);
    puts("Example driver done");
    return(0);
  }
}

