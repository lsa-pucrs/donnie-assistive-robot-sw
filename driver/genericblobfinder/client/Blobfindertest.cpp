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

// TODO: revisar comentarios e exemplo

/*Para testar esse código, segue um exemplo de .cfg e .world:

----- no .world------


define test_blobf blobfinder
(
	colors_count 2
	colors ["orange" "DarkBlue"]
	image [160 120] 
	range 5.00 		
	fov 60 
	)


define test position
(
		  
		  size [0.500 0.500 1.000]
		  
		  origin [0.000 0.000 0.000 0.000]

		  block
		  (
					 points 4
					 point[0] [1 0]
					 point[1] [1 1]
					 point[2] [0 1]
					 point[3] [0 0]
					 z [0 1]
		  )
		
		  test_blobf( )

)


test
(
		  name "test1"
		  pose [ -2.681 -4.101 0.000 -110.000]
		  color "green"
)
-----------------------
-------no .cfg---------

driver
( 
  name "stage"
  provides ["blobfinder:0"]
  model "test1" 
)

*/

#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <libplayerc++/playerc++.h>

int main(int argc, char** argv)
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
	
	BlobfinderProxy b(&c,0);
	
	c.Read();
	// Shows the resolution of the camera
	
	std::cout << "Width: " << b.GetWidth() << std::endl;
	std::cout << "Height: " << b.GetHeight() << std::endl;
	
	//Verifies and shows the position of the center of the blobs
	while(true)
	{
		c.Read();
        sleep(2);
        
        int bc = b.GetCount();
        
        std::cout << "No. Blobs: " << bc << std::endl;
        
        for(int i = 0; i < bc; i++)
        {
			std::cout << "blob " << i << "#########" << std::endl;
			playerc_blobfinder_blob_t blob;
			blob = b.GetBlob(i);
			
			std::cout <<"X: "<< blob.x << std::endl;
			std::cout <<"Y: "<< blob.y << std::endl;
			if(blob.color == 0x00FF0000) //red
			{
				std::cout <<"COLOR: RED" << std::endl;
			}
			else if(blob.color == 0x0000FF00)//green
			{
				std::cout <<"COLOR: GREEN"std::endl;
			}
			else if(blob.color == 0x000000FF)//blue
			{
				std::cout <<"COLOR: BUE"<< std::endl;
			}
			else
				std::cout <<"UNDEFINED COLOR: "<< blob.color << std::endl;
			
			std::cout << "############################"<<std::endl;			
		}
        
	}
}
	
