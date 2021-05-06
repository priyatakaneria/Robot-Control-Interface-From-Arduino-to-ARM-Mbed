/* File: DRV8835.cpp
 * Adopted from work by Cameron Isbell
 *
 * Description: library for DRV8835 Motor Driver
 * Assumptions: A is left and B is right
 */
 
#include "mbed.h"
#include "ZumoMotors.h"
 
 
ZumoMotors::ZumoMotors( PinName pinPwmL, PinName pinLin,
                      PinName pinPwmR, PinName pinRin) :
pwmL(pinPwmL),
Lin(pinLin),
pwmR(pinPwmR),
Rin(pinRin)
{
    Lin = 0;
    Rin = 0;
    pwmL.period(DRV8835_PWM_PERIOD_DEFAULT);
    pwmL = DRV8835_PWM_PULSEWIDTH_DEFAULT;
    pwmR.period(DRV8835_PWM_PERIOD_DEFAULT);
    pwmR = DRV8835_PWM_PULSEWIDTH_DEFAULT;
    motorL_stop();
    motorR_stop();
}
 
void ZumoMotors::stop()
{
    motorL_stop();
    motorR_stop();
}
 
void ZumoMotors::motorL_stop(void)
{
    pwmL = 0;
}
 
void ZumoMotors::motorR_stop(void)
{
    pwmR = 0;
}
 
void ZumoMotors::setSpeeds(float Left,float Right)
{
    //Set Right Speed and Direction
    if(Right<0)
    {
        motorR_rev(Right*-1);
    } else {
        motorR_fwd(Right);
    }
    
    //Set Left Speed and Direction
    if(Left<0)
    {
        motorL_rev(Left*-1);
    } else {
        motorL_fwd(Left);
    }
}
 
void ZumoMotors::motorL_fwd(float fPulsewidth)
{
    Lin = 0;
    pwmL = fPulsewidth;
}
void ZumoMotors::motorL_rev(float fPulsewidth)
{
    Lin = 1;
    pwmL = fPulsewidth;
}
 
void ZumoMotors::motorR_fwd(float fPulsewidth)
{
    Rin = 0;
    pwmR = fPulsewidth;
}
void ZumoMotors::motorR_rev(float fPulsewidth)
{
    Rin = 1;
    pwmR = fPulsewidth;
}
