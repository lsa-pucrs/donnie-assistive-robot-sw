#include <stdio.h>
#include <time.h>
#include <libplayerc++/playerc++.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main()
{
	using namespace PlayerCc;
	PlayerClient    c("localhost", 6665);
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
    	
    	//namedWindow("Frame",WINDOW_AUTOSIZE);
        //imshow( "Frame", frame );
        //waitKey(0);
    	c.Read();
    }
}
