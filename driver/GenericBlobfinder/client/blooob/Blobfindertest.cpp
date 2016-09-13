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

int main()
{
	using namespace PlayerCc;
	
	PlayerClient    c("localhost", 6666);
	
	BlobfinderProxy b(&c,0);
	
	c.Read();
	// Mostra a quantidade de pixels da câmera
	
	std::cout << "Width: " << b.GetWidth() << std::endl;
	std::cout << "Height: " << b.GetHeight() << std::endl;
	
	// Verifica e mostra a posição do centro do(s) blob(s), que estão na frente do robô, em relação a câmera e o número da cor
	
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
			std::cout <<"COR: "<< blob.color << std::endl;
			std::cout << "############################"<<std::endl;// " << i << "#########" << std::endl;			
		}
        
	}
}
	
