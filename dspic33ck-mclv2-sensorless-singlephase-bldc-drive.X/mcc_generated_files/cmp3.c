/**
  CMP3 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    cmp3.c

  @Summary
    This is the generated driver implementation file for the CMP3 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for CMP3.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.4
        Device            :  dsPIC33CK256MP508
        Driver Version    :  1.00
    The generated drivers are tested against the following:
        Compiler          :  XC16 v2.10
        MPLAB 	          :  MPLAB X v6.05
*/


/**
  Section: Included Files
*/

#include "cmp3.h"

/**
  Section: CMP3 APIs
*****************************************************************************************/
/******************************************************************************
*                                                                             
*    Function:			CMP3_Initialize
*    Description:       Initialization of the Comparator with Slope Compensation module                                                                         
*                                             
*    Return Value:      None
******************************************************************************/

void CMP3_Initialize(void)
{           

    // Disable the CMP module before the initialization
    CMP3_Disable();
	
	// Disable the Interrupt 
    IEC4bits.CMP3IE = 0;
	
	// Comparator Register settings
	DACCTRL1L = 0xC0; //CLKDIV 1:1; DACON disabled; DACSIDL disabled; FCLKDIV 1:1; CLKSEL FPLLO - System Clock with PLL Enabled; 
	DACCTRL2L = 0x55; //TMODTIME 85; 
	DACCTRL2H = 0x8A; //SSTIME 138; 
	DAC3CONH = 0x00; //TMCB 0; 
	DAC3CONL = 0xE008; //CMPPOL Non Inverted; HYSPOL Rising Edge; HYSSEL None; DACEN enabled; FLTREN disabled; CBE disabled; IRQM Rising or falling edge detect; INSEL CMP3B; DACOEN disabled; 

	//Slope Settings
	SLP3CONH = 0x00; //HME disabled; PSE Negative; SLOPEN disabled; TWME disabled; 
	SLP3CONL = 0x00; //HCFSEL None; SLPSTRT None; SLPSTOPB None; SLPSTOPA None; 
	SLP3DAT = 0x00; //SLPDAT 0; 
	DAC3DATL = 0x00; //DACDATL 0; 
	DAC3DATH = 0xBB8; //DACDATH 1500; 
    
    // Clearing IF flag before enabling the interrupt.
    IFS4bits.CMP3IF = 0;
    // Enabling CMP3 interrupt.
    IEC4bits.CMP3IE = 1;
	
    CMP3_Enable();

}

/******************************************************************************
*                                                                             
*    Function:			CMP3_ComparatorOuputStatusGet
*    Description:       Returns the status of the Comparator including the 
*						polarity                                                                       
*                                             
*    Return Value:      True if the Comparator Status is 1 and False if it is 0
******************************************************************************/
bool CMP3_ComparatorOuputStatusGet(void)
{
    return (DAC3CONLbits.CMPSTAT);
}

/******************************************************************************
*                                                                             
*    Function:			CMP3_Enable
*    Description:       Enables the common DAC module                                     
*      
*	 Parameters:		None                                       
*    Return Value:      None
******************************************************************************/
void CMP3_Enable(void)
{
    DACCTRL1Lbits.DACON = 1;
}

/******************************************************************************
*                                                                             
*    Function:			CMP3_Disable
*    Description:       Disables the common DAC module                                     
*      
*	 Parameters:		None                                       
*    Return Value:      None
******************************************************************************/
void CMP3_Disable(void)
{
    DACCTRL1Lbits.DACON = 0;
}

/******************************************************************************
*                                                                             
*    Function:			CMP3_SetInputSource
*    Description:       Set the source to the non-inverting input of the 
*						Comparator module                                                                     
*      
*	 Parameters:		Enumeration specifying the input source                                      
*    Return Value:      None
******************************************************************************/
void CMP3_SetInputSource(CMP3_INPUT inpSrc)
{
    DAC3CONLbits.INSEL = inpSrc;
}

/******************************************************************************
*                                                                             
*    Function:		CMP3_SetDACDataHighValue
*    Description:       Sets the DAC Data High Value                                     
*      
*    Parameters:        Value to be set as DAC Data High                                       
*    Return Value:      None
******************************************************************************/
void CMP3_SetDACDataHighValue(uint16_t dacVal)
{
    DAC3DATH = dacVal;
}

/******************************************************************************
*                                                                             
*    Function:		CMP3_SetDACDataLowValue
*    Description:       Sets the DAC Data Low Value                                     
*      
*    Parameters:        Value to be set as DAC Data Low                                       
*    Return Value:      None
******************************************************************************/
void CMP3_SetDACDataLowValue(uint16_t dacVal)
{
    DAC3DATL = dacVal;
}

/******************************************************************************
*                                                                             
*    Function:		CMP3_EnableDACOutput
*    Description:       Enables the DAC Output                                    
*      
*    Parameters:        None                                       
*    Return Value:      None
******************************************************************************/
void CMP3_EnableDACOutput(void)
{
    DAC3CONLbits.DACOEN = 1;
}

/******************************************************************************
*                                                                             
*    Function:		CMP3_DisableDACOutput
*    Description:       Disables the DAC Output                                    
*      
*    Parameters:        None                                       
*    Return Value:      None
******************************************************************************/
void CMP3_DisableDACOutput(void)
{
    DAC3CONLbits.DACOEN = 0;
}

/******************************************************************************
*                                                                             
*    Function:		CMP3_SetStartTrigger
*    Description:       Trigger set for the Start Signal of the Slope                                    
*      
*    Parameters:        Value indicating the trigger to be set                                       
*    Return Value:      None
******************************************************************************/
void CMP3_SetStartTrigger(CMP3_START_TRIGGER trigger)
{
    SLP3CONLbits.SLPSTRT = trigger;
}

/******************************************************************************
*                                                                             
*    Function:		CMP3_SetStopATrigger
*    Description:       Trigger set for the Stop A Signal of the Slope                                    
*      
*    Parameters:        Value indicating the trigger to be set                                       
*    Return Value:      None
******************************************************************************/
void CMP3_SetStopATrigger(CMP3_STOPA_TRIGGER trigger)
{
    SLP3CONLbits.SLPSTOPA = trigger;
}

/******************************************************************************
*                                                                             
*    Function:		CMP3_SetStopBTrigger
*    Description:       Trigger set for the Stop B Signal of the Slope                                    
*      
*    Parameters:        Value indicating the trigger to be set                                       
*    Return Value:      None
******************************************************************************/
void CMP3_SetStopBTrigger(CMP3_STOPB_TRIGGER trigger)
{
    SLP3CONLbits.SLPSTOPB = trigger;
}

/******************************************************************************
*                                                                             
*    Function:		CMP3_SetHystereticTrigger
*    Description:       Trigger set for the Hysteretic mode                                    
*      
*    Parameters:        Value indicating the trigger to be set                                       
*    Return Value:      None
******************************************************************************/
void CMP3_SetHystereticTrigger(CMP3_HYSTERETIC_FUNCTION trigger)
{
    SLP3CONLbits.HCFSEL = trigger;
}

/* Callback function for the CMP3 module */
void __attribute__ ((weak)) CMP3_CallBack(void)
{
    // Add your custom callback code here
}

/* Interrupt Service routine for CMP3 */


/**
 End of File
*/