#include <pthread.h>
#include "irobot-create.hh"
#include <chrono>
#include <thread>
#include <iostream>
#include "robotmotion.hh"

using namespace iRobot;
using namespace std;

#define TRAVELSPEED 100
#define SLEEP 200 
#define TSLEEP(x) this_thread::sleep_for(chrono::milliseconds(x))
void findMax(Create&);

void robotMotion(Create& robot, pthread_mutex_t* robomutex, pthread_mutex_t* cam_mutex, bool& end){
    cout << "In Motion Thread" << endl;
    bool enteredMaze = false;

    int speed = TRAVELSPEED;

    pthread_mutex_lock(robomutex);

    while(!robot.playButton()){

        robot.sendDriveCommand (speed, Create::DRIVE_STRAIGHT);
        this_thread::sleep_for(chrono::milliseconds(20));
        if (robot.bumpLeft () || robot.bumpRight () ) {
            robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
            this_thread::sleep_for(chrono::milliseconds(15));

            enteredMaze = true;

            robot.sendDriveCommand(-200, Create::DRIVE_STRAIGHT);
            this_thread::sleep_for(chrono::milliseconds(15));
            robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
            this_thread::sleep_for(chrono::milliseconds(50));

            short maxWallSignal = 0;
            short wallSignal = -1;

            speed = 100;
            robot.sendDriveCommand(speed, Create::DRIVE_INPLACE_COUNTERCLOCKWISE);
            std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
            
            while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < 2200){
                wallSignal = robot.wallSignal();
                //cout << "Wall signal: " << wallSignal << endl;
                if (wallSignal >= maxWallSignal ){
                    maxWallSignal = wallSignal;

                }
                this_thread::sleep_for(chrono::milliseconds(15));
            }

            cout << "MAX WALL SIGNAL: " << maxWallSignal << endl;
            robot.sendDriveCommand(speed, Create::DRIVE_INPLACE_CLOCKWISE);


            while ((wallSignal = robot.wallSignal()) < (maxWallSignal - 30)){

                std::this_thread::sleep_for(chrono::milliseconds(15));
            }

            robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
            this_thread::sleep_for(chrono::milliseconds(50));
            robot.sendDriveCommand(speed, Create::DRIVE_INPLACE_COUNTERCLOCKWISE);
            this_thread::sleep_for(chrono::milliseconds(200));
            robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
            this_thread::sleep_for(chrono::milliseconds(50));
            speed = TRAVELSPEED;
            robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
            this_thread::sleep_for(chrono::milliseconds(50));
        }
 
        cout << "Continous wall sensor: " << robot.wallSignal() << endl;
        this_thread::sleep_for(chrono::milliseconds(15));
        short wallsig = robot.wallSignal();
        short prevWall = wallsig;
        while(true){
            robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
            short desiredWallSigLow = 80;
            short desiredWallSigHigh = 100;
            prevWall = wallsig;
            wallsig = robot.wallSignal();

            if (wallsig < 30){
                robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
                TSLEEP(200);
                cout << "WOAH: WAll sig: " << wallsig << endl;
                findMax(std::ref(robot));
                cout << "OKAY: Wall sig: " << robot.wallSignal() << endl;
                robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
                TSLEEP(50);
            } else {
                if (wallsig < desiredWallSigLow){
                short radius = -100 + wallsig / (float) desiredWallSigLow * -750;
                robot.sendDriveCommand(TRAVELSPEED, radius);
                prevWall = wallsig;
                cout << "Radius : " << radius << endl;
                TSLEEP(15);
                }
                if ((wallsig) > desiredWallSigHigh){
                    short radius = 1000 - (wallsig - desiredWallSigHigh)/ (float) desiredWallSigHigh * 750;
                    robot.sendDriveCommand(TRAVELSPEED, radius);
                    prevWall = wallsig;
                    cout << "Radius : " << radius << endl;
                    TSLEEP(15);

                }
            }

            
            // if (wallsig < 20){
            //     robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
            //     this_thread::sleep_for(chrono::milliseconds(50));

            //     short maxWallSignal = 0;
            //     short wallSignal = -1;

            //     speed = 100;
            //     robot.sendDriveCommand(speed, Create::DRIVE_INPLACE_COUNTERCLOCKWISE);
            //     std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
                
            //     while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < 2200){
            //         wallSignal = robot.wallSignal();
            //         //cout << "Wall signal: " << wallSignal << endl;
            //         if (wallSignal >= maxWallSignal ){
            //             maxWallSignal = wallSignal;

            //         }
            //         this_thread::sleep_for(chrono::milliseconds(15));
            //     }

            //     cout << "MAX WALL SIGNAL: " << maxWallSignal << endl;
            //     robot.sendDriveCommand(speed, Create::DRIVE_INPLACE_CLOCKWISE);


            //     while ((wallSignal = robot.wallSignal()) < (maxWallSignal - 30)){

            //         std::this_thread::sleep_for(chrono::milliseconds(15));
            //     }

            //     robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
            //     this_thread::sleep_for(chrono::milliseconds(50));
            //     robot.sendDriveCommand(speed, Create::DRIVE_INPLACE_COUNTERCLOCKWISE);
            //     this_thread::sleep_for(chrono::milliseconds(200));
            //     robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
            //     this_thread::sleep_for(chrono::milliseconds(50));
            //     speed = TRAVELSPEED;
            //     robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
            //     this_thread::sleep_for(chrono::milliseconds(50));
            // }
            

           


            cout << "Wall Sig: " << wallsig << endl;
            TSLEEP(500);

        }
/**
        if (enteredMaze && (robot.wallSignal() < 3) && numTurns == 0 && numAdjust < 2){
 
            robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_INPLACE_CLOCKWISE);
            this_thread::sleep_for(chrono::milliseconds(100));
            robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
            this_thread::sleep_for(chrono::milliseconds(15));
            robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
            this_thread::sleep_for(chrono::milliseconds(100));

        }
        this_thread::sleep_for(chrono::milliseconds(15));
        if (enteredMaze && (robot.wallSignal() > 90) ){
            robot.sendDriveCommand(200, Create::DRIVE_INPLACE_COUNTERCLOCKWISE);
            this_thread::sleep_for(chrono::milliseconds(50));
            robot.sendDriveCommand(SPEED, Create::DRIVE_STRAIGHT);
            this_thread::sleep_for(chrono::milliseconds(50));

        }

        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, -1999);
        cout << "Turn at -1999" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, 1999);
        cout << "Turn at 1999" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, -1000);
        cout << "Turn at -1000" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, 1000);
        cout << "Turn at 1000" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, -500);
        cout << "Turn at -500" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, 500);
        cout << "Turn at 500" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, -250);
        cout << "Turn at -250" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, 250);
        cout << "Turn at 250" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, -100);
        cout << "Turn at -100" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, 100);
        cout << "Turn at 100" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, -50);
        cout << "Turn at -50" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, 50);
        cout << "Turn at 50" << endl; 
        TSLEEP(2000);
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        */
        cout << "Wall Signal : " << robot.wallSignal() << endl;
        

        pthread_mutex_unlock(robomutex);

        //cout << "Running time motion: " << progTime << endl;
        this_thread::sleep_for(chrono::milliseconds(SLEEP)); //fixed
        pthread_mutex_lock(robomutex);

    }

    std::cout << "Play button pressed, stopping Robot" << endl;
    robot.sendDriveCommand (0, Create::DRIVE_STRAIGHT);
    end = true;
    pthread_mutex_unlock(robomutex);

}

