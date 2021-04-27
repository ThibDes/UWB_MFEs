/***************************************************************************//**
* \file USBP_episr.c
* \version 3.20
*
* \brief
*  This file contains the Data endpoint Interrupt Service Routines.
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USBP_pvt.h"
#include "USBP_cydmac.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


#if (USBP_EP1_ISR_ACTIVE)
    /******************************************************************************
    * Function Name: USBP_EP_1_ISR
    ***************************************************************************//**
    *
    *  Endpoint 1 Interrupt Service Routine
    *
    ******************************************************************************/
    CY_ISR(USBP_EP_1_ISR)
    {

    #ifdef USBP_EP_1_ISR_ENTRY_CALLBACK
        USBP_EP_1_ISR_EntryCallback();
    #endif /* (USBP_EP_1_ISR_ENTRY_CALLBACK) */

        /* `#START EP1_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */
    
        USBP_ClearSieEpInterruptSource(USBP_SIE_INT_EP1_INTR);
            
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to be read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USBP_EP[USBP_EP1].addr & USBP_DIR_IN))
    #endif /* (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) USBP_SIE_EP_BASE.sieEp[USBP_EP1].epCr0;
            
            /* Toggle all endpoint types except ISOC. */
            if (USBP_GET_EP_TYPE(USBP_EP1) != USBP_EP_TYPE_ISOC)
            {
                USBP_EP[USBP_EP1].epToggle ^= USBP_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USBP_EP[USBP_EP1].apiEpState = USBP_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USBP_ENABLE_MIDI_STREAMING) && \
            !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
            if (USBP_midi_out_ep == USBP_EP1)
            {
                USBP_MIDI_OUT_Service();
            }
        #endif /* (USBP_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)) */
    
        /* `#START EP1_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USBP_EP_1_ISR_EXIT_CALLBACK
        USBP_EP_1_ISR_ExitCallback();
    #endif /* (USBP_EP_1_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */
    }

#endif /* (USBP_EP1_ISR_ACTIVE) */


#if (USBP_EP2_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USBP_EP_2_ISR
    ****************************************************************************//**
    *
    *  Endpoint 2 Interrupt Service Routine.
    *
    *******************************************************************************/
    CY_ISR(USBP_EP_2_ISR)
    {
    #ifdef USBP_EP_2_ISR_ENTRY_CALLBACK
        USBP_EP_2_ISR_EntryCallback();
    #endif /* (USBP_EP_2_ISR_ENTRY_CALLBACK) */

        /* `#START EP2_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */

        USBP_ClearSieEpInterruptSource(USBP_SIE_INT_EP2_INTR);

        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to be read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USBP_EP[USBP_EP2].addr & USBP_DIR_IN))
    #endif /* (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO) */
        {            
            /* Read CR0 register to clear SIE lock. */
            (void) USBP_SIE_EP_BASE.sieEp[USBP_EP2].epCr0;
            
            /* Toggle all endpoint types except ISOC. */
            if (USBP_GET_EP_TYPE(USBP_EP2) != USBP_EP_TYPE_ISOC)
            {
                USBP_EP[USBP_EP2].epToggle ^= USBP_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USBP_EP[USBP_EP2].apiEpState = USBP_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USBP_ENABLE_MIDI_STREAMING) && \
            !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
            if (USBP_midi_out_ep == USBP_EP2)
            {
                USBP_MIDI_OUT_Service();
            }
        #endif /* (USBP_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)) */        
    
        /* `#START EP2_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USBP_EP_2_ISR_EXIT_CALLBACK
        USBP_EP_2_ISR_ExitCallback();
    #endif /* (USBP_EP_2_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USBP_EP2_ISR_ACTIVE) */


#if (USBP_EP3_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USBP_EP_3_ISR
    ****************************************************************************//**
    *
    *  Endpoint 3 Interrupt Service Routine.
    *
    *******************************************************************************/
    CY_ISR(USBP_EP_3_ISR)
    {
    #ifdef USBP_EP_3_ISR_ENTRY_CALLBACK
        USBP_EP_3_ISR_EntryCallback();
    #endif /* (USBP_EP_3_ISR_ENTRY_CALLBACK) */

        /* `#START EP3_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */

        USBP_ClearSieEpInterruptSource(USBP_SIE_INT_EP3_INTR);    

        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to be read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USBP_EP[USBP_EP3].addr & USBP_DIR_IN))
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
        {            
            /* Read CR0 register to clear SIE lock. */
            (void) USBP_SIE_EP_BASE.sieEp[USBP_EP3].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USBP_GET_EP_TYPE(USBP_EP3) != USBP_EP_TYPE_ISOC)
            {
                USBP_EP[USBP_EP3].epToggle ^= USBP_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USBP_EP[USBP_EP3].apiEpState = USBP_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USBP_ENABLE_MIDI_STREAMING) && \
            !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
            if (USBP_midi_out_ep == USBP_EP3)
            {
                USBP_MIDI_OUT_Service();
            }
        #endif /* (USBP_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)) */        

        /* `#START EP3_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USBP_EP_3_ISR_EXIT_CALLBACK
        USBP_EP_3_ISR_ExitCallback();
    #endif /* (USBP_EP_3_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USBP_EP3_ISR_ACTIVE) */


