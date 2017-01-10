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
#include <libplayercore/playercore.h>

// #include <opencv2/core/core.hpp>
// #include <opencv2/imgcodecs.hpp>
#include <opencv2/video/video.hpp>
// #include <opencv2/videoio/videoio.hpp>
// #include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
// #include <opencv2/features2d/features2d.hpp>

// #include "vo_headers.h"

using namespace cv;
using namespace std;

//Global Variables
//const int max_corners = 100; //Parameter for Shi-Tomasi detector
//const int max_frames = 920; //Run for a specified number of frames

//Minimal number of features required for tracking
//const int min_feats = 80;
//const int step = 2; //FPS = FPS/step
//const int init_frame = 0; //Initial frame of the sequence
const int x_init = 300; //For plotting
const int y_init = 300; 

//Text parameters
char text[100];
int fontFace = FONT_HERSHEY_PLAIN;
double fontScale = 1;
int thickness = 1;
Point textOrg(10, 50);
double scale = 1.0;

////////////////////////////////////////////////////////////////////////////////
// The class for the driver
class VisualOdometry : public ThreadedDriver
{
  public:
    
    // Constructor; need that
    VisualOdometry(ConfigFile* cf, int section);

    // This method will be invoked on each incoming message
    virtual int ProcessMessage(QueuePointer &resp_queue, 
                               player_msghdr * hdr,
                               void * data);

  private:
  
	  player_devaddr_t odom_addr;
    player_position2d_data_t odom;
    player_devaddr_t cam_addr;
    Device * r_cam_dev;

		int cont, x, y, blockSize;
    Mat img1, img2, img1_g, img2_g, img1_u, img2_u;
    Mat K, D;
    vector <uchar> status;
    vector <float> err;
    Mat E, R, t, mask, prevImg;
    vector<Point2f> prevFeatures, currFeatures;
    Mat R_f, t_f, traj;
    Mat currImg, currImg_g, currImg_u;
    double focal, qualityLevel, minDistance;
    Point2f pp;

    int max_corners, min_feats;

    // Main function for device thread.
    virtual void Main();
    virtual int MainSetup();
    virtual void MainQuit();
};
// A factory creation function, declared outside of the class so that it
// can be invoked without any object context (alternatively, you can
// declare it static in the class).  In this function, we create and return
// (as a generic Driver*) a pointer to a new instance of this driver.
Driver* 
VisualOdometry_Init(ConfigFile* cf, int section)
{
  // Create and return a new instance of this driver
  return((Driver*)(new VisualOdometry(cf, section)));
}

// A driver registration function, again declared outside of the class so
// that it can be invoked without object context.  In this function, we add
// the driver into the given driver table, indicating which interface the
// driver can support and how to create a driver instance.
void VisualOdometry_Register(DriverTable* table)
{
  table->AddDriver("visualodometry", VisualOdometry_Init);
}

