/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.0
        Device            :  dsPIC33CK256MP508
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.70
        MPLAB 	          :  MPLAB X v5.50
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  TITLE: KULTHORN PROJECT - HIGH VOLTAGE SINGLE PHASE BLDC MOTOR DRIVER
**/

/**
  Section: Included Files
**/

#include <xc.h>
#include <stdbool.h>
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/X2Cscope/X2Cscope.h"
#include "mcc_generated_files/sccp3_tmr.h"
#include "mcc_generated_files/pwm.h"
#include "mcc_generated_files/adc1.h"
#include "userparams.h"
#include "button_service.h"

/**
  Section: Variables
**/

bool runMotor = 0;
bool runDirection = 0;
uint16_t StartMotor;
uint16_t RotorLocking;
uint16_t RunMotor;
uint16_t ForcedCommutation;
uint16_t commValue;
int16_t  CurrentPWMDutyCycle;
uint32_t PWMticks;  
uint32_t PWMticks_limit;  
uint16_t va, vb, x, y;
int16_t inputReference = 0;
uint16_t desiredDC = 0;
uint16_t upperDClimit, lowerDClimit = 0;
uint32_t temp;
uint16_t desiredSpeed;
uint32_t desiredPeriod;
int32_t desiredPeriodPI;
uint16_t desiredTicks, measuredTicks;
uint32_t timerValue,pastTimerValue,timerValueDelta;
uint16_t dutyCycle; 
uint16_t measuredSpeed, cmpvalue;
uint32_t overcurrentCounter = 0;
MOVING_AVG_SPEED_T movingAvgSpeed;
PI_CONTROLLER_T PI_Input;
#define ADCVB ADCBUF21
#define ADCVA ADCBUF20
#define OVERCURRENT_COUNTER 10000 //immediately
#define OVERCURRENT_DETECT DAC1CONLbits.CMPSTAT
/**
  Section: Function Declarations
**/

void InitializePWM(void);
void ADC_ISR(void);
void RotationSwitchingTable();
void MotorStart(void);
void ADC_ISR(void);
void CommutationState(void);
void PreCommutationState(void);
void MotorStop(void);
void CalcMovingAvgSpeed(int32_t instSpeed);
void InitMovingAvgSpeed(void);
void TimeCapture(void);
void PeriodAdjust(void);
void OverTemperature(void);
void OverCurrent(void);
/*
 *  MAIN APPLICATION
 */

/* Function:
    main()
  Summary:
    This routine controls the motor control operations via switches.
  Description:
    Initializes the system.
    Starts and stops and changes the drive direction of the motor.
  Precondition:
    None.
  Parameters:
    None
  Returns:
    1
  Remarks:
    None.
 */
int main(void)
{
    SYSTEM_Initialize();
    InitializePWM();
    ADC1_SetPOT1InterruptHandler(ADC_ISR);
    SCCP3_TMR_Period32BitSet(PERIOD_CONSTANT);
    BoardServiceInit();
    LED2_SetHigh(); //Device is programmed Indicator
    while (1)
    {
        X2CScope_Communicate();
        BoardService();
        
        if(StartStop())
        {
            if(runMotor == 0)
            {
                MotorStart();
                LED1_SetHigh(); //ON indicator
            }
            else
            {
                MotorStop();
                LED1_SetLow(); //OFF indicator
            }
        }
    }
    return 1; 
}


void MotorStart(void)
{
    runMotor = 1;
    StartMotor = 1;
    RotorLocking = 1;
    RunMotor = 0;
    ForcedCommutation = 0;
    commValue = 0;
    CurrentPWMDutyCycle = MPER*0.14; //ROTOR_LOCKING_DUTY; 
    PWMticks = 0;            
    PWMticks_limit = 2100;
    InitMovingAvgSpeed();
    RotationSwitchingTable();
    x=0;
}

