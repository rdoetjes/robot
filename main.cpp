#include <stddef.h>
#include <wiringPi.h>
#include "motor.h"
#include "rangesensor.h"
#include <iostream>
#include <ncurses.h>

#define D_STOP 0
#define D_FORWARD 1
#define D_REVERSE 2
#define D_LEFT 3
#define D_RIGHT 4
#define D_HLEFT 5
#define D_HRIGHT 6
#define CRUISE_SPEED 70

void drive(Motor *m1, Motor *m2, int direction, int pwm1, int pwm2){
  if (m1==NULL || m2==NULL)
    return;

  switch(direction) {
    case D_STOP:
     m1->stop(); m2->stop(); break;

    case D_FORWARD:
      m1->forward(pwm1); m2->forward(pwm2); break;

    case D_REVERSE:
      m1->reverse(pwm1); m2->reverse(pwm2); break;

    case D_LEFT:
      m1->stop(); m2->forward(pwm2); break;

    case D_HLEFT:
      m1->reverse(pwm1); m2->forward(pwm2); break;

    case D_RIGHT:
      m1->forward(pwm1); m2->stop(); break;

    case D_HRIGHT:
      m1->forward(pwm1); m2->reverse(pwm2); break;
  }
}

void demo(Motor *ml, Motor* mr){
     delay(3000);
     std::cout << "FW" << std::endl;
     drive(ml, mr, D_FORWARD, 100, 100);

     delay(3000);
     std::cout << "RE" << std::endl;
     drive(ml, mr, D_REVERSE, 100, 100);

     delay(3000);
     std::cout << "L" << std::endl;
     drive(ml, mr, D_LEFT, 100, 100);

     delay(3000);
     std::cout << "HL" << std::endl;
     drive(ml, mr, D_HLEFT, 100, 100);

     delay(3000);
     std::cout << "R" << std::endl;
     drive(ml, mr, D_RIGHT, 100, 100);

     delay(3000);
     std::cout << "HR" << std::endl;
     drive(ml, mr, D_HRIGHT, 100, 100);

     delay(3000);
     drive(ml, mr, D_STOP, 100, 100);
}

int main(){
     float range = 0;
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
 
     while(1) {
       if ( getch() == 'q') break;

       range = rs->measure();
       //std::cout << range << std::endl;
       if ( range > 30) 
         drive(ml, mr, D_FORWARD, CRUISE_SPEED, CRUISE_SPEED);
       else if(range <30 && range >15) {
         (rand()%2 == 0)?drive(ml, mr, D_HRIGHT, CRUISE_SPEED, CRUISE_SPEED):drive(ml, mr, D_HLEFT, CRUISE_SPEED, CRUISE_SPEED);
         delay(600);
       }
       else{ 
         drive(ml, mr, D_REVERSE, 100, 100);
         delay(500);
         (rand()%2 == 0)?drive(ml, mr, D_HRIGHT, 50, 50):drive(ml, mr, D_HLEFT, 50, 50);
	 delay(800);
       }
     }
     //Restore stdin to its normal blocking operation before we leave.
     endwin();
     drive(ml, mr, D_STOP, 0, 0);
  
     delete rs;
     delete mr;
     delete ml;
     
     return 0;
}
