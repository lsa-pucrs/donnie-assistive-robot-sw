/* This test client can be used to test Donnie both in 
 * simulation or with the real robot.
 * 
 * Simulation mode:
 *   $export DONNIE_IP="localhost"
 *   player donnie_sim.cfg &
 *   ./test-rangers
  * Robot mode:
 *   $export DONNIE_IP="use the robot ip address"
 *   player donnie.cfg &
 *   ./test-rangers
 *  
 * TODO:  colocar o cfg para robo real e simulacao
 * 
 */
#ifndef NDEBUG
#define NDEBUG
#endif

#include <libplayerc++/playerc++.h>
#include <iostream>
#include "utils.h"

int main(int argc, char *argv[]) {
    using namespace PlayerCc;
    using namespace std;

    string host = GetEnv("DONNIE_IP");
    int port = atoi(GetEnv("DONNIE_PORT").c_str());
    if (host.size() == 0) host = "localhost";
    if (port == 0) port = 6665;

    PlayerClient robot(host, port);
    RangerProxy bodySonarProxy(&robot, 0);
    RangerProxy headSonarProxy(&robot, 1);
    int i = 0;

    while (true) {
	robot.Read();
	cout << "Qnt body:" << bodySonarProxy.GetRangeCount() << endl;
	for (i = 0; i < bodySonarProxy.GetRangeCount(); i++) cout << bodySonarProxy[i] << " " << endl;
	cout << endl;
	cout << "Qnt head:" << headSonarProxy.GetRangeCount() << endl;
	for (i = 0; i < headSonarProxy.GetRangeCount(); i++) cout << headSonarProxy[i] << endl;
	cout << endl
	     << endl;
	sleep(2);
    }

    return 0;
}
