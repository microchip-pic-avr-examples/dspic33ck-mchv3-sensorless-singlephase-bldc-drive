/*
© [2023] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/X2Cscope/X2Cscope.h"
#include "button_service.h"
#include "mcc_generated_files/peripheral/../system/pins.h"
#include "mcc_generated_files/peripheral/sccp1.h"

// *****************************************************************************
/** MOTOR PARAMETERS*/
// ***************************************************************************** 
#define POLEPAIRS           2
#define POLES               4

// *****************************************************************************
/** MOTOR USER INTERFACE */
// ***************************************************************************** 
/* MCLV2 - RE8 - PIM#60 - DBG_LED1 - D2  */
/* MCHV3 - RE8 - PIM#60 - DBG_LED2 - D17 */
#define MOTOR_STARTED_LIGHT_INDICATOR   LED1_SetHigh()
#define MOTOR_STOPPED_LIGHT_INDICATOR   LED1_SetLow()

// *****************************************************************************
/** ADC CONSTANTS AND DEFINES*/
// ***************************************************************************** 
#define ADC_POTENTIOMETER               ADCBUF19
#define MAX_DUTY_CYCLE                  (MPER*0.2)
#define DUTY_CYCLE_SENSITIVITY          MAX_DUTY_CYCLE - (int32_t)((__builtin_mulss(potentiometer_data,MAX_DUTY_CYCLE)>>12))
#define ADC_VA  ADCBUF20
#define ADC_VB  ADCBUF21

// *****************************************************************************
/** PWM SEQUENCE */
// ***************************************************************************** 
const uint16_t PWM_STATE1[2] = {0x3400, 0x0000};
const uint16_t PWM_STATE2[2] = {0x0000, 0x3400};

// *****************************************************************************
/** SPEED CALCULATION */
// ***************************************************************************** 
#define FCY                             100000000UL
#define TIMER_PRESCALER                 64  
#define SPEED_MOVING_AVG_FILTER_SCALE   4
#define SPEED_MOVING_AVG_FILTER_SIZE    (uint16_t)(1 << SPEED_MOVING_AVG_FILTER_SCALE) 
#define MIN_OL_MOTORSPEED               300
#define MAXPERIOD                       (unsigned long)(((float)FCY / (float)TIMER_PRESCALER) / (float)((MIN_OL_MOTORSPEED * POLEPAIRS)/10))	
#define SPEED_MULTI                     (unsigned long)(((float)FCY/(float)(TIMER_PRESCALER*POLES))*(float)60) 
#define PWMTICKS                        (float)0.00005
#define FREQHZ                          (float)(TIMER_PRESCALER*2)/FCY
#define PERIODPERPULSE                  (float)FREQHZ/PWMTICKS
#define PERIOD_CONSTANT                 0xFFFFFFFF//(unsigned long)((float)MAXPERIOD *(float)MIN_OL_MOTORSPEED) 

typedef struct {
    uint32_t speedAcc;
    int32_t calculatedSpeed;
    uint32_t speedValue;
    uint16_t index;
    int16_t buffer[SPEED_MOVING_AVG_FILTER_SIZE];
    int32_t sum;
    int16_t avg;
} MOVING_AVG_SPEED_T;
MOVING_AVG_SPEED_T movingAvgSpeed;

// State Machine Procedures
bool StartMotor, RotorLocking, RunMotor, ForcedCommutation;
// ADC data
uint16_t potentiometer_data, duty_cycle, va, vb;
// PWM duty cycle output measurement
uint16_t measuredDutyCycle;
// PWM cycles, limit of PWM cycles, commutation sequence
uint16_t PWMticks, PWMticks_limit, commutationValue;
// Speed measurement parameters and output speed monitoring
uint32_t pastTimerValue,timerValueDelta,measuredTicks, timerValue, measuredSpeed;

void MotorInitialize(void);
void MotorReset(void);
void InitialParameters(void);
void PreCommutationState(void);
void StartUpProcedure(void);
void CommutationState(void);
void SetTimerPeriod(uint32_t value);
void TimeCapture(void);
void CalcMovingAvgSpeed(int32_t instCount);
void InitMovingAvgSpeed(void);
uint32_t GetTimerBuffer(void);

/*
    Main application
*/

int main(void)
{
    SYSTEM_Initialize();
    InitialParameters();
    BoardServiceInit();
    SetTimerPeriod(PERIOD_CONSTANT);
    
    while(1)
    {
        X2CScope_Communicate();
        BoardService();
        
        if(IsPressed_Button1())
        {
           if(!StartMotor)
           {
               MotorInitialize();
           }
           else
           {
               MotorReset();
           }
        }
    }    
}

/* Function:
    MotorInitialize()
  Summary:
    This routine starts the motor control procedures
  Description:
    LED indicator turns on to signal that the firmware is running
  Precondition:
    None.
  Parameters:
    None
  Returns:
    None.
  Remarks:
    None.
 */

void MotorInitialize()
{
    MOTOR_STARTED_LIGHT_INDICATOR;
    StartMotor = true;
    RotorLocking = true;
    ForcedCommutation = false;
}

/* Function:
    MotorReset()
  Summary:
    This routine terminates all motor control procedures and resets all parameters
  Description:
    LED indicator turns off to signal that the firmware is terminated
  Precondition:
    None.
  Parameters:
    None
  Returns:
    None.
  Remarks:
    None.
 */

void MotorReset()
{
    MOTOR_STOPPED_LIGHT_INDICATOR;
    InitialParameters();
}

