/*
 EW202 Lab 5.2 Elevator Control (basic logic control)
 D Evangelista, 2019
 */

#include "mbed.h"
#include "rtos.h"
#include "Motor.h"

// Calibration constants from Lab 5.1
#define CAL_A 1.0 /**!< Ax^2+... */ 
#define CAL_B 1.0 /**!< B*x+... */
#define CAL_C 1.0 /**!< C */

// from Lab 5.2 handout
#define DRIVE_DOWN -0.15
#define DRIVE_UP 0.55
#define DRIVE_HOLD 0.3




// Objects needed for Lab 5.2
Serial pc(USBTX,USBRX);
AnalogIn sensor(p20);
Motor motor(p26,p30,p29); 

// Function prototypes for calibration and logic_control
float calibration(float); 
float logic_control(float);





int main(void){
  int n_samples;
  float reference,ymeas,u; 

  // do forever
  while(1){

    // read in number of samples and reference height
    pc.scanf("%d,%f",&n_samples,&reference);

    // for n_samples... 
    for (int i=0; i<n_samples; i++){
      
      ymeas = calibration(sensor.read()); // get calibrated sensor value
      u = logic_control(reference-ymeas); // run logic control
      motor.speed(u); // drive the motor as required
      pc.printf("%d,%f,%f\n",i,ymeas,u); // print out telemetry string
      
      ThisThread::sleep_for(20); 
    } // for n_samples
    
  } // while(1)
} // main()







/**
   calibration() takes a GPD212 measurement and corrects for the nonlinearity,
   returning the height in inches.
   @param y the raw AnalogIn measurement, a float from 0.0-1.0
   @return float the height in inches. 
 */
float calibration(float y){
  return CAL_A*y*y + CAL_B*y + CAL_C;
}




/**
   logic_control() implements a simple dead band bang bang logic controller
   @param error is the error signal, reference-measurement, as a float
   @return float the motor duty cycle, a float from 0.0-1.0
*/
float logic_control(float error){
  
  if (error > 2.0)
    return DRIVE_DOWN;
  else if (error < -2.0)
    return DRIVE_UP;
  else
    return DRIVE_HOLD;

} // float logic_control(float)