#if (USBP_EP4_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USBP_EP_4_ISR
    ****************************************************************************//**
    *
    *  Endpoint 4 Interrupt Service Routine.
    *
    *******************************************************************************/
    CY_ISR(USBP_EP_4_ISR)
    {
    #ifdef USBP_EP_4_ISR_ENTRY_CALLBACK
        USBP_EP_4_ISR_EntryCallback();
    #endif /* (USBP_EP_4_ISR_ENTRY_CALLBACK) */

        /* `#START EP4_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */

        USBP_ClearSieEpInterruptSource(USBP_SIE_INT_EP4_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USBP_EP[USBP_EP4].addr & USBP_DIR_IN))
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) USBP_SIE_EP_BASE.sieEp[USBP_EP4].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USBP_GET_EP_TYPE(USBP_EP4) != USBP_EP_TYPE_ISOC)
            {
                USBP_EP[USBP_EP4].epToggle ^= USBP_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USBP_EP[USBP_EP4].apiEpState = USBP_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USBP_ENABLE_MIDI_STREAMING) && \
            !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
            if(USBP_midi_out_ep == USBP_EP4)
            {
                USBP_MIDI_OUT_Service();
            }
        #endif /* (USBP_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)) */        

        /* `#START EP4_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USBP_EP_4_ISR_EXIT_CALLBACK
        USBP_EP_4_ISR_ExitCallback();
    #endif /* (USBP_EP_4_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USBP_EP4_ISR_ACTIVE) */


#if (USBP_EP5_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USBP_EP_5_ISR
    ****************************************************************************//**
    *
    *  Endpoint 5 Interrupt Service Routine
    *
    *
    *******************************************************************************/
    CY_ISR(USBP_EP_5_ISR)
    {
    #ifdef USBP_EP_5_ISR_ENTRY_CALLBACK
        USBP_EP_5_ISR_EntryCallback();
    #endif /* (USBP_EP_5_ISR_ENTRY_CALLBACK) */

        /* `#START EP5_USER_CODE` Place your code here */

        /* `#END` */

    #if (defined(USBP_ENABLE_MIDI_STREAMING) && !defined(USBP_MAIN_SERVICE_MIDI_OUT) && \
                 USBP_ISR_SERVICE_MIDI_OUT && CY_PSOC3)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */

        USBP_ClearSieEpInterruptSource(USBP_SIE_INT_EP5_INTR);
    
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USBP_EP[USBP_EP5].addr & USBP_DIR_IN))
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
        {            
            /* Read CR0 register to clear SIE lock. */
            (void) USBP_SIE_EP_BASE.sieEp[USBP_EP5].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USBP_GET_EP_TYPE(USBP_EP5) != USBP_EP_TYPE_ISOC)
            {
                USBP_EP[USBP_EP5].epToggle ^= USBP_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USBP_EP[USBP_EP5].apiEpState = USBP_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO))        
        #if (defined(USBP_ENABLE_MIDI_STREAMING) && \
            !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
            if (USBP_midi_out_ep == USBP_EP5)
            {
                USBP_MIDI_OUT_Service();
            }
        #endif /* (USBP_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP5_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USBP_EP_5_ISR_EXIT_CALLBACK
        USBP_EP_5_ISR_ExitCallback();
    #endif /* (USBP_EP_5_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USBP_EP5_ISR_ACTIVE) */


