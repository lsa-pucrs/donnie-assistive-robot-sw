/*
* Desc: Player-based blobfinder using OpenCV2.
* Author: Daniel Einloft
* Date:  August 2016
* Laboratório de Sistemas Autônomos 
*  - https://lsa.pucrs.br/
*  - https://github.com/lsa-pucrs
* Faculdade de Informática - PUCRS  
*  - www.inf.pucrs.br
*/


/* GenericBlobfinder configuration file syntax:

driver
(
	name "Raspicam_driver"
	plugin "libRaspicam_driver"
    fps 1
	height 480
	width  640
	provides ["6665:camera:0"]

)
driver
(
	name "GenericBlobfinder"
	plugin "libGenericBlobfinder"
	min_blob_size 500
	debug 0
	reduce_noise 1
	requires ["6665:camera:0"]
	provides ["6666:blobfinder:0"]
)


*/

// ONLY if you need something that was #define'd as a result of configure 
// (e.g., HAVE_CFMAKERAW), then #include <config.h>, like so:
/*
#include <config.h>
*/

// TODO: separar a definicao da classe em um .h para gerar documentacao automatica com doxygen
// TODO: remover todas as referencias a raspicam pois este modulo nao depende dela

#if !defined (WIN32)
  #include <unistd.h>
#endif
#include <string.h>

#include <string.h>

#include <iostream>
#include <libplayercore/playercore.h>


#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;
using namespace std;
////////////////////////////////////////////////////////////////////////////////
// The class for the driver GenericBlobfinder

//! A Generic Blobfinder, used to find blue, red and green blobs through a camera frame
/*! This class receives a frame from a camera and, through opencv, detects red, blue 
and green objects. Then, outputs a blobfinder and its position using the blobfinder 
struct defined by player.*/
class GenericBlobfinder : public ThreadedDriver
{
  public:
    
	//! Constructor.
	//! Runs when player server was launched.
    GenericBlobfinder(ConfigFile* cf, int section);

	//! This method will be invoked on each incoming message.
    virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr,void * data);
  private:

	//! Main function for device thread.
    virtual void Main();
    //! Runs when a client connects to this driver.
    virtual int MainSetup();
    //! Runs when a client disconnects from this driver. 
    virtual void MainQuit();

    //! Used to publish the blobs detected.
    int  publicar();
    //! Used for debugging,controlled by the variable debug.
    void Debug_mode();
    //! Filter the colors from the original freme, generating images with the three colors for detection (Red, Green and Blue).
    void Filter_colors();
 	//! Used to reduce the noise from the image. The performance will be affected if it's used.
    void Reduce_noise();
    //! Used to detect and find the shapes of the blobs after the filtering.
    void Find_blobs();
    //! Reset all the blobs.
    void Reset_blobs();


    //! Blobfinder interface.
    player_devaddr_t blobfinder_addr; 
    //! Camera interface.
    player_devaddr_t cam_addr; 

    //! Blobfinder data.
    player_blobfinder_data_t data;  
    //! Camera device.
    Device * r_cam_dev;			
    
    //! Frame sent by the camera and analysed for blob detection.
    Mat frame; 
    //! Debug variable to control the Debug_Mode() method. 
    /*! Debug variable can be:
		1 - show original frame
		2 - show thresholded image for red blobs.
		3 - show thresholded image for green blobs.
		4- show thresholded image for blue blobs.
		5- show blobs found, with thresholded image for red blobs.
	*/
    int debug;
    //! Vector of blobfinders, limited to 10 blobs.Can be used on the configuration file. Default value is 0.
    player_blobfinder_blob blob_buffer[10]; 
    //! Matrixes for storing the filtered frame by the colors Red, Green and Blue.
    Mat R, G, B;
    //! Number of blobs detected.
    int blobs;
    //! Minimum size of the blobs. Can be used on the configuration file. Default value is 1000.
    int min_blob_size;
    //! Used as a flag to control if the method Reduce_noise() is used or not. If its value is 1, the program will use the method.Can be used on the configuration file. Default value is 0.
    int reduce_noise;
};

// A factory creation function, declared outside of the class so that it
// can be invoked without any object context (alternatively, you can
// declare it static in the class).  In this function, we create and return
// (as a generic Driver*) a pointer to a new instance of this driver.
Driver* GenericBlobfinder_Init(ConfigFile* cf, int section)
{
  // Create and return a new instance of this driver
  return((Driver*)(new GenericBlobfinder(cf, section)));
}

// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void GenericBlobfinder_Register(DriverTable* table)
{
  table->AddDriver("GenericBlobfinder", GenericBlobfinder_Init);
}



