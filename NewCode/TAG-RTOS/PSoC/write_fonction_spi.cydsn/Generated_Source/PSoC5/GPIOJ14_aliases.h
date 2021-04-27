/*******************************************************************************
* File Name: GPIOJ14.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_GPIOJ14_ALIASES_H) /* Pins GPIOJ14_ALIASES_H */
#define CY_PINS_GPIOJ14_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define GPIOJ14_0			(GPIOJ14__0__PC)
#define GPIOJ14_0_INTR	((uint16)((uint16)0x0001u << GPIOJ14__0__SHIFT))

#define GPIOJ14_INTR_ALL	 ((uint16)(GPIOJ14_0_INTR))

#endif /* End Pins GPIOJ14_ALIASES_H */


/* [] END OF FILE */
