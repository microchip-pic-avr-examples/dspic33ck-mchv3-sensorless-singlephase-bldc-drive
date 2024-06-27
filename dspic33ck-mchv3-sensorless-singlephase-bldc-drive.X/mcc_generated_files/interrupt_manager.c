/**
  System Interrupts Generated Driver File 

  @Company:
    Microchip Technology Inc.

  @File Name:
    interrupt_manager.h

  @Summary:
    This is the generated driver implementation file for setting up the
    interrupts using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description:
    This source file provides implementations for PIC24 / dsPIC33 / PIC32MM MCUs interrupts.
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
    Section: Includes
*/
#include <xc.h>

/**
    void INTERRUPT_Initialize (void)
*/
void INTERRUPT_Initialize (void)
{
    //    ADCAN18: ADC AN18 Convert Done
    //    Priority: 1
        IPC27bits.ADCAN18IP = 1;
    //    ADCAN24: ADC AN24 Convert Done
    //    Priority: 1
        IPC48bits.ADCAN24IP = 1;
    //    ADCAN3: ADC AN3 Convert Done
    //    Priority: 1
        IPC23bits.ADCAN3IP = 1;
    //    ADCAN4: ADC AN4 Convert Done
    //    Priority: 1
        IPC23bits.ADCAN4IP = 1;
    //    ADCAN25: ADC AN25 Convert Done
    //    Priority: 1
        IPC48bits.ADCAN25IP = 1;
    //    CMPI: Comparator 3 
    //    Priority: 1
        IPC19bits.CMP3IP = 1;
    //    CMPI: Comparator 1 
    //    Priority: 1
        IPC19bits.CMP1IP = 1;
    //    TI: Timer 1
    //    Priority: 1
        IPC0bits.T1IP = 1;
}
