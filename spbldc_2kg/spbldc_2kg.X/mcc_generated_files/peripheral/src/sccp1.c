/**
  SCCP1 Generated Driver Source File
 
  @Company
    Microchip Technology Inc.
 
  @File Name
    sccp1.c
 
  @Summary
    This is the generated driver source file for SCCP1 driver using CCL.
 
  @Description
    This file provides API definitions for SCCP1 driver.
    Generation Information :
        Product Revision  :  CCL 
        Driver Revision   :  0.1.0
        Device            :  dsPIC33CK256MP508
*/

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

/**
  Section: Included Files
*/

#include <xc.h>
#include "../sccp1.h"

/**
  Section: SCCP1 Module APIs
*/

// Set the SCCP1 module to the options selected in the user interface.
void SCCP1_Initialize(void) {
    // CCPON enabled; CCPSIDL disabled; CCPSLP disabled; TMRSYNC disabled; CLKSEL FOSC/2; TMRPS 1:64; TMR32 32 Bit; CCSEL disabled; MOD 16-Bit/32-Bit Timer; 
    CCP1CON1L = 0x80E0;
    // OPSRC Timer Interrupt Event; RTRGEN disabled; IOPS Each Time Base Period Match; TRIGEN disabled; ONESHOT disabled; ALTSYNC disabled; SYNC None; 
    CCP1CON1H = 0x0;
    // PWMRSEN disabled; ASDGM disabled; SSDG disabled; ASDG 0x0; 
    CCP1CON2L = 0x0;
    // OENSYNC disabled; OCAEN disabled; ICGSM Level-Sensitive mode; AUXOUT Disabled; ICSEL IC1; 
    CCP1CON2H = 0x0;
    // OETRIG disabled; OSCNT None; POLACE disabled; PSSACE Tri-state; 
    CCP1CON3H = 0x0;
    // ICGARM disabled; TRSET disabled; TRCLR disabled; ASEVT disabled; SCEVT disabled; ICDIS disabled; ICOV disabled; 
    CCP1STATL = 0x0;
    // TMRL 0x0000; 
    CCP1TMRL = 0x0;
    // TMRH 0x0000; 
    CCP1TMRH = 0x0;
    // PRL 0x0000; 
    CCP1PRL = 0x0;
    // PRH 0x0000; 
    CCP1PRH = 0x0;
    // CMPA 0x0000; 
    CCP1RA = 0x0;
    // CMPB 0x0000; 
    CCP1RB = 0x0;
    // BUFL 0x0000; 
    CCP1BUFL = 0x0;
    // BUFH 0x0000; 
    CCP1BUFH = 0x0;

    IFS0bits.CCP1IF = 0;
    IEC0bits.CCP1IE = 1;
}

void __attribute__ ((interrupt, no_auto_psv)) _CCP1Interrupt(void)
{
     IFS0bits.CCP1IF = 0;
}

