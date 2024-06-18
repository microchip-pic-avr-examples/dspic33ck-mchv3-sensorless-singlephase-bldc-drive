// MCLV2 FAN BLADE CODE 48V
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
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.4
        Device            :  dsPIC33CK256MP508
    The generated drivers are tested against the following:
        Compiler          :  XC16 v2.10
        MPLAB 	          :  MPLAB X v6.05
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
  Section: Included Files
 */
#include <xc.h>
#include <stdbool.h>
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/adc1.h"
#include "mcc_generated_files/sccp1_tmr.h"
#include "mcc_generated_files/sccp2_tmr.h"
#include "mcc_generated_files/cmp1.h"
#include "mcc_generated_files/cmp3.h"
#include "mcc_generated_files/X2Cscope/X2Cscope.h"
#include "userparams.h"
#include "button_service.h"
/******************************************************************************/
/*
                         Main application
*/
/******************************************************************************/
int main(void) 
{
    /* DEVICE INITIALIZATIONS */
    SYSTEM_Initialize();
    Drive_Initializations();
    BoardServiceInit();
    while (1) 
    {
        X2CScope_Communicate();
        BoardService();
        
        if(StartStop())
        {
            if(!enable_drive)
            {
                MotorStatus = MOTOR_STATUS_ON;
                LED_RE8_SetHigh();
                enable_drive = 1;
            }
            else
            {
                MotorStop();
                MotorStatus = MOTOR_STATUS_OFF;
                LED_RE8_SetLow();
                enable_drive = 0;
            }
        }
    }
    return 1;
}

void Drive_Initializations(void)
{
    /* Drive Functions */
    InitializePhase();
    InitializePWM();
    InitializeTimer();
    CorrectionPGDCTable();
    CorrectionTicksTable();
    PWMticksTable();
    PGDCTable();
    RotationSwitchingTable();
    CMP1_SetDACDataHighValue(OVERCURRENT_DETECTION_LIMIT);
    state = INIT;
    /* ADC ISR parameters for Back EMF and current sense monitoring */
    vb = 0;
    ibus = 0;
    totalvoltagebus = 0;
    LED_RE9_SetHigh();
    cmp1 = 0;
    cmp3 = 0;
    ioc = 1; //cmp isr bemf status flag
    once = 0;
}

void __attribute__ ( ( __interrupt__ , auto_psv, weak ) ) _ADCAN3Interrupt ( void )
{
    ibus = IBUS_AN3;
    
    if(ibus > max_ibus)
    {
        max_ibus = ibus;
    }
    //clear the AN3_IBUS interrupt flag
    IFS5bits.ADCAN3IF = 0;
}

void __attribute__((__interrupt__, auto_psv)) _ADCAN4Interrupt(void) 
{
    vb = BEMF_VB;
    totalvoltagebus = ADC_VBUS;
    
    if(vb > max_vb)
    {
        max_vb = vb;
    }
    
    if((PG1STATbits.CAHALF == 1) || (PG2STATbits.CAHALF == 1))
    {
        IFS0bits.T1IF = true;
    }
    
    IFS5bits.ADCAN4IF = 0;
    StateMachine();
    X2CScope_Update();
}

