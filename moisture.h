/*
 * moisture.h
 *
 */

#ifndef MOISTURE_H_
#define MOISTURE_H_


// Initialize the moisture sensor
void moisture_ADCInit();

// Turn on ADC sampling
uint16_t moisture_adcRead();

#endif /* MOISTURE_H_ */
