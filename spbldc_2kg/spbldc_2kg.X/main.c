// <editor-fold defaultstate="collapsed" desc="© [2023] Microchip Technology Inc. and its subsidiaries.">
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
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Includes">
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/X2Cscope/X2Cscope.h"
#include "button_service.h"
#include "mcc_generated_files/peripheral/../system/pins.h"
#include "mcc_generated_files/peripheral/sccp1.h"
#include "mcc_generated_files/uart/uart1.h"
#include "hmi_uart.h"
#include "mcc_generated_files/cmp/cmp3.h"
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Kulthorn Declarations">

// *****************************************************************************
/** Motor Ratings (from Name Plate Details or Datasheet)*/
// ***************************************************************************** 
/* MCLV2 - RE8 - PIM#60 - DBG_LED1 - D2  */
/* MCHV3 - RE8 - PIM#60 - DBG_LED2 - D17 */
#define MOTOR_STARTED_LIGHT_INDICATOR   LED1_SetHigh()
#define MOTOR_STOPPED_LIGHT_INDICATOR   LED1_SetLow()

// *****************************************************************************
/** ADC RELATED */
// ***************************************************************************** 
#define ADC_POTENTIOMETER               ADCBUF19
#define MAX_DUTY_CYCLE                  (MPER-1)
#define DUTY_CYCLE_SENSITIVITY          MAX_DUTY_CYCLE - (int32_t)((__builtin_mulss(potentiometer_data,MAX_DUTY_CYCLE)>>12))
#define ADC_VA  ADCBUF20
#define ADC_VB  ADCBUF21

// *****************************************************************************
/** PWM RELATED */
// ***************************************************************************** 
#define OVERRIDE_PWM_TO_DISABLE_OUTPUT  0x3000
#define SET_DUTYCYCLE_TO_ZERO           0x0000
// *****************************************************************************
/** SPEED CALCULATION RELATED */
// ***************************************************************************** 
#define FCY                     100000000UL
#define TIMER_PRESCALER     64
#define MIN_OL_MOTORSPEED   300    // Specify the min openloop speed in rpm of motor
#define POLEPAIRS           2
#define POLES               4
#define MAXPERIOD	(unsigned long)(((float)FCY / (float)TIMER_PRESCALER) / (float)((MIN_OL_MOTORSPEED * POLEPAIRS)/10))	
#define SPEED_MULTI     (unsigned long)(((float)FCY/(float)(TIMER_PRESCALER*POLES))*(float)60) 
#define PWMTICKS            (float)0.00005
#define FREQHZ              (float)(TIMER_PRESCALER*2)/FCY
#define PERIODPERPULSE      (float)FREQHZ/PWMTICKS
#define SPEED_MOVING_AVG_FILTER_SCALE      4
#define SPEED_MOVING_AVG_FILTER_SIZE       (uint16_t)(1 << SPEED_MOVING_AVG_FILTER_SCALE) 
#define PERIOD_CONSTANT  0xFFFFFFFF//(unsigned long)((float)MAXPERIOD *(float)MIN_OL_MOTORSPEED) 
#define OVERCURRENT_COUNTER 10000
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

uint16_t overcurrentCounter;
uint16_t potentiometer_data, duty_cycle, va, vb;
bool StartMotor, RotorLocking, RunMotor, ForcedCommutation;
uint16_t measuredDutyCycle;
uint16_t inc, PWMticks, PWMticks_limit, commutationValue;
uint32_t pastTimerValue,timerValueDelta,measuredTicks,measuredSpeed, timerValue = 0;
int16_t desiredTicks[150];
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
void OverCurrent(void);
uint32_t GetTimerBuffer(void);
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="HMI Declarations">
static uint16_t UARTValue;
static uint16_t UART_L;
static uint16_t UART_H;
static uint16_t UARTSpeedV;
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Main Code">
int main(void)
{
    SYSTEM_Initialize();
    InitialParameters();
    BoardServiceInit();
    
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

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Kulthorn Firmware Code Functions">

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
    
    for(inc = 0; inc < 150; inc++)
    {
        desiredTicks[inc] = MPER*(0.1*(inc));
    }
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

void InitialParameters()
{
    /* Disable PWM */ 
    PG1IOCONL = PG2IOCONL = OVERRIDE_PWM_TO_DISABLE_OUTPUT;       
    PG1DC = PG2DC = SET_DUTYCYCLE_TO_ZERO;
    
    /* Reset Motor parameters*/
    StartMotor = false;                     //StartMotor tells if motor control procedure has started
    RotorLocking = false;                   //Rotor Locking runs the motor to realign the rotor
    RunMotor = false;                       //RunMotor tells if motor has started moving
    ForcedCommutation = false;              //Forced Commutation is the sensorless control to drive the motor
    measuredDutyCycle = SET_DUTYCYCLE_TO_ZERO;//Initializes duty cycle
    PWMticks = 0;                           //PWM ticks increments up to the period of half an electrical cycle
    SetTimerPeriod(PERIOD_CONSTANT);        //Sets the SCCP1 timer period
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
            if(PWMticks < 0x1388) //40,000 ticks = 2 seconds of rotor locking
            {
                PWMticks++;
                measuredDutyCycle = MPER-1;
                PG1DC = PG2DC = measuredDutyCycle;
                PG1IOCONL = 0x3400;
                PG2IOCONL = 0x0000;
            }
            else if(PWMticks < 0x2710) //40,000 ticks = 2 seconds of rotor locking
            {
                PWMticks++;
                measuredDutyCycle = MPER-1;
                PG1DC = PG2DC = measuredDutyCycle;
                PG2IOCONL = 0x3400;
                PG1IOCONL = 0x0000;
            }
            else if(PWMticks < 0x3A98) //40,000 ticks = 2 seconds of rotor locking
            {
                PWMticks++;
                measuredDutyCycle = MPER-1;
                PG1DC = PG2DC = measuredDutyCycle;
                PG1IOCONL = 0x3400;
                PG2IOCONL = 0x0000;
            }
            else if(PWMticks < 0x4E20) //40,000 ticks = 2 seconds of rotor locking
            {
                PWMticks++;
                measuredDutyCycle = MPER-1;
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
                            measuredDutyCycle = MPER-1;
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

void OverCurrent(void)
{
    if (CMP3_StatusGet())
    {
        if (overcurrentCounter > OVERCURRENT_COUNTER)
        {
            MotorReset();
            overcurrentCounter = 0;
        }
        overcurrentCounter++;
    }
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

    UARTSpeedV = (measuredSpeed << 2);
    UARTSpeedV = (UARTSpeedV >> 1);
    UART_H = (UARTSpeedV >> 8) & 0xFF;
    UART_L = (UARTSpeedV & 0xFF) | 0x80;
    
    UART1_Write (UART_L);
    UART1_Write (UART_H);
}
// </editor-fold>
