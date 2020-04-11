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
#define CRUISE_SPEED 70

unsigned int micros_next_move = 0;

void drive(Motor *m1, Motor *m2, int direction, int pwm1, int pwm2, int msContinuousDrive){
  if (m1==NULL || m2==NULL)
    return;

  //we instructed the motor to drive msContinuousDrive uninterrupted, we do handle a stop 
  if ( micros() < micros_next_move && direction != D_STOP)
    return;


  switch(direction) {
    case D_STOP:
     m1->stop(); m2->stop(); mvprintw(1,2, "MOVE: STOP"); break;

    case D_FORWARD:
      m1->forward(pwm1); m2->forward(pwm2); mvprintw(1,2, "MOVE: FORWARD"); break;

    case D_REVERSE:
      m1->reverse(pwm1); m2->reverse(pwm2); mvprintw(1,2, "MOVE: REVERSE"); break;

    case D_LEFT:
      m1->stop(); m2->forward(pwm2);        mvprintw(1,2, "MOVE: LEFT   "); break;

    case D_HLEFT:
      m1->reverse(pwm1); m2->forward(pwm2); mvprintw(1,2, "MOVE: H_LEFT "); break;

    case D_RIGHT:
      m1->forward(pwm1); m2->stop();        mvprintw(1,2, "MOVE: RIGHT  "); break;

    case D_HRIGHT:
      m1->forward(pwm1); m2->reverse(pwm2); mvprintw(1,2, "MOVE: H_RIGHT"); break;
  }

  micros_next_move = micros() + (msContinuousDrive * 1000);
}

void demo(Motor *ml, Motor* mr, int runSec){
   mvprintw(0,0, "DEMO MOTOR: ");

   delay(runSec * 1000);
   drive(ml, mr, D_FORWARD, 100, 100, 0);

   delay(runSec * 1000);
   drive(ml, mr, D_REVERSE, 100, 100, 0);

   delay(runSec * 1000);
   drive(ml, mr, D_LEFT, 100, 100, 0);

   delay(runSec * 1000);
   drive(ml, mr, D_HLEFT, 100, 100, 0);

   delay(runSec * 1000);
   drive(ml, mr, D_RIGHT, 100, 100, 0);

   delay(runSec * 1000);
   drive(ml, mr, D_HRIGHT, 100, 100, 0);

   delay(runSec * 1000);
   drive(ml, mr, D_STOP, 100, 100, 0);
}

void move(Motor *ml, Motor *mr, RangeSensor *rs) {
  float range;

  range = rs->measure();
  mvprintw(2,2, "RANGE: %.2f cm", range);

  if ( range > 30)
     drive(ml, mr, D_FORWARD, CRUISE_SPEED, CRUISE_SPEED, 0);
  else if(range <30 && range >15) {
     (rand()%2 == 0)?drive(ml, mr, D_HRIGHT, CRUISE_SPEED, CRUISE_SPEED, 800):drive(ml, mr, D_HLEFT, CRUISE_SPEED, CRUISE_SPEED, 800);
  }
  else{
    drive(ml, mr, D_REVERSE, 100, 100, 500);
    (rand()%2 == 0)?drive(ml, mr, D_HRIGHT, 50, 50, 800):drive(ml, mr, D_HLEFT, 50, 50, 800);
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

     mvprintw(0, 40, "ROBOT v0.1");
     while(1) {
       //stop robot when q is pressedÂ
       key = getch();
       if ( key == 'q' ) break;
       if ( key == 'd' ) demo(ml, mr, 2);

       //move the robot
       move(ml, mr, rs);

       delay(5);
     }

     //Restore stdin to its normal blocking operation before we leave.
     endwin();
     drive(ml, mr, D_STOP, 0, 0, 0);
  
     //destruct objects
     delete rs;
     delete mr;
     delete ml;
     
     return 0;
}
