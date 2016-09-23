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


/* GenericBlobfinder configuration file sintaxe:

driver
(
	name "blobfinderraspicam"
	plugin "/opt/player-stage/src/player-3.0.2/build/examples/plugins/blobfinderraspicam/libblobfinderraspicam.so"
	#deifnes the minimum size (in pixels) for a blob to be detected. Default value is 1000
	min_blob_size 500
  	#reduce noise values can be 1 or 0. Default value is 0.
  	reduce_noise 1
  	#debug values can be 0,1,2,3,4 and 5. Default value is 0.
  	debug 1
	provides ["blobfinder:0"]
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

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace cv;
using namespace std;
////////////////////////////////////////////////////////////////////////////////
// The class for the driver
class GenericBlobfinder : public ThreadedDriver
{
  public:
    
    // Constructor; need that
    GenericBlobfinder(ConfigFile* cf, int section);

    // This method will be invoked on each incoming message
    //virtual int ProcessMessage(QueuePointer &resp_queue, 
    //                           player_msghdr * hdr,
    //                           void * data);

    virtual int ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr,void * data);
  private:

    // Main function for device thread.
    virtual void Main();
    virtual int MainSetup();
    virtual void MainQuit();
    int  publicar();
    void Debug_mode();
    void Filter_colors();
    void Reduce_noise();
    void Find_blobs();
    void Reset_blobs();



    player_devaddr_t blobfinder_addr; //o blobfinder a ser enviado.
    player_blobfinder_data_t data;  //data used to publish the blobfinder :)
    player_devaddr_t cam_addr;
    Device * r_cam_dev;
    //Device * r_blobfinder_dev;
    


   // Mat teste;
    //blobfinder variables.
    //Mat img1;
    Mat frame; 
    int debug;// = 5;
    //struct blob blob_buffer[10]; //limit of 10 blobs
    player_blobfinder_blob blob_buffer[10]; //limit of 10 blobs
    Mat R, G, B;
    int blobs;// = 0;
    int min_blob_size;
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

  findContours(RR, contoursR, hierarchyR, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
  findContours(GG, contoursG, hierarchyG, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
  findContours(BB, contoursB, hierarchyB, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

  //get X,Y and area of red blobs..
  for (int i = 0; i < contoursR.size(); i++)
  {
    //for (int index = 0; index >= 0; index = hierarchyR[index][0]) 
    //{

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
    //}
  }
  //get X,Y and area of green blobs..

  for (int i = 0; i < contoursG.size(); i++)
  {
    //for (int index = 0; index >= 0; index = hierarchyG[index][0])
   //{

      Moments moment = moments((cv::Mat)contoursG[i]);
      curret_blob.area = moment.m00;
      //filter small areas, to reduce even more noise
      if ((curret_blob.area > min_blob_size) && blobs<10)
      {
        curret_blob.x = (moment.m10 / curret_blob.area);
        curret_blob.y = (moment.m01 / curret_blob.area);
        curret_blob.color = 0x0000FF00; //0x00RRGGBB
        curret_blob.id = blobs;
        blob_buffer[blobs] = curret_blob;
        blobs++;
      }
    //}
  }

  //get X,Y and area of blue blobs..
  for (int i = 0; i < contoursB.size(); i++)
  {
    //for (int index = 0; index >= 0; index = hierarchyB[index][0])
    //{

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
    //}
  }
	//cout<<blobs<<endl;
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
  if(reduce_noise==1)
  {
  	cv::inRange(frame, cv::Scalar(109, 0, 87), cv::Scalar(256, 256, 256), R); //vermelho
  	cv::inRange(frame, cv::Scalar(21, 175, 47), cv::Scalar(56, 256, 184), G); //verde 
  	cv::inRange(frame, cv::Scalar(0, 32, 48), cv::Scalar(30, 256, 256), B); //azul 
  }
  else
  {
  	cv::inRange(frame, cv::Scalar(109, 0,63), cv::Scalar(256, 256, 256), R); //vermelho
  	cv::inRange(frame, cv::Scalar(21, 169, 17), cv::Scalar(64, 256, 169), G); //verde 
  	cv::inRange(frame, cv::Scalar(0, 28, 30), cv::Scalar(45, 237, 256), B); //azul 
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
	//cout<<frame.size().width<<"x"<<frame.size().height<<endl;
       // cout<<frame.size().width<<"x"<<frame.size().height<<endl; 
 
   namedWindow("Frame", 1);
   imshow("Frame", frame);
   // namedWindow("teste",4);
   // imshow("teste",teste);

   cv::waitKey(30);
 //   imwrite("Frame.png", frame);
  }
  else if (debug == 2)
  {
       // cout<<R.size().width<<"x"<<R.size().height<<endl;
       // cout<<teste.size().width<<"x"<<teste.size().height<<endl;

    namedWindow("Frame R", 1);
    imshow("Frame R", R);

 //  namedWindow("teste",4);
//    imshow("teste",teste);
    cv::waitKey(30);

    //imwrite("R.png", R);
  }
  else if (debug == 3)
  {
    //    cout<<G.size().width<<"x"<<G.size().height<<endl;
     //   cout<<teste.size().width<<"x"<<teste.size().height<<endl;
    namedWindow("Frame G", 1);
    imshow("Frame G", G);
  //  namedWindow("teste",4);
  //  imshow("teste",teste);

    cv::waitKey(30);

       // imwrite("G.png", G);

  }
  else if (debug == 4)
  {
        cout<<B.size().width<<"x"<<B.size().height<<endl;
    //    cout<<teste.size().width<<"x"<<teste.size().height<<endl;
    namedWindow("Frame B", 1);
    imshow("Frame B", B);
   // namedWindow("teste",4);
  //  imshow("teste",teste);

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
   // namedWindow("teste",4);
   // imshow("teste",teste);
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


  memset(&(this->blobfinder_addr), 0, sizeof blobfinder_addr); //zera o struct com a função de C "memset". 
                                                              //Sintaxe: memset(&espaço_de_memoria_a_modificar, valor_que_deseja_modificar, tamanho, em bytes)
  memset(&(this->cam_addr), 0, sizeof (player_devaddr_t));

  if (cf->ReadDeviceAddr(&(this->blobfinder_addr), section, "provides", //verifica se o blobfinder no arquivo de configuração está sendo utilizado de forma correta, provendo um blobfinder.
                           PLAYER_BLOBFINDER_CODE, -1, NULL))           //este código não só verifica o .cfg mas cria o struct do blobfinder. :) 
  {
    this->SetError(-1);
    return;
  }
  if (this->AddInterface(this->blobfinder_addr))
  {
    this->SetError(-1);
    return;
  }
  
  if(cf->ReadDeviceAddr(&(this->cam_addr), section, "requires", PLAYER_CAMERA_CODE, -1, NULL))
  {
    this->SetError(-1);
    return;
  }

  min_blob_size =  cf->ReadInt(section, "min_blob_size", 1000);
  reduce_noise = cf->ReadInt(section, "reduce_noise", 0);
  debug = cf->ReadInt(section, "debug",0 );

  blobs=0;
  // Read an option from the configuration file
  //this->foop = cf->ReadInt(section, "foo", 0);

  return;
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int GenericBlobfinder::MainSetup() //método inicializador! similar ao "setup"do arduino..
{   
  puts("Generic Blobfinder initializing! ");

  ///set up raspicam and configuration stuff :)


  // Here you do whatever is necessary to setup the device, like open and
  // configure a serial port.

  this->r_cam_dev = deviceTable->GetDevice(this->cam_addr);
  if (!(this->r_cam_dev)) return -1;
  if (this->r_cam_dev->Subscribe(this->InQueue))
  {
    this->r_cam_dev = NULL;
    return -1;
  }

 // printf("Was foo option given in config file? %d\n", this->foop);
    
  puts("Example driver ready");

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


int GenericBlobfinder::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr,void * data)
{
/*
  uint32_t idle_width, idle_height, u, v;
  player_blobfinder_data_t* data_teste;
    player_blobfinder_blob_t* actual_blob;
*/

    player_camera_data_t* cam_data;

  //PLAYER_WARN("New message received");
  if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, cam_addr))
  {
    //  PLAYER_WARN("requires cam received");
      cam_data = reinterpret_cast<player_camera_data_t *>(data); 
      //Mat img(cam_data->height, cam_data->width, CV_8UC3, cam_data->image);

/* --- Get the first two frames in the sequence */

        Mat(cam_data->height, cam_data->width, CV_8UC3, cam_data->image).copyTo(frame);
	      //  cout<<"frame:"<<frame.size().width<<"x"<<frame.size().height<<endl;
     
    //    frame.copyTo(teste);
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