////////////////////////////////////////////////////////////////////////////////
// Constructor.  Retrieve options from the configuration file and do any
// pre-Setup() setup.
VisualOdometry::VisualOdometry(ConfigFile* cf, int section)
    : ThreadedDriver(cf, section, false, PLAYER_MSGQUEUE_DEFAULT_MAXLEN)
{
  memset(&(this->odom), 0, sizeof (player_position2d_data_t));
  memset(&(this->odom_addr), 0, sizeof (player_devaddr_t));
  memset(&(this->cam_addr), 0, sizeof (player_devaddr_t));

  if(cf->ReadDeviceAddr(&(this->odom_addr), section, "provides", PLAYER_POSITION2D_CODE, -1, NULL)){
    this->SetError(-1);
    return;
  }
  if(this->AddInterface(this->odom_addr)){
    this->SetError(-1);
    return;
  }

  if(cf->ReadDeviceAddr(&(this->cam_addr), section, "requires", PLAYER_CAMERA_CODE, -1, NULL)){
    this->SetError(-1);
    return;
  }
	
  cont = 0;

  /**** PSEye - Chessboard ****/  
  // double data_K[3][3] = { {5.4031996828378419e+02, 0., 3.1950000000000000e+02},
  //                           {0., 5.4031996828378419e+02, 2.3950000000000000e+02}, 
  //                           {0., 0., 1.} };
  // double data_D[5][1] = { {-1.7508379316048078e-01}, {4.8264503776547346e-01},
  //                         {0.}, {0.}, {-6.0315693613290489e-01} };
  // double focal = 540.319;

  /**** PSEye - Asymmetric Circles ****/  
  // double data_K[3][3] = { {5.3954832498228882e+02, 0., 3.1950000000000000e+02},
  //                          {0., 5.3954832498228882e+02, 2.3950000000000000e+02},
  //                          {0., 0., 1.} };
  //  double data_D[5][1] = { {-1.5826448052188563e-01}, {6.4655315028476346e-01}, 
  //                           {0.}, {0.}, {-1.7891107880201187e+00} };
  //  double focal = 539.548;

//------------------------------------------------------------------------------------

  /**** Raspicam - Chessboard ****/  
  double data_K[3][3] = { {6.3849455462107289e+02, 0., 3.1950000000000000e+02},
                          {0., 6.3849455462107289e+02, 2.3950000000000000e+02},
                          {0., 0., 1.} };
  double data_D[5][1] = { {1.0669779490296029e-01}, {6.2682352210603698e-02}, 
                          {0.}, {0.}, {-2.1165296376497547e+00} };
  focal = 638.494;

/**** Raspicam - Asymmetric Circles ****/
  // double data_K[3][3] = { {6.1359945831083678e+02, 0., 3.1950000000000000e+02},
  //                         {0., 6.1359945831083678e+02, 2.3950000000000000e+02},
  //                         {0., 0., 1.} };
  // double data_D[5][1] = { {1.9495202964438164e-02}, {7.4132544087216756e-01}, 
  //                         {0.}, {0.}, {-2.6828304576484476e+00} };
  // focal = 613.599;
  
  Mat(3, 3, CV_64F, data_K).copyTo(K); //Camera intrinsic matrix K
  Mat(5, 1, CV_64F, data_D).copyTo(D); //Distortion coefficients

  //Detector parameters
  qualityLevel = cf->ReadFloat(section, "qualityLevel", 0.01);
  minDistance = cf->ReadInt(section, "minDistance", 10);
  blockSize = cf->ReadInt(section, "blockSize", 3);

  max_corners = cf->ReadInt(section, "max_corners", 200);
  min_feats = cf->ReadInt(section, "min_feats", 100);

  //From intrinsical parameters matrix K
  pp.x = 319.5;
  pp.y = 239.5;

  return;
}

////////////////////////////////////////////////////////////////////////////////
// Set up the device.  Return 0 if things go well, and -1 otherwise.
int VisualOdometry::MainSetup()
{   
  puts("VisualOdometry driver initialising setup");

  // Here you do whatever is necessary to setup the device, like open and
  // configure a serial port.
  
  this->r_cam_dev = deviceTable->GetDevice(this->cam_addr);
  if (!(this->r_cam_dev)) return -1;
  if (this->r_cam_dev->Subscribe(this->InQueue))
  {
    this->r_cam_dev = NULL;
    return -1;
  }

  puts("VisualOdometry driver ready");

  return(0);
}


////////////////////////////////////////////////////////////////////////////////
// Shutdown the device
void VisualOdometry::MainQuit()
{
  puts("Shutting VisualOdometry driver down");

  // Here you would shut the device down by, for example, closing a
  // serial port.
  vector<Point2f>::iterator it;
  it = prevFeatures_match.begin();
  while(it != prevFeatures_match.end()){
    it = prevFeatures_match.erase(it);
  }
  it = currFeatures_match.begin();
  while(it != currFeatures_match.end()){
    it = currFeatures_match.erase(it);
  }
  currImg.release();
  currImg_g.release();
  currImg_u.release();
  imwrite("trajectory.png", traj); //save trajectory

  puts("VisualOdometry driver has been shutdown");
}

