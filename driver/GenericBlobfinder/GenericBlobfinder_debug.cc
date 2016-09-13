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
    


    Mat teste;
    //blobfinder variables.
    //Mat img1;
    Mat frame; 
    int debug;// = 5;
    //struct blob blob_buffer[10]; //limit of 10 blobs
    player_blobfinder_blob blob_buffer[10]; //limit of 10 blobs
    Mat R, G, B;
    int blobs;// = 0;
    int foop;
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
  cout<<"Deletando blobs..."<<endl;
  memset(blob_buffer, 0, sizeof(blob_buffer));
  for(int i=0;i< blobs;i++)
      cout<<blob_buffer[i].id<<" ";
  cout<<endl;
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


  cout<<"Contornos R:"<<contoursR.size()<<endl;
  cout<<"Contornos G:"<<contoursG.size()<<endl;
  cout<<"Contornos B:"<<contoursB.size()<<endl;


  //get X,Y and area of red blobs..
  for (int i = 0; i < contoursR.size(); i++)
  {
    for (int index = 0; index >= 0; index = hierarchyR[index][0]) 
    {

      Moments momentR = moments((cv::Mat)contoursR[index]);
      curret_blob.area = momentR.m00;
      //filter small areas, to reduce even more noise
      if ((curret_blob.area > 1000) && blobs<10 )
      {
        curret_blob.x = (momentR.m10 / curret_blob.area);
        curret_blob.y = (momentR.m01 / curret_blob.area);
        curret_blob.color = 0x00FF0000; //0x00RRGGBB
        curret_blob.id = blobs;
        blob_buffer[blobs] = curret_blob;
        blobs++;
      }
    }
  }
  //get X,Y and area of green blobs..

  for (int i = 0; i < contoursG.size(); i++)
  {
    for (int index = 0; index >= 0; index = hierarchyG[index][0])
    {

      Moments momentG = moments((cv::Mat)contoursG[index]);
      curret_blob.area = momentG.m00;
      //filter small areas, to reduce even more noise
      if ((curret_blob.area > 1000) && blobs<10)
      {
        curret_blob.x = (momentG.m10 / curret_blob.area);
        curret_blob.y = (momentG.m01 / curret_blob.area);
        curret_blob.color = 0x0000FF00; //0x00RRGGBB
        curret_blob.id = blobs;
        blob_buffer[blobs] = curret_blob;
        blobs++;
      }
    }
  }

  //get X,Y and area of blue blobs..
  for (int i = 0; i < contoursB.size(); i++)
  {
    for (int index = 0; index >= 0; index = hierarchyB[index][0])
    {

      Moments momentB = moments((cv::Mat)contoursB[index]);
      curret_blob.area = momentB.m00;
      //filter small areas, to reduce even more noise
      if ((curret_blob.area > 1000) && blobs<10)
      {
        curret_blob.x = (momentB.m10 / curret_blob.area);
        curret_blob.y = (momentB.m01 / curret_blob.area);
        curret_blob.color = 0x000000FF; //0x00RRGGBB
        curret_blob.id = blobs;
        blob_buffer[blobs] = curret_blob;
        blobs++;
      }
    }
  }
	cout<<"numero de blobs encontradas:"<<blobs<<endl;
}
//reduce noise using erode and dilate functions
void GenericBlobfinder::Reduce_noise()
{
  Mat erodeElement = getStructuringElement(MORPH_RECT, Size(8, 8));
  Mat dilateElement = getStructuringElement(MORPH_RECT, Size(16,16));

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
  cv::inRange(frame, cv::Scalar(117, 0, 87), cv::Scalar(256, 256, 256), R); //vermelho
  cv::inRange(frame, cv::Scalar(21, 175, 47), cv::Scalar(56, 256, 184), G); //verde 
  cv::inRange(frame, cv::Scalar(0, 32, 48), cv::Scalar(30, 256, 256), B); //azul 
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
    namedWindow("teste",4);
    imshow("teste",teste);

   cv::waitKey(30);
 //   imwrite("Frame.png", frame);
  }
  else if (debug == 2)
  {
    namedWindow("Frame", 1);
    imshow("Frame", R);
    cv::waitKey(30);
    //imwrite("R.png", R);
  }
  else if (debug == 3)
  {
    namedWindow("Frame", 1);
    imshow("Frame", G);
    namedWindow("teste",4);
    imshow("teste",teste);

    cv::waitKey(30);

       // imwrite("G.png", G);

  }
  else if (debug == 4)
  {
    namedWindow("Frame", 1);
    imshow("Frame", B);
    namedWindow("teste",4);
    imshow("teste",teste);

    cv::waitKey(30);
       // imwrite("B.png", B);

  }
  else if (debug == 5)
  {
    //for (int i=0; i < blobs; i++)
    //  cout << i << ":" << blob_buffer[i].color << "->area:" << blob_buffer[i].area << "/x:" << blob_buffer[i].x << "/y:" << blob_buffer[i].y << endl;
    
    //imwrite("R.png", R);
    //imwrite("G.png", G);
    //imwrite("B.png", B);
    namedWindow("teste",4);
    imshow("teste",teste);
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
  blobs=0;
  debug=5;
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

  printf("Was foo option given in config file? %d\n", this->foop);
    
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





//int BlobFinderRASPICAM::ProcessMessage(QueuePointer & resp_queue, //este método é responsável por enviar o blobfindeer ( no nosso caso) para outro modulo do player.
 //                                 player_msghdr * hdr,            //lembra do "provides" do .cfg? A partir da função "publish" a gente provê o blobfinder            
 //                                 void * data)
//int BlobFinderRASPICAM::ProcessMessage()
//{
//  player_blobfinder_data_t data;
//  //teste #1: criação d eum blobfinder generico e ver como o programa reage.
// int i;
//  data.width = 4;
//  data.height = 6;
//  data.blobs_count =2;
//  data.blobs = (player_blobfinder_blob_t*)calloc(2,sizeof(data.blobs[0]));
// for (i = 0; i < 2; i++)
//  {
//    // Set the data to pass back
//    data.blobs[i].id = i;  // TODO
//    data.blobs[i].color = i+20;  // TODO
//    data.blobs[i].area = 4*6;
//    data.blobs[i].x = 3;
//    data.blobs[i].y = 5;
//    data.blobs[i].left = 1;
//    data.blobs[i].top = 4;
//    data.blobs[i].right = 3;
//    data.blobs[i].bottom = 4;
//    data.blobs[i].range = 0;
//  }
//  Publish(blobfinder_addr,PLAYER_MSGTYPE_DATA,PLAYER_BLOBFINDER_DATA_BLOBS,&data);
//  if (data.blobs) free(data.blobs);
//  data.blobs = NULL;
//  data.blobs_count = 0;
//
//    
//
//
//  // Process messages here.  Send a response if necessary, using Publish().
//  // If you handle the message successfully, return 0.  Otherwise,
//  // return -1, and a NACK will be sent for you, if a response is required.
//  return(0);
//}




int GenericBlobfinder::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr,void * data)
{
/*
  uint32_t idle_width, idle_height, u, v;
  player_blobfinder_data_t* data_teste;
    player_blobfinder_blob_t* actual_blob;
*/

    player_camera_data_t* cam_data;

  PLAYER_WARN("New message received");
  if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, cam_addr))
  {
      PLAYER_WARN("requires cam received");
      cam_data = reinterpret_cast<player_camera_data_t *>(data); 
      //Mat img(cam_data->height, cam_data->width, CV_8UC3, cam_data->image);

/* --- Get the first two frames in the sequence */

        Mat(cam_data->height, cam_data->width, CV_8UC3, cam_data->image).copyTo(frame);
        frame.copyTo(teste);
        Filter_colors();
        Reduce_noise();
        Find_blobs();
        Debug_mode();
        publicar();
        Reset_blobs();
        cout<<"final looop"<<endl;
        //imwrite("oeee.png",frame);
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
    data.blobs[i].id = blob_buffer[i].id;  // TODO
    data.blobs[i].color = blob_buffer[i].color;  // TODO
    data.blobs[i].area = blob_buffer[i].area;
    data.blobs[i].x = blob_buffer[i].x;
    data.blobs[i].y = blob_buffer[i].y;
    //data.blobs[i].left = 1;
    //data.blobs[i].top = 4;
    //data.blobs[i].right = 3;
    //data.blobs[i].bottom = 4;
    //data.blobs[i].range = 0;
  }

  Publish(blobfinder_addr,PLAYER_MSGTYPE_DATA,PLAYER_BLOBFINDER_DATA_BLOBS,&data);

  if (data.blobs) 
    free(data.blobs);
  data.blobs = NULL;
  data.blobs_count = 0;




///////////OLD v /////////////////////
  //teste #1: criação d eum blobfinder generico e ver como o programa reage.
  //int i;
  //data.width = 4;
  //data.height = 6;
  //data.blobs_count =2;
  //data.blobs = (player_blobfinder_blob_t*)calloc(2,sizeof(data.blobs[0]));
  //for (i = 0; i < 2; i++)
  //{
  //  // Set the data to pass back
  //  data.blobs[i].id = i;  // TODO
  //  data.blobs[i].color = i+20;  // TODO
  //  data.blobs[i].area = 4*6;
  //  data.blobs[i].x = 3*i;
  //  data.blobs[i].y = 5*i;
  //  data.blobs[i].left = 1;
  //  data.blobs[i].top = 4;
  //  data.blobs[i].right = 3;
  //  data.blobs[i].bottom = 4;
  //  data.blobs[i].range = 0;
  //}
  //Publish(blobfinder_addr,PLAYER_MSGTYPE_DATA,PLAYER_BLOBFINDER_DATA_BLOBS,&data);
  //if (data.blobs) free(data.blobs);
  //data.blobs = NULL;
  //data.blobs_count = 0;
//
    


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

