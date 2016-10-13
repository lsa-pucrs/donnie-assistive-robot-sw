#include <stdio.h>
#include <time.h>
#include <libplayerc++/playerc++.h>

#include <opencv2/core/core.hpp>
//#include <opencv2/imgcodecs.hpp>
//#include <opencv2/video/video.hpp>
//#include <opencv2/videoio/videoio.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/calib3d/calib3d.hpp>
//#include <opencv2/features2d/features2d.hpp>

using namespace std;
using namespace cv;

int main()
{
	using namespace PlayerCc;
	
	PlayerClient    c("localhost", 6665);
	CameraProxy camera(&c,0);
	//c.Read();
	
	int width = static_cast<int>(camera.GetWidth()), height = static_cast<int>(camera.GetHeight()), size = static_cast<int>(camera.GetImageSize());
	uint8_t* image;
	
	cout << "width: " << width << endl;
	cout << "height: " << height << endl;
	cout << "size: " << size << endl;

    while(1){
    	camera.GetImage(image);
    	Mat frame(height, width, CV_8UC3, image);
	cout << "escrevendo arquivo..." << endl;
    	imwrite("teste.png", frame);
    	//c.Read();
    }
}
