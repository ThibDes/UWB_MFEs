/*******************************************************************************
* File Name: GPIOJ12.h  
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

#if !defined(CY_PINS_GPIOJ12_H) /* Pins GPIOJ12_H */
#define CY_PINS_GPIOJ12_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "GPIOJ12_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 GPIOJ12__PORT == 15 && ((GPIOJ12__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    GPIOJ12_Write(uint8 value);
void    GPIOJ12_SetDriveMode(uint8 mode);
uint8   GPIOJ12_ReadDataReg(void);
uint8   GPIOJ12_Read(void);
void    GPIOJ12_SetInterruptMode(uint16 position, uint16 mode);
uint8   GPIOJ12_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the GPIOJ12_SetDriveMode() function.
     *  @{
     */
        #define GPIOJ12_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define GPIOJ12_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define GPIOJ12_DM_RES_UP          PIN_DM_RES_UP
        #define GPIOJ12_DM_RES_DWN         PIN_DM_RES_DWN
        #define GPIOJ12_DM_OD_LO           PIN_DM_OD_LO
        #define GPIOJ12_DM_OD_HI           PIN_DM_OD_HI
        #define GPIOJ12_DM_STRONG          PIN_DM_STRONG
        #define GPIOJ12_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define GPIOJ12_MASK               GPIOJ12__MASK
#define GPIOJ12_SHIFT              GPIOJ12__SHIFT
#define GPIOJ12_WIDTH              1u

/* Interrupt constants */
#if defined(GPIOJ12__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in GPIOJ12_SetInterruptMode() function.
     *  @{
     */
        #define GPIOJ12_INTR_NONE      (uint16)(0x0000u)
        #define GPIOJ12_INTR_RISING    (uint16)(0x0001u)
        #define GPIOJ12_INTR_FALLING   (uint16)(0x0002u)
        #define GPIOJ12_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define GPIOJ12_INTR_MASK      (0x01u) 
#endif /* (GPIOJ12__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define GPIOJ12_PS                     (* (reg8 *) GPIOJ12__PS)
/* Data Register */
#define GPIOJ12_DR                     (* (reg8 *) GPIOJ12__DR)
/* Port Number */
#define GPIOJ12_PRT_NUM                (* (reg8 *) GPIOJ12__PRT) 
/* Connect to Analog Globals */                                                  
#define GPIOJ12_AG                     (* (reg8 *) GPIOJ12__AG)                       
/* Analog MUX bux enable */
#define GPIOJ12_AMUX                   (* (reg8 *) GPIOJ12__AMUX) 
/* Bidirectional Enable */                                                        
#define GPIOJ12_BIE                    (* (reg8 *) GPIOJ12__BIE)
/* Bit-mask for Aliased Register Access */
#define GPIOJ12_BIT_MASK               (* (reg8 *) GPIOJ12__BIT_MASK)
/* Bypass Enable */
#define GPIOJ12_BYP                    (* (reg8 *) GPIOJ12__BYP)
/* Port wide control signals */                                                   
#define GPIOJ12_CTL                    (* (reg8 *) GPIOJ12__CTL)
/* Drive Modes */
#define GPIOJ12_DM0                    (* (reg8 *) GPIOJ12__DM0) 
#define GPIOJ12_DM1                    (* (reg8 *) GPIOJ12__DM1)
#define GPIOJ12_DM2                    (* (reg8 *) GPIOJ12__DM2) 
/* Input Buffer Disable Override */
#define GPIOJ12_INP_DIS                (* (reg8 *) GPIOJ12__INP_DIS)
/* LCD Common or Segment Drive */
#define GPIOJ12_LCD_COM_SEG            (* (reg8 *) GPIOJ12__LCD_COM_SEG)
/* Enable Segment LCD */
#define GPIOJ12_LCD_EN                 (* (reg8 *) GPIOJ12__LCD_EN)
/* Slew Rate Control */
#define GPIOJ12_SLW                    (* (reg8 *) GPIOJ12__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define GPIOJ12_PRTDSI__CAPS_SEL       (* (reg8 *) GPIOJ12__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define GPIOJ12_PRTDSI__DBL_SYNC_IN    (* (reg8 *) GPIOJ12__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define GPIOJ12_PRTDSI__OE_SEL0        (* (reg8 *) GPIOJ12__PRTDSI__OE_SEL0) 
#define GPIOJ12_PRTDSI__OE_SEL1        (* (reg8 *) GPIOJ12__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define GPIOJ12_PRTDSI__OUT_SEL0       (* (reg8 *) GPIOJ12__PRTDSI__OUT_SEL0) 
#define GPIOJ12_PRTDSI__OUT_SEL1       (* (reg8 *) GPIOJ12__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define GPIOJ12_PRTDSI__SYNC_OUT       (* (reg8 *) GPIOJ12__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(GPIOJ12__SIO_CFG)
    #define GPIOJ12_SIO_HYST_EN        (* (reg8 *) GPIOJ12__SIO_HYST_EN)
    #define GPIOJ12_SIO_REG_HIFREQ     (* (reg8 *) GPIOJ12__SIO_REG_HIFREQ)
    #define GPIOJ12_SIO_CFG            (* (reg8 *) GPIOJ12__SIO_CFG)
    #define GPIOJ12_SIO_DIFF           (* (reg8 *) GPIOJ12__SIO_DIFF)
#endif /* (GPIOJ12__SIO_CFG) */

/* Interrupt Registers */
#if defined(GPIOJ12__INTSTAT)
    #define GPIOJ12_INTSTAT            (* (reg8 *) GPIOJ12__INTSTAT)
    #define GPIOJ12_SNAP               (* (reg8 *) GPIOJ12__SNAP)
    
	#define GPIOJ12_0_INTTYPE_REG 		(* (reg8 *) GPIOJ12__0__INTTYPE)
#endif /* (GPIOJ12__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_GPIOJ12_H */


/* [] END OF FILE */
