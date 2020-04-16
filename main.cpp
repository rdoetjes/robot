#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stddef.h>
#include <wiringPi.h>
#include "motor.h"
#include "rangesensor.h"
#include <iostream>
#include <iomanip>
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>

#define D_STOP 0
#define D_FORWARD 1
#define D_REVERSE 2
#define D_HLEFT 3
#define D_HRIGHT 4
#define D_LEFT 5
#define D_RIGHT 6
#define CRUISE_SPEED 1000
int oldDir = 0;

using namespace cv;
using namespace std;

VideoCapture cap;

//convenience structure, used by drive function
struct driveMotors{
  Motor *mr;
  Motor *ml;
  int direction;
  int pwm1;
  int pwm2;
  int msContinuous;
  unsigned int micros_next_move;
};

unsigned int iteration;

/******************************************************************************
 * Checks to see if a directory exists. Note: This method only checks the
 * existence of the full path AND if path leaf is a dir.
 *
 * @return  >0 if dir exists AND is a dir,
 *           0 if dir does not exist OR exists but not a dir,
 *          <0 if an error occurred (errno is also set)
 *****************************************************************************/
int dirExists(const char* const path)
{
    struct stat info;

    int statRC = stat( path, &info );
    if( statRC != 0 )
    {
        if (errno == ENOENT)  { return 0; } // something along the path does not exist
        if (errno == ENOTDIR) { return 0; } // something in path prefix is not a dir
        return -1;
    }

    return ( info.st_mode & S_IFDIR ) ? 1 : 0;
}

/*
We want to measure distance, only every so often, this global variable
allows us to track when to measure again, without blocking the process
*/
unsigned int micros_next_measure = 0;

/*
drive method takes a driveMotors pointer.
This pointer contains the instructions for the motors, which are instances of the Motor class
*/
void drive(driveMotors *d){
  if (d->mr==NULL || d->ml==NULL)
    return;

  //we instructed the motor to drive msContinuousDrive uninterrupted, we do handle a stop 
  if ( micros() < d->micros_next_move && d->direction != D_STOP)
    return;

  oldDir = d->direction;

  switch(d->direction) {
    case D_STOP:
      d->mr->stop(); d->ml->stop(); break;

    case D_FORWARD:
      d->mr->forward(d->pwm1); d->ml->forward(d->pwm2); break;

    case D_REVERSE:
      d->mr->reverse(d->pwm1); d->ml->reverse(d->pwm2); break;

    case D_LEFT:
      d->mr->stop(); d->ml->forward(d->pwm2); break;       

    case D_HLEFT:
       d->mr->reverse(d->pwm1); d->ml->forward(d->pwm2); break;

    case D_RIGHT:
      d->mr->forward(d->pwm1); d->ml->stop(); break;       

    case D_HRIGHT:
      d->mr->forward(d->pwm1); d->ml->reverse(d->pwm2); break;
  }

  d->micros_next_move = micros() + (d->msContinuous * 1000);
}

void takePic(const char *path, driveMotors *d){
  int t = d->direction;

  //stop current motion so we have motion blur free images
  if (d->direction != oldDir){
    d->direction = D_STOP;
    drive(d);
  } else { return; }

  Mat frame, dst;
  Size size(213,160);
  delay(1600);
  cap.read(frame);
  resize(frame, dst ,size);

  if( frame.empty() ) return; // end of video stream
  flip(dst, frame, 0);
  
  cv::imwrite(path, frame);
  imshow("pic", frame);
}

