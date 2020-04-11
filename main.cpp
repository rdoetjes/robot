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
#define CRUISE_SPEED 100

struct driveMotors{
  Motor *mr;
  Motor *ml;
  int direction;
  int pwm1;
  int pwm2;
  int msContinuous;
  unsigned int micros_next_move;
};

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

void moveRobot(driveMotors *d, RangeSensor *rs) {
  float range;

  range = rs->measure();
  mvprintw(2,2, "RANGE: %.2f cm    ", range);

  if ( range > 30){
     d->direction = D_FORWARD;
     d->pwm1 = d->pwm2 = CRUISE_SPEED;
     d->msContinuous = 0;
     drive(d);
  }
  else if(range <30 && range >15) {
     if (rand()%2 == 0){ 
       d->pwm1 = d->pwm2 = 80;
       d->msContinuous = 800;
       d->direction = D_HLEFT;
       drive(d);
     }
     else {
       d->pwm1 = d->pwm2 = 80;
       d->msContinuous = 800;
       d->direction = D_HRIGHT;
       drive(d);
     }
  }  
  else{
    d->direction = D_REVERSE;
    d->pwm1 = d->pwm2 = CRUISE_SPEED;
    d->msContinuous = 500;
    drive(d);
  }
}

int main(){
 
     //Set stdin to non-blocking, no echo.
     initscr();
     nodelay(stdscr, true);
     noecho();
     raw();

     srand(time(0)); //I tried NULL as well
     wiringPiSetup();
     Motor *ml=new Motor(9, 8);
     Motor *mr=new Motor(16, 15);
     RangeSensor *rs = new RangeSensor(7, 0);
     int key;

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
       d->pwm1=d->pwm2=CRUISE_SPEED;
       moveRobot(d, rs);

       delay(5);
     }

     //Restore stdin to its normal blocking operation before we leave.
     endwin();
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
