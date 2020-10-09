/*
 * This example uses the ZumoMotors library to drive each motor on the Zumo
 * forward, then backward. The yellow user LED is on when a motor should be
 * running forward and off when a motor should be running backward. If a
 * motor on your Zumo has been flipped, you can correct its direction by
 * uncommenting the call to flipLeftMotor() or flipRightMotor() in the setup()
 * function.
 */

// #include <Wire.h>
#include "ZumoMotors.h"
#include "mbed.h"

ZumoMotors motors(PTD0, PTC12, PTC4, PTC3);

int main(){
    while(1){

        //turn right
    for (float speed = 0; speed <= 400; speed++){
        float correctspeed = (speed * 51 / 80)/255;
        motors.motorL_fwd(correctspeed);
        wait_us(2000);
        // wait(1.0);
        //motors.motorR_stop();
        // motors.motorL_stop();
        // wait(1.0);
    }

    for (float speed = 400; speed >= 0; speed--){
        float correctspeed2 = (speed * 51 / 80)/255;
        motors.motorL_fwd(correctspeed2);
        wait_us(2000);
    }
        
        //go back
        //motors.motorR_rev(0.4);
    for (float speed = 0; speed <= 400; speed++){
        float correctspeed3 = (speed * 51 / 80)/255;
        motors.motorL_rev(correctspeed3);
        wait_us(2000);
        // wait(1.0);
        // //motors.motorR_stop();
        // motors.motorL_stop();
        // wait(1.0);
    }

    for (float speed = 400; speed >= 0; speed--){
        float correctspeed4 = (speed * 51 / 80)/255;
        motors.motorL_rev(correctspeed4);
        wait_us(2000);
    }

         //turn left
        //motors.motorR_fwd(0.4);
    for (float speed = 0; speed <= 400; speed++){
        float correctspeed5 = (speed * 51 / 80)/255;
        motors.motorR_fwd(correctspeed5);
        wait_us(2000);
        // wait(1.0);
        // //motors.motorR_stop();
        // motors.motorR_stop();
        // wait(1.0);
    }

    for (float speed = 400; speed >= 0; speed--){
        float correctspeed6 = (speed * 51 / 80)/255;
        motors.motorR_fwd(correctspeed6);
        wait_us(2000);
    }
        
        //go back
        //motors.motorR_rev(0.4);
    for (float speed = 0; speed <= 400; speed++){
        float correctspeed7 = (speed * 51 / 80)/255;
        motors.motorR_rev(correctspeed7);
        wait_us(2000);
        // wait(1.0);
        // //motors.motorR_stop();
        // motors.motorR_stop();
        // wait(1.0);
    }

    for (float speed = 400; speed >= 0; speed--){
        float correctspeed8 = (speed * 51 / 80)/255;
        motors.motorR_rev(correctspeed8);
        wait_us(2000);
    }
        
        //drive backward
        // motors.motorR_fwd(0.8);
        // motors.motorL_rev(0.8);
        // wait(1.0);
        // motors.motorR_stop();
        // motors.motorL_stop();
        // wait(1.0);

        // //drive forward
        // motors.motorR_rev(0.8);
        // motors.motorL_fwd(0.8);
        // wait(1.0);
        // motors.motorR_stop();
        // motors.motorL_stop();
        // wait(1.0);
    }
}