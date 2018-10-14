#include <vib_belt.h>

using namespace std;

int main(int argc, char** argv) {
    vib_belt* belt = NULL;

    // treatment for the arguments
    if (argc > 1){
        if (argv[1][0] == '-'){
            switch (argv[1][1]) {
                case 'p':
                    belt = new vib_belt(argv[2], atoi(argv[3]));
                    break;
                default:
                    cout << "The list of know parameters are: " << endl;
                    cout << "To run the belt on a user defined serial port, host and port: ./vib_belt -p <hostname> <port>" << endl;
                    return 0;
            }
		}
    }else{
        // default vib_belt constructor
        belt = new vib_belt();
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
