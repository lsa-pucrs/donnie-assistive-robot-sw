#include <stdio.h>
#include <time.h>
#include <libplayerc++/playerc++.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
	using namespace PlayerCc;
    //default arguments
    std::string host = "localhost"; 
    int port = 6665;
  
    //Arguments treatment
    for(int i=1;i<argc;i++){      
    	if(argv[i][0]=='-'&argv[i][1]=='h'){
    	  host = argv[i+1];  
    	  i=i+1;
    	}
    	else if(argv[i][0]=='-'&argv[i][1]=='p'){
    	  port = atoi(argv[i+1]);  //convert to int
    	  i=i+1;
    	}
    	else {
    	  std::cout << std::endl << "./main [options]" << std::endl;
    	  std::cout << "Where [options] can be:" << std::endl;
    	  std::cout << "  -h <ip>        : host ip where Player is running.Default: localhost" << std::endl;
    	  std::cout << "  -p <port>      : port where Player will listen. Default: 6665" << std::endl << std::endl;
    	  return -1;
    	}
    }
	
	PlayerClient    c(host.c_str(),port);
	CameraProxy camera(&c,0);
	c.Read();

	int width = static_cast<int>(camera.GetWidth()), height = static_cast<int>(camera.GetHeight());
	int size = static_cast<int>(camera.GetImageSize());
	uint8_t* image;

	cout << "width: " << width << endl;
	cout << "height: " << height << endl;
	cout << "size: " << size << endl;
	image = reinterpret_cast<uint8_t *>(malloc(size*3));

    while(1){
    	camera.GetImage(image);
    	Mat frame(height, width, CV_8UC3, image);
		cout << "escrevendo arquivo..." << endl;
    	imwrite("teste.jpg", frame);
    	
    	namedWindow("Frame",WINDOW_AUTOSIZE);
        imshow( "Frame", frame );
        waitKey(100);
    	c.Read();
    }
}