void StateMachine(void) {
    switch (state) 
    {
        case INIT: //INIT: resets all parameters to 0 or constants
            /* Rotor Position Correction Parameters */
            ticks = 0;
            correction_index = 0;
            repositiondelay = 0;
            /* Run Time Parameters */
            PWMticks_forLevel = 0;
            PGDC_forLevel = 0;
            drive_index = 0;
            PWMticks = INITIAL_PWMTICKS;
            PWMticks_limit = INITIAL_PWMTICKSLIMIT;
            TargetSpeed = INITIAL_TARGET_SPEED; //400RPM
            previousTargetSpeed = 0;
            MotorSpeedUp = 0;
            MotorSpeedDown = 0;
            UpdateSpeed = FORCED_COMMUTATION_INITIAL_SPEED; //300RPM = closed loop starting speed
            UpdatePWMDutyCycle = FORCED_COMMUTATION_INITIAL_DUTY_CYCLE;
            //CMP3_SetDACDataHighValue(INITIAL_BACK_EMF_THRESHOLD);
            /* Measured Speed Calculation */
            EstimatedSpeed = 0;
            temp = 0;
            timer1_buffer = 0;
            average_index = 0;
            commutationValue = 0; //do not change needs to be set as 0
            if (MotorStatus) 
            {
                state = CHARGE_BOOTSTRAP;
            }
            break;

        case CHARGE_BOOTSTRAP:
        CurrentPWMDutyCycle = 1;
        PG1DC = PG2DC = CurrentPWMDutyCycle;
        PG1IOCONL = PG2IOCONL = 0x3400; //Set PWMxL to high
        if (repositiondelay < COUNT_DELAY) 
        {
            repositiondelay++;
        } 
        else 
        {
            /* Reset PWMticks to 100RPM for FORCED COMMUTATION*/
            PWMticks = INITIAL_PWMTICKS;
            PWMticks_limit = INITIAL_PWMTICKSLIMIT;
            drive_index = 0;
            commutationValue = 1; //needs to be set as 1
            StartUpSpeed = INITIAL_STARTUP_SPEED; //100RPM = Start up starts
            repositiondelay = 0;
            state = POSITION_CORRECTION;
        }

        break;
            
        case POSITION_CORRECTION:
            if (drive_index > 1) ///output: 2 loops??
            {
                bemf_average = max_vb - INITIAL_BACK_EMF_THRESHOLD_ADJUSTMENT;
                CMP3_SetDACDataHighValue(bemf_average);
                state = ANTI_MOTOR_COASTING;
            }
            CorrectionPWMTickCounter(); //output 1 loop: drive_index = 1
            break;

        case ANTI_MOTOR_COASTING:
            CurrentPWMDutyCycle = 1;
            PG1DC = PG2DC = CurrentPWMDutyCycle;
            PG1IOCONL = PG2IOCONL = 0x3400; //Set PWMxL to high
            if (repositiondelay < COUNT_DELAY) 
            {
                repositiondelay++;
            } 
            else 
            {
                /* Reset PWMticks to 100RPM for FORCED COMMUTATION*/
                PWMticks = INITIAL_PWMTICKS;
                PWMticks_limit = INITIAL_PWMTICKSLIMIT;
                drive_index = 0;
                commutationValue = 1; //needs to be set as 1
                StartUpSpeed = INITIAL_STARTUP_SPEED; //100RPM = Start up starts
                state = FORCED_COMMUTATION;
            }

            break;

        case FORCED_COMMUTATION:
            if (PWMticks_limit <= FORCED_COMMUTATION_TARGET_TICKS) //1000ticks = 300RPM
            {
                state = CLOSED_LOOP;
            }
            ForcedPWMTickCounter();
            
            if(EstimatedSpeed > 310)
            {
                EstimatedSpeed = 0;
            }
            break;

        case CLOSED_LOOP:
            SpeedMonitoring(); //Monitor increase/decrease speed
            ClosedLoopPWMTickCounter();
            /* data */
            if((EstimatedSpeed > 600) && (once == 0))
            {
                max_vb = 0;
                once++;
            }
            if (EstimatedSpeed >= 650)
            {
                bemf_average = max_vb - CLOSEDLOOP_BACK_EMF_THRESHOLD_ADJUSTMENT;
                if(bemf_average > 1500)
                {
                    CMP3_SetDACDataHighValue(bemf_average);
                }
            }
            if (MotorStatus == MOTOR_STATUS_OFF) {
                MotorStop();
            }
            break;
    }
}

void CorrectionPWMTickCounter(void) {
    if (++PWMticks > PWMticks_limit) {
        PositionCorrection();
    }
}

void PositionCorrection(void) 
{
    PWMticks = 0;
    if (++commutationValue > 1) 
    {
        commutationValue = 0;
    }
    drive_index++;
    //Set drive index to PWM override and duty cycle
    PWMticks_limit = CORRECTIONTICKS_MEMBER[drive_index];
    CurrentPWMDutyCycle = CORRECTIONPGDC_MEMBER[drive_index];
    CommutationState();
}

void ForcedPWMTickCounter(void) {
    if (++PWMticks > PWMticks_limit) {
        ForcedCommutation();
    }
}