//reset valor to avoid errors.
void GenericBlobfinder::Reset_blobs()
{
  memset(blob_buffer, 0, sizeof(blob_buffer));
  blobs = 0;
}
//Find blobs. Uses functions like contours and moments from opencv
void GenericBlobfinder::Find_blobs()
{
  vector< vector<Point> > contoursR, contoursG, contoursB;
  vector<Vec4i> hierarchyR,hierarchyG, hierarchyB;
  player_blobfinder_blob curret_blob;

  //find contours of each blob
  Mat RR, GG, BB;
  R.copyTo(RR);
  G.copyTo(GG);
  B.copyTo(BB);

  //find the contours
  findContours(RR, contoursR, hierarchyR, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
  findContours(GG, contoursG, hierarchyG, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
  findContours(BB, contoursB, hierarchyB, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

  //get X,Y and area of red blobs..
  for (int i = 0; i < contoursR.size(); i++)
  {
      Moments moment = moments((cv::Mat)contoursR[i]);
      curret_blob.area = moment.m00;
      //filter small areas, to reduce even more noise
      if ((curret_blob.area > min_blob_size) && blobs<10 )
      {
        curret_blob.x = (moment.m10 / curret_blob.area);
        curret_blob.y = (moment.m01 / curret_blob.area);
        curret_blob.color = 0x00FF0000; //0x00RRGGBB
        curret_blob.id = blobs;
        blob_buffer[blobs] = curret_blob;
        blobs++;
      }
  }
  //get X,Y and area of green blobs..
  for (int i = 0; i < contoursG.size(); i++)
  {
      Moments moment = moments((cv::Mat)contoursG[i]);
      curret_blob.area = moment.m00;
      //filter small areas, to reduce even more noise
      if ((curret_blob.area > min_blob_size) && blobs<10)
      {
        curret_blob.x = (moment.m10 / curret_blob.area);
        curret_blob.y = (moment.m01 / curret_blob.area);
        curret_blob.color = 0x0000FF00; 
        curret_blob.id = blobs;
        blob_buffer[blobs] = curret_blob;
        blobs++;
      }
  }
  //get X,Y and area of blue blobs..
  for (int i = 0; i < contoursB.size(); i++)
  {
      Moments moment = moments((cv::Mat)contoursB[i]);
      curret_blob.area = moment.m00;
      //filter small areas, to reduce even more noise
      if ((curret_blob.area > min_blob_size) && blobs<10)
      {
        curret_blob.x = (moment.m10 / curret_blob.area);
        curret_blob.y = (moment.m01 / curret_blob.area);
        curret_blob.color = 0x000000FF; //0x00RRGGBB
        curret_blob.id = blobs;
        blob_buffer[blobs] = curret_blob;
        blobs++;
      }
  }
}

//reduce noise using erode and dilate functions
void GenericBlobfinder::Reduce_noise()
{
  Mat erodeElement = getStructuringElement(MORPH_RECT, Size(2,2));
  Mat dilateElement = getStructuringElement(MORPH_RECT, Size(3,3));

  erode(R, R, erodeElement); 
  dilate(R, R, dilateElement);
  erode(G, G, erodeElement);
  dilate(G, G, dilateElement);
  erode(B, B, erodeElement);
  dilate(B, B, dilateElement);
}

//threshold image for red, green and blue blobs.
void GenericBlobfinder::Filter_colors()
{
  cvtColor(frame, frame, COLOR_RGB2HSV);
  //filter colors using the noise reduction. It's better, but slower.
  if(reduce_noise==1)
  {
  	cv::inRange(frame, cv::Scalar(109, 0, 87), cv::Scalar(256, 256, 256), R); //red
  	cv::inRange(frame, cv::Scalar(21, 175, 47), cv::Scalar(56, 256, 184), G); //green 
  	cv::inRange(frame, cv::Scalar(0, 32, 48), cv::Scalar(30, 256, 256), B);  //blue 
  }
  else //filter colors withouth noise reduction. 
  {
  	cv::inRange(frame, cv::Scalar(109, 0,63), cv::Scalar(256, 256, 256), R); //red
  	cv::inRange(frame, cv::Scalar(21, 169, 17), cv::Scalar(64, 256, 169), G); //green 
  	cv::inRange(frame, cv::Scalar(0, 28, 30), cv::Scalar(45, 237, 256), B); //blue 
  }


}
/*debug function. "debug variable can be:
1 - show original frame
2 - show thresholded image for red blobs
3 - show thresholded image for green blobs
4- show thresholded image for blue blobs
5- show blobs found, with thresholded image for red blobs
*/
void GenericBlobfinder::Debug_mode()
{
  if (debug == 1)
  {
   namedWindow("Frame", 1);
   imshow("Frame", frame);
   cv::waitKey(30);
 //   imwrite("Frame.png", frame);
  }
  else if (debug == 2)
  {
    namedWindow("Frame R", 1);
    imshow("Frame R", R);
    cv::waitKey(30);
    //imwrite("R.png", R);
  }
  else if (debug == 3)
  {
    namedWindow("Frame G", 1);
    imshow("Frame G", G);
    cv::waitKey(30);
    // imwrite("G.png", G);
  }
  else if (debug == 4)
  {
    namedWindow("Frame B", 1);
    imshow("Frame B", B);
    cv::waitKey(30);
     // imwrite("B.png", B);
  }
  else if (debug == 5)
  {
    for (int i=0; i < blobs; i++)
      cout << i << ":" << blob_buffer[i].color << "->area:" << blob_buffer[i].area << "/x:" << blob_buffer[i].x << "/y:" << blob_buffer[i].y << endl;
    
    //imwrite("R.png", R);
    //imwrite("G.png", G);
    //imwrite("B.png", B);
    namedWindow("FrameR", 1);
    imshow("FrameR", R);
    namedWindow("FrameG", 2);
    imshow("FrameG", G);
    namedWindow("FrameB", 3);
    imshow("FrameB", B);
    cv::waitKey(30);

  }
}

////////////////////////////////////////////////////////////////////////////////
// Constructor.  Retrieve options from the configuration file and do any
// pre-Setup() setup.
GenericBlobfinder::GenericBlobfinder(ConfigFile* cf, int section): ThreadedDriver(cf, section, false, PLAYER_MSGQUEUE_DEFAULT_MAXLEN)
{


  memset(&(this->blobfinder_addr), 0, sizeof blobfinder_addr);                                                            
  memset(&(this->cam_addr), 0, sizeof (player_devaddr_t));

  if (cf->ReadDeviceAddr(&(this->blobfinder_addr), section, "provides", //check if the blobfinder in the .cfg file is being used correctly, providing a blobfinder.
                           PLAYER_BLOBFINDER_CODE, -1, NULL))           
  {
    this->SetError(-1);
    return;
  }
  if (this->AddInterface(this->blobfinder_addr))
  {
    this->SetError(-1);
    return;
  }
  
  if(cf->ReadDeviceAddr(&(this->cam_addr), section, "requires", PLAYER_CAMERA_CODE, -1, NULL))//check if the camera in the .cfg file is being used correctly, requiring a camera.
  {
    this->SetError(-1);
    return;
  }

  min_blob_size =  cf->ReadInt(section, "min_blob_size", 1000);
  reduce_noise = cf->ReadInt(section, "reduce_noise", 0);
  debug = cf->ReadInt(section, "debug",0 );
  blobs=0;
  return;
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int GenericBlobfinder::MainSetup() //setup method
{   
  puts("Generic Blobfinder initializing! ");

  ///set up camera and configuration stuff 

  // Here you do whatever is necessary to setup the device, like open and
  // configure a serial port.
  this->r_cam_dev = deviceTable->GetDevice(this->cam_addr);
  if (!(this->r_cam_dev)) return -1;
  if (this->r_cam_dev->Subscribe(this->InQueue))
  {
    this->r_cam_dev = NULL;
    return -1;
  }    
  puts("GenericBlobfinder driver ready");

  return(0);
}

////////////////////////////////////////////////////////////////////////////////
// Shutdown the device
void GenericBlobfinder::MainQuit() // este método serve para fechar tudo que ficou aberto. fechar a raspicam e o opencv.
{
  puts("Shutting example driver down");
  // Here you would shut the device down by, for example, closing a
  // serial port.
  puts("Example driver has been shutdown");
}

//this method will process 
int GenericBlobfinder::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr,void * data)
{
    player_camera_data_t* cam_data;
  //PLAYER_WARN("New message received");
  if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, cam_addr))
  {
    //  PLAYER_WARN("requires cam received");
      cam_data = reinterpret_cast<player_camera_data_t *>(data); 

      Mat(cam_data->height, cam_data->width, CV_8UC3, cam_data->image).copyTo(frame);
        Filter_colors();
        if(reduce_noise==1)
        	Reduce_noise();
        //Debug_mode();
        Find_blobs();
        Debug_mode();
        publicar();
        Reset_blobs();
  }
    return(0);
}

int GenericBlobfinder::publicar()
{


  data.blobs = (player_blobfinder_blob_t*)calloc(blobs,sizeof(data.blobs[0]));
  data.blobs_count = blobs;
  data.width  = frame.size().width;
  data.height = frame.size().height;

  for (int i = 0; i < blobs; i++)
  {
    // Set the data to pass back
    data.blobs[i].id = blob_buffer[i].id;  
    data.blobs[i].color = blob_buffer[i].color; 
    data.blobs[i].area = blob_buffer[i].area;
    data.blobs[i].x = blob_buffer[i].x;
    data.blobs[i].y = blob_buffer[i].y;
  }

  Publish(blobfinder_addr,PLAYER_MSGTYPE_DATA,PLAYER_BLOBFINDER_DATA_BLOBS,&data);

  if (data.blobs) 
    free(data.blobs);
  data.blobs = NULL;
  data.blobs_count = 0;

  // Process messages here.  Send a response if necessary, using Publish().
  // If you handle the message successfully, return 0.  Otherwise,
  // return -1, and a NACK will be sent for you, if a response is required.
  return(0);
}

////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void GenericBlobfinder::Main() 
{
  // The main loop; interact with the device here
  //puts("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  for(;;)
  {
    // test if we are supposed to cancel
    pthread_testcancel();

    // Process incoming messages.  ExampleDriver::ProcessMessage() is
    // called on each message.
    ProcessMessages();
   // publicar();
    // Interact with the device, and push out the resulting data, using
    // Driver::Publish()

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
    puts("Generic Blobfinder driver initializing  " );
    GenericBlobfinder_Register(table);
    puts("Generic Blobfinder driver done");
    return(0);
  }
}