void ADC_ISR(void)
{
    //Back EMF reading; not really needed, just for monitoring
    vb = ADCVB;
    va = ADCVA;
    
    inputReference = ADCBUF_POTENTIOMETER;
    desiredDC =(uint16_t)MAX_DUTYCYCLE - ((__builtin_mulss(inputReference,MAX_DUTYCYCLE)>>12));
    temp =  (int32_t)(__builtin_muluu(desiredDC,(MAX_CL_MOTORSPEED - MIN_CL_MOTORSPEED)));
    desiredSpeed = (int16_t)((__builtin_divud(temp,(MAX_DUTYCYCLE)))+ MIN_CL_MOTORSPEED);
    desiredPeriod = (inputReference*PERIOD_MULTI)+MIN_PERIOD; //or MAX_PERIOD + (inputReference*PERIOD_MULTI); 
    desiredTicks = (PERIODPERPULSE)*desiredPeriod;

    if(desiredTicks > 1000)
    {
        upperDClimit = MPER*0.15;
        lowerDClimit = MPER*0.1;
    }
    else if(desiredTicks > 900)
    {
        upperDClimit = MPER*0.2;
        lowerDClimit = MPER*0.15;
    }
    else if(desiredTicks > 800)
    {
        upperDClimit = MPER*0.25;
        lowerDClimit = MPER*0.2;
    }
    else if(desiredTicks > 700)
    {
        upperDClimit = MPER*0.3;
        lowerDClimit = MPER*0.25;
    }
    else if(desiredTicks > 600)
    {
        upperDClimit = MPER*0.35;
        lowerDClimit = MPER*0.3;
    }
    else if(desiredTicks > 500)
    {
        upperDClimit = MPER*0.4;
        lowerDClimit = MPER*0.35;
    }
    else if(desiredTicks > 450)
    {
        upperDClimit = MPER*0.45;
        lowerDClimit = MPER*0.4;
    }
    else if(desiredTicks > 400)
    {
        upperDClimit = MPER*0.48;
        lowerDClimit = MPER*0.45;
    }
    else if (desiredTicks > 350)
    {
        upperDClimit = MPER*0.51;
        lowerDClimit = MPER*0.48;
    }
    else if(desiredTicks > 300)
    {
        upperDClimit = MPER*0.54;
        lowerDClimit = MPER*0.51;
    }
    else if(desiredTicks > 275)
    {
        upperDClimit = MPER*0.57;
        lowerDClimit = MPER*0.54;
    }
    
    //STEP 1: START MOTOR AND ROTOR LOCK
    if(StartMotor)
    {
        if(RotorLocking) //Rotor Locking Sequence removed
        {
            if(PWMticks < 0x9C40) //40,000 ticks = 2 seconds of rotor locking
            {
                PWMticks++;
                CurrentPWMDutyCycle = MPER*0.05;
                PG1DC = PG2DC = CurrentPWMDutyCycle;
                PG1IOCONL = 0x3400;
                PG2IOCONL = 0x0000;
            }
            else
            {
                RotorLocking = 0;
                RunMotor = 1;
                ForcedCommutation = 1;
                PWMticks = 2101; // OPENLOOP_MAX_PWM_TICKS
                commValue = 0;
            }
        }
     //STEP 2: START OF FORCED COMMUTATION
        if(RunMotor)
        {
            if (ForcedCommutation)
            {
                if (PWMticks_limit > 2100)
                {
                    CurrentPWMDutyCycle = MPER*0.14;
                }   
            }
            PG1DC = PG2DC = CurrentPWMDutyCycle; //CurrentPWMDutyCycle;
            ++PWMticks; 
            
            if(++PWMticks > PWMticks_limit)
            {
                PreCommutationState();
            }
        }
    }
    BoardServiceStepIsr();
    X2CScope_Update();
    IFS6bits.ADCAN11IF = 0;
}


