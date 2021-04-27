/*******************************************************************************
* File Name: GPIOJ13.h  
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

#if !defined(CY_PINS_GPIOJ13_H) /* Pins GPIOJ13_H */
#define CY_PINS_GPIOJ13_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "GPIOJ13_aliases.h"

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 GPIOJ13__PORT == 15 && ((GPIOJ13__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

/**
* \addtogroup group_general
* @{
*/
void    GPIOJ13_Write(uint8 value);
void    GPIOJ13_SetDriveMode(uint8 mode);
uint8   GPIOJ13_ReadDataReg(void);
uint8   GPIOJ13_Read(void);
void    GPIOJ13_SetInterruptMode(uint16 position, uint16 mode);
uint8   GPIOJ13_ClearInterrupt(void);
/** @} general */

/***************************************
*           API Constants        
***************************************/
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup driveMode Drive mode constants
     * \brief Constants to be passed as "mode" parameter in the GPIOJ13_SetDriveMode() function.
     *  @{
     */
        #define GPIOJ13_DM_ALG_HIZ         PIN_DM_ALG_HIZ
        #define GPIOJ13_DM_DIG_HIZ         PIN_DM_DIG_HIZ
        #define GPIOJ13_DM_RES_UP          PIN_DM_RES_UP
        #define GPIOJ13_DM_RES_DWN         PIN_DM_RES_DWN
        #define GPIOJ13_DM_OD_LO           PIN_DM_OD_LO
        #define GPIOJ13_DM_OD_HI           PIN_DM_OD_HI
        #define GPIOJ13_DM_STRONG          PIN_DM_STRONG
        #define GPIOJ13_DM_RES_UPDWN       PIN_DM_RES_UPDWN
    /** @} driveMode */
/** @} group_constants */
    
/* Digital Port Constants */
#define GPIOJ13_MASK               GPIOJ13__MASK
#define GPIOJ13_SHIFT              GPIOJ13__SHIFT
#define GPIOJ13_WIDTH              1u

/* Interrupt constants */
#if defined(GPIOJ13__INTSTAT)
/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in GPIOJ13_SetInterruptMode() function.
     *  @{
     */
        #define GPIOJ13_INTR_NONE      (uint16)(0x0000u)
        #define GPIOJ13_INTR_RISING    (uint16)(0x0001u)
        #define GPIOJ13_INTR_FALLING   (uint16)(0x0002u)
        #define GPIOJ13_INTR_BOTH      (uint16)(0x0003u) 
    /** @} intrMode */
/** @} group_constants */

    #define GPIOJ13_INTR_MASK      (0x01u) 
#endif /* (GPIOJ13__INTSTAT) */


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define GPIOJ13_PS                     (* (reg8 *) GPIOJ13__PS)
/* Data Register */
#define GPIOJ13_DR                     (* (reg8 *) GPIOJ13__DR)
/* Port Number */
#define GPIOJ13_PRT_NUM                (* (reg8 *) GPIOJ13__PRT) 
/* Connect to Analog Globals */                                                  
#define GPIOJ13_AG                     (* (reg8 *) GPIOJ13__AG)                       
/* Analog MUX bux enable */
#define GPIOJ13_AMUX                   (* (reg8 *) GPIOJ13__AMUX) 
/* Bidirectional Enable */                                                        
#define GPIOJ13_BIE                    (* (reg8 *) GPIOJ13__BIE)
/* Bit-mask for Aliased Register Access */
#define GPIOJ13_BIT_MASK               (* (reg8 *) GPIOJ13__BIT_MASK)
/* Bypass Enable */
#define GPIOJ13_BYP                    (* (reg8 *) GPIOJ13__BYP)
/* Port wide control signals */                                                   
#define GPIOJ13_CTL                    (* (reg8 *) GPIOJ13__CTL)
/* Drive Modes */
#define GPIOJ13_DM0                    (* (reg8 *) GPIOJ13__DM0) 
#define GPIOJ13_DM1                    (* (reg8 *) GPIOJ13__DM1)
#define GPIOJ13_DM2                    (* (reg8 *) GPIOJ13__DM2) 
/* Input Buffer Disable Override */
#define GPIOJ13_INP_DIS                (* (reg8 *) GPIOJ13__INP_DIS)
/* LCD Common or Segment Drive */
#define GPIOJ13_LCD_COM_SEG            (* (reg8 *) GPIOJ13__LCD_COM_SEG)
/* Enable Segment LCD */
#define GPIOJ13_LCD_EN                 (* (reg8 *) GPIOJ13__LCD_EN)
/* Slew Rate Control */
#define GPIOJ13_SLW                    (* (reg8 *) GPIOJ13__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define GPIOJ13_PRTDSI__CAPS_SEL       (* (reg8 *) GPIOJ13__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define GPIOJ13_PRTDSI__DBL_SYNC_IN    (* (reg8 *) GPIOJ13__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define GPIOJ13_PRTDSI__OE_SEL0        (* (reg8 *) GPIOJ13__PRTDSI__OE_SEL0) 
#define GPIOJ13_PRTDSI__OE_SEL1        (* (reg8 *) GPIOJ13__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define GPIOJ13_PRTDSI__OUT_SEL0       (* (reg8 *) GPIOJ13__PRTDSI__OUT_SEL0) 
#define GPIOJ13_PRTDSI__OUT_SEL1       (* (reg8 *) GPIOJ13__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define GPIOJ13_PRTDSI__SYNC_OUT       (* (reg8 *) GPIOJ13__PRTDSI__SYNC_OUT) 

/* SIO registers */
#if defined(GPIOJ13__SIO_CFG)
    #define GPIOJ13_SIO_HYST_EN        (* (reg8 *) GPIOJ13__SIO_HYST_EN)
    #define GPIOJ13_SIO_REG_HIFREQ     (* (reg8 *) GPIOJ13__SIO_REG_HIFREQ)
    #define GPIOJ13_SIO_CFG            (* (reg8 *) GPIOJ13__SIO_CFG)
    #define GPIOJ13_SIO_DIFF           (* (reg8 *) GPIOJ13__SIO_DIFF)
#endif /* (GPIOJ13__SIO_CFG) */

/* Interrupt Registers */
#if defined(GPIOJ13__INTSTAT)
    #define GPIOJ13_INTSTAT            (* (reg8 *) GPIOJ13__INTSTAT)
    #define GPIOJ13_SNAP               (* (reg8 *) GPIOJ13__SNAP)
    
	#define GPIOJ13_0_INTTYPE_REG 		(* (reg8 *) GPIOJ13__0__INTTYPE)
#endif /* (GPIOJ13__INTSTAT) */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_GPIOJ13_H */


/* [] END OF FILE */