void ForcedCommutation(void) {
    PWMticks = 0;
    if (++commutationValue > 1) 
    {
        commutationValue = 0;
    }
    drive_index++;
    //Set drive index to PWM override and duty cycle
    PWMticks_limit = TICKS_MEMBER[drive_index];
    CurrentPWMDutyCycle = PGDC_MEMBER[drive_index];
    TimeCapture();
    SpeedEstimation();
    CommutationState();
}

void ClosedLoopPWMTickCounter(void) {
    if (++PWMticks > PWMticks_limit) {
        ClosedLoopPreCommutationState();
    }
}

void SpeedMonitoring(void) {
    SpeedEstimation();
    if (TargetSpeed > NOMINAL_SPEED) 
    {
        TargetSpeed = NOMINAL_SPEED;
    } 
    else if (TargetSpeed < MINIMUM_SPEED) 
    {
        TargetSpeed = MINIMUM_SPEED;
    }

    if (TargetSpeed != EstimatedSpeed) //condition when speed is changed by user
    {
        if (TargetSpeed > EstimatedSpeed) {
            MotorSpeedUp = 1;
            MotorSpeedDown = 0;
            //when Target Speed is greater than Estimated Speed, it means motor needs to be faster
            //lower PWMticks = decrement
        } else if (TargetSpeed < EstimatedSpeed) {
            MotorSpeedUp = 0;
            MotorSpeedDown = 1;
            //when Target Speed is less than Estimated Speed, it means motor needs to be slower
            //higher PWMticks = increment
        }
    }
    PWMticks_forLevel = (uint16_t) (__builtin_divud((const unsigned long) (RPM_TO_TICKS), (const uint16_t) (TargetSpeed))); //important for speed pwmtickslimit
    // PWMticks_forLevel = (60/0.000050) / (TargetSpeed * POLES);
}

void ClosedLoopPreCommutationState(void) {
    PWMticks = 0;
    timer1_buffer = 0;
    if (++commutationValue > 1) {
        commutationValue = 0;

        /* RPM TO TICKS
         * TICKS = 60/(4*0.0001*RPM)
         * Example: TICKS = 60/(4*0.0001*100RPM) = 1500 ticks
         * In C Language: 
         * #define RPM_TO_TICKS  300000 // 60/(4*0.00005)
         * uint16_t UpdateSpeed;
         * TICKS = RPM_TO_TICKS / TargetSpeed = (uint16_t) (__builtin_divud((const unsigned long) (RPM_TO_TICKS), (const uint16_t) (UpdateSpeed)));
         */
        if ((PWMticks_limit > PWMticks_forLevel) && (MotorSpeedUp)) //When TargetSpeed increases
        {
            //If has arrays
            //drive_index++;

            UpdateSpeed+=LINEAR_RESPONSE_SENSITIVITY_SPEED;
            UpdatePWMticks_limit = (uint16_t) (__builtin_divud((const unsigned long) (RPM_TO_TICKS), (const uint16_t) (UpdateSpeed)));
            UpdatePWMDutyCycle+=LINEAR_RESPONSE_SENSITIVITY_DUTY_CYCLE; 
        } 
        else if ((PWMticks_limit < PWMticks_forLevel) && (MotorSpeedDown)) //When TargetSpeed decreases
        {
            //If has arrays
            //drive_index--;

            UpdateSpeed-=LINEAR_RESPONSE_SENSITIVITY_SPEED;
            UpdatePWMticks_limit = (uint16_t) (__builtin_divud((const unsigned long) (RPM_TO_TICKS), (const uint16_t) (UpdateSpeed)));
            UpdatePWMDutyCycle-=LINEAR_RESPONSE_SENSITIVITY_DUTY_CYCLE; 
        }
        //Set drive index to PWM override and duty cycle
//        PWMticks_limit = TICKS_MEMBER[drive_index];
//        CurrentPWMDutyCycle = PGDC_MEMBER[drive_index];
        PWMticks_limit = UpdatePWMticks_limit;
        CurrentPWMDutyCycle = UpdatePWMDutyCycle;
    }

    TimeCapture();
    //IFS0bits.T1IF = true;
    CommutationState();
}

