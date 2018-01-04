/*
 * light.c
 *

 */

#include <stdint.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include "launchpad.h"
#include "light.h"
#include <driverlib/adc.h>


// light sensor is connected to port J6
void light_ADCInit() {
    // Enable the ADC1 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    // Configure ADC sequencer #0
    ADCSequenceConfigure(ADC1_BASE, 0 /* sequencer #0 */,
                         ADC_TRIGGER_PROCESSOR, 0 /* priority */);
    // Configure a single step of the sequencer
    ADCSequenceStepConfigure(ADC1_BASE, 0, 0 /* step */,
                             ADC_CTL_IE /* set interrupt flag */
                             | ADC_CTL_END /* mark the last step */
                             | ADC_CTL_CH6 /* ADC channel 6 */);
    // Enable the ADC sequencer
    ADCSequenceEnable(ADC1_BASE, 0 /* sequencer 0*/);
}

uint16_t light_adcRead()
{
    uint32_t adcReading;
    // Trigger ADC sample sequence
    ADCProcessorTrigger(ADC1_BASE, 0 /* sequencer #0 */);
    // Wait until the sample sequence has completed
    while (!ADCIntStatus(ADC1_BASE, 0 /* sequencer #0 */,
                         false /* no interrupt mask */)) {

    }
    // Read ADC 1
    ADCSequenceDataGet(ADC1_BASE, 0 /* sequencer #0 */,
                       &adcReading);
    return (uint16_t) adcReading;
}






