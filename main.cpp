/*
 EW202 Lab 5.2 Elevator Control (basic logic control)
 D Evangelista, 2019
 */

#include "mbed.h"
#include "rtos.h"
#include "Motor.h"

// NB #define is a compiler directive; everytime the compiler sees the
// symbol CAL_A it replaces it with -59.7382 before compiling...
// This is a standard way to code setpoints, gains, etc, with the
// #defines often places in a .h file that can be overwritten when
// the device is re-calibrated. This avoids introducing bugs into
// working code when only the setpoints must be changed. 

// Calibration constants from Lab 5.1,
#define CAL_VERSION "instructor station, 14 Feb 2019"
#define CAL_A -59.7382 /**!< Ax^2+... */ 
#define CAL_B 79.7480  /**!< B*x+... */
#define CAL_C -2.9079  /**!< C */

// from Lab 5.2 handout
#define LOGIC_VERSION "21 Feb 2019"
#define LOGIC_DRIVE_DOWN -0.15 // pwm duty cycle to drive down
#define LOGIC_DRIVE_UP 0.55    // to drive up 
#define LOGIC_DRIVE_HOLD 0.3   // to hold still?
#define LOGIC_ERROR_HIGH 2.0   // error band
#define LOGIC_ERROR_LOW -2.0   



// (global) Hardware objects needed for Lab 5.2
Serial pc(USBTX,USBRX);
AnalogIn sensor(p20);
Motor motor(p26,p30,p29); 

// Function prototypes for calibration and logic_control
float calibration(float); 
float logic_control(float);





int main(void){
  int n_samples, i;
  float reference,ymeas,u;

  // do forever
  while(1){

    // read in number of samples and reference height
    pc.scanf("%d,%f",&n_samples,&reference);

    // for n_samples... 
    for (i=0; i<n_samples; i++){
      
      ymeas = calibration(sensor.read()); // get calibrated sensor value
      u = logic_control(reference-ymeas); // run logic control
      motor.speed(u); // drive the motor as required
      pc.printf("%d,%f,%f\n",i,ymeas,u); // print out telemetry string
      
      ThisThread::sleep_for(20); 
    } // for n_samples
    
  } // while(1)
} // main()







/**
   @brief GPD212 sensor calibration
   calibration() takes a GPD212 measurement and corrects for the nonlinearity,
   returning the height in inches.
   @param y the raw AnalogIn measurement, a float from 0.0-1.0
   @return float the height in inches. 
 */
float calibration(float y){
  return CAL_A*y*y + CAL_B*y + CAL_C;
}




/**
   @brief logic control
   logic_control() implements a simple dead band bang bang logic controller
   @param error is the error signal, reference-measurement, as a float
   @return float the motor duty cycle, a float from 0.0-1.0
*/
float logic_control(float error){
  
  if (error > LOGIC_ERROR_HIGH)
    return LOGIC_DRIVE_DOWN;
  else if (error < LOGIC_ERROR_LOW)
    return LOGIC_DRIVE_UP;
  else
    return LOGIC_DRIVE_HOLD;

} // float logic_control(float)
