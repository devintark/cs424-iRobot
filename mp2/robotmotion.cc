#include <pthread.h>
#include "irobot-create.hh"
#include <chrono>
#include <thread>
#include <iosteam>



using namespace iRobot;

void robotMotion(Create& robot, pthread_mutex_t& robomutex){

    int speed = 200;
    robot.sendDriveCommand (speed, Create::DRIVE_STRAIGHT);
    if (robot.bumpLeft () || robot.bumpRight ()) {
            speed = 0;
            robot.sendDriveCommand(speed, Create::DRIVE_STRAIGHT);
            this_thread::sleep_for(chrono::milliseconds(15));
            cout << "Bump : " << count++ << endl;
            speed = -100;
            robot.sendDriveCommand(speed, Create::DRIVE_STRAIGHT);


    }




}