#if (USBP_EP6_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USBP_EP_6_ISR
    ****************************************************************************//**
    *
    *  Endpoint 6 Interrupt Service Routine.
    *
    *
    *******************************************************************************/
    CY_ISR(USBP_EP_6_ISR)
    {
    #ifdef USBP_EP_6_ISR_ENTRY_CALLBACK
        USBP_EP_6_ISR_EntryCallback();
    #endif /* (USBP_EP_6_ISR_ENTRY_CALLBACK) */

        /* `#START EP6_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */

        USBP_ClearSieEpInterruptSource(USBP_SIE_INT_EP6_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USBP_EP[USBP_EP6].addr & USBP_DIR_IN))
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) USBP_SIE_EP_BASE.sieEp[USBP_EP6].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USBP_GET_EP_TYPE(USBP_EP6) != USBP_EP_TYPE_ISOC)
            {
                USBP_EP[USBP_EP6].epToggle ^= USBP_EPX_CNT_DATA_TOGGLE;
            }
            
            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USBP_EP[USBP_EP6].apiEpState = USBP_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USBP_ENABLE_MIDI_STREAMING) && \
            !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
            if (USBP_midi_out_ep == USBP_EP6)
            {
                USBP_MIDI_OUT_Service();
            }
        #endif /* (USBP_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP6_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USBP_EP_6_ISR_EXIT_CALLBACK
        USBP_EP_6_ISR_ExitCallback();
    #endif /* (USBP_EP_6_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USBP_EP6_ISR_ACTIVE) */


#if (USBP_EP7_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USBP_EP_7_ISR
    ****************************************************************************//**
    *
    *  Endpoint 7 Interrupt Service Routine.
    *
    *
    *******************************************************************************/
    CY_ISR(USBP_EP_7_ISR)
    {
    #ifdef USBP_EP_7_ISR_ENTRY_CALLBACK
        USBP_EP_7_ISR_EntryCallback();
    #endif /* (USBP_EP_7_ISR_ENTRY_CALLBACK) */

        /* `#START EP7_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */
    
        USBP_ClearSieEpInterruptSource(USBP_SIE_INT_EP7_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USBP_EP[USBP_EP7].addr & USBP_DIR_IN))
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
        {           
            /* Read CR0 register to clear SIE lock. */
            (void) USBP_SIE_EP_BASE.sieEp[USBP_EP7].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USBP_GET_EP_TYPE(USBP_EP7) != USBP_EP_TYPE_ISOC)
            {
                USBP_EP[USBP_EP7].epToggle ^= USBP_EPX_CNT_DATA_TOGGLE;
            }
            
            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USBP_EP[USBP_EP7].apiEpState = USBP_EVENT_PENDING;
        }


    #if (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USBP_ENABLE_MIDI_STREAMING) && \
            !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
            if(USBP_midi_out_ep == USBP_EP7)
            {
                USBP_MIDI_OUT_Service();
            }
        #endif /* (USBP_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP7_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USBP_EP_7_ISR_EXIT_CALLBACK
        USBP_EP_7_ISR_ExitCallback();
    #endif /* (USBP_EP_7_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USBP_EP7_ISR_ACTIVE) */