int VisualOdometry::ProcessMessage(QueuePointer & resp_queue, player_msghdr * hdr, void * data)
{
  player_camera_data_t* cam_data;

  PLAYER_WARN("New message received");

  if (Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA, PLAYER_CAMERA_DATA_STATE, cam_addr)){
      PLAYER_WARN("requires cam received");
      cam_data = reinterpret_cast<player_camera_data_t *>(data); 
      //cout << "cont " << cont << endl;

      if(cont==0){
/* --- Get the first frame */
        Mat(cam_data->height, cam_data->width, CV_8UC1, cam_data->image).copyTo(img1);
        undistort(img1, img1_u, K, D);//Undistort them
        cont++;
      }
      else if(cont==1){
/* --- Get the second frame in the sequence */
        Mat(cam_data->height, cam_data->width, CV_8UC1, cam_data->image).copyTo(img2);
        undistort(img2, img2_u, K, D);
        cont++;

/* --- Detect features using the Shi-Tomasi corner detector */
        vector<Point2f> points1, points2;
        goodFeaturesToTrack(img1_u, points1, max_corners, qualityLevel, minDistance, Mat(), blockSize);//, true);

/* ----- Track features to the next frame */
        calcOpticalFlowPyrLK(img1_u, img2_u, points1, points2, status, err);
        //Get back the good measurements
        vector<Point2f> points1_match, points2_match;
        for(unsigned int i = 0; i<status.size(); i++){
          if(status[i]){
            points1_match.push_back(points1[i]);
            points2_match.push_back(points2[i]);
          }
        }

/* ----- Find the Essential, Rotation and Translation matrices */
        E = findEssentialMat(points2_match, points1_match, focal, pp, RANSAC, 0.9, 1.0, mask);
        recoverPose(E, points2_match, points1_match, R, t, focal, pp, mask);
        prevImg = img2_u.clone();
        prevFeatures = points2_match;
        R_f = R.clone();
        t_f = t.clone();
        traj = Mat::zeros(600, 600, CV_8UC3);
      }
      else{
        Mat(cam_data->height, cam_data->width, CV_8UC1, cam_data->image).copyTo(currImg);
        undistort(currImg, currImg_u, K, D);
        cont++;

        //Feature tracking
        calcOpticalFlowPyrLK(prevImg, currImg_u, prevFeatures, currFeatures, status, err);

        //Get back the good measurements
        vector<Point2f> prevFeatures_match, currFeatures_match;
        for(unsigned int i = 0; i<status.size(); i++){
          if(status[i]){
            prevFeatures_match.push_back(prevFeatures[i]);
            currFeatures_match.push_back(currFeatures[i]);
          }
        }
        //Essential matrix and pose
        E = findEssentialMat(currFeatures_match, prevFeatures_match, focal, pp, RANSAC, 0.999, 1.0, mask); 
        recoverPose(E, currFeatures_match, prevFeatures_match, R, t, focal, pp, mask); 

        //Increment over last position
        t_f = t_f + scale*(R_f*t);
        R_f = R*R_f;
       
        //If tracked features move outside camera’s field of view
        if ((int)prevFeatures_match.size() < min_feats){
          cout << "Number of features: " << prevFeatures_match.size() << ", detecting..." << endl;
          //Feature detection
          goodFeaturesToTrack(prevImg, prevFeatures, max_corners, qualityLevel, minDistance, Mat(), blockSize);//, true);
          calcOpticalFlowPyrLK(prevImg, currImg_u, prevFeatures, currFeatures, status, err);
        }
        
        prevImg = currImg_u.clone();
        prevFeatures = currFeatures;
       
        //Draw features on image
        Mat view = currImg;
        for(unsigned int i = 0; i<currFeatures_match.size(); i++){
          circle(view, currFeatures_match[i], 1, Scalar(0, 0, 0), 2);
        }

        //Draw trajectory
        x = int(t_f.at<double>(0)) + x_init;
        y = int(t_f.at<double>(2)) + y_init;
        circle(traj, Point(x, y) ,1, Scalar(0,0,255), 2);
        rectangle( traj, Point(10, 30), Point(550, 80), Scalar(0,0,0), -1);
        sprintf(text, "Coordinates: x = %02f, y = %02f, z = %02f", t_f.at<double>(0), t_f.at<double>(1), t_f.at<double>(2));
        putText(traj, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
       
        // Show camera feed and trajectory
        imshow( "Camera feed", view );
        imshow( "Trajectory", traj );
        waitKey(1);
        odom.pos.px = t_f.at<double>(0);
        odom.pos.py = t_f.at<double>(1);
        odom.pos.pa = 0;
        Publish(odom_addr, PLAYER_MSGTYPE_DATA, PLAYER_POSITION2D_DATA_STATE, &odom, sizeof(odom), NULL);
       }
      
      return 0;
  }

  // Process messages here.  Send a response if necessary, using Publish().
  // If you handle the message successfully, return 0.  Otherwise,
  // return -1, and a NACK will be sent for you, if a response is required.
  return(0);
}


////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void VisualOdometry::Main() 
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
    puts("Visual odometry driver initializing");
    VisualOdometry_Register(table);
    puts("Visual odometry driver done ");
    return(0);
  }
}

