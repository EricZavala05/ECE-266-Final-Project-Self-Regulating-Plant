/*
 * light.h
 *
 */

#ifndef LIGHT_H_
#define LIGHT_H_


// Initialize the light sensor
void light_ADCInit();

// Turn on ADC sampling
uint16_t light_adcRead();


#endif /* LIGHT_H_ */