#if (USBP_EP8_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USBP_EP_8_ISR
    ****************************************************************************//**
    *
    *  Endpoint 8 Interrupt Service Routine
    *
    *
    *******************************************************************************/
    CY_ISR(USBP_EP_8_ISR)
    {
    #ifdef USBP_EP_8_ISR_ENTRY_CALLBACK
        USBP_EP_8_ISR_EntryCallback();
    #endif /* (USBP_EP_8_ISR_ENTRY_CALLBACK) */

        /* `#START EP8_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */

        USBP_ClearSieEpInterruptSource(USBP_SIE_INT_EP8_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (USBP_EP[USBP_EP8].addr & USBP_DIR_IN))
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) USBP_SIE_EP_BASE.sieEp[USBP_EP8].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (USBP_GET_EP_TYPE(USBP_EP8) != USBP_EP_TYPE_ISOC)
            {
                USBP_EP[USBP_EP8].epToggle ^= USBP_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            USBP_EP[USBP_EP8].apiEpState = USBP_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(USBP_ENABLE_MIDI_STREAMING) && \
            !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
            if (USBP_midi_out_ep == USBP_EP8)
            {
                USBP_MIDI_OUT_Service();
            }
        #endif /* (USBP_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP8_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USBP_EP_8_ISR_EXIT_CALLBACK
        USBP_EP_8_ISR_ExitCallback();
    #endif /* (USBP_EP_8_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(USBP_ENABLE_MIDI_STREAMING) && \
        !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && USBP_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (USBP_EP8_ISR_ACTIVE) */


#if (USBP_SOF_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: USBP_SOF_ISR
    ****************************************************************************//**
    *
    *  Start of Frame Interrupt Service Routine.
    *
    *
    *******************************************************************************/
    CY_ISR(USBP_SOF_ISR)
    {
    #ifdef USBP_SOF_ISR_ENTRY_CALLBACK
        USBP_SOF_ISR_EntryCallback();
    #endif /* (USBP_SOF_ISR_ENTRY_CALLBACK) */

        /* `#START SOF_USER_CODE` Place your code here */

        /* `#END` */

        USBP_ClearSieInterruptSource(USBP_INTR_SIE_SOF_INTR);

    #ifdef USBP_SOF_ISR_EXIT_CALLBACK
        USBP_SOF_ISR_ExitCallback();
    #endif /* (USBP_SOF_ISR_EXIT_CALLBACK) */
    }
#endif /* (USBP_SOF_ISR_ACTIVE) */


#if (USBP_BUS_RESET_ISR_ACTIVE)
/*******************************************************************************
* Function Name: USBP_BUS_RESET_ISR
****************************************************************************//**
*
*  USB Bus Reset Interrupt Service Routine.  Calls _Start with the same
*  parameters as the last USER call to _Start
*
*
*******************************************************************************/
CY_ISR(USBP_BUS_RESET_ISR)
{
#ifdef USBP_BUS_RESET_ISR_ENTRY_CALLBACK
    USBP_BUS_RESET_ISR_EntryCallback();
#endif /* (USBP_BUS_RESET_ISR_ENTRY_CALLBACK) */

    /* `#START BUS_RESET_USER_CODE` Place your code here */

    /* `#END` */

    USBP_ClearSieInterruptSource(USBP_INTR_SIE_BUS_RESET_INTR);

    USBP_ReInitComponent();

#ifdef USBP_BUS_RESET_ISR_EXIT_CALLBACK
    USBP_BUS_RESET_ISR_ExitCallback();
#endif /* (USBP_BUS_RESET_ISR_EXIT_CALLBACK) */
}
#endif /* (USBP_BUS_RESET_ISR_ACTIVE) */


#if (USBP_LPM_ACTIVE)
/***************************************************************************
* Function Name: USBP_INTR_LPM_ISR
************************************************************************//**
*
*   Interrupt Service Routine for LPM of the interrupt sources.
*
*
***************************************************************************/
CY_ISR(USBP_LPM_ISR)
{
#ifdef USBP_LPM_ISR_ENTRY_CALLBACK
    USBP_LPM_ISR_EntryCallback();
#endif /* (USBP_LPM_ISR_ENTRY_CALLBACK) */

    /* `#START LPM_BEGIN_USER_CODE` Place your code here */

    /* `#END` */

    USBP_ClearSieInterruptSource(USBP_INTR_SIE_LPM_INTR);

    /* `#START LPM_END_USER_CODE` Place your code here */

    /* `#END` */

#ifdef USBP_LPM_ISR_EXIT_CALLBACK
    USBP_LPM_ISR_ExitCallback();
#endif /* (USBP_LPM_ISR_EXIT_CALLBACK) */
}
#endif /* (USBP_LPM_ACTIVE) */


#if (USBP_EP_MANAGEMENT_DMA && USBP_ARB_ISR_ACTIVE)
    /***************************************************************************
    * Function Name: USBP_ARB_ISR
    ************************************************************************//**
    *
    *  Arbiter Interrupt Service Routine.
    *
    *
    ***************************************************************************/
    CY_ISR(USBP_ARB_ISR)
    {
        uint8 arbIntrStatus;
        uint8 epStatus;
        uint8 ep = USBP_EP1;

    #ifdef USBP_ARB_ISR_ENTRY_CALLBACK
        USBP_ARB_ISR_EntryCallback();
    #endif /* (USBP_ARB_ISR_ENTRY_CALLBACK) */

        /* `#START ARB_BEGIN_USER_CODE` Place your code here */

        /* `#END` */

        /* Get pending ARB interrupt sources. */
        arbIntrStatus = USBP_ARB_INT_SR_REG;

        while (0u != arbIntrStatus)
        {
            /* Check which EP is interrupt source. */
            if (0u != (arbIntrStatus & 0x01u))
            {
                /* Get endpoint enable interrupt sources. */
                epStatus = (USBP_ARB_EP_BASE.arbEp[ep].epSr & USBP_ARB_EP_BASE.arbEp[ep].epIntEn);

                /* Handle IN endpoint buffer full event: happens only once when endpoint buffer is loaded. */
                if (0u != (epStatus & USBP_ARB_EPX_INT_IN_BUF_FULL))
                {
                    if (0u != (USBP_EP[ep].addr & USBP_DIR_IN))
                    {
                        /* Clear data ready status. */
                        USBP_ARB_EP_BASE.arbEp[ep].epCfg &= (uint8) ~USBP_ARB_EPX_CFG_IN_DATA_RDY;

                    #if (CY_PSOC3 || CY_PSOC5LP)
                        #if (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u))
                            /* Set up common area DMA with rest of data. */
                            if(USBP_inLength[ep] > USBP_DMA_BYTES_PER_BURST)
                            {
                                USBP_LoadNextInEP(ep, 0u);
                            }
                            else
                            {
                                USBP_inBufFull[ep] = 1u;
                            }
                        #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u)) */
                    #endif /* (CY_PSOC3 || CY_PSOC5LP) */

                        /* Arm IN endpoint. */
                        USBP_SIE_EP_BASE.sieEp[ep].epCr0 = USBP_EP[ep].epMode;

                    #if (defined(USBP_ENABLE_MIDI_STREAMING) && USBP_ISR_SERVICE_MIDI_IN)
                        if (ep == USBP_midi_in_ep)
                        {
                            /* Clear MIDI input pointer. */
                            USBP_midiInPointer = 0u;
                        }
                    #endif /* (USBP_ENABLE_MIDI_STREAMING) */
                    }
                }

            #if (USBP_EP_MANAGEMENT_DMA_MANUAL)
                /* Handle DMA completion event for OUT endpoints. */
                if (0u != (epStatus & USBP_ARB_EPX_SR_DMA_GNT))
                {
                    if (0u == (USBP_EP[ep].addr & USBP_DIR_IN))
                    {
                        /* Notify user that data has been copied from endpoint buffer. */
                        USBP_EP[ep].apiEpState = USBP_NO_EVENT_PENDING;

                        /* DMA done coping data: OUT endpoint has to be re-armed by user. */
                    }
                }
            #endif /* (USBP_EP_MANAGEMENT_DMA_MANUAL) */

            #if (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO)
                /* Handle DMA completion event for OUT endpoints. */
                if (0u != (epStatus & USBP_ARB_EPX_INT_DMA_TERMIN))
                {
                    uint32 channelNum = USBP_DmaChan[ep];

                    /* Restore burst counter for endpoint. */
                    USBP_DmaEpBurstCnt[ep] = USBP_DMA_GET_BURST_CNT(USBP_DmaEpBurstCntBackup[ep]);

                    /* Disable DMA channel to restore descriptor configuration. The on-going transfer is aborted. */
                    USBP_CyDmaChDisable(channelNum);

                    /* Generate DMA tr_out signal to notify USB IP that DMA is done. This signal is not generated
                    * when transfer was aborted (it occurs when host writes less bytes than buffer size).
                    */
                    USBP_CyDmaTriggerOut(USBP_DmaBurstEndOut[ep]);

                    /* Restore destination address for output endpoint. */
                    USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR0, (void*) ((uint32) USBP_DmaEpBufferAddrBackup[ep]));
                    USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR1, (void*) ((uint32) USBP_DmaEpBufferAddrBackup[ep] +
                                                                                                                   USBP_DMA_BYTES_PER_BURST));

                    /* Restore number of data elements to transfer which was adjusted for last burst. */
                    if (0u != (USBP_DmaEpLastBurstEl[ep] & USBP_DMA_DESCR_REVERT))
                    {
                        USBP_CyDmaSetNumDataElements(channelNum, (USBP_DmaEpLastBurstEl[ep] >> USBP_DMA_DESCR_SHIFT),
                                                                             USBP_DMA_GET_MAX_ELEM_PER_BURST(USBP_DmaEpLastBurstEl[ep]));
                    }

                    /* Validate descriptor 0 and 1 (also reset current state). Command to start with descriptor 0. */
                    USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR0);
                    if (USBP_DmaEpBurstCntBackup[ep] > 1u)
                    {
                        USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR1);
                    }
                    USBP_CyDmaSetDescriptor0Next(channelNum);

                    /* Enable DMA channel: configuration complete. */
                    USBP_CyDmaChEnable(channelNum);
                    
                    
                    /* Read CR0 register to clear SIE lock. */
                    (void) USBP_SIE_EP_BASE.sieEp[ep].epCr0;
                    
                    /* Toggle all endpoint types except ISOC. */
                    if (USBP_GET_EP_TYPE(ep) != USBP_EP_TYPE_ISOC)
                    {
                        USBP_EP[ep].epToggle ^= USBP_EPX_CNT_DATA_TOGGLE;
                    }
            
                    /* Notify user that data has been copied from endpoint buffer. */
                    USBP_EP[ep].apiEpState = USBP_EVENT_PENDING;
                    
                #if (defined(USBP_ENABLE_MIDI_STREAMING) && \
                    !defined(USBP_MAIN_SERVICE_MIDI_OUT) && USBP_ISR_SERVICE_MIDI_OUT)
                    if (USBP_midi_out_ep == ep)
                    {
                        USBP_MIDI_OUT_Service();
                    }
                #endif /* (USBP_ISR_SERVICE_MIDI_OUT) */
                }
            #endif /* (CY_PSOC4 && USBP_EP_MANAGEMENT_DMA_AUTO) */


                /* `#START ARB_USER_CODE` Place your code here for handle Buffer Underflow/Overflow */

                /* `#END` */

            #ifdef USBP_ARB_ISR_CALLBACK
                USBP_ARB_ISR_Callback(ep, epStatus);
            #endif /* (USBP_ARB_ISR_CALLBACK) */

                /* Clear serviced endpoint interrupt sources. */
                USBP_ARB_EP_BASE.arbEp[ep].epSr = epStatus;
            }

            ++ep;
            arbIntrStatus >>= 1u;
        }

        /* `#START ARB_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef USBP_ARB_ISR_EXIT_CALLBACK
        USBP_ARB_ISR_ExitCallback();
    #endif /* (USBP_ARB_ISR_EXIT_CALLBACK) */
    }

