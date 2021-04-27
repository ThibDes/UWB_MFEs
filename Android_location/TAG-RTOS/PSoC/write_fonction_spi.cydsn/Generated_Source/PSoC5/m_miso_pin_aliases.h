/*******************************************************************************
* File Name: m_miso_pin.h  
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

#if !defined(CY_PINS_m_miso_pin_ALIASES_H) /* Pins m_miso_pin_ALIASES_H */
#define CY_PINS_m_miso_pin_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*              Constants        
***************************************/
#define m_miso_pin_0			(m_miso_pin__0__PC)
#define m_miso_pin_0_INTR	((uint16)((uint16)0x0001u << m_miso_pin__0__SHIFT))

#define m_miso_pin_INTR_ALL	 ((uint16)(m_miso_pin_0_INTR))

#endif /* End Pins m_miso_pin_ALIASES_H */


/* [] END OF FILE */