void PreCommutationState(void)
{
    PWMticks = 0;
    if (++commValue > 1)
    {
        commValue = 0;
        if(ForcedCommutation == 1)
        {
            if (PWMticks_limit > 2000)
            {
                PWMticks_limit -= 10;
                CurrentPWMDutyCycle = MPER*0.15;
            }
            else if (PWMticks_limit > 1500)
            {
                PWMticks_limit -= 10;
                CurrentPWMDutyCycle = MPER*0.15;
            }
            else if (PWMticks_limit > 1200)
            {
                PWMticks_limit -= 10;
                CurrentPWMDutyCycle = MPER*0.15;
            }
        }
        
    if(x<200)
    {
        x++;
    }
        
    else
    {
        ForcedCommutation = 0;
        if (measuredTicks > desiredTicks) //PWMticks_limit > 800
        {
            PWMticks_limit -= 1;
            if(CurrentPWMDutyCycle < desiredDC)
            {
                if(CurrentPWMDutyCycle > upperDClimit)
                {
                    CurrentPWMDutyCycle = upperDClimit;
                }
                else
                {
                    CurrentPWMDutyCycle += 2;
                }
            }
        }
        if (measuredTicks < desiredTicks) //PWMticks_limit > 800
        {
            PWMticks_limit += 1;
            if(CurrentPWMDutyCycle > desiredDC)
            {
                if(CurrentPWMDutyCycle > upperDClimit)
                {
                    CurrentPWMDutyCycle = upperDClimit;
                }
                else
                {
                    CurrentPWMDutyCycle -= 2;
                }
            }
        }
    }
    }
    CommutationState();  
}


void TimeCapture(void)
{
    timerValue = SCCP3_TMR_Counter32BitGet();
    
    if (timerValue <= pastTimerValue)
    {
        timerValueDelta = PERIOD_CONSTANT - (pastTimerValue - timerValue);
    }
    else
    {
        timerValueDelta = timerValue - pastTimerValue;
    }
    pastTimerValue = timerValue;
    CalcMovingAvgSpeed(timerValueDelta);
    measuredTicks = timerValueDelta * PERIODPERPULSE;
}

void CommutationState(void)
{
    if(commValue == 1)
    {
        TimeCapture();
    }
    else
    {
        TimeCapture();
    }
    PG1IOCONL = PWM_STATE1[commValue];
    PG2IOCONL = PWM_STATE2[commValue];
    //dutyCycle = PG1DC; //for checking in x2cscope
}


/* Function:
    InitializePWM()
  Summary:
    This routine initializes the PWM Generators and duty cycle prior
    to enabling the PWM Generator
  Description:
    Overrides the PWM generators to disable PWM output. 
  Precondition:
    None.
  Parameters:
    None
  Returns:
    None.
  Remarks:
    None.
 */
void InitializePWM(void)
{
    PG1IOCONL = PG2IOCONL = 0x3000;
    PG1DC = PG2DC = 0x0000;
	PG1CONLbits.ON = PG2CONLbits.ON = 1;
}

/* Function:
    HALL_ISR()
  Summary:
    This routine defines the Hall Sensor State and finds parameters for the
    calculation of moving average speed.
  Description:
    Designates high or low hall value,\ and calculates timer value for 
    moving average speed.
  Precondition:
    None.
  Parameters:
    None
  Returns:
    None.
  Remarks:
    None.
 */

void RotationSwitchingTable()
{
	uint16_t arrayIndex = 0;
    
	if(runDirection == 0)
    {
	    for(arrayIndex = 0; arrayIndex < 4; arrayIndex++)
        {
            PWM_STATE1[arrayIndex] = PWM_STATE1_CLKW[arrayIndex];
            PWM_STATE2[arrayIndex] = PWM_STATE2_CLKW[arrayIndex];
        }
	}
    else
    {
		for(arrayIndex = 0;arrayIndex < 4; arrayIndex++)
        {
            PWM_STATE1[arrayIndex] = PWM_STATE1_CLKW[3-arrayIndex];
            PWM_STATE2[arrayIndex] = PWM_STATE2_CLKW[3-arrayIndex];
        }
    }
}


void MotorStop(void)
{
    StartMotor = 0;
    RotorLocking = 0;
    RunMotor = 0;      // reset run flag
    ForcedCommutation = 0;
    CurrentPWMDutyCycle = 1; // Set PWM to the min value
    PG1DC = PG2DC = 0;
    PG1IOCONL = 0x3000;
    PG2IOCONL = 0x3000;
    runMotor = 0;
}

