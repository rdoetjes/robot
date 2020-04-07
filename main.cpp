#include <stddef.h>
#include <wiringPi.h>
#include "motor.h"
#include "rangesensor.h"
#include <iostream>

#define D_STOP 0
#define D_FORWARD 1
#define D_REVERSE 2
#define D_LEFT 3
#define D_RIGHT 4
#define D_HLEFT 5
#define D_HRIGHT 6

void drive(Motor *m1, Motor *m2, int direction){
  if (m1!=NULL && m2!=NULL){
  
    switch(direction) {
       case D_STOP:
	  m1->stop(); m2->stop(); break;

       case D_FORWARD:
	  m1->forward(); m2->forward(); break;

       case D_REVERSE:
	  m1->reverse(); m2->reverse(); break;

       case D_LEFT:
	  m1->stop(); m2->forward(); break;

       case D_HLEFT:
	  m1->reverse(); m2->forward(); break;

       case D_RIGHT:
	  m1->forward(); m2->stop(); break;

       case D_HRIGHT:
	  m1->forward(); m2->reverse(); break;
    }
  }
}

void demo(Motor *ml, Motor* mr){
     delay(3000);
     std::cout << "FW" << std::endl;
     drive(ml, mr, D_FORWARD);

     delay(3000);
     std::cout << "RE" << std::endl;
     drive(ml, mr, D_REVERSE);

     delay(3000);
     std::cout << "L" << std::endl;
     drive(ml, mr, D_LEFT);

     delay(3000);
     std::cout << "HL" << std::endl;
     drive(ml, mr, D_HLEFT);

     delay(3000);
     std::cout << "R" << std::endl;
     drive(ml, mr, D_RIGHT);

     delay(3000);
     std::cout << "HR" << std::endl;
     drive(ml, mr, D_HRIGHT);

     delay(3000);
     drive(ml, mr, D_STOP);
}

int main(){
        wiringPiSetup();
        //Motor *ml=new Motor(9, 8);
        //Motor *mr=new Motor(0, 7);
        RangeSensor *rs = new RangeSensor(15, 16);
        std::cout << rs->measure() << std::endl;
        //demo(ml, mr);
	return 0;
}
