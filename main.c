/*
 * File:   main.c
 * Author: Samuel Marroquín Isaza
 *
 * Created on May 18, 2026, 8:16 PM
 */


#include <xc.h>
#include "Servo.h"
void main(void) {
    Servo_Init();
    TRISB=1;
    while(1){
        if((RB1==0)&&(RB0==0)){
            Servo_Set(25);
        }else if((RB1==0)&&(RB0==1)){
            Servo_Set(50);
        }else if((RB1==1)&&(RB0==0)){
            Servo_Set(75);
        }else if((RB1==1)&&(RB0==1)){
            Servo_Set(100);
        }else{
            Servo_Set(0);
        }
    }
}
