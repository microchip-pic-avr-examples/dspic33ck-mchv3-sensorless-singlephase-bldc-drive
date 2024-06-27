/**
  SCCP2 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    sccp2.c

  @Summary
    This is the generated driver implementation file for the SCCP2 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides implementations for driver APIs for SCCP2. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.4
        Device            :  dsPIC33CK256MP508
    The generated drivers are tested against the following:
        Compiler          :  XC16 v2.10
        MPLAB             :  MPLAB X v6.05
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

#include "sccp2_tmr.h"

/**
  Section: Data Type Definitions
*/

/**
  SCCP2 Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

  @Description
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

*/
typedef struct _SCCP2_TMR_OBJ_STRUCT
{

    /* Timer Elapsed */
    bool                                                    primaryTimer16Elapsed;
    bool                                                    secondaryTimer16Elapsed;
    bool                                                    Timer32Elapsed;
} SCCP2_TMR_OBJ;

static SCCP2_TMR_OBJ sccp2_timer_obj;
void SCCP2_TMR_Initialize(void)
{
    // CCPON disabled; MOD 16-Bit/32-Bit Timer; CCSEL disabled; CCPSIDL disabled; T32 32 Bit; CCPSLP disabled; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; 
    CCP2CON1L = (0x20 & 0x7FFF); //Disabling CCPON bit
    //RTRGEN disabled; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; OPS Each Time Base Period Match; SYNC None; OPSSRC Timer Interrupt Event; 
    CCP2CON1H = 0x00;
    //ASDGM disabled; SSDG disabled; ASDG 0; PWMRSEN disabled; 
    CCP2CON2L = 0x00;
    //ICGSM Level-Sensitive mode; ICSEL IC2; AUXOUT Disabled; OCAEN disabled; OENSYNC disabled; 
    CCP2CON2H = 0x00;
    //OETRIG disabled; OSCNT None; POLACE disabled; PSSACE Tri-state; 
    CCP2CON3H = 0x00;
    //ICDIS disabled; SCEVT disabled; TRSET disabled; ICOV disabled; ASEVT disabled; ICGARM disabled; TRCLR disabled; 
    CCP2STATL = 0x00;
    //TMR 0; 
    CCP2TMRL = 0x00;
    //TMR 0; 
    CCP2TMRH = 0x00;
    //PR 0; 
    CCP2PRL = 0x00;
    //PR 0; 
    CCP2PRH = 0x00;
    //CMP 0; 
    CCP2RA = 0x00;
    //CMP 0; 
    CCP2RB = 0x00;
    //BUF 0; 
    CCP2BUFL = 0x00;
    //BUF 0; 
    CCP2BUFH = 0x00;


    IFS1bits.CCP2IF = 0;

    IFS1bits.CCT2IF = 0;
      
    // Enabling SCCP2 interrupt.
    IEC1bits.CCP2IE = 1;

    // Enabling SCCP2 interrupt.
    IEC1bits.CCT2IE = 1;

}

void SCCP2_TMR_Start( void )
{
    /* Reset the status information */
    sccp2_timer_obj.primaryTimer16Elapsed = false;
    sccp2_timer_obj.secondaryTimer16Elapsed = false;
    sccp2_timer_obj.Timer32Elapsed = false;

    /* Start the Timer */
    CCP2CON1Lbits.CCPON = true;
}

void SCCP2_TMR_Stop( void )
{
    /* Stop the Timer */
    CCP2CON1Lbits.CCPON = false;
}

void __attribute__ ((weak)) SCCP2_TMR_Timer32CallBack(void)
{
    // Add your custom callback code here
}




void SCCP2_TMR_Period32BitSet( uint32_t value )
{
    /* Update the period values */
    CCP2PRL = (value & 0x0000FFFF);
    CCP2PRH = ((value & 0xFFFF0000)>>16);

    /* Reset the status information */
    sccp2_timer_obj.Timer32Elapsed = false;
}

uint32_t SCCP2_TMR_Period32BitGet( void )
{
    uint32_t periodVal = 0xFFFFFFFF;
    
    /* get the timer period value and return it */
    periodVal = (((uint32_t)CCP2PRH <<16) | CCP2PRL);

    return(periodVal);
}

void SCCP2_TMR_Counter32BitSet ( uint32_t value )
{
    /* Update the counter values */
    CCP2TMRL = (value & 0x0000FFFF);
    CCP2TMRH = ((value & 0xFFFF0000)>>16);
    /* Reset the status information */
    sccp2_timer_obj.Timer32Elapsed = false;
}

uint32_t SCCP2_TMR_Counter32BitGet( void )
{
    uint32_t counterVal = 0xFFFFFFFF;

    /* get the timer period value and return it */
    counterVal = (((uint32_t)CCP2TMRH <<16) | (CCP2TMRL));

    return(counterVal);
}

bool SCCP2_TMR_Timer32ElapsedThenClear(void)
{
    bool status;
    
    status = sccp2_timer_obj.Timer32Elapsed ;
    
    if(status == true)
    {
        sccp2_timer_obj.Timer32Elapsed = false;
    }
    return status;
}
/**
 End of File
*/