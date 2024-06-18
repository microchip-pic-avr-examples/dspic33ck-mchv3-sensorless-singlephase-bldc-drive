/*******************************************************************************
 *  User-defined Parameters for Sensorless Single Phase BLDCM 
 *  Constants, Sequence and Routine Header File
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

void InitializePWM(void);
void Drive_Initializations(void);
void MotorStart(void);
void RotationSwitchingTable(void);
void InitMovingAvgSpeed(void);
void PreCommutationState(void);
void CommutationState(void);
void OvercurrentDetection(void);
void TimeCapture(void);
void InitializeTimer(void);
void CorrectionPGDCTable(void);
void CorrectionTicksTable(void);
void PWMticksTable(void);
void PGDCTable(void);
uint32_t GetTimerBuffer(void);
void SetTimerPeriod(uint32_t value);
void SpeedEstimation(void);
void StateMachine(void);
void CorrectionPWMTickCounter(void);
void PositionCorrection(void);
void ForcedPWMTickCounter(void);
void ForcedCommutation(void);
void SpeedMonitoring(void);
void ClosedLoopPWMTickCounter(void);
void ClosedLoopPreCommutationState(void);
void MotorStop(void);
void InitializePhase (void);

#define MCHV3_VOLTAGE_BUS   300
#define MCLV2_VOLTAGE_BUS   24
#define PWM_MPER_10KHZ      4999
#define OVERCURRENT_DETECTION_ENABLE    1

/* Constants */
#define FCY                             100000000UL //System clock
#define SCCPTIMER_PRESCALER                 64 //SCCP timer prescaler
#define POLES                           4      // Number of poles 

//SCCP TIMER SPEED CALCULATION
#define PERIOD_CONSTANT                 0xFFFFFFFF//(unsigned long)((float)MAXPERIOD *(float)MIN_OL_MOTORSPEED) //SCCP Max period
#define PWMTICKS                        (float)0.000100 //50us
#define FREQHZ                          (float)(SCCPTIMER_PRESCALER*2)/FCY //used for measuredTicks
#define PERIODPERPULSE                  (float)FREQHZ/PWMTICKS //used for measuredTicks

//= 60 * FCY / 4 * 64 * timerValueDelta
//TIMER 1 SPEED ESTIMATION
#define TIMER1_PRESCALER            256 //Timer1 prescaler
#define TIMER1_PRESCALER_OVER_FCY   (float)(TIMER1_PRESCALER/FCY) //256/100MHz
//TIMER1BUFFER TO SPEED OR SPEED TO TIMER1BUFF
#define TARGETSPEED_TO_TIMER1TICKS_CONST    5859375 //(60*FCY)/(POLES*TIMER1_PRESCALER) = (60*100MHz)/(4*256)
//TARGETSPEED_TO_TIMER1TICKS_CONST = (60*INPUT FREQUENCY OF TIMER1) / (NUMBER OF POLES * TIMER1 PRESCALER)
//TARGETSPEED_TO_TIMER1TICKS_CONST = (60*100MHz) / (4*256) = 5859375
//Example: 5859375 / 400RPM = 14648
//Example: 5859375 / 14648 = 400RPM

//Overcurrent calc
//{[0.5/(4.32/32767)/32768]*[32768]}-0.5 = tempanswer
//tempanswer *2047 /2^15 + 2048 = 2284

//TARGET SPEED TO PWMTICKS CONVERSION
#define RPM_TO_TICKS       150000 // [ 60/(4*0.00005) ] / (RPM)
//Example: RPM_TO_TICKS / TargetSpeed = 300000 / 400RPM = 750 ticks 

/* INITIAL SPEED SETTING AT DEFAULT */
#define INITIALLY_SET_TO_ZERO                   0
#define INITIAL_PWMTICKSLIMIT                   1500
#define INITIAL_PWMTICKS                        1501
#define INITIAL_TARGET_SPEED                    500 //Set in INIT for final Target Speed
#define INITIAL_STARTUP_SPEED                   100 //Set in Anti Motor Coasting before Forced Commutation
#define FORCED_COMMUTATION_TARGET_TICKS         500 //Forced Commutation target PWMticks_level
#define FORCED_COMMUTATION_INITIAL_DUTY_CYCLE   1940
//#define FORCED_COMMUTATION_INITIAL_DUTY_CYCLE   ((float)(18.63))*(PWM_MPER_10KHZ/MCLV2_VOLTAGE_BUS)
#define FORCED_COMMUTATION_INITIAL_SPEED        300 //Forced Commutation Target Speed before transition to Closed Loop
#define NOMINAL_SPEED                           710
#define MINIMUM_SPEED                           300
/* Anti Motor Coasting constants */
#define ANTIMOTOR_COASTING_DELAY_IN_SECONDS     (uint8_t)2 //Anti motor coasting duration
#define COUNTS_FOR_DELAY                        (uint16_t)20000
#define COUNT_DELAY                             (uint16_t)(ANTIMOTOR_COASTING_DELAY_IN_SECONDS*COUNTS_FOR_DELAY) //10000 - conversion of seconds to pwm ticks DO NOT CHANGE

