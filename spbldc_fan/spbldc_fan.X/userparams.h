/*******************************************************************************
 *  User-defined Parameters for Sensored Single Phase BLDCM Constants, Sequence 
 *  and Routine Header File
 * 
 *  File Name:
 *    userparams.h
 *   
 *  Summary:
 *    This header file lists Single Phase BLDCM drive and motor fault 
 *    configuration related functions and definitions.
 *
 *  Description:
 *    Definitions in the file are for dsPIC33CK256MP508 microcontroller
 *    plugged onto the Low Voltage Motor Control (LVMC) Board from Microchip.
 */
/*******************************************************************************/
/*******************************************************************************
 * Copyright (c) 2019 released Microchip Technology Inc.  All rights reserved.
 *
 * SOFTWARE LICENSE AGREEMENT:
 * 
 * Microchip Technology Incorporated ("Microchip") retains all ownership and
 * intellectual property rights in the code accompanying this message and in all
 * derivatives hereto.  You may use this code, and any derivatives created by
 * any person or entity by or on your behalf, exclusively with Microchip's
 * proprietary products.  Your acceptance and/or use of this code constitutes
 * agreement to the terms and conditions of this notice.
 *
 * CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO
 * WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S
 * PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.
 *
 * YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE,
 * WHETHER IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF
 * STATUTORY DUTY),STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE,
 * FOR ANY INDIRECT, SPECIAL,PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, FOR COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE CODE,
 * HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR
 * THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWABLE BY LAW,
 * MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS CODE,
 * SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO
 * HAVE THIS CODE DEVELOPED.
 *
 * You agree that you are solely responsible for testing the code and
 * determining its suitability.  Microchip has no obligation to modify, test,
 * certify, or support the code.
 *
 *******************************************************************************/
#ifndef USERPARAMS_H
#define	USERPARAMS_H

// *****************************************************************************
// Section: Included Files
// *****************************************************************************
#include <stdint.h>
#include "mcc_generated_files/pin_manager.h"
#include "motor_control/motor_control_declarations.h"
#include "motor_control/motor_control_types.h"

// *****************************************************************************
// Section: LOOP CONTROLLER
// *****************************************************************************  
/* Instruction:
 * Default Loop Controller is in Closed Loop mode.
 * Comment out the following line to run the motor in Open Loop mode. */
//#define CLOSEDLOOP
//#define OPENLOOP



//define SET_SPEED 1500
// *****************************************************************************
// Section: Constants
// *****************************************************************************
/* Potentiometer (Speed Reference - PIN # 61 - RB9) */
#define ADCBUF_POTENTIOMETER ADCBUF19
/* Hall Sensor (HALL3 - PIN # 57 - RE10) */
#define HALLSENSOR                 HALL1_GetValue()
// Instruction cycle frequency (Hz) - 100,000,000 Hz
#define FCY                     100000000UL
#define PWM_FREQ_HZ             20000
//Converting to 1.15 Numerical Format
#define Q15(Float_Value)	\
((Float_Value < 0.0) ? (int16_t)(32768 * (Float_Value) - 0.5) \
: (int16_t)(32767 * (Float_Value) + 0.5))

#define MAX_PERIOD  46875
#define MIN_PERIOD  10190
#define PERIOD_MULTI    (MAX_PERIOD-MIN_PERIOD)>>12

#define PWMTICKS            (float)0.00005
#define FREQHZ              (float)(TIMER_PRESCALER*2)/FCY
#define PERIODPERPULSE      (float)FREQHZ/PWMTICKS
    
#define TICKSPI (float)(((64*2)/100000000)/0.00005)
// *****************************************************************************
/** Motor Ratings (from Name Plate Details or Datasheet)*/
// *****************************************************************************   
#define POLEPAIRS		    2      // Number of pole pairs    
#define POLES               4      // Number of poles 
#define MAX_CL_MOTORSPEED   4000   // Specify the maximum speed in rpm of motor 

#define MIN_OL_MOTORSPEED   500    // Specify the min openloop speed in rpm of motor
#define MIN_CL_MOTORSPEED   500    // Specify the min openloop speed in rpm of motor

