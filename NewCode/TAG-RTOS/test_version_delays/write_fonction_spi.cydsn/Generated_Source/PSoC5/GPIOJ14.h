/*******************************************************************************
* File Name: GPIOJ14.h  
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

#if !defined(CY_PINS_GPIOJ14_H) /* Pins GPIOJ14_H */
#define CY_PINS_GPIOJ14_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "GPIOJ14_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 GPIOJ14__PORT == 15 && ((GPIOJ14__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    GPIOJ14_Write(uint8 value);
void    GPIOJ14_SetDriveMode(uint8 mode);
uint8   GPIOJ14_ReadDataReg(void);
uint8   GPIOJ14_Read(void);
void    GPIOJ14_SetInterruptMode(uint16 position, uint16 mode);
uint8   GPIOJ14_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the GPIOJ14_SetDriveMode() function.
     *  @{
     */
        #define GPIOJ14_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define GPIOJ14_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define GPIOJ14_DM_RES_UP          PIN_DM_RES_UP
        #define GPIOJ14_DM_RES_DWN         PIN_DM_RES_DWN
        #define GPIOJ14_DM_OD_LO           PIN_DM_OD_LO
        #define GPIOJ14_DM_OD_HI           PIN_DM_OD_HI
        #define GPIOJ14_DM_STRONG          PIN_DM_STRONG
        #define GPIOJ14_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define GPIOJ14_MASK               GPIOJ14__MASK
#define GPIOJ14_SHIFT              GPIOJ14__SHIFT
#define GPIOJ14_WIDTH              1u

/* Interrupt constants */
#if defined(GPIOJ14__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in GPIOJ14_SetInterruptMode() function.
     *  @{
     */
        #define GPIOJ14_INTR_NONE      (uint16)(0x0000u)
        #define GPIOJ14_INTR_RISING    (uint16)(0x0001u)
        #define GPIOJ14_INTR_FALLING   (uint16)(0x0002u)
        #define GPIOJ14_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define GPIOJ14_INTR_MASK      (0x01u) 
#endif /* (GPIOJ14__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define GPIOJ14_PS                     (* (reg8 *) GPIOJ14__PS)
/* Data Register */
#define GPIOJ14_DR                     (* (reg8 *) GPIOJ14__DR)
/* Port Number */
#define GPIOJ14_PRT_NUM                (* (reg8 *) GPIOJ14__PRT) 
/* Connect to Analog Globals */                                                  
#define GPIOJ14_AG                     (* (reg8 *) GPIOJ14__AG)                       
/* Analog MUX bux enable */
#define GPIOJ14_AMUX                   (* (reg8 *) GPIOJ14__AMUX) 
/* Bidirectional Enable */                                                        
#define GPIOJ14_BIE                    (* (reg8 *) GPIOJ14__BIE)
/* Bit-mask for Aliased Register Access */
#define GPIOJ14_BIT_MASK               (* (reg8 *) GPIOJ14__BIT_MASK)
/* Bypass Enable */
#define GPIOJ14_BYP                    (* (reg8 *) GPIOJ14__BYP)
/* Port wide control signals */                                                   
#define GPIOJ14_CTL                    (* (reg8 *) GPIOJ14__CTL)
/* Drive Modes */
#define GPIOJ14_DM0                    (* (reg8 *) GPIOJ14__DM0) 
#define GPIOJ14_DM1                    (* (reg8 *) GPIOJ14__DM1)
#define GPIOJ14_DM2                    (* (reg8 *) GPIOJ14__DM2) 
/* Input Buffer Disable Override */
#define GPIOJ14_INP_DIS                (* (reg8 *) GPIOJ14__INP_DIS)
/* LCD Common or Segment Drive */
#define GPIOJ14_LCD_COM_SEG            (* (reg8 *) GPIOJ14__LCD_COM_SEG)
/* Enable Segment LCD */
#define GPIOJ14_LCD_EN                 (* (reg8 *) GPIOJ14__LCD_EN)
/* Slew Rate Control */
#define GPIOJ14_SLW                    (* (reg8 *) GPIOJ14__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define GPIOJ14_PRTDSI__CAPS_SEL       (* (reg8 *) GPIOJ14__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define GPIOJ14_PRTDSI__DBL_SYNC_IN    (* (reg8 *) GPIOJ14__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define GPIOJ14_PRTDSI__OE_SEL0        (* (reg8 *) GPIOJ14__PRTDSI__OE_SEL0) 
#define GPIOJ14_PRTDSI__OE_SEL1        (* (reg8 *) GPIOJ14__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define GPIOJ14_PRTDSI__OUT_SEL0       (* (reg8 *) GPIOJ14__PRTDSI__OUT_SEL0) 
#define GPIOJ14_PRTDSI__OUT_SEL1       (* (reg8 *) GPIOJ14__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define GPIOJ14_PRTDSI__SYNC_OUT       (* (reg8 *) GPIOJ14__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(GPIOJ14__SIO_CFG)
    #define GPIOJ14_SIO_HYST_EN        (* (reg8 *) GPIOJ14__SIO_HYST_EN)
    #define GPIOJ14_SIO_REG_HIFREQ     (* (reg8 *) GPIOJ14__SIO_REG_HIFREQ)
    #define GPIOJ14_SIO_CFG            (* (reg8 *) GPIOJ14__SIO_CFG)
    #define GPIOJ14_SIO_DIFF           (* (reg8 *) GPIOJ14__SIO_DIFF)
#endif /* (GPIOJ14__SIO_CFG) */

/* Interrupt Registers */
#if defined(GPIOJ14__INTSTAT)
    #define GPIOJ14_INTSTAT            (* (reg8 *) GPIOJ14__INTSTAT)
    #define GPIOJ14_SNAP               (* (reg8 *) GPIOJ14__SNAP)
    
	#define GPIOJ14_0_INTTYPE_REG 		(* (reg8 *) GPIOJ14__0__INTTYPE)
#endif /* (GPIOJ14__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_GPIOJ14_H */


/* [] END OF FILE */
