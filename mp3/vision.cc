#include "vision.hh"

/*
 * from mp2

void processImages(vector<Mat> images) {
    cout << images.size() << " images taken." << endl;
    cout << "Starting processImages" << endl;
    RobotIdentification test;
    for (int i = 0; i < images.size(); i++) {
        cout << "runIdentify on Image #" << i << endl;
        test.runIdentify(images[i]);
    }
    cout << "Ending processImages" << endl;
}

*/
pthread_mutex_t image_mutex = PTHREAD_MUTEX_INITIALIZER;

void robotImage(Create& robot, pthread_mutex_t *stream_mutex, vector<Mat>& images, bool& end) {
    bool usedWeapon = false;
    RobotIdentification test;
    while (!end) {
        this_thread::sleep_for(std::chrono::milliseconds(3000));
        if (images.size() > 0) {
            pthread_mutex_lock(&image_mutex);
            Mat image = images.pop_back();
            pthread_mutex_unlock(&image_mutex);
            if (test.runIdentify(image)) {
                cout << "found magic lamp!" << endl;
                if (!usedWeapon) {
                    pthread_mutex_lock(stream_mutex);
                    robot.sendLedCommand(Create::LED_PLAY, Create::LED_COLOR_RED, Create::LED_INTENSITY_FULL);
                    this_thread::sleep_for(chrono::milliseconds(2000));
                    robot.sendLedCommand(Create::LED_PLAY, 0, 0);
                    pthread_mutex_unlock(stream_mutex);
                }
            }
            cout << "Processing picture taken..." << endl;
        }
    }
}

void robotCamera(Create& robot, pthread_mutex_t *stream_mutex, vector<Mat>& images, bool& end) {
    raspicam::RaspiCam_Cv Camera;
    if (!Camera.open()) {
     cerr << "Error opening the camera" << endl;
     return;
    }
    cout << "Opened Camera" << endl;

    while (!end){
        this_thread::sleep_for(std::chrono::milliseconds(3000));
        Mat bgr_image;
        Camera.grab();
        Camera.retrieve(bgr_image);
        pthread_mutex_lock(&image_mutex);
        images.push_back(bgr_image);
        pthread_mutex_unlock(&image_mutex);
    }
}