#endif /*  (USBP_ARB_ISR_ACTIVE && USBP_EP_MANAGEMENT_DMA) */


#if (USBP_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)

    /******************************************************************************
    * Function Name: USBP_EPxDmaDone
    ***************************************************************************//**
    *
    * \internal
    *  Endpoint  DMA Done Interrupt Service Routine basic function .
    *  
    *  \param dmaCh
    *  number of DMA channel
    *  
    *  \param ep
    *  number of USB end point
    *  
    *  \param dmaDone
    *  transfer completion flag
    *  
    *  \return
    *   updated transfer completion flag
    *
    ******************************************************************************/
    CY_INLINE static void USBP_EPxDmaDone(uint8 dmaCh, uint8 ep)
    {
        uint32 nextAddr;

        /* Manage data elements which remain to transfer. */
        if (0u != USBP_DmaEpBurstCnt[ep])
        {
            if(USBP_DmaEpBurstCnt[ep] <= 2u)
            {
                /* Adjust length of last burst. */
                USBP_CyDmaSetNumDataElements(dmaCh,
                                                    ((uint32) USBP_DmaEpLastBurstEl[ep] >> USBP_DMA_DESCR_SHIFT),
                                                    ((uint32) USBP_DmaEpLastBurstEl[ep] &  USBP_DMA_BURST_BYTES_MASK));
            }
            

            /* Advance source for input endpoint or destination for output endpoint. */
            if (0u != (USBP_EP[ep].addr & USBP_DIR_IN))
            {
                /* Change source for descriptor 0. */
                nextAddr = (uint32) USBP_CyDmaGetSrcAddress(dmaCh, USBP_DMA_DESCR0);
                nextAddr += (2u * USBP_DMA_BYTES_PER_BURST);
                USBP_CyDmaSetSrcAddress(dmaCh, USBP_DMA_DESCR0, (void *) nextAddr);

                /* Change source for descriptor 1. */
                nextAddr += USBP_DMA_BYTES_PER_BURST;
                USBP_CyDmaSetSrcAddress(dmaCh, USBP_DMA_DESCR1, (void *) nextAddr);
            }
            else
            {
                /* Change destination for descriptor 0. */
                nextAddr  = (uint32) USBP_CyDmaGetDstAddress(dmaCh, USBP_DMA_DESCR0);
                nextAddr += (2u * USBP_DMA_BYTES_PER_BURST);
                USBP_CyDmaSetDstAddress(dmaCh, USBP_DMA_DESCR0, (void *) nextAddr);

                /* Change destination for descriptor 1. */
                nextAddr += USBP_DMA_BYTES_PER_BURST;
                USBP_CyDmaSetDstAddress(dmaCh, USBP_DMA_DESCR1, (void *) nextAddr);
            }

            /* Enable DMA to execute transfer as it was disabled because there were no valid descriptor. */
            USBP_CyDmaValidateDescriptor(dmaCh, USBP_DMA_DESCR0);
            
            --USBP_DmaEpBurstCnt[ep];
            if (0u != USBP_DmaEpBurstCnt[ep])
            {
                USBP_CyDmaValidateDescriptor(dmaCh, USBP_DMA_DESCR1);
                --USBP_DmaEpBurstCnt[ep];
            }
            
            USBP_CyDmaChEnable (dmaCh);
            USBP_CyDmaTriggerIn(USBP_DmaReqOut[ep]);
        }
        else
        {
            /* No data to transfer. False DMA trig. Ignore.  */
        }

    }

    #if (USBP_DMA1_ACTIVE)
        /******************************************************************************
        * Function Name: USBP_EP1_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 1 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USBP_EP1_DMA_DONE_ISR(void)
        {

            USBP_EPxDmaDone((uint8)USBP_EP1_DMA_CH,
                                                  USBP_EP1);
                
        }
    #endif /* (USBP_DMA1_ACTIVE) */


    #if (USBP_DMA2_ACTIVE)
        /******************************************************************************
        * Function Name: USBP_EP2_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 2 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USBP_EP2_DMA_DONE_ISR(void)
        {

            USBP_EPxDmaDone((uint8)USBP_EP2_DMA_CH,
                                                  USBP_EP2);
        }
    #endif /* (USBP_DMA2_ACTIVE) */


    #if (USBP_DMA3_ACTIVE)
        /******************************************************************************
        * Function Name: USBP_EP3_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 3 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USBP_EP3_DMA_DONE_ISR(void)
        {

            USBP_EPxDmaDone((uint8)USBP_EP3_DMA_CH,
                                                  USBP_EP3);
        }
    #endif /* (USBP_DMA3_ACTIVE) */


    #if (USBP_DMA4_ACTIVE)
        /******************************************************************************
        * Function Name: USBP_EP4_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 4 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USBP_EP4_DMA_DONE_ISR(void)
        {

            USBP_EPxDmaDone((uint8)USBP_EP4_DMA_CH,
                                                  USBP_EP4);
        }
    #endif /* (USBP_DMA4_ACTIVE) */


    #if (USBP_DMA5_ACTIVE)
        /******************************************************************************
        * Function Name: USBP_EP5_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 5 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USBP_EP5_DMA_DONE_ISR(void)
        {

            USBP_EPxDmaDone((uint8)USBP_EP5_DMA_CH,
                                                  USBP_EP5);
        }
    #endif /* (USBP_DMA5_ACTIVE) */


    #if (USBP_DMA6_ACTIVE)
        /******************************************************************************
        * Function Name: USBP_EP6_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 6 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USBP_EP6_DMA_DONE_ISR(void)
        {

            USBP_EPxDmaDone((uint8)USBP_EP6_DMA_CH,
                                                  USBP_EP6);
        }
    #endif /* (USBP_DMA6_ACTIVE) */


    #if (USBP_DMA7_ACTIVE)
        /******************************************************************************
        * Function Name: USBP_EP7_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 7 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USBP_EP7_DMA_DONE_ISR(void)
        {

            USBP_EPxDmaDone((uint8)USBP_EP7_DMA_CH,
                                                  USBP_EP7);
        }
    #endif /* (USBP_DMA7_ACTIVE) */


    #if (USBP_DMA8_ACTIVE)
        /******************************************************************************
        * Function Name: USBP_EP8_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 8 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void USBP_EP8_DMA_DONE_ISR(void)
        {

            USBP_EPxDmaDone((uint8)USBP_EP8_DMA_CH,
                                                  USBP_EP8);
        }
    #endif /* (USBP_DMA8_ACTIVE) */


