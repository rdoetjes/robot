#include <stddef.h>
#include <wiringPi.h>
#include "motor.h"
#include "rangesensor.h"
#include <iostream>
#include <ncurses.h>			/* ncurses.h includes stdio.h */  
#include <string.h> 

#define D_STOP 0
#define D_FORWARD 1
#define D_REVERSE 2
#define D_LEFT 3
#define D_RIGHT 4
#define D_HLEFT 5
#define D_HRIGHT 6
#define CRUISE_SPEED 1000

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

  switch(d->direction) {
    case D_STOP:
      d->mr->stop(); d->ml->stop(); mvprintw(1,2, "MOVE: STOP"); break;

    case D_FORWARD:
      d->mr->forward(d->pwm1); d->ml->forward(d->pwm2); mvprintw(1,2, "MOVE: FORWARD"); break;

    case D_REVERSE:
      d->mr->reverse(d->pwm1); d->ml->reverse(d->pwm2); mvprintw(1,2, "MOVE: REVERSE"); break;

    case D_LEFT:
      d->mr->stop(); d->ml->forward(d->pwm2);        mvprintw(1,2, "MOVE: LEFT   "); break;

    case D_HLEFT:
       d->mr->reverse(d->pwm1); d->ml->forward(d->pwm2); mvprintw(1,2, "MOVE: H_LEFT "); break;

    case D_RIGHT:
      d->mr->forward(d->pwm1); d->ml->stop();        mvprintw(1,2, "MOVE: RIGHT  "); break;

    case D_HRIGHT:
      d->mr->forward(d->pwm1); d->ml->reverse(d->pwm2); mvprintw(1,2, "MOVE: H_RIGHT"); break;
  }

  d->micros_next_move = micros() + (d->msContinuous * 1000);
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
void moveRobot(driveMotors *d, RangeSensor *rs) {
  float range;

  if (micros() < micros_next_measure) 
    return;

  range = rs->measure();
  if (range == -1)
    return;

  mvprintw(2,2, "RANGE: %.2f cm    ", range);
  //The random factors make this fuzzy (non intelligent but still less likely to get stuck
  if ( range > 30){
     d->direction = D_FORWARD;
     d->pwm1 = d->pwm2 = CRUISE_SPEED;
     d->msContinuous = 0;
     drive(d);
  }
  else if(range <30 && range >15) {
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
    d->direction = D_REVERSE;
    d->pwm1 = d->pwm2 = CRUISE_SPEED;
    d->msContinuous = rand()%(1200-600 + 1) + 600;
    drive(d);
  }

  micros_next_measure = micros() + (100*1000);
  iteration ++;
  mvprintw(3, 2, "ITERATION: %d",iteration);

  //do something completely arbitrary ever 1000th iteration
  if (iteration % 100 == 0){
    int nd = rand()%(6-1 + 1) + 1;
    while (nd == d->direction || nd == D_STOP) { nd = rand()%(6-1 + 1) + 1; }
    d->direction = nd;
    d->pwm1 = d->pwm2 = CRUISE_SPEED;
    d->msContinuous = rand()%(1200-600 + 1) + 600;
    drive(d);
  }
}

int main(){
     iteration = 0;
     //Set stdin to non-blocking, no echo to read keyboard 
     initscr();
     nodelay(stdscr, true);
     noecho();
     raw();

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

     mvprintw(0, 40, "ROBOT v0.1");
     while(1) {
       //stop robot when q is pressedÂ
       key = getch();
       if ( key == 'q' ) break;

       //move the robot use cruise speed as a hard default, just in case
       d->pwm1=d->pwm2=CRUISE_SPEED; //cruise speed is a bit lower than the 100% duty cycle

       moveRobot(d, rs); //perform the basic measure and move actions

       delay(5); //safe CPU cycles, because measuring and moving isn't all that fast anyways
     }

     //Restore stdin to its normal blocking operation before we leave.
     endwin();

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