void findMax(Create& robot){
    robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
        this_thread::sleep_for(chrono::milliseconds(50));

        short maxWallSignal = 0;
        short wallSignal = -1;

        short speed = 100;
        robot.sendDriveCommand(speed, Create::DRIVE_INPLACE_COUNTERCLOCKWISE);
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        
        while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < 2000){
            wallSignal = robot.wallSignal();
            //cout << "Wall signal: " << wallSignal << endl;
            if (wallSignal >= maxWallSignal ){
                maxWallSignal = wallSignal;

            }
            this_thread::sleep_for(chrono::milliseconds(15));
        }

        cout << "MAX WALL SIGNAL: " << maxWallSignal << endl;
        robot.sendDriveCommand(speed, Create::DRIVE_INPLACE_CLOCKWISE);


        while ((wallSignal = robot.wallSignal()) < (maxWallSignal)){

            std::this_thread::sleep_for(chrono::milliseconds(15));
        }

        robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
        this_thread::sleep_for(chrono::milliseconds(50));
        // robot.sendDriveCommand(speed, Create::DRIVE_INPLACE_COUNTERCLOCKWISE);
        // this_thread::sleep_for(chrono::milliseconds(200));
        // robot.sendDriveCommand(0, Create::DRIVE_STRAIGHT);
        // this_thread::sleep_for(chrono::milliseconds(50));
        speed = TRAVELSPEED;
        robot.sendDriveCommand(TRAVELSPEED, Create::DRIVE_STRAIGHT);
        this_thread::sleep_for(chrono::milliseconds(50));
}