#else
    #if (USBP_EP_DMA_AUTO_OPT == 0u)
        /******************************************************************************
        * Function Name: USBP_EP_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        CY_ISR(USBP_EP_DMA_DONE_ISR)
        {
            uint8 int8Status;
            uint8 int17Status;
            uint8 ep_status;
            uint8 ep = USBP_EP1;

        #ifdef USBP_EP_DMA_DONE_ISR_ENTRY_CALLBACK
            USBP_EP_DMA_DONE_ISR_EntryCallback();
        #endif /* (USBP_EP_DMA_DONE_ISR_ENTRY_CALLBACK) */

            /* `#START EP_DMA_DONE_BEGIN_USER_CODE` Place your code here */

            /* `#END` */

            /* Read clear on read status register with EP source of interrupt. */
            int17Status = USBP_EP17_DMA_Done_SR_Read() & USBP_EP17_SR_MASK;
            int8Status  = USBP_EP8_DMA_Done_SR_Read()  & USBP_EP8_SR_MASK;

            while (int8Status != 0u)
            {
                while (int17Status != 0u)
                {
                    if ((int17Status & 1u) != 0u)  /* If EpX interrupt present. */
                    {
                        /* Read Endpoint Status Register. */
                        ep_status = USBP_ARB_EP_BASE.arbEp[ep].epSr;

                        if ((0u == (ep_status & USBP_ARB_EPX_SR_IN_BUF_FULL)) &&
                            (0u ==USBP_inBufFull[ep]))
                        {
                            /* `#START EP_DMA_DONE_USER_CODE` Place your code here */

                            /* `#END` */

                        #ifdef USBP_EP_DMA_DONE_ISR_CALLBACK
                            USBP_EP_DMA_DONE_ISR_Callback();
                        #endif /* (USBP_EP_DMA_DONE_ISR_CALLBACK) */

                            /* Transfer again 2 last bytes into pre-fetch endpoint area. */
                            USBP_ARB_EP_BASE.arbEp[ep].rwWaMsb = 0u;
                            USBP_ARB_EP_BASE.arbEp[ep].rwWa = (USBP_DMA_BYTES_PER_BURST * ep) - USBP_DMA_BYTES_REPEAT;
                            USBP_LoadNextInEP(ep, 1u);

                            /* Set Data ready status to generate DMA request. */
                            USBP_ARB_EP_BASE.arbEp[ep].epCfg |= USBP_ARB_EPX_CFG_IN_DATA_RDY;
                        }
                    }

                    ep++;
                    int17Status >>= 1u;
                }

                int8Status >>= 1u;

                if (int8Status != 0u)
                {
                    /* Prepare pointer for EP8. */
                    ep = USBP_EP8;
                    int17Status = int8Status & 0x01u;
                }
            }

            /* `#START EP_DMA_DONE_END_USER_CODE` Place your code here */

            /* `#END` */

    #ifdef USBP_EP_DMA_DONE_ISR_EXIT_CALLBACK
        USBP_EP_DMA_DONE_ISR_ExitCallback();
    #endif /* (USBP_EP_DMA_DONE_ISR_EXIT_CALLBACK) */
        }
    #endif /* (USBP_EP_DMA_AUTO_OPT == 0u) */
