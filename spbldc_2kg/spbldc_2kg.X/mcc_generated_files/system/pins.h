/**
 * PINS Generated Driver Header File 
 * 
 * @file      pins.h
 *            
 * @defgroup  pinsdriver Pins Driver
 *            
 * @brief     The Pin Driver directs the operation and function of 
 *            the selected device pins using dsPIC MCUs.
 *
 * @skipline @version   Firmware Driver Version 1.0.2
 *
 * @skipline @version   PLIB Version 1.2.0
 *
 * @skipline  Device : dsPIC33CK256MP508
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

#ifndef PINS_H
#define PINS_H
// Section: Includes
#include <xc.h>

// Section: Device Pin Macros

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD5 GPIO Pin which has a custom name of Button_S2_MCLV to High
 * @pre      The RD5 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define Button_S2_MCLV_SetHigh()          (_LATD5 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RD5 GPIO Pin which has a custom name of Button_S2_MCLV to Low
 * @pre      The RD5 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define Button_S2_MCLV_SetLow()           (_LATD5 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RD5 GPIO Pin which has a custom name of Button_S2_MCLV
 * @pre      The RD5 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define Button_S2_MCLV_Toggle()           (_LATD5 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RD5 GPIO Pin which has a custom name of Button_S2_MCLV
 * @param    none
 * @return   none  
 */
#define Button_S2_MCLV_GetValue()         _RD5

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD5 GPIO Pin which has a custom name of Button_S2_MCLV as Input
 * @param    none
 * @return   none  
 */
#define Button_S2_MCLV_SetDigitalInput()  (_TRISD5 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RD5 GPIO Pin which has a custom name of Button_S2_MCLV as Output
 * @param    none
 * @return   none  
 */
#define Button_S2_MCLV_SetDigitalOutput() (_TRISD5 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE5 GPIO Pin which has a custom name of S1_MCHV3 to High
 * @pre      The RE5 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define S1_MCHV3_SetHigh()          (_LATE5 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE5 GPIO Pin which has a custom name of S1_MCHV3 to Low
 * @pre      The RE5 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define S1_MCHV3_SetLow()           (_LATE5 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RE5 GPIO Pin which has a custom name of S1_MCHV3
 * @pre      The RE5 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define S1_MCHV3_Toggle()           (_LATE5 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RE5 GPIO Pin which has a custom name of S1_MCHV3
 * @param    none
 * @return   none  
 */
#define S1_MCHV3_GetValue()         _RE5

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE5 GPIO Pin which has a custom name of S1_MCHV3 as Input
 * @param    none
 * @return   none  
 */
#define S1_MCHV3_SetDigitalInput()  (_TRISE5 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE5 GPIO Pin which has a custom name of S1_MCHV3 as Output
 * @param    none
 * @return   none  
 */
#define S1_MCHV3_SetDigitalOutput() (_TRISE5 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE7 GPIO Pin which has a custom name of Button_S3_MCLV to High
 * @pre      The RE7 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define Button_S3_MCLV_SetHigh()          (_LATE7 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE7 GPIO Pin which has a custom name of Button_S3_MCLV to Low
 * @pre      The RE7 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define Button_S3_MCLV_SetLow()           (_LATE7 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RE7 GPIO Pin which has a custom name of Button_S3_MCLV
 * @pre      The RE7 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define Button_S3_MCLV_Toggle()           (_LATE7 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RE7 GPIO Pin which has a custom name of Button_S3_MCLV
 * @param    none
 * @return   none  
 */
#define Button_S3_MCLV_GetValue()         _RE7

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE7 GPIO Pin which has a custom name of Button_S3_MCLV as Input
 * @param    none
 * @return   none  
 */
#define Button_S3_MCLV_SetDigitalInput()  (_TRISE7 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE7 GPIO Pin which has a custom name of Button_S3_MCLV as Output
 * @param    none
 * @return   none  
 */
#define Button_S3_MCLV_SetDigitalOutput() (_TRISE7 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE8 GPIO Pin which has a custom name of LED1 to High
 * @pre      The RE8 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define LED1_SetHigh()          (_LATE8 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE8 GPIO Pin which has a custom name of LED1 to Low
 * @pre      The RE8 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED1_SetLow()           (_LATE8 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RE8 GPIO Pin which has a custom name of LED1
 * @pre      The RE8 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED1_Toggle()           (_LATE8 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RE8 GPIO Pin which has a custom name of LED1
 * @param    none
 * @return   none  
 */
#define LED1_GetValue()         _RE8

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE8 GPIO Pin which has a custom name of LED1 as Input
 * @param    none
 * @return   none  
 */
#define LED1_SetDigitalInput()  (_TRISE8 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE8 GPIO Pin which has a custom name of LED1 as Output
 * @param    none
 * @return   none  
 */
#define LED1_SetDigitalOutput() (_TRISE8 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE9 GPIO Pin which has a custom name of LED2 to High
 * @pre      The RE9 must be set as Output Pin             
 * @param    none
 * @return   none  
 */
#define LED2_SetHigh()          (_LATE9 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RE9 GPIO Pin which has a custom name of LED2 to Low
 * @pre      The RE9 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED2_SetLow()           (_LATE9 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RE9 GPIO Pin which has a custom name of LED2
 * @pre      The RE9 must be set as Output Pin
 * @param    none
 * @return   none  
 */
#define LED2_Toggle()           (_LATE9 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RE9 GPIO Pin which has a custom name of LED2
 * @param    none
 * @return   none  
 */
#define LED2_GetValue()         _RE9

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE9 GPIO Pin which has a custom name of LED2 as Input
 * @param    none
 * @return   none  
 */
#define LED2_SetDigitalInput()  (_TRISE9 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RE9 GPIO Pin which has a custom name of LED2 as Output
 * @param    none
 * @return   none  
 */
#define LED2_SetDigitalOutput() (_TRISE9 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Initializes the PINS module
 * @param    none
 * @return   none  
 */
void PINS_Initialize(void);



#endif
