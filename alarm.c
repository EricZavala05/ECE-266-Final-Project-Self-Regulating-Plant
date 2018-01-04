/*
 * alarm.c
 *
 */


#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include "alarm.h"

// intensity level table. This table may be calibrated to each Tiva C LaunchPad
typedef struct {
    uint32_t pwmPeriod; // frequency
    uint32_t pwmDutyCycle;  // loudness
} pwm_buzz;

// musical notes
// A3 = 1 C4 = 2 C#4 = 3, D4 = 4, D#4 = 5, E4 = 6,
// F4 = 7, F#4 = 8, G4 = 9, G#4 = 10, A4 = 11, A#4 = 12, B4 = 13
// G3 = 14, B3 = 15, F3 = 16, no sound = 17
pwm_buzz intensityLvlTable[] = {
  {227227,10000}, {190840 , 10000}, {180505, 10000}, {170068, 10000}, {160772, 10000},
  {151515, 10000}, {143266, 10000}, {135135, 10000}, {127551, 10000}, {120481, 10000},
  {113636, 10000}, {107296, 10000}, {101214, 10000}, {255102, 10000}, {202429, 10000},
  {285714, 10000}, {100, 1}
};

#define BUZZ_INTENSITY_LEVEL_NUM     (sizeof(intensityLvlTable) / sizeof(pwm_buzz))


/*
 * Initialize the timer PWM functions connected to the Buzzer
 *    Buzzer:   PC4, WT0CCP0, 32/64-bit Timer 0 channel A
 *      Port J16
 */
void
pwmBuzzInit()
{
    //Enable the GPIO port peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Enable Wide Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);

    // Connect pins to those timers
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinConfigure(GPIO_PC4_WT0CCP0);

    // Select PWM for Wide Timer 0 sub-Timer A
    TimerConfigure(WTIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM));

    // The default setting is very bright, so change it to low density
    TimerLoadSet(WTIMER0_BASE, TIMER_A, 100);
    TimerMatchSet(WTIMER0_BASE, TIMER_A, 100-1);

    // Enable the Wide Timer 0's TimerA
    TimerEnable(WTIMER0_BASE, TIMER_A);
}

/*
 * Set the intensity level of the Buzzer. The lowest level is 1.
 */
void
pwmBuzzSetIntensity(int intensityLevel)
{
    int n = intensityLevel - 1;
    pwm_buzz *pwm = &intensityLvlTable[n];

   // Range check on the intensity level
    if (n >= BUZZ_INTENSITY_LEVEL_NUM)
        return;

    TimerLoadSet(WTIMER0_BASE, TIMER_A, pwm->pwmPeriod);
    TimerMatchSet(WTIMER0_BASE, TIMER_A, pwm->pwmPeriod - pwm->pwmDutyCycle);
}




