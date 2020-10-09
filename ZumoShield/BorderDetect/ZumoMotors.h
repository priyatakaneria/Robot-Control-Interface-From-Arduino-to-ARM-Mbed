/* File: DRV8835.h
 * Adopted from work by Cameron Isbell
 *
 * Description: library for DRV8835 Motor Driver
 * Assumptions: A is left and B is right
 */ 
#ifndef __DRV8835_H__
#define __DRV8835_H__
 
#include "mbed.h"
 
#define DRV8835_PWM_PERIOD_DEFAULT      (0.00005)   // 2 ms period                      // 50KHz (0.00002)
#define DRV8835_PWM_PULSEWIDTH_DEFAULT  (0.50)      // 50% duty cycle
 
class ZumoMotors
{
public:
    ZumoMotors( PinName pinPwmL, PinName pinLin,
               PinName pinPwmR, PinName pinRin);
 
    void motorL_stop(void);
    void motorL_fwd(float fPulsewidth);
    void motorL_rev(float fPulsewidth);
    void motorR_stop(void);
    void motorR_fwd(float fPulsewidth);
    void motorR_rev(float fPulsewidth);
    void setSpeeds(float Left,float Right);
    void stop(void);
    
private:
    PwmOut pwmL;
    DigitalOut Lin;
    PwmOut pwmR;
    DigitalOut Rin;
};
 
#endif /* __DRV8835_H__ */
 