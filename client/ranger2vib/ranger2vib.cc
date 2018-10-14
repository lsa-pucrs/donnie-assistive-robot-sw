#include <ranger2vib.h>

using namespace std;

int main(int argc, char** argv) {
    ranger2vib* belt = NULL;

    // treatment for the arguments
    if (argc > 1){
        if (argv[1][0] == '-'){
            switch (argv[1][1]) {
                case 'p':
                    belt = new ranger2vib(argv[2], atoi(argv[3]));
                    break;
                default:
                    cout << "The list of know parameters are: " << endl;
                    cout << "To run the belt on a user defined serial port, host and port: ./ranger2vib -p <hostname> <port>" << endl;
                    return 0;
            }
		}
    }else{
        // default ranger2vib constructor
        belt = new ranger2vib();
	}
	
	if (belt == NULL){
        perror("Failed to connect.");
        return -1;

	}

    cout << "done connected" << endl;

    while (1){
		usleep(500); //controls the update frequency
		belt->loop();
	}

    return 0;
}