#define MAX_DUTYCYCLE             (MPER*0.6)      //Specify maximum duty cycle for PWM
#define MIN_DUTYCYCLE             (MPER*0.1)      //Specify minimum duty cycle for PWM
#define MAX_TMR_COUNT              65535
// ***************************************************************************** 
#define TIMER_PRESCALER     64
// Period Calculation
// Period = (FCY / TIMER_PRESCALE) / (RPM * NO_POLEPAIRS )/10
#define MAXPERIOD	(unsigned long)(((float)FCY / (float)TIMER_PRESCALER) / (float)((MIN_OL_MOTORSPEED * POLEPAIRS)/10))	
#define MINPERIOD	(unsigned long)(((float)FCY / (float)TIMER_PRESCALER) / (float)((MAX_CL_MOTORSPEED * POLEPAIRS)/10))

//Maximum number of ticks in lowest speed for the counter used
#define PERIOD_CONSTANT  (unsigned long)((float)MAXPERIOD *(float)MIN_OL_MOTORSPEED) 

// *****************************************************************************
#define REVERSE_DROP_SPEED   800   // Speed to be reduced in rpm before reversing the direction
// ***************************************************************************** 
/** Constants for Mathematical Computation */
#define TICKS            FCY/(TIMER_PRESCALER)

/**  SPEED MULTIPLIER CALCULATION = ((FCY*60)/(TIMER_PRESCALER*POLEPAIRS))  */
#define SPEED_MULTI     (unsigned long)(((float)FCY/(float)(TIMER_PRESCALER*POLES))*(float)60) 
#define REV_SPEED_LIMIT   (unsigned long) ((float)(SPEED_MULTI)/(float)(REVERSE_DROP_SPEED*2))

//******************************************************************************
/** Velocity Control Loop Coefficients */
#define SPEEDCNTR_PTERM        Q15(0.9999)
#define SPEEDCNTR_ITERM        Q15(0.0009)
#define SPEEDCNTR_CTERM        Q15(0.9999)
#define SPEEDCNTR_OUTMAX       Q15(0.9999)
#define SPEEDCNTR_OUTMIN       Q15(0.0)

// *****************************************************************************
/** Moving Average - No of Samples*/
#define SPEED_MOVING_AVG_FILTER_SCALE      4
#define SPEED_MOVING_AVG_FILTER_SIZE       (uint16_t)(1 << SPEED_MOVING_AVG_FILTER_SCALE) 
// ***************************************************************************** 
//HANDLE TO CHANGE DEFAULT SPIN DIRECTION
#define RUN_DIRECTION   false         //boolean logic > true = 1, false = 0; CW



// *****************************************************************************
/** Overtemperature Detection */
// ***************************************************************************** 
#define ADCBUF_DSPICTEMP ADCBUF24
#define OVERTEMP_LIMITER 925 //55 degrees celsius limit
#define OVERTEMP_COUNTER 6120000 //5 minutes
uint16_t measureTemp;
uint32_t counterTemp;
uint16_t monitorTemp;

/**
  Section: Single Phase Sequence
**/
const uint16_t PWM_STATE1_CLKW[4] = {0x3400, 0x0000, 0x3000, 0x3000};
const uint16_t PWM_STATE2_CLKW[4] = {0x0000, 0x3400, 0x3000, 0x3000};
uint16_t PWM_STATE1[4];
uint16_t PWM_STATE2[4];


// *****************************************************************************
// Section: Enums, Structures
// *****************************************************************************
typedef struct {
    uint32_t speedAcc;
    int32_t calculatedSpeed;
    uint32_t speedValue;
    uint16_t index;
    int16_t buffer[SPEED_MOVING_AVG_FILTER_SIZE];
    int32_t sum;
    int16_t avg;
} MOVING_AVG_SPEED_T;

typedef struct {
    MC_PIPARMIN_T piSpeedCalculation;
    MC_PIPARMOUT_T piOutputSpeed;
} PI_CONTROLLER_T;

typedef struct
{
    uint16_t measure;
    uint16_t monitor;
    uint32_t counter;
} OVERTEMP_T;

typedef enum
{
    INIT             = 0,         /* Initialize Run time parameters */
    CMD_WAIT         = 1,         /* Wait for Run command */
    RUN              = 2,         /* Run the motor */
    CHANGE_DIRECTION = 3,         /* Change motor running direction */
    STOP             = 4,         /* Stop the motor */
} STATE_T;

#endif	/** USERPARAMS_H */