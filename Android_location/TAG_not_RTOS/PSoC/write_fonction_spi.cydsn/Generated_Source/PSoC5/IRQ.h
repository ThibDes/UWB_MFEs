/*******************************************************************************
* File Name: IRQ.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_IRQ_H)
#define CY_ISR_IRQ_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void IRQ_Start(void);
void IRQ_StartEx(cyisraddress address);
void IRQ_Stop(void);

CY_ISR_PROTO(IRQ_Interrupt);

void IRQ_SetVector(cyisraddress address);
cyisraddress IRQ_GetVector(void);

void IRQ_SetPriority(uint8 priority);
uint8 IRQ_GetPriority(void);

void IRQ_Enable(void);
uint8 IRQ_GetState(void);
void IRQ_Disable(void);

void IRQ_SetPending(void);
void IRQ_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the IRQ ISR. */
#define IRQ_INTC_VECTOR            ((reg32 *) IRQ__INTC_VECT)

/* Address of the IRQ ISR priority. */
#define IRQ_INTC_PRIOR             ((reg8 *) IRQ__INTC_PRIOR_REG)

/* Priority of the IRQ interrupt. */
#define IRQ_INTC_PRIOR_NUMBER      IRQ__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable IRQ interrupt. */
#define IRQ_INTC_SET_EN            ((reg32 *) IRQ__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the IRQ interrupt. */
#define IRQ_INTC_CLR_EN            ((reg32 *) IRQ__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the IRQ interrupt state to pending. */
#define IRQ_INTC_SET_PD            ((reg32 *) IRQ__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the IRQ interrupt. */
#define IRQ_INTC_CLR_PD            ((reg32 *) IRQ__INTC_CLR_PD_REG)


#endif /* CY_ISR_IRQ_H */


/* [] END OF FILE */