void PWMDisableOutputs(void)
{
    PG2DC = 0;      
    PG1DC = 0;

    PG2IOCONLbits.OVRDAT = 0;  // Low State for PWM2H,L, if Override is Enabled
    PG1IOCONLbits.OVRDAT = 0;  // Low State for PWM1H,L, if Override is Enabled

    PG2IOCONLbits.OVRENH = 1;  // OVRDAT<1> provides data for output on PWM2H
    PG1IOCONLbits.OVRENH = 1;  // OVRDAT<1> provides data for output on PWM1H

    PG2IOCONLbits.OVRENL = 1;  // OVRDAT<0> provides data for output on PWM2L
    PG1IOCONLbits.OVRENL = 1;  // OVRDAT<0> provides data for output on PWM1L
}

/* Function:
    PWMEnableOutputs()
  Summary:
    This routine enables the PWM Generators.
  Description:
    Overrides the PWM Generators to enable outputs to continue 
    motor driving operations.
  Precondition:
    None.
  Parameters:
    None
  Returns:
    None.
  Remarks:
    None.
 */
void PWMEnableOutputs(void)
{
    PG2DC = 0;      
    PG1DC = 0;

    PG2IOCONLbits.OVRENH = 0;  // PWM generator provides data for PWM2H pin
    PG1IOCONLbits.OVRENH = 0;  // PWM generator provides data for PWM1H pin

    PG2IOCONLbits.OVRENL = 0;  // PWM generator provides data for PWM2L pin
    PG1IOCONLbits.OVRENL = 0;  // PWM generator provides data for PWM1L pin
}

/* Function:
    InitMovingAvgSpeed()
  Summary:
    This routine initializes parameters for moving average speed calculation.
  Description:
    None.
  Precondition:
    None.
  Parameters:
    None
  Returns:
    None.
  Remarks:
    None.
 */
void InitMovingAvgSpeed(void)
{
    uint16_t i;

    for (i = 0; i < SPEED_MOVING_AVG_FILTER_SIZE; i++) {
        movingAvgSpeed.buffer[i] = 0;
    }

    movingAvgSpeed.index = 0;
    movingAvgSpeed.sum = 0;
    movingAvgSpeed.avg = 0;
}

/* Function:
    CalcMovingAvgSpeed()
  Summary:
    This routine calculates the moving average speed of the motor.
  Description:
    None.
  Precondition:
    None.
  Parameters:
    None
  Returns:
    None.
  Remarks:
    None.
 */
void CalcMovingAvgSpeed(int32_t instCount) 
{
    uint16_t i;
    
    movingAvgSpeed.speedValue =  instCount;
    movingAvgSpeed.calculatedSpeed = (int32_t) (__builtin_divud(SPEED_MULTI, movingAvgSpeed.speedValue));
    movingAvgSpeed.buffer[movingAvgSpeed.index] = movingAvgSpeed.calculatedSpeed;
    movingAvgSpeed.index++;
    if (movingAvgSpeed.index >= SPEED_MOVING_AVG_FILTER_SIZE)
    {
        movingAvgSpeed.index = 0;
    }
    movingAvgSpeed.sum = 0;
    for (i = 0; i < SPEED_MOVING_AVG_FILTER_SIZE; i++) 
    {
        movingAvgSpeed.sum = movingAvgSpeed.sum + movingAvgSpeed.buffer[i];
        movingAvgSpeed.avg = movingAvgSpeed.sum >> SPEED_MOVING_AVG_FILTER_SCALE;
    }
    measuredSpeed = movingAvgSpeed.avg;
}



void OverTemperature(void)
{
    measureTemp = ADCBUF_DSPICTEMP;
    if(measureTemp < OVERTEMP_LIMITER)
    {
        counterTemp++;
        if (counterTemp > OVERTEMP_COUNTER)
        {
            MotorStop();
            counterTemp = 0;
        }
    }
}
/* Constants For Overcurrent Protection */


void OverCurrent(void)
{
    if (OVERCURRENT_DETECT)
    {
        if (overcurrentCounter > OVERCURRENT_COUNTER)
        {
            MotorStop();
            overcurrentCounter = 0;
        }
        overcurrentCounter++;
    }
}


/**
 End of File
*/