/*
moveRobot takes a driveMotors instance, and Range~Sensor instancve
It will perform a range meeasurement every 100ms, because the bit banging required is CPU intensive
Based on the distance:
   -the robot will move forward if the obstance is further than 30cm away
   -the robot will randomly do a hard left or hard right when when distance between 30 and 15cm
   it will do this for 800ms (non-blocking)
   -the roboot will reverse for a duration of 800ms wqhewn the distance is smaller than 15cm
 
   The random left or right is a fuzzy component that should help getting the robot out of tight spots
*/
void moveRobot(driveMotors *d, RangeSensor *rs, Mat mat) {
  float range;

  if (micros() < micros_next_measure) 
    return;

  range = rs->measure();
  if (range == -1)
    return;

  stringstream file;
  stringstream s;
  file << iteration << ".jpg";
  

  //The random factors make this fuzzy (non intelligent but still less likely to get stuck
  if ( range > 30){
     s  << "./1/" << file.str();
     takePic(s.str().c_str(), d);

     d->direction = D_FORWARD;
     d->pwm1 = d->pwm2 = CRUISE_SPEED;
     d->msContinuous = 0;
     drive(d);
  }
  else if(range <30 && range >10) {
     s << "./2/" << file.str();
     takePic(s.str().c_str(), d);

     if (rand()%2 == 0){ 
       d->pwm1 = d->pwm2 = 80;
       d->msContinuous = rand()%(1200-200 + 1) + 600;
       d->direction = D_HLEFT;
       drive(d);
     }
     else {
       d->pwm1 = d->pwm2 = 80;
       d->msContinuous = rand()%(1200-200 + 1) + 600;
       d->direction = D_HRIGHT;
       drive(d);
     }
  }  
  else{
    s << "./3/" << file.str();
    takePic(s.str().c_str(), d);

    d->direction = D_REVERSE;
    d->pwm1 = d->pwm2 = CRUISE_SPEED;
    d->msContinuous = rand()%(1200-600 + 1) + 600;
    drive(d);
  }

  micros_next_measure = micros() + (100*1000);
  iteration ++;

  //do something completely arbitrary ever 1000th iteration
  if (iteration % 100 == 0){
    int nd = (rand() % 4 + 1);
    while (nd == d->direction || nd == D_STOP) { nd = (rand() % 4 + 1); }
    d->direction = nd;
    d->pwm1 = d->pwm2 = CRUISE_SPEED;
    d->msContinuous = rand()%(1200-800 + 1) + 800;
    drive(d);
  }

  stringstream srange;
  srange << std::fixed << std::setprecision(2) << range << " cm";
  putText(mat, srange.str().c_str(), Point2f(10,10), FONT_HERSHEY_PLAIN, 1,  Scalar(0,0,255,255));
  imshow("Camera", mat);
}
void createDir(){
  if ( dirExists("./1/") == 0 ) mkdir ("./1", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if ( dirExists("./2/") == 0 ) mkdir ("./2", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if ( dirExists("./3/") == 0 ) mkdir ("./3", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

int main(){
     createDir();

     int deviceID = 0;             // 0 = open default camera
     int apiID = cv::CAP_ANY;      // 0 = autodetect default API
     // open selected camera using selected API

     cap.open(deviceID + apiID);
     cap.set(CAP_PROP_FRAME_WIDTH, 640/2);
     cap.set(CAP_PROP_FRAME_HEIGHT, 480/2);

     // check if we succeeded
     if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
     }

     iteration = 0;

     srand(time(0)); //I tried NULL as well
     wiringPiSetup(); //We use the wiringPi GPIO method (read their documentation)

     Motor *ml=new Motor(9, 8); //left motor is connected to WiringPi-GPIO 9 and 8

     Motor *mr=new Motor(16, 15); //right motor is connected to WiringPi-GPIO 16 and 15

     RangeSensor *rs = new RangeSensor(7, 0); //range finder trigeer is connected to WiringPI-GPIO 7 and echo to 0
     
     int key; //key holds the ASCII number of the chracter pressed on the keyboard

     driveMotors *d=new driveMotors;
     d->mr=mr;
     d->ml=ml;
     d->pwm1=d->pwm2=CRUISE_SPEED;
     d->direction = D_FORWARD;
     d->msContinuous = 0;
     d->micros_next_move = 0;

     Mat frame, prframe;
     while(1) {

      cap.read(frame);
      if( frame.empty() ) break; // end of video stream
      flip(frame, prframe, 0);
     
      if (waitKey(5) >= 0)
        break;

      //move the robot use cruise speed as a hard default, just in case
      d->pwm1=d->pwm2=CRUISE_SPEED; //cruise speed is a bit lower than the 100% duty cycle
      moveRobot(d, rs, prframe); //perform the basic measure and move actions

      delay(5); //safe CPU cycles, because measuring and moving isn't all that fast anyways
    }

    //stop the motors
    d->direction = D_STOP;
    d->msContinuous = 0;
    d->pwm1 = d->pwm2 = 0;
    drive(d);
 
    //destruct objects
    delete rs;
    delete mr;
    delete ml;
    free(d);
     
    return 0;
}
