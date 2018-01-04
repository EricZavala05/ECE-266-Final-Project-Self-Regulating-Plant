/*
 * main.c for ECE 266 Final Project
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/systick.h>
#include <driverlib/timer.h>
#include "launchpad.h"
#include "seg7.h"
#include "seg7digit.h"
#include "moisture.h"
#include "alarm.h"
#include "light.h"

// Display mode of the 7-segment display
// display starts in moisture mode mode
enum {
    Light, Moist
}  sysState = Moist;


//  music to be played when alarm rings
int notes [] = {1,1,2,1,4,2,6,4,2,2,6,2,9,2,
                6,2,14,14,15,14,2,14,4,2,16,
                16,2,16,2,16,2,15};

uint8_t a = 0;
uint8_t b = 0;
uint8_t c = 0;
uint8_t d = 0;

// The initial state of the 7-segment display: "0000"
seg7Display_t seg7Display = {
    0, 0, 0, 0
};

// Callback function for updating display
void
displayUpdate(uint32_t time)                          // The scheduled time
{
    uint32_t position;
    if (sysState == Moist) {        // set the display to show the Moisture level as a percent
        position = moisture_adcRead();
        position = (position * 100) / 4095;
        if((position / 10) == 10)
        {
            seg7Display.d4 = 0;
            seg7Display.d3 = 1;
            seg7Display.d2 = 0;
            seg7Display.d1 = 0;
        }
        else
        {
            c = position / 10;
            d = position % (c*10);
            seg7Display.d1 = d;
            seg7Display.d2 = c;
            seg7Display.d3 = 0;
            seg7Display.d4 = 0;
        }
    }
    else
        if(sysState == Light) {      // set the display to show the light intensity level as a percent
            position = light_adcRead();
            position = (position * 100) / 4095;
             if((position / 10) == 10)
             {
                 seg7Display.d4 = 0;
                 seg7Display.d3 = 1;
                 seg7Display.d2 = 0;
                 seg7Display.d1 = 0;
             }
             else
             {
                 c = position / 10;
                 d = position % (c*10);
                 seg7Display.d1 = d;
                 seg7Display.d2 = c;
                 seg7Display.d3 = 0;
                 seg7Display.d4 = 0;
                 }
        }

    seg7DigitUpdate(&seg7Display);

    // Call back after 200 milliseconds
    schdCallback(displayUpdate, time + 200);
}

// Callback function for checking push button.
void
checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay;

    switch (code) {
    case 1:                     // first pushbutton to be used to switch between
        if(sysState == Light)    // light and moisture display
        {
            sysState = Moist;
        }
        else
            sysState = Light;
        delay = 250;                // software debouncing
        break;

    case 2:

        delay = 250;                // software debouncing
        break;

    default:
        delay = 10;
    }
    schdCallback(checkPushButton, time + delay);
}

/*
 * Change note played by buzzer
 */
void
pwmBuzzPlay(uint32_t time)
{
    static int i = 0;
    uint32_t position;

    // Pre-schedule the next callback with 250 ms delay
    schdCallback(pwmBuzzPlay, time + 250);

    // read moisture level
    position = moisture_adcRead();
    position = (position * 100) / 4095;

    // Return immediately if moisture level is greater than 30%
    if (position > 30)
    {
        pwmBuzzSetIntensity(17); // mutes the sound from the buzzer
        return;
    }
    // Cycle through the notes of the song
    //
    pwmBuzzSetIntensity(notes[i]);
    if(i == 31)
    {
        i = 0;
    }
    else
        i++;
}
/*
 *  initialize pins for output to control light bulb
 */
void
setup()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7,GPIO_PIN_7);

}
/*
 * Checks and changes state of light bulb
 */
void
bulbControl(uint32_t time)
{
    uint32_t position;
    position = light_adcRead();
    position = (position * 100) / 4095;
    if(position > 50) // if light intensity is grater than threshold turn off bulb
    {
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
    }
    else
        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_PIN_7);
    schdCallback(bulbControl, time + 1 );

}
int main(void)
{
    lpInit();
    moisture_ADCInit();
    light_ADCInit();
    seg7Init();
    pwmBuzzInit();
    setup();

    schdCallback(checkPushButton,1010);
    schdCallback(displayUpdate, 1020);
    schdCallback(pwmBuzzPlay, 1030 );
    schdCallback(bulbControl, 1040);

    while(true)
    {
        schdExecute();
    }

}
