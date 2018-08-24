#include <vib_belt.h>

using namespace std;

int main(int argc, char** argv) {
    vib_belt* belt = NULL;

    // treatment for the arguments
    if (argc > 1)
        if (argv[1][0] == '-')
            switch (argv[1][1]) {
                case 'p':
                    belt = new vib_belt(argv[2]);
                    break;
                case 'e':
                    belt = new vib_belt(argv[2], argv[3], atoi(argv[4]));
                    break;
                default:
unknow_command:
                    cout << "The list of know parameters are: " << endl;
                    cout << "To run the belt on the default port </dev/ttyACM0>: ./vib_belt" << endl;
                    cout << "To run the belt on a user defined serial port: ./vib_belt -p <serial port>" << endl;
                    cout << "To run the belt on a user defined serial port, host and port: ./vib_belt -e <serial port> <hostname> <port>" << endl;
                    return 0;
            }
        else
            // jumps to the unknow_command label
            goto unknow_command;
    else
        // default vib_belt constructor
        belt = new vib_belt();

	if (belt == NULL)
		cout << "Mega bug !!! " << endl;

    // connect to the serial and check for errors in the connection
    if (!belt->connect()) {
        perror("Failed to connect.");
        return -1;
 
	}
        
    cout << "done connected" << endl;
    //cout << "Rangers: " << belt->myranger->GetRangeCount() << endl;
    //cout << "Ranger[0]: " << belt->myranger->GetRange(0)  << endl;
    //cout << "done reading 1" << endl;
    //belt->robot->Read();
    //cout << "Rangers: " << belt->myranger->GetRangeCount() << endl;
    //cout << "Ranger[0]: " << belt->myranger->GetRange(0)  << endl;
    //cout << "done reading 2" << endl;
	//cout << "?" << endl;
    // main loop for the belt
    while (1){
		usleep(100); //little delay
		cout << "Rangers - main: " << belt->myranger->GetRangeCount() << endl;
		belt->loop();
	}
    // close the serial communication
    belt->exit();

    return 0;
}
