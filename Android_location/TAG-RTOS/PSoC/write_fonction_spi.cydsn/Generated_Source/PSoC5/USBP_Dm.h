/*******************************************************************************
* File Name: USBP_Dm.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_USBP_Dm_H) /* Pins USBP_Dm_H */
#define CY_PINS_USBP_Dm_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "USBP_Dm_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 USBP_Dm__PORT == 15 && ((USBP_Dm__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    USBP_Dm_Write(uint8 value);
void    USBP_Dm_SetDriveMode(uint8 mode);
uint8   USBP_Dm_ReadDataReg(void);
uint8   USBP_Dm_Read(void);
void    USBP_Dm_SetInterruptMode(uint16 position, uint16 mode);
uint8   USBP_Dm_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the USBP_Dm_SetDriveMode() function.
     *  @{
     */
        #define USBP_Dm_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define USBP_Dm_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define USBP_Dm_DM_RES_UP          PIN_DM_RES_UP
        #define USBP_Dm_DM_RES_DWN         PIN_DM_RES_DWN
        #define USBP_Dm_DM_OD_LO           PIN_DM_OD_LO
        #define USBP_Dm_DM_OD_HI           PIN_DM_OD_HI
        #define USBP_Dm_DM_STRONG          PIN_DM_STRONG
        #define USBP_Dm_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define USBP_Dm_MASK               USBP_Dm__MASK
#define USBP_Dm_SHIFT              USBP_Dm__SHIFT
#define USBP_Dm_WIDTH              1u

/* Interrupt constants */
#if defined(USBP_Dm__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in USBP_Dm_SetInterruptMode() function.
     *  @{
     */
        #define USBP_Dm_INTR_NONE      (uint16)(0x0000u)
        #define USBP_Dm_INTR_RISING    (uint16)(0x0001u)
        #define USBP_Dm_INTR_FALLING   (uint16)(0x0002u)
        #define USBP_Dm_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define USBP_Dm_INTR_MASK      (0x01u) 
#endif /* (USBP_Dm__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define USBP_Dm_PS                     (* (reg8 *) USBP_Dm__PS)
/* Data Register */
#define USBP_Dm_DR                     (* (reg8 *) USBP_Dm__DR)
/* Port Number */
#define USBP_Dm_PRT_NUM                (* (reg8 *) USBP_Dm__PRT) 
/* Connect to Analog Globals */                                                  
#define USBP_Dm_AG                     (* (reg8 *) USBP_Dm__AG)                       
/* Analog MUX bux enable */
#define USBP_Dm_AMUX                   (* (reg8 *) USBP_Dm__AMUX) 
/* Bidirectional Enable */                                                        
#define USBP_Dm_BIE                    (* (reg8 *) USBP_Dm__BIE)
/* Bit-mask for Aliased Register Access */
#define USBP_Dm_BIT_MASK               (* (reg8 *) USBP_Dm__BIT_MASK)
/* Bypass Enable */
#define USBP_Dm_BYP                    (* (reg8 *) USBP_Dm__BYP)
/* Port wide control signals */                                                   
#define USBP_Dm_CTL                    (* (reg8 *) USBP_Dm__CTL)
/* Drive Modes */
#define USBP_Dm_DM0                    (* (reg8 *) USBP_Dm__DM0) 
#define USBP_Dm_DM1                    (* (reg8 *) USBP_Dm__DM1)
#define USBP_Dm_DM2                    (* (reg8 *) USBP_Dm__DM2) 
/* Input Buffer Disable Override */
#define USBP_Dm_INP_DIS                (* (reg8 *) USBP_Dm__INP_DIS)
/* LCD Common or Segment Drive */
#define USBP_Dm_LCD_COM_SEG            (* (reg8 *) USBP_Dm__LCD_COM_SEG)
/* Enable Segment LCD */
#define USBP_Dm_LCD_EN                 (* (reg8 *) USBP_Dm__LCD_EN)
/* Slew Rate Control */
#define USBP_Dm_SLW                    (* (reg8 *) USBP_Dm__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define USBP_Dm_PRTDSI__CAPS_SEL       (* (reg8 *) USBP_Dm__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define USBP_Dm_PRTDSI__DBL_SYNC_IN    (* (reg8 *) USBP_Dm__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define USBP_Dm_PRTDSI__OE_SEL0        (* (reg8 *) USBP_Dm__PRTDSI__OE_SEL0) 
#define USBP_Dm_PRTDSI__OE_SEL1        (* (reg8 *) USBP_Dm__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define USBP_Dm_PRTDSI__OUT_SEL0       (* (reg8 *) USBP_Dm__PRTDSI__OUT_SEL0) 
#define USBP_Dm_PRTDSI__OUT_SEL1       (* (reg8 *) USBP_Dm__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define USBP_Dm_PRTDSI__SYNC_OUT       (* (reg8 *) USBP_Dm__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(USBP_Dm__SIO_CFG)
    #define USBP_Dm_SIO_HYST_EN        (* (reg8 *) USBP_Dm__SIO_HYST_EN)
    #define USBP_Dm_SIO_REG_HIFREQ     (* (reg8 *) USBP_Dm__SIO_REG_HIFREQ)
    #define USBP_Dm_SIO_CFG            (* (reg8 *) USBP_Dm__SIO_CFG)
    #define USBP_Dm_SIO_DIFF           (* (reg8 *) USBP_Dm__SIO_DIFF)
#endif /* (USBP_Dm__SIO_CFG) */

/* Interrupt Registers */
#if defined(USBP_Dm__INTSTAT)
    #define USBP_Dm_INTSTAT            (* (reg8 *) USBP_Dm__INTSTAT)
    #define USBP_Dm_SNAP               (* (reg8 *) USBP_Dm__SNAP)
    
	#define USBP_Dm_0_INTTYPE_REG 		(* (reg8 *) USBP_Dm__0__INTTYPE)
#endif /* (USBP_Dm__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_USBP_Dm_H */


/* [] END OF FILE */