/* Function:
    SetTimerPeriod()
  Summary:
    This routine sets the assigned value to the SCCP1 Period register
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

void SetTimerPeriod(uint32_t value)
{
    CCP1PRL = (value & 0x0000FFFF);
    CCP1PRH = ((value & 0xFFFF0000)>>16);
}

/* Function:
    GetTimerBuffer()
  Summary:
    This routine returns the value of the SCCP1 timer buffer register
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

uint32_t GetTimerBuffer(void)
{
    uint32_t counterVal = 0xFFFFFFFF;
    counterVal = (((uint32_t)CCP1TMRH <<16) | (CCP1TMRL));
    return counterVal;
}

/* Interrupt:
    ADC19 Interrupt Routine
  Summary:
    This interrupt is called every PWM cycle (50us) to change the PWM control over the motor
  Description:
    None
  Precondition:
    None.
  Parameters:
    None
  Returns:
    None.
  Remarks:
    None.
 */

void InitialParameters()
{
    /* Disable PWM */ 
    PG1IOCONL = 0x3000;
    PG2IOCONL = 0x3000;
    PG1DC = PG2DC = 0x0000;

    /* Reset Motor parameters*/
    StartMotor = false;
    RotorLocking = false;
    RunMotor = false;
    ForcedCommutation = false;
    measuredDutyCycle = 0;
    PWMticks = 0;  
    pastTimerValue = 0;
    timerValueDelta = 0;
    measuredTicks = 0;
    timerValue = 0;
    measuredSpeed = 0;
}

/* Interrupt:
    ADC19 Interrupt Routine
  Summary:
    This interrupt is called every PWM cycle (50us) to change the PWM control over the motor
  Description:
    Includes Rotor Locking and Forced Commutation procedures
  Precondition:
    None.
  Parameters:
    None
  Returns:
    None.
  Remarks:
    None.
 */

void __attribute__ ( ( __interrupt__ , auto_psv) ) _ADCAN19Interrupt ( void )
{
    /* Potentiometer reading from ADC */
    potentiometer_data = ADC_POTENTIOMETER; // ADC reading of potentiometer
    duty_cycle = DUTY_CYCLE_SENSITIVITY; // Calculated duty cycle value
    
    /* Motor Phase 1 and 2 Back EMF reading from ADC */
    va = ADC_VA;
    vb = ADC_VB;
    
    /* Motor Procedure during ADC Interrupt */
    if(StartMotor)
    {
        if(RotorLocking)
        {
            if(PWMticks < 0x1388) //5,000 ticks 
            {
                PWMticks++;
                measuredDutyCycle = MPER*0.04;
                PG1DC = PG2DC = measuredDutyCycle;
                PG1IOCONL = 0x3400;
                PG2IOCONL = 0x0000;
            }
            else if(PWMticks < 0x2710) //10,000 ticks 
            {
                PWMticks++;
                measuredDutyCycle = MPER*0.06;
                PG1DC = PG2DC = measuredDutyCycle;
                PG2IOCONL = 0x3400;
                PG1IOCONL = 0x0000;
            }
            else if(PWMticks < 0x3A98) //15,000 ticks
            {
                PWMticks++;
                measuredDutyCycle = MPER*0.08;
                PG1DC = PG2DC = measuredDutyCycle;
                PG1IOCONL = 0x3400;
                PG2IOCONL = 0x0000;
            }
            else if(PWMticks < 0x4E20) //20,000 ticks 
            {
                PWMticks++;
                measuredDutyCycle = MPER*0.1;
                PG1DC = PG2DC = measuredDutyCycle;
                PG2IOCONL = 0x3400;
                PG1IOCONL = 0x0000;
            }
            else
            {
                RotorLocking = false;
                RunMotor = true;
                ForcedCommutation = true;
                PWMticks = 3001; // OPENLOOP_MAX_PWM_TICKS
                PWMticks_limit = 3000;
                commutationValue = 0;
            }
        }
        
        if(RunMotor)
        {
            if(ForcedCommutation)
            {
                if(++PWMticks > PWMticks_limit)
                {
                    PWMticks = 0;
                    if (++commutationValue > 1)
                    {
                        commutationValue = 0;
                        if (PWMticks_limit > 1000)
                        {
                            PWMticks_limit -= 200;
                            measuredDutyCycle = MPER*0.12;
                        }
                    }
                }
                
                PG1DC = PG2DC = measuredDutyCycle;
                TimeCapture();
                CommutationState();  
            }
        }
    }
    X2CScope_Update();
    BoardServiceStepIsr();
    IFS6bits.ADCAN19IF = 0; // Clear the Channel_AN19 interrupt flag
}

/* Function:
    CommutationState()
  Summary:
    This routine controls the PWM override registers based on the position of the rotor.
  Description:
    Position of rotor is represented by commutationValue.
  Precondition:
    None.
  Parameters:
    None
  Returns:
    None.
  Remarks:
    None.
 */

void CommutationState(void)
{
    if(commutationValue)
    {
        PG2IOCONL = 0x0000;
        PG1IOCONL = 0x3400;
    }
    else
    {
        PG2IOCONL = 0x3400;
        PG1IOCONL = 0x0000;
    }
}

/* Function:
    TimeCapture()
  Summary:
    This routine measures the period of one electrical cycle.
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

void TimeCapture(void)
{
    timerValue = GetTimerBuffer();
    
    if (timerValue <= pastTimerValue)
    {
        timerValueDelta = (PERIOD_CONSTANT - (pastTimerValue - timerValue)) * 1000;
    }
    else
    {
        timerValueDelta = (timerValue - pastTimerValue) * 1000;
    }
    pastTimerValue = timerValue;
    CalcMovingAvgSpeed(timerValueDelta);
    measuredTicks = timerValueDelta * 500 * PERIODPERPULSE;
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
    movingAvgSpeed.calculatedSpeed = (int32_t) (SPEED_MULTI/movingAvgSpeed.speedValue);
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