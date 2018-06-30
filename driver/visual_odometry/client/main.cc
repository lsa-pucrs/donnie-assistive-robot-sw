#include <stdio.h>
#include <time.h>
#include <libplayerc++/playerc++.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>

using namespace std;
using namespace cv;


int main()
{
	using namespace PlayerCc;
	
	PlayerClient    c("localhost", 6665);
	Position2dProxy p(&c,2);

	while(1){
		c.Read();
		usleep(1000);
		cout << "x: " << p.GetXPos() << endl;
		cout << "y: " << p.GetYPos() << endl;
		cout << "yaw: " << p.GetYaw() << endl;
	}
}