#endif /* (CY_PSOC4) */
#endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */


#if (CY_PSOC4)
    /***************************************************************************
    * Function Name: USBP_IntrHandler
    ************************************************************************//**
    *
    *   Interrupt handler for Hi/Mid/Low ISRs.
    *
    *  regCause - The cause register of interrupt. One of the three variants:
    *       USBP_INTR_CAUSE_LO_REG - Low interrupts.
    *       USBP_INTR_CAUSE_MED_REG - Med interrupts.
    *       USBP_INTR_CAUSE_HI_REG - - High interrupts.
    *
    *
    ***************************************************************************/
    CY_INLINE static void USBP_IntrHandler(uint32 intrCause)
    {
        /* Array of pointers to component interrupt handlers. */
        static const cyisraddress USBP_isrCallbacks[] =
        {

        };

        uint32 cbIdx = 0u;

        /* Check arbiter interrupt source first. */
        if (0u != (intrCause & USBP_INTR_CAUSE_ARB_INTR))
        {
            USBP_isrCallbacks[USBP_ARB_EP_INTR_NUM]();
        }

        /* Check all other interrupt sources (except arbiter and resume). */
        intrCause = (intrCause  & USBP_INTR_CAUSE_CTRL_INTR_MASK) |
                    ((intrCause & USBP_INTR_CAUSE_EP1_8_INTR_MASK) >>
                                  USBP_INTR_CAUSE_EP_INTR_SHIFT);

        /* Call interrupt handlers for active interrupt sources. */
        while (0u != intrCause)
        {
            if (0u != (intrCause & 0x1u))
            {
                 USBP_isrCallbacks[cbIdx]();
            }

            intrCause >>= 1u;
            ++cbIdx;
        }
    }


    /***************************************************************************
    * Function Name: USBP_INTR_HI_ISR
    ************************************************************************//**
    *
    *   Interrupt Service Routine for the high group of the interrupt sources.
    *
    *
    ***************************************************************************/
    CY_ISR(USBP_INTR_HI_ISR)
    {
        USBP_IntrHandler(USBP_INTR_CAUSE_HI_REG);
    }

    /***************************************************************************
    * Function Name: USBP_INTR_MED_ISR
    ************************************************************************//**
    *
    *   Interrupt Service Routine for the medium group of the interrupt sources.
    *
    *
    ***************************************************************************/
    CY_ISR(USBP_INTR_MED_ISR)
    {
       USBP_IntrHandler(USBP_INTR_CAUSE_MED_REG);
    }

    /***************************************************************************
    * Function Name: USBP_INTR_LO_ISR
    ************************************************************************//**
    *
    *   Interrupt Service Routine for the low group of the interrupt sources.
    *
    *
    ***************************************************************************/
    CY_ISR(USBP_INTR_LO_ISR)
    {
        USBP_IntrHandler(USBP_INTR_CAUSE_LO_REG);
    }
#endif /* (CY_PSOC4) */


/* [] END OF FILE */
