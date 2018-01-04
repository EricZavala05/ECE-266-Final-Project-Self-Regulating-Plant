/*
 * alarm.h
 *
 */

#ifndef ALARM_H_
#define ALARM_H_

/*
 * Initialize the timer PWM functions connected to the buzzer.
 */
void pwmBuzzInit();

/*
 * Set the intensity of the buzzer
 */
void pwmBuzzSetIntensity(int intensityLevel);



#endif /* ALARM_H_ */
