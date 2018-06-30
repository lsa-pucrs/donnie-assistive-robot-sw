#include "vo_headers.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace cv;

//! Frames path
char path[50];

//! Parameter for Shi-Tomasi detector
const int max_corners = 1800; 
//! Minimal number of features required for tracking
const int min_feats = 1200;
//! Step for reading. FPS = FPS/step
const int step = 1; 
//! Initial frame of the sequence
const int init_frame = 0; 

//! For plotting trajectory
const int x_init = 500; 
const int y_init = 300; 

//! Text parameters
char text[100];
int fontFace = FONT_HERSHEY_PLAIN;
double fontScale = 1;
int thickness = 1;
Point textOrg(10, 50);
double scale = 1.0;


int main(int argc, char **argv){

	srand (time(NULL)); //! Random seed for drawing feature points

	//! Camera intrinsic matrix K
    Mat K;
    //! Distortion coefficients
	Mat D;

//! Camera calibration ------------------------------------------------------------
	/*! -- PSEye - Chessboard --
	double data_K[3][3] = { {5.4031996828378419e+02, 0., 3.1950000000000000e+02},
							{0., 5.4031996828378419e+02, 2.3950000000000000e+02}, 
							{0., 0., 1.} };
	double data_D[5][1] = { {-1.7508379316048078e-01}, {4.8264503776547346e-01},
						  {0.}, {0.}, {-6.0315693613290489e-01} };
	double focal = 540.319;
	*/

	/*! -- PSEye - Asymmetric Circles --
	double data_K[3][3] = { {5.3954832498228882e+02, 0., 3.1950000000000000e+02},
						   {0., 5.3954832498228882e+02, 2.3950000000000000e+02},
						   {0., 0., 1.} };
	double data_D[5][1] = { {-1.5826448052188563e-01}, {6.4655315028476346e-01}, 
							{0.}, {0.}, {-1.7891107880201187e+00} };
	double focal = 539.548;
	*/

	/*! Raspicam - Chessboard */
	double data_K[3][3] = { {6.3849455462107289e+02, 0., 3.1950000000000000e+02},
						  {0., 6.3849455462107289e+02, 2.3950000000000000e+02},
						  {0., 0., 1.} };
	double data_D[5][1] = { {1.0669779490296029e-01}, {6.2682352210603698e-02}, 
						  {0.}, {0.}, {-2.1165296376497547e+00} };
	focal = 638.494;

	/*! -- Raspicam - Asymmetric Circles --
	double data_K[3][3] = { {6.1359945831083678e+02, 0., 3.1950000000000000e+02},
						  {0., 6.1359945831083678e+02, 2.3950000000000000e+02},
						  {0., 0., 1.} };
	double data_D[5][1] = { {1.9495202964438164e-02}, {7.4132544087216756e-01}, 
						  {0.}, {0.}, {-2.6828304576484476e+00} };
	focal = 613.599;
	*/
//! -------------------------------------------------------------------------------
	Mat(3, 3, CV_64F, data_K).copyTo(K); //! Camera intrinsic matrix K
  	Mat(5, 1, CV_64F, data_D).copyTo(D); //! Distortion coefficients
	
	int max_frames;
	//! Not enough arguments
	if(argc != 3){
		cout << "\tUsage: ./mvo_test <path_to_sequence_file> <max_frames>" << endl;
		return -1;
	}
	max_frames = atoi(argv[2]);

	//! Get path
	sprintf(path, "%s", argv[1]);	

	Size size(640,480);
  	Mat img1, img2;
	char imgpath[50];
	sprintf(imgpath, "%s/%06d.png", path, init_frame);
	img1 = imread(imgpath);
    resize(img1,img1,size);
	sprintf(imgpath, "%s/%06d.png", path, init_frame + step);
	img2 = imread(imgpath);
	resize(img2,img2,size);
	Mat img1_g, img2_g;
	cvtColor(img1, img1_g, COLOR_RGB2GRAY); //! Convert them to grayscale
	cvtColor(img2, img2_g, COLOR_RGB2GRAY);
	Mat img1_u, img2_u;
	undistort(img1_g, img1_u, K, D); //! Undistort them
	undistort(img2_g, img2_u, K, D);

	/*! --- Detect features using the Shi-Tomasi corner detector */
	vector<Point2f> points1, points2; 

	//! Detector parameters
	double qualityLevel = 0.01;
	double minDistance = 10;
	int blockSize = 3;


	//! Detect
	goodFeaturesToTrack(img1_u, points1, max_corners, qualityLevel, minDistance, Mat(), blockSize); 
	
	/*! ----- Track features to the next frame */
	vector <uchar> status;
	vector <float> err;

	calcOpticalFlowPyrLK(img1_u, img2_u, points1, points2, status, err); 
	
	//! Get back the good measurements
	vector<Point2f> points1_match, points2_match;
	for(unsigned int i = 0; i<status.size(); i++){
		if(status[i]){
			points1_match.push_back(points1[i]);
			points2_match.push_back(points2[i]);
		}
	}

	/*! ----- Find the Essential, Rotation and Translation matrices */
	Point2f pp(319.5, 239.5);
	Mat E, R, t, mask;
	E = findEssentialMat(points2_match, points1_match, focal, pp, RANSAC, 0.9, 1.0, mask);
	recoverPose(E, points2_match, points1_match, R, t, focal, pp, mask);
	Mat prevImg = img2_u.clone();
	Mat currImg, currImg_g, currImg_u;
	vector<Point2f> prevFeatures = points2_match;
	vector<Point2f> currFeatures;
	Mat R_f, t_f;
	R_f = R.clone();
	t_f = t.clone();
	Mat traj = Mat::zeros(600, 600, CV_8UC3);
	
	//! Main loop
	for(int i = init_frame + 2*step; i<max_frames; i = i+step){
		//usleep(50000);
		sprintf(imgpath, "%s/%06d.png", path, i);
		currImg = imread(imgpath); //! Read
		resize(currImg,currImg,size);
		
		cvtColor(currImg, currImg_g, COLOR_RGB2GRAY); //! Convert to grayscale
		undistort(currImg_g, currImg_u, K, D);	//! Undistort       

		//! Feature tracking
		calcOpticalFlowPyrLK(prevImg, currImg_u, prevFeatures, currFeatures, status, err);
		
		//! Get back the good measurements
		vector<Point2f> prevFeatures_match, currFeatures_match;
		for(unsigned int i = 0; i<status.size(); i++){
			if(status[i]){
				prevFeatures_match.push_back(prevFeatures[i]);
				currFeatures_match.push_back(currFeatures[i]);
			}
		}

		//! Essential matrix and pose
		E = findEssentialMat(currFeatures_match, prevFeatures_match, focal, pp, RANSAC, 0.999, 1.0, mask);
		recoverPose(E, currFeatures_match, prevFeatures_match, R, t, focal, pp, mask); //! R – Recovered relative rotation.
																						//! t – Recoverd relative translation.
		//! Increment over last position
		t_f = t_f + scale*(R_f*t);
		R_f = R*R_f;
		
		//! If tracked features move outside camera’s field of view
		if ((int)prevFeatures_match.size() < min_feats){
			cout << "Number of features: " << prevFeatures_match.size() << ", detecting..." << endl;
			//! Feature detection
			goodFeaturesToTrack(prevImg, prevFeatures, max_corners, qualityLevel, minDistance, Mat(), blockSize);
			calcOpticalFlowPyrLK(prevImg, currImg_u, prevFeatures, currFeatures, status, err);
		}

		prevImg = currImg_u.clone();
		prevFeatures = currFeatures;

		//! Draw features on image
        Mat view = currImg;
        for(unsigned int i = 0; i<currFeatures_match.size(); i++){
          circle(view, currFeatures_match[i], 1, Scalar(0, 0, 0), 2);
        }

        //! Draw trajectory
        int x = int(t_f.at<double>(0)) + x_init;
        int y = int(t_f.at<double>(2)) + y_init;
        circle(traj, Point(x, y) ,1, Scalar(0,0,255), 2);
        rectangle( traj, Point(10, 30), Point(550, 80), Scalar(0,0,0), -1);
        sprintf(text, "Coordinates: x = %02f, y = %02f, z = %02f", t_f.at<double>(0), t_f.at<double>(1), t_f.at<double>(2));
        putText(traj, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
       
		//! Show camera feed and trajectory
		imshow( "Camera feed", view );
		imshow( "Trajectory", traj );
		imwrite("trajectory.png", traj);
		//! Update the image windows
		waitKey(1);
	}

	waitKey(0);
	return 0;
}