void CommutationState(void) {
    PG1DC = PG2DC = CurrentPWMDutyCycle;
    PG1IOCONL = PWM_STATE1[commutationValue];
    PG2IOCONL = PWM_STATE2[commutationValue];
    /* commutationValue = 0;
     * PG1IOCONL = PWM_STATE1[0] = 0x6000; //AL = PWM, AH = LOW
     * PG2IOCONL = PWM_STATE2[0] = 0x3800; //BL = LOW, BH = HIGH
     * PWM LED INDICATOR AL and BH (1 and 4) are ON
     * VA - (OUT) ; VB + (IN)
     * commutationValue = 1;
     * PG1IOCONL = PWM_STATE1[1] = 0x3800; //AL = LOW, AH = HIGH
     * PG2IOCONL = PWM_STATE2[1] = 0x6000; //BL = PWM, BH = LOW
     * PWM LED INDICATOR AH and BL (2 and 3) are ON 
     * VA + (IN) ; VB - (OUT) */
}

void __attribute__((interrupt, no_auto_psv)) _CMP3Interrupt(void) //CMP3 BEMF ISR
{
    cmp3 = DAC3CONLbits.CMPSTAT; //AN4_VB comparator out data
    
    if ((DAC3CONLbits.CMPSTAT)&&(!ioc)) {
        ioc = 1;
        //IFS0bits.T1IF = true;
    }
    if ((!DAC3CONLbits.CMPSTAT)&&(ioc)) {
        ioc = 0;
        //IFS0bits.T1IF = true;
    }

    // clear the CMP3 interrupt flag
    IFS4bits.CMP3IF = 0;
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _CMP1Interrupt(void)
{
    cmp1 = DAC1CONLbits.CMPSTAT; //IBUS comparator out data
    
    if(OVERCURRENT_DETECTION_ENABLE)
    {
        if (DAC1CONLbits.CMPSTAT) 
        {
            overcurrentCounter++;
            if (overcurrentCounter > 25) //(PWMticks_limit*OVERCURRENT_SENSITIVITY_DETECTION)
            {
                PG1IOCONL = PG2IOCONL = 0x3000;
                PG1DC = PG2DC = 0;
                PG1CONLbits.ON = PG2CONLbits.ON = 0;
                LED_RE8_SetLow();
                LED_RE9_SetLow();
            }
        }
    }
    
    // clear the CMP1 interrupt flag
    IFS4bits.CMP1IF = 0;
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _CCT2Interrupt ( void )
{
    if(IFS1bits.CCT2IF)
    {         
        
        IFS1bits.CCT2IF = 0;
    }
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt() 
{
    uint8_t i;
    timer1_buffer = TMR1;
    SCCP2_TMR_Period32BitSet(timer1_buffer);
    SCCP2_TMR_Counter32BitSet(0x0000);
    
    
    timer1_buffer_average[average_index] = timer1_buffer;
    average_index++;
    if(average_index > AVERAGE_INDEX_FILTER_SIZE)
    {
        average_index = 0;
    }
    speed_sum = 0;
    for(i=0; i<AVERAGE_INDEX_FILTER_SIZE; i++)
    {
        speed_sum = speed_sum + timer1_buffer_average[i];
        speed_average = speed_sum >> AVERAGE_INDEX_FILTER_SCALE;
    }
    TMR1 = 0x00;
    IFS0bits.T1IF = false;
}

void SpeedEstimation(void)  //Speed estimation based on cmp3 isr + timer1 int
{
    EstimatedSpeed = TARGETSPEED_TO_TIMER1TICKS_CONST / speed_average;
    
    //temp = (uint16_t)(__builtin_divud((const unsigned long)(TARGETSPEED_TO_TIMER1TICKS_CONST), (const unsigned short)(timer1_buffer))); 
    //ANSWER is calculation of PWMticks_forLevel directly from TargetSpeed using timer
    //Compare with PWMticks_limit (hardcoded)
    //PWMticks_forLevel = (60/0.000050) / (TargetSpeed * POLES);
}

void TimeCapture(void) 
{
    timerValue = GetTimerBuffer();

    if (timerValue <= pastTimerValue) {
        timerValueDelta = (PERIOD_CONSTANT - (pastTimerValue - timerValue));
    } else {
        timerValueDelta = (timerValue - pastTimerValue);
    }
    pastTimerValue = timerValue;
    measuredTicks = timerValueDelta * PERIODPERPULSE; // timerValueDelta * ((64*2)/(100MHz)) / (50us)
    temp = (uint16_t) (__builtin_divud((const unsigned long) (RPM_TO_TICKS), (const uint16_t) (measuredTicks))); // (60 / (4*50us)) / measuredTicks
    measured_Speed = temp*2;
}

void InitializePWM(void) {
    PG1CONLbits.ON = PG2CONLbits.ON = 0;
    PG1IOCONL = PG2IOCONL = 0x3000;
    PG1DC = PG2DC = 0;
    PG1CONLbits.ON = PG2CONLbits.ON = 1;
}

void InitializeTimer(void) {
    CCP1CON1Lbits.CCPON = 0x1; //Enabling CCP
    SetTimerPeriod(PERIOD_CONSTANT);
}

void InitializePhase (void)
{
    PG1CONLbits.ON = PG2CONLbits.ON = 0;
    MPHASE = 0x01;
    PG1CONLbits.ON = PG2CONLbits.ON = 1;
}

void CorrectionPGDCTable() {
    uint16_t arrayIndex = 0;
    for (arrayIndex = 0; arrayIndex < CORRECTION_ITERATION; arrayIndex++) {
        CORRECTIONPGDC_MEMBER[arrayIndex] = CORRECTIONPGDC_TABLE[arrayIndex];
    }
}

void CorrectionTicksTable() {
    uint16_t arrayIndex = 0;
    for (arrayIndex = 0; arrayIndex < CORRECTION_ITERATION; arrayIndex++) {
        CORRECTIONTICKS_MEMBER[arrayIndex] = CORRECTIONTICKS_TABLE[arrayIndex];
    }
}

void PWMticksTable() {
    uint16_t arrayIndex = 0;
    for (arrayIndex = 0; arrayIndex < COMMUTATION_ITERATION; arrayIndex++) {
        TICKS_MEMBER[arrayIndex] = TICKS_TABLE[arrayIndex];
    }
}

void PGDCTable() {
    uint16_t arrayIndex = 0;
    for (arrayIndex = 0; arrayIndex < COMMUTATION_ITERATION; arrayIndex++) {
        PGDC_MEMBER[arrayIndex] = PGDC_TABLE[arrayIndex];
    }
}

void RotationSwitchingTable() {
    uint16_t arrayIndex = 0;

    for (arrayIndex = 0; arrayIndex < 2; arrayIndex++) {
        PWM_STATE1[arrayIndex] = PWM_STATE1_CLKW[arrayIndex];
        PWM_STATE2[arrayIndex] = PWM_STATE2_CLKW[arrayIndex];
    }
}

void SetTimerPeriod(uint32_t value) {
    CCP1PRL = (value & 0x0000FFFF);
    CCP1PRH = ((value & 0xFFFF0000) >> 16);
}

uint32_t GetTimerBuffer(void) {
    uint32_t counterVal = 0xFFFFFFFF;
    counterVal = (((uint32_t) CCP1TMRH << 16) | (CCP1TMRL));
    return counterVal;
}

void MotorStop(void) {
    /* Function Initializations */
    InitializePWM();
    InitializeTimer();
    PWMticksTable();
    PGDCTable();
    RotationSwitchingTable();

    /* Drive PERIOD and DUTY CYCLE initialization */
    TargetSpeed = INITIAL_TARGET_SPEED; //initially set to 1000rpm / 300 ticks @ closed loop
    previousTargetSpeed = 0;
    MotorSpeedUp = 0;
    MotorSpeedDown = 0;
    PWMticks = 0;
    PWMticks_limit = 0;
    measured_Speed = 0;
    CurrentPWMDutyCycle = 0; //ROTOR_LOCKING_DUTY; 
    drive_index = 0;

    /* Drive "state machine" */
    PWMticks_forLevel = 0;
    PGDC_forLevel = 0;
    correction_index = 0;
    repositiondelay = 0;

    state = INIT;

#ifdef STALL_DETECTION
    AutoRecovery();
#endif
}

/**
 End of File
 */

