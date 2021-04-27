/*******************************************************************************
* File Name: GPIOJ11.h  
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

#if !defined(CY_PINS_GPIOJ11_H) /* Pins GPIOJ11_H */
#define CY_PINS_GPIOJ11_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "GPIOJ11_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 GPIOJ11__PORT == 15 && ((GPIOJ11__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    GPIOJ11_Write(uint8 value);
void    GPIOJ11_SetDriveMode(uint8 mode);
uint8   GPIOJ11_ReadDataReg(void);
uint8   GPIOJ11_Read(void);
void    GPIOJ11_SetInterruptMode(uint16 position, uint16 mode);
uint8   GPIOJ11_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the GPIOJ11_SetDriveMode() function.
     *  @{
     */
        #define GPIOJ11_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define GPIOJ11_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define GPIOJ11_DM_RES_UP          PIN_DM_RES_UP
        #define GPIOJ11_DM_RES_DWN         PIN_DM_RES_DWN
        #define GPIOJ11_DM_OD_LO           PIN_DM_OD_LO
        #define GPIOJ11_DM_OD_HI           PIN_DM_OD_HI
        #define GPIOJ11_DM_STRONG          PIN_DM_STRONG
        #define GPIOJ11_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define GPIOJ11_MASK               GPIOJ11__MASK
#define GPIOJ11_SHIFT              GPIOJ11__SHIFT
#define GPIOJ11_WIDTH              1u

/* Interrupt constants */
#if defined(GPIOJ11__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in GPIOJ11_SetInterruptMode() function.
     *  @{
     */
        #define GPIOJ11_INTR_NONE      (uint16)(0x0000u)
        #define GPIOJ11_INTR_RISING    (uint16)(0x0001u)
        #define GPIOJ11_INTR_FALLING   (uint16)(0x0002u)
        #define GPIOJ11_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define GPIOJ11_INTR_MASK      (0x01u) 
#endif /* (GPIOJ11__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define GPIOJ11_PS                     (* (reg8 *) GPIOJ11__PS)
/* Data Register */
#define GPIOJ11_DR                     (* (reg8 *) GPIOJ11__DR)
/* Port Number */
#define GPIOJ11_PRT_NUM                (* (reg8 *) GPIOJ11__PRT) 
/* Connect to Analog Globals */                                                  
#define GPIOJ11_AG                     (* (reg8 *) GPIOJ11__AG)                       
/* Analog MUX bux enable */
#define GPIOJ11_AMUX                   (* (reg8 *) GPIOJ11__AMUX) 
/* Bidirectional Enable */                                                        
#define GPIOJ11_BIE                    (* (reg8 *) GPIOJ11__BIE)
/* Bit-mask for Aliased Register Access */
#define GPIOJ11_BIT_MASK               (* (reg8 *) GPIOJ11__BIT_MASK)
/* Bypass Enable */
#define GPIOJ11_BYP                    (* (reg8 *) GPIOJ11__BYP)
/* Port wide control signals */                                                   
#define GPIOJ11_CTL                    (* (reg8 *) GPIOJ11__CTL)
/* Drive Modes */
#define GPIOJ11_DM0                    (* (reg8 *) GPIOJ11__DM0) 
#define GPIOJ11_DM1                    (* (reg8 *) GPIOJ11__DM1)
#define GPIOJ11_DM2                    (* (reg8 *) GPIOJ11__DM2) 
/* Input Buffer Disable Override */
#define GPIOJ11_INP_DIS                (* (reg8 *) GPIOJ11__INP_DIS)
/* LCD Common or Segment Drive */
#define GPIOJ11_LCD_COM_SEG            (* (reg8 *) GPIOJ11__LCD_COM_SEG)
/* Enable Segment LCD */
#define GPIOJ11_LCD_EN                 (* (reg8 *) GPIOJ11__LCD_EN)
/* Slew Rate Control */
#define GPIOJ11_SLW                    (* (reg8 *) GPIOJ11__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define GPIOJ11_PRTDSI__CAPS_SEL       (* (reg8 *) GPIOJ11__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define GPIOJ11_PRTDSI__DBL_SYNC_IN    (* (reg8 *) GPIOJ11__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define GPIOJ11_PRTDSI__OE_SEL0        (* (reg8 *) GPIOJ11__PRTDSI__OE_SEL0) 
#define GPIOJ11_PRTDSI__OE_SEL1        (* (reg8 *) GPIOJ11__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define GPIOJ11_PRTDSI__OUT_SEL0       (* (reg8 *) GPIOJ11__PRTDSI__OUT_SEL0) 
#define GPIOJ11_PRTDSI__OUT_SEL1       (* (reg8 *) GPIOJ11__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define GPIOJ11_PRTDSI__SYNC_OUT       (* (reg8 *) GPIOJ11__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(GPIOJ11__SIO_CFG)
    #define GPIOJ11_SIO_HYST_EN        (* (reg8 *) GPIOJ11__SIO_HYST_EN)
    #define GPIOJ11_SIO_REG_HIFREQ     (* (reg8 *) GPIOJ11__SIO_REG_HIFREQ)
    #define GPIOJ11_SIO_CFG            (* (reg8 *) GPIOJ11__SIO_CFG)
    #define GPIOJ11_SIO_DIFF           (* (reg8 *) GPIOJ11__SIO_DIFF)
#endif /* (GPIOJ11__SIO_CFG) */

/* Interrupt Registers */
#if defined(GPIOJ11__INTSTAT)
    #define GPIOJ11_INTSTAT            (* (reg8 *) GPIOJ11__INTSTAT)
    #define GPIOJ11_SNAP               (* (reg8 *) GPIOJ11__SNAP)
    
	#define GPIOJ11_0_INTTYPE_REG 		(* (reg8 *) GPIOJ11__0__INTTYPE)
#endif /* (GPIOJ11__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_GPIOJ11_H */


/* [] END OF FILE */