/* Linear Response Sensitivity */
#define LINEAR_RESPONSE_SENSITIVITY_SPEED       10 //10RPM is default = increments by 10 RPM during closed loop
#define LINEAR_RESPONSE_SENSITIVITY_DUTY_CYCLE  74 //closed loop speed increases/decreases by 0.672V

/* Speed Estimation Average Filter */
#define AVERAGE_INDEX_FILTER_SCALE              4
#define AVERAGE_INDEX_FILTER_SIZE               (uint16_t)(1 << AVERAGE_INDEX_FILTER_SCALE)   
uint8_t average_index;
uint16_t timer1_buffer_average[AVERAGE_INDEX_FILTER_SIZE];
uint32_t speed_sum;
uint16_t speed_average;

/* Commutation Drive Index Table */
#define COMMUTATION_ITERATION                   22 //total members of drive array
const uint16_t TICKS_TABLE[COMMUTATION_ITERATION] = {1500, 1500, 1364, 1250, 1154, 1071, 1000, 938, 882, 833, 789, 750, 714, 682, 652, 625, 600, 577, 556, 536, 517, 500};
const uint16_t PGDC_TABLE[COMMUTATION_ITERATION] = {800, 800, 880, 920, 960, 1000, 1040, 1080, 1120, 1160, 1200, 1240, 1300, 1360, 1420, 1480, 1540, 1620, 1700, 1780, 1860, 1940};
uint16_t TICKS_MEMBER[COMMUTATION_ITERATION];
uint16_t PGDC_MEMBER[COMMUTATION_ITERATION];

/* Rotor Position Correction Drive Index Table */
#define CORRECTION_ITERATION    3
const uint16_t CORRECTIONTICKS_TABLE[CORRECTION_ITERATION] = {0, 3500, 3500};
const uint16_t CORRECTIONPGDC_TABLE[CORRECTION_ITERATION] = {0, 800, 800};
uint16_t CORRECTIONTICKS_MEMBER[CORRECTION_ITERATION];
uint16_t CORRECTIONPGDC_MEMBER[CORRECTION_ITERATION];

/* Section: Single Phase Sequence */
const uint16_t PWM_STATE1_CLKW[2] = {0x3800, 0x6000}; //A TO B
const uint16_t PWM_STATE2_CLKW[2] = {0x6000, 0x3800}; //B TO A
uint16_t PWM_STATE1[2];
uint16_t PWM_STATE2[2];

/* LED SWITCHES */
#define DBG_LED1_ON     1

int16_t bemf_average;
#define INITIAL_BACK_EMF_THRESHOLD_ADJUSTMENT   400
#define INITIAL_BACK_EMF_THRESHOLD              3100
#define CLOSEDLOOP_BACK_EMF_THRESHOLD_ADJUSTMENT    1000
#define OVERCURRENT_SENSITIVITY_DETECTION  (float)(0.8) //triggers overcurrent flag when 80% of period is higher than threshold
#define OVERCURRENT_DETECTION_LIMIT             0x945

/* ADC buffers */
#define BEMF_VB     ADCBUF4
#define ADC_VBUS    ADCBUF18
#define IBUS_AN3    ADCBUF3

/* Firmware Parameters */

bool enable_delay, BEMF_status;
uint32_t enable_drive, timer1_buffer;
uint16_t StartMotor, RotorPreposition, RunMotor, OpenLoop, ClosedLoop, EndStartUp, StartUp2;
uint16_t MotorSpeedUp, MotorSpeedDown;
uint16_t PWMticks_forLevel, PGDC_forLevel, StepPeriod;


uint16_t cmp1, cmp3;
int16_t totalvoltagebus, va, vb, max_vb, ibus, max_ibus;
uint16_t MotorSpeedUp, MotorSpeedDown;

uint8_t ioc, once;

uint16_t NewTargetSpeed, TargetSpeed, previousTargetSpeed, PWMticks_forLevel, PGDC_forLevel, TargetSpeedReached, StartUpSpeed; //
int16_t UpdatePWMDutyCycle, CurrentPWMDutyCycle; //
uint16_t UpdatePWMticks_limit, PWMticks, PWMticks_limit, reference_Speed, measured_Speed, drive_index, temp; //
uint32_t pastTimerValue, timerValueDelta, measuredTicks, timerValue;
uint16_t commutationValue, repositiondelay; //
int16_t bemf_data, isense_data; //
bool cmp_data; //
uint16_t overcurrentCounter, Overcurrent_Flag;
uint32_t motorStallCounter = 0;
uint16_t ticks, correction_index, Startup_CurrentPWMDutyCycle; //     
uint8_t state, MotorStatus; //                 
uint16_t EstimatedSpeed, mper;
uint16_t UpdateSpeed;

typedef enum {
    INIT = 0, /* Initialize Run time parameters */
    CHARGE_BOOTSTRAP = 1, /* Charge bootstrap capacitors */
    POSITION_CORRECTION = 2, /* Rotor position correction */
    ANTI_MOTOR_COASTING = 3, /* Motor Brake */
    FORCED_COMMUTATION = 4, /* Start Up */
    CLOSED_LOOP = 5, /* Run the motor */
} STATEMACHINE_STATES;

typedef enum {
    MOTOR_STATUS_OFF = 0, /* Disables drive */
    MOTOR_STATUS_ON = 1, /* Enables drive */
} MOTOR_STATUS_STATES;


#endif	/** USERPARAMS_H */