/*******************************************************************************
  Board Service Configuration Routine Source File

  File Name:
    button_service.c

  Summary:
    This file includes subroutine for initializing the button service routine
    to control the hardware of the development board.

  Description:
    Definitions in the file are for dsPIC33CK256MP508 microcontroller
    plugged onto the Low Voltage Motor Control (LVMC) Board from Microchip.
 *******************************************************************************/
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "button_service.h"
#include "mcc_generated_files/pin_manager.h"

BUTTON_T buttonStartStop;

BUTTON_T buttonFwdRev;

// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
static void ButtonGroupInitialize(void);
static void ButtonScan(BUTTON_T *, bool);
uint16_t boardServiceISRCounter = 0;
// *****************************************************************************

bool StartStop(void)
{
    if(buttonStartStop.status)
    {
        buttonStartStop.status = false;
        return true;
    }
    else
    {
        return false;
    }
}

bool ForwardReverse(void)
{
    if(buttonFwdRev.status)
    {
        buttonFwdRev.status = false;
        return true;
    }
    else
    {
        return false;
    }
}


void BoardServiceStepIsr(void) {
    if (boardServiceISRCounter < BOARD_SERVICE_TICK_COUNT) {
        boardServiceISRCounter += 1;
    }
}

void BoardService(void) {
    if (boardServiceISRCounter == BOARD_SERVICE_TICK_COUNT) {

        // Button scanning loop for Button 1 to start Motor
        ButtonScan(&buttonStartStop, S1);
        // Button scanning loop for SW2 to change direction
        /*
        ButtonScan(&buttonFwdRev, SW2);
        boardServiceISRCounter = 0;
        */
    }
}

void BoardServiceInit(void) {
    ButtonGroupInitialize();
    boardServiceISRCounter = BOARD_SERVICE_TICK_COUNT;
}

void ButtonScan(BUTTON_T *pButton, bool button) {
    if (button == true) {
        if (pButton->debounceCount < BUTTON_DEBOUNCE_COUNT) {
            pButton->debounceCount++;
            pButton->state = BUTTON_DEBOUNCE;
        }
    }
    else {
        if (pButton->debounceCount < BUTTON_DEBOUNCE_COUNT) {
            pButton->state = BUTTON_NOT_PRESSED;
        }
        else {
            pButton->state = BUTTON_PRESSED;
            pButton->status = true;
        }
        pButton->debounceCount = 0;
    }
}

void ButtonGroupInitialize(void) {
    buttonStartStop.state = BUTTON_NOT_PRESSED;
    buttonStartStop.debounceCount = 0;
    buttonStartStop.state = false;

    buttonFwdRev.state = BUTTON_NOT_PRESSED;
    buttonFwdRev.debounceCount = 0;
    buttonFwdRev.state = false;

}
/*
 End of file
 */



