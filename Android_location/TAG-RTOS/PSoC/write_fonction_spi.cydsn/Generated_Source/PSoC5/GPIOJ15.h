/*******************************************************************************
* File Name: GPIOJ15.h  
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

#if !defined(CY_PINS_GPIOJ15_H) /* Pins GPIOJ15_H */
#define CY_PINS_GPIOJ15_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "GPIOJ15_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 GPIOJ15__PORT == 15 && ((GPIOJ15__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    GPIOJ15_Write(uint8 value);
void    GPIOJ15_SetDriveMode(uint8 mode);
uint8   GPIOJ15_ReadDataReg(void);
uint8   GPIOJ15_Read(void);
void    GPIOJ15_SetInterruptMode(uint16 position, uint16 mode);
uint8   GPIOJ15_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the GPIOJ15_SetDriveMode() function.
     *  @{
     */
        #define GPIOJ15_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define GPIOJ15_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define GPIOJ15_DM_RES_UP          PIN_DM_RES_UP
        #define GPIOJ15_DM_RES_DWN         PIN_DM_RES_DWN
        #define GPIOJ15_DM_OD_LO           PIN_DM_OD_LO
        #define GPIOJ15_DM_OD_HI           PIN_DM_OD_HI
        #define GPIOJ15_DM_STRONG          PIN_DM_STRONG
        #define GPIOJ15_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define GPIOJ15_MASK               GPIOJ15__MASK
#define GPIOJ15_SHIFT              GPIOJ15__SHIFT
#define GPIOJ15_WIDTH              1u

/* Interrupt constants */
#if defined(GPIOJ15__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in GPIOJ15_SetInterruptMode() function.
     *  @{
     */
        #define GPIOJ15_INTR_NONE      (uint16)(0x0000u)
        #define GPIOJ15_INTR_RISING    (uint16)(0x0001u)
        #define GPIOJ15_INTR_FALLING   (uint16)(0x0002u)
        #define GPIOJ15_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define GPIOJ15_INTR_MASK      (0x01u) 
#endif /* (GPIOJ15__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define GPIOJ15_PS                     (* (reg8 *) GPIOJ15__PS)
/* Data Register */
#define GPIOJ15_DR                     (* (reg8 *) GPIOJ15__DR)
/* Port Number */
#define GPIOJ15_PRT_NUM                (* (reg8 *) GPIOJ15__PRT) 
/* Connect to Analog Globals */                                                  
#define GPIOJ15_AG                     (* (reg8 *) GPIOJ15__AG)                       
/* Analog MUX bux enable */
#define GPIOJ15_AMUX                   (* (reg8 *) GPIOJ15__AMUX) 
/* Bidirectional Enable */                                                        
#define GPIOJ15_BIE                    (* (reg8 *) GPIOJ15__BIE)
/* Bit-mask for Aliased Register Access */
#define GPIOJ15_BIT_MASK               (* (reg8 *) GPIOJ15__BIT_MASK)
/* Bypass Enable */
#define GPIOJ15_BYP                    (* (reg8 *) GPIOJ15__BYP)
/* Port wide control signals */                                                   
#define GPIOJ15_CTL                    (* (reg8 *) GPIOJ15__CTL)
/* Drive Modes */
#define GPIOJ15_DM0                    (* (reg8 *) GPIOJ15__DM0) 
#define GPIOJ15_DM1                    (* (reg8 *) GPIOJ15__DM1)
#define GPIOJ15_DM2                    (* (reg8 *) GPIOJ15__DM2) 
/* Input Buffer Disable Override */
#define GPIOJ15_INP_DIS                (* (reg8 *) GPIOJ15__INP_DIS)
/* LCD Common or Segment Drive */
#define GPIOJ15_LCD_COM_SEG            (* (reg8 *) GPIOJ15__LCD_COM_SEG)
/* Enable Segment LCD */
#define GPIOJ15_LCD_EN                 (* (reg8 *) GPIOJ15__LCD_EN)
/* Slew Rate Control */
#define GPIOJ15_SLW                    (* (reg8 *) GPIOJ15__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define GPIOJ15_PRTDSI__CAPS_SEL       (* (reg8 *) GPIOJ15__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define GPIOJ15_PRTDSI__DBL_SYNC_IN    (* (reg8 *) GPIOJ15__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define GPIOJ15_PRTDSI__OE_SEL0        (* (reg8 *) GPIOJ15__PRTDSI__OE_SEL0) 
#define GPIOJ15_PRTDSI__OE_SEL1        (* (reg8 *) GPIOJ15__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define GPIOJ15_PRTDSI__OUT_SEL0       (* (reg8 *) GPIOJ15__PRTDSI__OUT_SEL0) 
#define GPIOJ15_PRTDSI__OUT_SEL1       (* (reg8 *) GPIOJ15__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define GPIOJ15_PRTDSI__SYNC_OUT       (* (reg8 *) GPIOJ15__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(GPIOJ15__SIO_CFG)
    #define GPIOJ15_SIO_HYST_EN        (* (reg8 *) GPIOJ15__SIO_HYST_EN)
    #define GPIOJ15_SIO_REG_HIFREQ     (* (reg8 *) GPIOJ15__SIO_REG_HIFREQ)
    #define GPIOJ15_SIO_CFG            (* (reg8 *) GPIOJ15__SIO_CFG)
    #define GPIOJ15_SIO_DIFF           (* (reg8 *) GPIOJ15__SIO_DIFF)
#endif /* (GPIOJ15__SIO_CFG) */

/* Interrupt Registers */
#if defined(GPIOJ15__INTSTAT)
    #define GPIOJ15_INTSTAT            (* (reg8 *) GPIOJ15__INTSTAT)
    #define GPIOJ15_SNAP               (* (reg8 *) GPIOJ15__SNAP)
    
	#define GPIOJ15_0_INTTYPE_REG 		(* (reg8 *) GPIOJ15__0__INTTYPE)
#endif /* (GPIOJ15__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_GPIOJ15_H */


/* [] END OF FILE */
