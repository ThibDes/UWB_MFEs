/***************************************************************************//**
* \file USBP.c
* \version 3.20
*
* \brief
*  This file contains the global USBFS API functions.
*
* Note:
*  Many of the functions use an endpoint number. SRAM arrays are sized with 9
*  elements, so they are indexed directly by epNumber.  The SIE and ARB
*  registers are indexed by variations of epNumber - 1.
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
#include "USBP_hid.h"
#include "USBP_Dp.h"


/***************************************
* Global data allocation
***************************************/

/** Indicates whether the USBFS has been initialized. The variable is
* initialized to 0 after device reset and set to 1 the first time USBFS_Start()
* is called. This allows the Component to restart without reinitialization after
* the first call to the USBFS_Start() routine.
* If re-initialization of the Component is required, the variable should be set
* to 0 before the USBFS_Start() routine is called. Alternatively, the USBFS can
* be reinitialized by calling both USBFS_Init() and USBFS_InitComponent()
* functions.
*/
uint8 USBP_initVar = 0u;

#if (USBP_EP_MANAGEMENT_DMA)
    #if (CY_PSOC4)
        static void USBP_InitEpDma(void);

        /* DMA chanels assigend for endpoints. */
        const uint8 USBP_DmaChan[USBP_MAX_EP] =
        {
            0u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u,
        };        
    #else
        /* DMA chanels assigend for endpoints. */
        uint8 USBP_DmaChan[USBP_MAX_EP];
        
        /* DMA TDs require for PSoC 3/5LP operation. */
        uint8 USBP_DmaTd[USBP_MAX_EP];
    #endif /* (CY_PSOC4) */
#endif /* (USBP_EP_MANAGEMENT_DMA) */

#if (USBP_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)
    /* Number of DMA bursts. */
    uint8  USBP_DmaEpBurstCnt   [USBP_MAX_EP];
    
    /* Number of bytes to transfer in last DMA burst. */
    uint8  USBP_DmaEpLastBurstEl[USBP_MAX_EP];

    /* Storage for arrays above. */
    uint8  USBP_DmaEpBurstCntBackup  [USBP_MAX_EP];
    uint32 USBP_DmaEpBufferAddrBackup[USBP_MAX_EP];
     
    /* DMA trigger mux output for usb.dma_req[0-7]. */
    const uint8 USBP_DmaReqOut[USBP_MAX_EP] =
    {
        0u,
        USBP_ep1_dma__TR_OUTPUT,
        USBP_ep2_dma__TR_OUTPUT,
        0u,
        0u,
        0u,
        0u,
        0u,
        0u,
    };

    /* DMA trigger mux output for usb.dma_burstend[0-7]. */
    const uint8 USBP_DmaBurstEndOut[USBP_MAX_EP] =
    {
        0u,
        USBP_BURSTEND_0_TR_OUTPUT,
        USBP_BURSTEND_1_TR_OUTPUT,
        USBP_BURSTEND_2_TR_OUTPUT,
        USBP_BURSTEND_3_TR_OUTPUT,
        USBP_BURSTEND_4_TR_OUTPUT,
        USBP_BURSTEND_5_TR_OUTPUT,
        USBP_BURSTEND_6_TR_OUTPUT,
        USBP_BURSTEND_7_TR_OUTPUT
    };
    
#else
    #if (USBP_EP_DMA_AUTO_OPT == 0u)
        static uint8 clearInDataRdyStatus = USBP_ARB_EPX_CFG_DEFAULT;
        uint8 USBP_DmaNextTd[USBP_MAX_EP];
        const uint8 USBP_epX_TD_TERMOUT_EN[USBP_MAX_EP] =
        {
            0u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u,
        };

        volatile uint16 USBP_inLength[USBP_MAX_EP];
        const uint8 *USBP_inDataPointer[USBP_MAX_EP];
        volatile uint8 USBP_inBufFull[USBP_MAX_EP];
    #endif /* (USBP_EP_DMA_AUTO_OPT == 0u) */
#endif /* (CY_PSOC4) */
#endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */


/*******************************************************************************
* Function Name: USBP_Start
****************************************************************************//**
*
*   This function performs all required initialization for the USBFS component.
*   After this function call, the USB device initiates communication with the
*   host by pull-up D+ line. This is the preferred method to begin component
*   operation.
*
*   Note that global interrupts have to be enabled because interrupts are
*   required for USBFS component operation.
*
*   PSoC 4200L devices: when USBFS component configured to DMA with Automatic
*   Buffer Management, the DMA interrupt priority is changed to the highest
*   (priority 0) inside this function.
*
*   PSoC 3/PSoC 5LP devices: when USBFS component configured to DMA with
*   Automatic Buffer Management, the Arbiter interrupt priority is changed to
*   the highest (priority 0) inside this function.
*
*  \param device
*          Contains the device number of the desired device descriptor.
*          The device number can be found in the Device Descriptor Tab of
*          "Configure" dialog, under the settings of desired Device Descriptor,
*          in the "Device Number" field.
*  \param mode:
*   The operating voltage. This determines whether the voltage regulator
*   is enabled for 5V operation or if pass through mode is used for 3.3V
*   operation. Symbolic names and their associated values are given in the
*   following list.
*
*       *USBP_3V_OPERATION* - Disable voltage regulator and pass-
*                                      through Vcc for pull-up
*
*       *USBP_5V_OPERATION* - Enable voltage regulator and use
*                                      regulator for pull-up
*
*       *USBP_DWR_POWER_OPERATION* - Enable or disable the voltage
*                                      regulator depending on the power supply
*                                      voltage configuration in the DWR tab.
*                                      For PSoC 3/5LP devices, the VDDD supply
*                                      voltage is considered and for PSoC 4A-L,
*                                      the VBUS supply voltage is considered.*
* \globalvars
*  \ref USBP_initVar
*
* \sideeffect
*   This function will reset all communication states to default.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_Start(uint8 device, uint8 mode) 
{
    if (0u == USBP_initVar)
    {
        USBP_Init();
        USBP_initVar = 1u;
    }

    USBP_InitComponent(device, mode);
}


/*******************************************************************************
* Function Name: USBP_Init
****************************************************************************//**
*
* This function initializes or restores the component according to the
* customizer Configure dialog settings. It is not necessary to call
* USBP_Init() because the USBP_Start() routine calls
* this function and is the preferred method to begin component operation.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_Init(void) 
{
#if (CY_PSOC4)
    /* Enable clock to USB IP. */
    USBP_USB_CLK_EN_REG = USBP_USB_CLK_CSR_CLK_EN;

    /* The internal regulator (CR1.REG_ENABLE) is enabled in
    * USBP_InitComponent() if it is required.
    */

    /* Enable USBIO control on drive mode of D+ and D- pins. */
    USBP_USBIO_CR1_REG &= ~ (uint32) USBP_USBIO_CR1_IOMODE;

    /* Set number of LF CLK to detect UBS bus reset. */
    USBP_BUS_RST_CNT_REG = USBP_DEFUALT_BUS_RST_CNT;

    /* Select VBUS detection source and clear PHY isolate. The application level
    * must ensure that VBUS is valid. There is no need to wait 2us before VBUS is valid.
    */
    USBP_POWER_CTRL_REG = USBP_DEFAULT_POWER_CTRL_VBUS;

    /* Enable PHY detector and single-ended and differential receivers. */
    USBP_POWER_CTRL_REG |= USBP_DEFAULT_POWER_CTRL_PHY;

    /* Suspend clear sequence. */
    USBP_POWER_CTRL_REG &= (uint32) ~USBP_POWER_CTRL_SUSPEND;
    CyDelayUs(USBP_WAIT_SUSPEND_DEL_DISABLE);
    USBP_POWER_CTRL_REG &= (uint32) ~USBP_POWER_CTRL_SUSPEND_DEL;

    /* Sets IMO lock options and clear all other bits. */
    USBP_CR1_REG = USBP_DEFUALT_CR1;

    /* Configure level (hi, lo, med) for each interrupt source. */
    USBP_INTR_LVL_SEL_REG = USBP_DEFAULT_INTR_LVL_SEL;

    /* Configure interrupt sources from: SOF, Bus Reset and EP0. */
    USBP_INTR_SIE_MASK_REG = USBP_DEFAULT_INTR_SIE_MASK;

#else
    uint8 enableInterrupts = CyEnterCriticalSection();

#if (USBP_EP_MANAGEMENT_DMA)
    uint16 i;
#endif /* (USBP_EP_MANAGEMENT_DMA) */

    /* Enable USB block. */
    USBP_PM_ACT_CFG_REG |= USBP_PM_ACT_EN_FSUSB;
    /* Enable USB block for Standby Power Mode. */
    USBP_PM_STBY_CFG_REG |= USBP_PM_STBY_EN_FSUSB;

    /* Enable core clock. */
    USBP_USB_CLK_EN_REG = USBP_USB_CLK_ENABLE;

    USBP_CR1_REG = USBP_CR1_ENABLE_LOCK;

    /* ENABLING USBIO PADS IN USB MODE FROM I/O MODE. */
    /* Ensure USB transmit enable is low (USB_USBIO_CR0.ten). - Manual Transmission - Disabled. */
    USBP_USBIO_CR0_REG &= (uint8) ~USBP_USBIO_CR0_TEN;
    CyDelayUs(USBP_WAIT_REG_STABILITY_50NS);  /* ~50ns delay. */
    /* Disable USBIO by asserting PM.USB_CR0.fsusbio_pd_n(Inverted.
    *  high. These bits will be set low by the power manager out-of-reset.
    *  Also confirm USBIO pull-up is disabled.
    */
    USBP_PM_USB_CR0_REG &= (uint8) ~(USBP_PM_USB_CR0_PD_N |
                                                 USBP_PM_USB_CR0_PD_PULLUP_N);

    /* Select IOMODE to USB mode. */
    USBP_USBIO_CR1_REG &= (uint8) ~USBP_USBIO_CR1_IOMODE;

    /* Enable USBIO reference by setting PM.USB_CR0.fsusbio_ref_en. */
    USBP_PM_USB_CR0_REG |= USBP_PM_USB_CR0_REF_EN;
    /* Reference is available for 1us after regulator is enabled. */
    CyDelayUs(USBP_WAIT_REG_STABILITY_1US);
    /* OR 40us after power is restored. */
    CyDelayUs(USBP_WAIT_VREF_RESTORE);
    /* Ensure single-ended disable bits are low (PRT15.INP_DIS[7:6])(input receiver enabled). */
    USBP_DM_INP_DIS_REG &= (uint8) ~USBP_DM_MASK;
    USBP_DP_INP_DIS_REG &= (uint8) ~USBP_DP_MASK;

    /* Enable USBIO. */
    USBP_PM_USB_CR0_REG |= USBP_PM_USB_CR0_PD_N;
    CyDelayUs(USBP_WAIT_PD_PULLUP_N_ENABLE);
    /* Set USBIO pull-up enable. */
    USBP_PM_USB_CR0_REG |= USBP_PM_USB_CR0_PD_PULLUP_N;

    /* Reset Arbiter Write Address register for endpoint 1. */
    CY_SET_REG8(USBP_ARB_RW1_WA_PTR,     0u);
    CY_SET_REG8(USBP_ARB_RW1_WA_MSB_PTR, 0u);

#if (USBP_EP_MANAGEMENT_DMA)
    /* Initialize transfer descriptor. This will be used to detect DMA state - initialized or not. */
    for (i = 0u; i < USBP_MAX_EP; ++i)
    {
        USBP_DmaTd[i] = DMA_INVALID_TD;

    #if (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u))
        USBP_DmaNextTd[i] = DMA_INVALID_TD;
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u)) */
    }
#endif /* (USBP_EP_MANAGEMENT_DMA) */

    CyExitCriticalSection(enableInterrupts);
#endif /* (CY_PSOC4) */

    /* Configure interrupts from USB block. */
#if (CY_PSOC4)
    /* Configure hi_int: set handler and priority. */
    CyIntSetPriority     (USBP_INTR_HI_VECT_NUM,  USBP_INTR_HI_PRIORITY);
    (void) CyIntSetVector(USBP_INTR_HI_VECT_NUM, &USBP_INTR_HI_ISR);

    /* Configure lo_int: set handler and priority. */
    CyIntSetPriority     (USBP_INTR_LO_VECT_NUM,  USBP_INTR_LO_PRIORITY);
    (void) CyIntSetVector(USBP_INTR_LO_VECT_NUM, &USBP_INTR_LO_ISR);

    /* Configure med_int: set handler and priority (routed through DSI). */
    CyIntSetPriority     (USBP_INTR_MED_VECT_NUM,  USBP_INTR_MED_PRIORITY);
    (void) CyIntSetVector(USBP_INTR_MED_VECT_NUM, &USBP_INTR_MED_ISR);

#else
    /* Set bus reset interrupt. */
    CyIntSetPriority(USBP_BUS_RESET_VECT_NUM, USBP_BUS_RESET_PRIOR);
    (void) CyIntSetVector(USBP_BUS_RESET_VECT_NUM,   &USBP_BUS_RESET_ISR);

    /* Set Control Endpoint Interrupt. */
    CyIntSetPriority(USBP_EP_0_VECT_NUM, USBP_EP_0_PRIOR);
    (void) CyIntSetVector(USBP_EP_0_VECT_NUM,   &USBP_EP_0_ISR);

    /* Set SOF interrupt. */
    #if (USBP_SOF_ISR_ACTIVE)
        CyIntSetPriority     (USBP_SOF_VECT_NUM,  USBP_SOF_PRIOR);
        (void) CyIntSetVector(USBP_SOF_VECT_NUM, &USBP_SOF_ISR);
    #endif /* (USBP_SOF_ISR_ACTIVE) */

    /* Set Data Endpoint 1 Interrupt. */
    #if (USBP_EP1_ISR_ACTIVE)
        CyIntSetPriority     (USBP_EP_1_VECT_NUM,  USBP_EP_1_PRIOR);
        (void) CyIntSetVector(USBP_EP_1_VECT_NUM,  &USBP_EP_1_ISR);
    #endif /* (USBP_EP1_ISR_ACTIVE) */

    /* Set Data Endpoint 2 Interrupt. */
    #if (USBP_EP2_ISR_ACTIVE)
        CyIntSetPriority     (USBP_EP_2_VECT_NUM,  USBP_EP_2_PRIOR);
        (void) CyIntSetVector(USBP_EP_2_VECT_NUM, &USBP_EP_2_ISR);
    #endif /* (USBP_EP2_ISR_ACTIVE) */

    /* Set Data Endpoint 3 Interrupt. */
    #if (USBP_EP3_ISR_ACTIVE)
        CyIntSetPriority     (USBP_EP_3_VECT_NUM,  USBP_EP_3_PRIOR);
        (void) CyIntSetVector(USBP_EP_3_VECT_NUM, &USBP_EP_3_ISR);
    #endif /* (USBP_EP3_ISR_ACTIVE) */

    /* Set Data Endpoint 4 Interrupt. */
    #if (USBP_EP4_ISR_ACTIVE)
        CyIntSetPriority     (USBP_EP_4_VECT_NUM,  USBP_EP_4_PRIOR);
        (void) CyIntSetVector(USBP_EP_4_VECT_NUM, &USBP_EP_4_ISR);
    #endif /* (USBP_EP4_ISR_ACTIVE) */

    /* Set Data Endpoint 5 Interrupt. */
    #if (USBP_EP5_ISR_ACTIVE)
        CyIntSetPriority     (USBP_EP_5_VECT_NUM,  USBP_EP_5_PRIOR);
        (void) CyIntSetVector(USBP_EP_5_VECT_NUM, &USBP_EP_5_ISR);
    #endif /* (USBP_EP5_ISR_ACTIVE) */

    /* Set Data Endpoint 6 Interrupt. */
    #if (USBP_EP6_ISR_ACTIVE)
        CyIntSetPriority     (USBP_EP_6_VECT_NUM,  USBP_EP_6_PRIOR);
        (void) CyIntSetVector(USBP_EP_6_VECT_NUM, &USBP_EP_6_ISR);
    #endif /* (USBP_EP6_ISR_ACTIVE) */

     /* Set Data Endpoint 7 Interrupt. */
    #if (USBP_EP7_ISR_ACTIVE)
        CyIntSetPriority     (USBP_EP_7_VECT_NUM,  USBP_EP_7_PRIOR);
        (void) CyIntSetVector(USBP_EP_7_VECT_NUM, &USBP_EP_7_ISR);
    #endif /* (USBP_EP7_ISR_ACTIVE) */

    /* Set Data Endpoint 8 Interrupt. */
    #if (USBP_EP8_ISR_ACTIVE)
        CyIntSetPriority     (USBP_EP_8_VECT_NUM,  USBP_EP_8_PRIOR);
        (void) CyIntSetVector(USBP_EP_8_VECT_NUM, &USBP_EP_8_ISR);
    #endif /* (USBP_EP8_ISR_ACTIVE) */

    /* Set ARB Interrupt. */
    #if (USBP_EP_MANAGEMENT_DMA && USBP_ARB_ISR_ACTIVE)
        CyIntSetPriority     (USBP_ARB_VECT_NUM,  USBP_ARB_PRIOR);
        (void) CyIntSetVector(USBP_ARB_VECT_NUM, &USBP_ARB_ISR);
    #endif /* (USBP_EP_MANAGEMENT_DMA && USBP_ARB_ISR_ACTIVE) */
#endif /* (CY_PSOC4) */

    /* Common: Configure GPIO interrupt for wakeup. */
#if (USBP_DP_ISR_ACTIVE)
    CyIntSetPriority     (USBP_DP_INTC_VECT_NUM,  USBP_DP_INTC_PRIORITY);
    (void) CyIntSetVector(USBP_DP_INTC_VECT_NUM, &USBP_DP_ISR);
#endif /* (USBP_DP_ISR_ACTIVE) */

#if (USBP_EP_MANAGEMENT_DMA && CY_PSOC4)
    /* Initialize DMA channels. */
    USBP_InitEpDma();
#endif /* (USBP_EP_MANAGEMENT_DMA && CY_PSOC4) */
}


/*******************************************************************************
* Function Name: USBP_InitComponent
****************************************************************************//**
*
*   This function initializes the component’s global variables and initiates
*   communication with the host by pull-up D+ line.
*
* \param device:
*   Contains the device number of the desired device descriptor. The device
*   number can be found in the Device Descriptor Tab of "Configure" dialog,
*   under the settings of desired Device Descriptor, in the *Device Number*
*   field.
*  \param mode:
*   The operating voltage. This determines whether the voltage regulator
*   is enabled for 5V operation or if pass through mode is used for 3.3V
*   operation. Symbolic names and their associated values are given in the
*   following list.
*
*       *USBP_3V_OPERATION* - Disable voltage regulator and pass-
*                                      through Vcc for pull-up
*
*       *USBP_5V_OPERATION* - Enable voltage regulator and use
*                                      regulator for pull-up
*
*       *USBP_DWR_POWER_OPERATION* - Enable or disable the voltage
*                                      regulator depending on the power supply
*                                      voltage configuration in the DWR tab.
*                                      For PSoC 3/5LP devices, the VDDD supply
*                                      voltage is considered and for PSoC 4A-L,
*                                      the VBUS supply voltage is considered.
*
* \globalvars
*   \ref USBP_device
*   \ref USBP_transferState
*   \ref USBP_configuration
*   \ref USBP_deviceStatus
*
*   \ref USBP_deviceAddress - Contains the current device address. This
*       variable is initialized to zero in this API. The Host starts to communicate
*      to the device with address 0 and then sets it to a whatever value using a
*      SET_ADDRESS request.
*
*   \ref USBP_lastPacketSize - Initialized to 0;
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_InitComponent(uint8 device, uint8 mode) 
{
    /* Initialize _hidProtocol variable to comply with
    *  HID 7.2.6 Set_Protocol Request:
    *  "When initialized, all devices default to report protocol."
    */
#if defined(USBP_ENABLE_HID_CLASS)
    uint8 i;

    for (i = 0u; i < USBP_MAX_INTERFACES_NUMBER; i++)
    {
        USBP_hidProtocol[i] = USBP_PROTOCOL_REPORT;
    }
#endif /* USBP_ENABLE_HID_CLASS */

    /* Store device number to access descriptor. */
    USBP_device = device;

    /* Reset component internal variables. */
    USBP_transferState   = USBP_TRANS_STATE_IDLE;
    USBP_configurationChanged = 0u;
    USBP_configuration   = 0u;
    USBP_interfaceNumber = 0u;
    USBP_deviceAddress   = 0u;
    USBP_deviceStatus    = 0u;
    USBP_lastPacketSize  = 0u;

    /* Enable component interrupts. */
#if (CY_PSOC4)
    CyIntEnable(USBP_INTR_HI_VECT_NUM);
    CyIntEnable(USBP_INTR_MED_VECT_NUM);
    CyIntEnable(USBP_INTR_LO_VECT_NUM);
#else
    CyIntEnable(USBP_BUS_RESET_VECT_NUM);
    CyIntEnable(USBP_EP_0_VECT_NUM);

    #if (USBP_SOF_ISR_ACTIVE)
        CyIntEnable(USBP_SOF_VECT_NUM);
    #endif /* (USBP_SOF_ISR_ACTIVE) */

    #if (USBP_EP1_ISR_ACTIVE)
        CyIntEnable(USBP_EP_1_VECT_NUM);
    #endif /* (USBP_EP1_ISR_ACTIVE) */

    #if (USBP_EP2_ISR_ACTIVE)
        CyIntEnable(USBP_EP_2_VECT_NUM);
    #endif /* (USBP_EP5_ISR_ACTIVE) */

    #if (USBP_EP3_ISR_ACTIVE)
        CyIntEnable(USBP_EP_3_VECT_NUM);
    #endif /* (USBP_EP5_ISR_ACTIVE) */

    #if (USBP_EP4_ISR_ACTIVE)
        CyIntEnable(USBP_EP_4_VECT_NUM);
    #endif /* (USBP_EP5_ISR_ACTIVE) */

    #if (USBP_EP5_ISR_ACTIVE)
        CyIntEnable(USBP_EP_5_VECT_NUM);
    #endif /* (USBP_EP5_ISR_ACTIVE) */

    #if (USBP_EP6_ISR_ACTIVE)
        CyIntEnable(USBP_EP_6_VECT_NUM);
    #endif /* USBP_EP6_ISR_REMOVE */

    #if (USBP_EP7_ISR_ACTIVE)
        CyIntEnable(USBP_EP_7_VECT_NUM);
    #endif /* (USBP_EP7_ISR_ACTIVE) */

    #if (USBP_EP8_ISR_ACTIVE)
        CyIntEnable(USBP_EP_8_VECT_NUM);
    #endif /* (USBP_EP8_ISR_ACTIVE) */
#endif /* (CY_PSOC4) */

#if (USBP_EP_MANAGEMENT_DMA && USBP_ARB_ISR_ACTIVE)
    /* Enable ARB EP interrupt sources. */
    USBP_ARB_INT_EN_REG = USBP_DEFAULT_ARB_INT_EN;

    #if (CY_PSOC3 || CY_PSOC5)
        CyIntEnable(USBP_ARB_VECT_NUM);
    #endif /* (CY_PSOC3 || CY_PSOC5) */
#endif   /* (USBP_EP_MANAGEMENT_DMA && USBP_ARB_ISR_ACTIVE) */

/* Arbiter configuration for DMA transfers. */
#if (USBP_EP_MANAGEMENT_DMA)
    /* Configure Arbiter for Manual or Auto DMA operation and clear configuration completion. */
    USBP_ARB_CFG_REG = USBP_DEFAULT_ARB_CFG;

    #if (CY_PSOC4)
        /* Enable DMA operation. */
        CyDmaEnable();

        #if (USBP_EP_MANAGEMENT_DMA_AUTO)
            /* Change DMA priority to be highest. */
             CyIntSetPriority(CYDMA_INTR_NUMBER, USBP_DMA_AUTO_INTR_PRIO);
        #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
    #endif /* (CY_PSOC4) */

    #if (USBP_EP_MANAGEMENT_DMA_AUTO)
        #if (CY_PSOC4)
            /* Enable DMA interrupt to handle DMA management. */
            CyIntEnable(CYDMA_INTR_NUMBER);
        #else
            #if (USBP_EP_DMA_AUTO_OPT == 0u)
                /* Initialize interrupts which handle verification of successful DMA transaction. */
                USBP_EP_DMA_Done_isr_StartEx(&USBP_EP_DMA_DONE_ISR);
                USBP_EP17_DMA_Done_SR_InterruptEnable();
                USBP_EP8_DMA_Done_SR_InterruptEnable();
            #endif /* (USBP_EP_DMA_AUTO_OPT == 0u) */
        #endif /* (CY_PSOC4) */
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
#endif /* (USBP_EP_MANAGEMENT_DMA) */

    /* Enable USB regulator depends on operation voltage. IMO Locking is enabled in Init(). */
    switch(mode)
    {
    case USBP_3V_OPERATION:
        /* Disable regulator for 3V operation. */
        USBP_CR1_REG &= (uint8) ~USBP_CR1_REG_ENABLE;
        break;

    case USBP_5V_OPERATION:
        /* Enable regulator for 5V operation. */
        USBP_CR1_REG |= (uint8)  USBP_CR1_REG_ENABLE;
        break;

    default: /* Check DWR settings of USB power supply. */
    #if (USBP_VDDD_MV < USBP_3500MV)
        /* Disable regulator for 3V operation. */
        USBP_CR1_REG &= (uint8) ~USBP_CR1_REG_ENABLE;
    #else
        /* Enable regulator for 5V operation. */
        USBP_CR1_REG |= (uint8)  USBP_CR1_REG_ENABLE;
    #endif /* (USBP_VDDD_MV < USBP_3500MV) */
        break;
    }

#if (CY_PSOC4)
    /* Clear bus activity. */
    USBP_CR1_REG &= (uint32) ~USBP_CR1_BUS_ACTIVITY;

    /* Clear EP0 count register. */
    USBP_EP0_CNT_REG = USBP_CLEAR_REG;

    /* Set EP0.CR: ACK Setup, NAK IN/OUT. */
    USBP_EP0_CR_REG = USBP_MODE_NAK_IN_OUT;

    #if (USBP_LPM_ACTIVE)
        if (NULL != USBP_GetBOSPtr())
        {
            /* Enable LPM and acknowledge LPM packets for active device.
            * Reset NYET_EN and SUB_RESP bits in the LPM_CTRL register.
            */
            USBP_LPM_CTRL_REG = (USBP_LPM_CTRL_LPM_EN | \
                                             USBP_LPM_CTRL_LPM_ACK_RESP);
        }
        else
        {
            /* Disable LPM for active device. */
            USBP_LPM_CTRL_REG &= (uint32) ~USBP_LPM_CTRL_LPM_EN;
        }
    #endif /* (USBP_LPM_ACTIVE) */

    /* Enable device to responds to USB traffic with address 0. */
    USBP_CR0_REG = USBP_DEFUALT_CR0;

#else
    /* Set EP0.CR: ACK Setup, STALL IN/OUT. */
    USBP_EP0_CR_REG = USBP_MODE_STALL_IN_OUT;

    /* Enable device to respond to USB traffic with address 0. */
    USBP_CR0_REG = USBP_DEFUALT_CR0;
    CyDelayCycles(USBP_WAIT_CR0_REG_STABILITY);
#endif /* (CY_PSOC4) */

    /* Enable D+ pull-up and keep USB control on IO. */
    USBP_USBIO_CR1_REG = USBP_USBIO_CR1_USBPUEN;
}


/*******************************************************************************
* Function Name: USBP_ReInitComponent
****************************************************************************//**
*
*  This function reinitialize the component configuration and is
*  intend to be called from the Reset interrupt.
*
* \globalvars
*   USBP_device - Contains the device number of the desired Device
*        Descriptor. The device number can be found in the Device Descriptor tab
*       of the Configure dialog, under the settings of the desired Device Descriptor,
*       in the Device Number field.
*   USBP_transferState - This variable is used by the communication
*       functions to handle the current transfer state. Initialized to
*       TRANS_STATE_IDLE in this API.
*   USBP_configuration - Contains the current configuration number
*       set by the Host using a SET_CONFIGURATION request.
*       Initialized to zero in this API.
*   USBP_deviceAddress - Contains the current device address. This
*       variable is initialized to zero in this API. The Host starts to communicate
*      to the device with address 0 and then sets it to a whatever value using
*      a SET_ADDRESS request.
*   USBP_deviceStatus - Initialized to 0.
*       This is a two-bit variable which contains the power status in the first bit
*       (DEVICE_STATUS_BUS_POWERED or DEVICE_STATUS_SELF_POWERED) and the remote
*       wakeup status (DEVICE_STATUS_REMOTE_WAKEUP) in the second bit.
*   USBP_lastPacketSize - Initialized to 0;
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_ReInitComponent(void) 
{
    /* Initialize _hidProtocol variable to comply with HID 7.2.6 Set_Protocol
    *  Request: "When initialized, all devices default to report protocol."
    */
#if defined(USBP_ENABLE_HID_CLASS)
    uint8 i;

    for (i = 0u; i < USBP_MAX_INTERFACES_NUMBER; i++)
    {
        USBP_hidProtocol[i] = USBP_PROTOCOL_REPORT;
    }
#endif /* USBP_ENABLE_HID_CLASS */

    /* Reset component internal variables. */
    USBP_transferState   = USBP_TRANS_STATE_IDLE;
    USBP_configurationChanged = 0u;
    USBP_configuration   = 0u;
    USBP_interfaceNumber = 0u;
    USBP_deviceAddress   = 0u;
    USBP_deviceStatus    = 0u;
    USBP_lastPacketSize  = 0u;

#if (CY_PSOC4)
    /* Set EP0.CR: ACK Setup, NAK IN/OUT. */
    USBP_EP0_CR_REG = USBP_MODE_NAK_IN_OUT;
#else
    /* Set EP0.CR: ACK Setup, STALL IN/OUT. */
    USBP_EP0_CR_REG = USBP_MODE_STALL_IN_OUT;
#endif /* (CY_PSOC4) */

    /* Enable device to respond to USB traffic with address 0. */
    USBP_CR0_REG = USBP_DEFUALT_CR0;
}


/*******************************************************************************
* Function Name: USBP_Stop
****************************************************************************//**
*
*  This function shuts down the USB function including to release
*  the D+ pull-up and disabling the SIE.
*
* \globalvars
*   \ref USBP_configuration
*
*   USBP_deviceAddress - Contains the current device address. This
*       variable is initialized to zero in this API. The Host starts to communicate
*      to the device with address 0 and then sets it to a whatever value using
*      a SET_ADDRESS request.
*
*   \ref USBP_deviceStatus
*
*   \ref USBP_configurationChanged
*
*   USBP_intiVar -  This variable is set to zero
*
*******************************************************************************/
void USBP_Stop(void) 
{
    uint8 enableInterrupts;

#if (USBP_EP_MANAGEMENT_DMA)
    /* Stop all DMA channels. */
    USBP_Stop_DMA(USBP_MAX_EP);
#endif /* (USBP_EP_MANAGEMENT_DMA) */

    enableInterrupts = CyEnterCriticalSection();

    /* Disable USB IP to respond to USB traffic. */
    USBP_CR0_REG &= (uint8) ~USBP_CR0_ENABLE;

    /* Disable D+ pull-up. */
    USBP_USBIO_CR1_REG &= (uint8) ~ USBP_USBIO_CR1_USBPUEN;

#if (CY_PSOC4)
    /* Disable USBFS block. */
    USBP_POWER_CTRL_REG &= (uint32) ~USBP_POWER_CTRL_ENABLE;
#else
    /* Clear power active and standby mode templates. */
    USBP_PM_ACT_CFG_REG  &= (uint8) ~USBP_PM_ACT_EN_FSUSB;
    USBP_PM_STBY_CFG_REG &= (uint8) ~USBP_PM_STBY_EN_FSUSB;

    /* Ensure single-ended disable bits are high (PRT15.INP_DIS[7:6])
     * (input receiver disabled). */
    USBP_DM_INP_DIS_REG |= (uint8) USBP_DM_MASK;
    USBP_DP_INP_DIS_REG |= (uint8) USBP_DP_MASK;

#endif /* (CY_PSOC4) */

    CyExitCriticalSection(enableInterrupts);

    /* Disable component interrupts. */
#if (CY_PSOC4)
    CyIntDisable(USBP_INTR_HI_VECT_NUM);
    CyIntDisable(USBP_INTR_LO_VECT_NUM);
    CyIntDisable(USBP_INTR_MED_VECT_NUM);
#else

    CyIntDisable(USBP_BUS_RESET_VECT_NUM);
    CyIntDisable(USBP_EP_0_VECT_NUM);

    #if (USBP_SOF_ISR_ACTIVE)
        CyIntDisable(USBP_SOF_VECT_NUM);
    #endif /* (USBP_SOF_ISR_ACTIVE) */

    #if (USBP_EP1_ISR_ACTIVE)
        CyIntDisable(USBP_EP_1_VECT_NUM);
    #endif /* (USBP_EP1_ISR_ACTIVE) */

    #if (USBP_EP2_ISR_ACTIVE)
        CyIntDisable(USBP_EP_2_VECT_NUM);
    #endif /* (USBP_EP2_ISR_ACTIVE) */

    #if (USBP_EP3_ISR_ACTIVE)
        CyIntDisable(USBP_EP_3_VECT_NUM);
    #endif /* (USBP_EP3_ISR_ACTIVE) */

    #if (USBP_EP4_ISR_ACTIVE)
        CyIntDisable(USBP_EP_4_VECT_NUM);
    #endif /* (USBP_EP4_ISR_ACTIVE) */

    #if (USBP_EP5_ISR_ACTIVE)
        CyIntDisable(USBP_EP_5_VECT_NUM);
    #endif /* (USBP_EP5_ISR_ACTIVE) */

    #if (USBP_EP6_ISR_ACTIVE)
        CyIntDisable(USBP_EP_6_VECT_NUM);
    #endif /* USBP_EP6_ISR_REMOVE */

    #if (USBP_EP7_ISR_ACTIVE)
        CyIntDisable(USBP_EP_7_VECT_NUM);
    #endif /* (USBP_EP7_ISR_ACTIVE) */

    #if (USBP_EP8_ISR_ACTIVE)
        CyIntDisable(USBP_EP_8_VECT_NUM);
    #endif /* (USBP_EP8_ISR_ACTIVE) */

    #if (USBP_DP_ISR_ACTIVE)
        /* Clear active mode Dp interrupt source history. */
        (void) USBP_Dp_ClearInterrupt();
        CyIntClearPending(USBP_DP_INTC_VECT_NUM);
    #endif /* (USBP_DP_ISR_ACTIVE). */

#endif /* (CY_PSOC4) */

    /* Reset component internal variables. */
    USBP_configurationChanged = 0u;
    USBP_configuration   = 0u;
    USBP_interfaceNumber = 0u;
    USBP_deviceAddress   = 0u;
    USBP_deviceStatus    = 0u;

    /* It is mandatory for correct device startup. */
    USBP_initVar = 0u;
}


/*******************************************************************************
* Function Name: USBP_CheckActivity
****************************************************************************//**
*
*  This function returns the activity status of the bus. It clears the hardware
*  status to provide updated status on the next call of this function. It
*  provides a way to determine whether any USB bus activity occurred. The
*  application should use this function to determine if the USB suspend
*  conditions are met.
*
*
* \return
*  cystatus: Status of the bus since the last call of the function.
*  Return Value |   Description
*  -------------|---------------------------------------------------------------
*  1            |Bus activity was detected since the last call to this function
*  0            |Bus activity was not detected since the last call to this function
*
*
*******************************************************************************/
uint8 USBP_CheckActivity(void) 
{
    uint8 cr1Reg = USBP_CR1_REG;

    /* Clear bus activity. */
    USBP_CR1_REG = (cr1Reg & (uint8) ~USBP_CR1_BUS_ACTIVITY);

    /* Get bus activity. */
    return ((0u != (cr1Reg & USBP_CR1_BUS_ACTIVITY)) ? (1u) : (0u));
}


/*******************************************************************************
* Function Name: USBP_GetConfiguration
****************************************************************************//**
*
*  This function gets the current configuration of the USB device.
*
* \return
*  Returns the currently assigned configuration. Returns 0 if the device
*  is not configured
*
*******************************************************************************/
uint8 USBP_GetConfiguration(void) 
{
    return (USBP_configuration);
}


/*******************************************************************************
* Function Name: USBP_IsConfigurationChanged
****************************************************************************//**
*
*  This function returns the clear-on-read configuration state.  It is useful
*  when the host sends double SET_CONFIGURATION request with the same 
*  configuration number or changes alternate settings of the interface. 
*  After configuration has been changed the OUT endpoints must be enabled and IN 
*  endpoint must be loaded with data to start communication with the host.
*
* \return
*  None-zero value when new configuration has been changed, otherwise zero is
*  returned.
*
* \globalvars
*
*  \ref USBP_configurationChanged - This variable is set to 1 after
*   a SET_CONFIGURATION request and cleared in this function.
*
*******************************************************************************/
uint8 USBP_IsConfigurationChanged(void) 
{
    uint8 res = 0u;

    if (USBP_configurationChanged != 0u)
    {
        res = USBP_configurationChanged;
        USBP_configurationChanged = 0u;
    }

    return (res);
}


/*******************************************************************************
* Function Name: USBP_GetInterfaceSetting
****************************************************************************//**
*
*  This function gets the current alternate setting for the specified interface.
*  It is useful to identify which alternate settings are active in the specified 
*  interface.
*
*  \param
*  interfaceNumber interface number
*
* \return
*  Returns the current alternate setting for the specified interface.
*
*******************************************************************************/
uint8  USBP_GetInterfaceSetting(uint8 interfaceNumber)
                                                    
{
    return (USBP_interfaceSetting[interfaceNumber]);
}


/*******************************************************************************
* Function Name: USBP_GetEPState
****************************************************************************//**
*
*  This function returns the state of the requested endpoint.
*
* \param epNumber Data endpoint number
*
* \return
*  Returns the current state of the specified USBFS endpoint. Symbolic names and
*  their associated values are given in the following table. Use these constants
*  whenever you write code to change the state of the endpoints, such as ISR
*  code, to handle data sent or received.
*
*  Return Value           | Description
*  -----------------------|-----------------------------------------------------
*  USBFS_NO_EVENT_PENDING |The endpoint is awaiting SIE action
*  USBFS_EVENT_PENDING    |The endpoint is awaiting CPU action
*  USBFS_NO_EVENT_ALLOWED |The endpoint is locked from access
*  USBFS_IN_BUFFER_FULL   |The IN endpoint is loaded and the mode is set to ACK IN
*  USBFS_IN_BUFFER_EMPTY  |An IN transaction occurred and more data can be loaded
*  USBFS_OUT_BUFFER_EMPTY |The OUT endpoint is set to ACK OUT and is waiting for data
*  USBFS_OUT_BUFFER_FULL  |An OUT transaction has occurred and data can be read
*
*******************************************************************************/
uint8 USBP_GetEPState(uint8 epNumber) 
{
    return (USBP_EP[epNumber].apiEpState);
}


/*******************************************************************************
* Function Name: USBP_GetEPCount
****************************************************************************//**
*
*  This function supports Data Endpoints only(EP1-EP8).
*  Returns the transfer count for the requested endpoint.  The value from
*  the count registers includes 2 counts for the two byte checksum of the
*  packet.  This function subtracts the two counts.
*
*  \param epNumber Data Endpoint Number.
*                   Valid values are between 1 and 8.
*
* \return
*  Returns the current byte count from the specified endpoint or 0 for an
*  invalid endpoint.
*
*******************************************************************************/
uint16 USBP_GetEPCount(uint8 epNumber) 
{
    uint16 cntr = 0u;

    if ((epNumber > USBP_EP0) && (epNumber < USBP_MAX_EP))
    {
        /* Get 11-bits EP counter where epCnt0 - 3 bits MSB and epCnt1 - 8 bits LSB. */
        cntr  = ((uint16) USBP_SIE_EP_BASE.sieEp[epNumber].epCnt0) & USBP_EPX_CNT0_MASK;
        cntr  = ((uint16) (cntr << 8u)) | ((uint16) USBP_SIE_EP_BASE.sieEp[epNumber].epCnt1);
        cntr -= USBP_EPX_CNTX_CRC_COUNT;
    }

    return (cntr);
}


#if (USBP_EP_MANAGEMENT_DMA)
#if (CY_PSOC4)
    /*******************************************************************************
    * Function Name: USBP_InitEpDma
    ****************************************************************************//**
    *
    *  This function configures priority for all DMA channels utilized by the
    *  component. Also sets callbacks for DMA auto mode.
    *
    *******************************************************************************/
    static void USBP_InitEpDma(void)
    {
    #if (USBP_DMA1_ACTIVE)
        CYDMA_CH_CTL_BASE.ctl[USBP_ep1_dma_CHANNEL] = USBP_ep1_dma_CHANNEL_CFG;
    #endif /* (USBP_DMA1_ACTIVE) */

    #if (USBP_DMA2_ACTIVE)
        CYDMA_CH_CTL_BASE.ctl[USBP_ep2_dma_CHANNEL] = USBP_ep2_dma_CHANNEL_CFG;
    #endif /* (USBP_DMA2_ACTIVE) */

    #if (USBP_DMA3_ACTIVE)
        CYDMA_CH_CTL_BASE.ctl[USBP_ep3_dma_CHANNEL] = USBP_ep3_dma_CHANNEL_CFG;
    #endif /* (USBP_DMA3_ACTIVE) */

    #if (USBP_DMA4_ACTIVE)
        CYDMA_CH_CTL_BASE.ctl[USBP_ep4_dma_CHANNEL] = USBP_ep4_dma_CHANNEL_CFG;
    #endif /* (USBP_DMA4_ACTIVE) */

    #if (USBP_DMA5_ACTIVE)
        CYDMA_CH_CTL_BASE.ctl[USBP_ep5_dma_CHANNEL] = USBP_ep5_dma_CHANNEL_CFG;
    #endif /* (USBP_DMA5_ACTIVE) */

    #if (USBP_DMA6_ACTIVE)
        CYDMA_CH_CTL_BASE.ctl[USBP_ep6_dma_CHANNEL] = USBP_ep6_dma_CHANNEL_CFG;
    #endif /* (USBP_DMA6_ACTIVE) */

    #if (USBP_DMA7_ACTIVE)
        CYDMA_CH_CTL_BASE.ctl[USBP_ep7_dma_CHANNEL] = USBP_ep7_dma_CHANNEL_CFG;
    #endif /* (USBP_DMA7_ACTIVE) */

    #if (USBP_DMA8_ACTIVE)
        CYDMA_CH_CTL_BASE.ctl[USBP_ep8_dma_CHANNEL] = USBP_ep8_dma_CHANNEL_CFG;
    #endif /* (USBP_DMA8_ACTIVE) */

    #if (USBP_EP_MANAGEMENT_DMA_AUTO)
        /* Initialize DMA channel callbacks. */
        #if (USBP_DMA1_ACTIVE)
            (void) USBP_ep1_dma_SetInterruptCallback(&USBP_EP1_DMA_DONE_ISR);
        #endif /* (USBP_DMA1_ACTIVE) */

        #if (USBP_DMA2_ACTIVE)
            (void) USBP_ep2_dma_SetInterruptCallback(&USBP_EP2_DMA_DONE_ISR);
        #endif /* (USBP_DMA2_ACTIVE) */

        #if (USBP_DMA3_ACTIVE)
            (void) USBP_ep3_dma_SetInterruptCallback(&USBP_EP3_DMA_DONE_ISR);
        #endif /* (USBP_DMA3_ACTIVE) */

        #if (USBP_DMA4_ACTIVE)
            (void) USBP_ep4_dma_SetInterruptCallback(&USBP_EP4_DMA_DONE_ISR);
        #endif /* (USBP_DMA4_ACTIVE) */

        #if (USBP_DMA5_ACTIVE)
            (void) USBP_ep5_dma_SetInterruptCallback(&USBP_EP5_DMA_DONE_ISR);
        #endif /* (USBP_DMA5_ACTIVE) */

        #if (USBP_DMA6_ACTIVE)
            (void) USBP_ep6_dma_SetInterruptCallback(&USBP_EP6_DMA_DONE_ISR);
        #endif /* (USBP_DMA6_ACTIVE) */

        #if (USBP_DMA7_ACTIVE)
            (void) USBP_ep7_dma_SetInterruptCallback(&USBP_EP7_DMA_DONE_ISR);
        #endif /* (USBP_DMA7_ACTIVE) */

        #if (USBP_DMA8_ACTIVE)
            (void) USBP_ep8_dma_SetInterruptCallback(&USBP_EP8_DMA_DONE_ISR);
        #endif /* (USBP_DMA8_ACTIVE) */
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
    }
#else


    /***************************************************************************
    * Function Name: USBP_InitEP_DMA
    ************************************************************************//**
    *
    *  This function allocates and initializes a DMA channel to be used by the
    *  USBP_LoadInEP() or USBP_ReadOutEP() APIs for data
    *  transfer. It is available when the Endpoint Memory Management parameter
    *  is set to DMA.
    *
    *  This function is automatically called from the USBFS_LoadInEP() and USBFS_ReadOutEP() APIs.
    *
    *  \param epNumber Contains the data endpoint number.
    *            Valid values are between 1 and 8.
    *  \param *pData Pointer to a data array that is related to the EP transfers.
    *
    * \reentrant No.
    *
    ***************************************************************************/
    void USBP_InitEP_DMA(uint8 epNumber, const uint8 *pData)
                                                                    
    {
        uint16 src;
        uint16 dst;

    #if (CY_PSOC3)
        src = HI16(CYDEV_SRAM_BASE);
        dst = HI16(CYDEV_PERIPH_BASE);
        pData = pData;
    #else
        if ((USBP_EP[epNumber].addr & USBP_DIR_IN) != 0u)
        {
            /* IN endpoint: source is memory buffer. */
            src = HI16(pData);
            dst = HI16(CYDEV_PERIPH_BASE);
        }
        else
        {
            /* OUT endpoint: source is USB IP memory buffer. */
            src = HI16(CYDEV_PERIPH_BASE);
            dst = HI16(pData);
        }
    #endif  /* (CY_PSOC3) */

        switch(epNumber)
        {
    #if (USBP_DMA1_ACTIVE)
        case USBP_EP1:
            USBP_DmaChan[epNumber] = USBP_ep1_DmaInitialize(USBP_DMA_BYTES_PER_BURST,
                                                                                    USBP_DMA_REQUEST_PER_BURST, src, dst);
            break;
    #endif /* (USBP_DMA1_ACTIVE) */

    #if (USBP_DMA2_ACTIVE)
        case USBP_EP2:
            USBP_DmaChan[epNumber] = USBP_ep2_DmaInitialize(USBP_DMA_BYTES_PER_BURST,
                                                                                    USBP_DMA_REQUEST_PER_BURST, src, dst);
            break;
    #endif /* (USBP_DMA2_ACTIVE) */

    #if (USBP_DMA3_ACTIVE)
            case USBP_EP3:
                USBP_DmaChan[epNumber] = USBP_ep3_DmaInitialize(USBP_DMA_BYTES_PER_BURST,
                                                                                        USBP_DMA_REQUEST_PER_BURST, src, dst);
            break;
    #endif /* (USBP_DMA3_ACTIVE) */

    #if (USBP_DMA4_ACTIVE)
            case USBP_EP4:
                USBP_DmaChan[epNumber] = USBP_ep4_DmaInitialize(USBP_DMA_BYTES_PER_BURST,
                                                                                        USBP_DMA_REQUEST_PER_BURST, src, dst);
            break;
    #endif /* (USBP_DMA4_ACTIVE) */

    #if (USBP_DMA5_ACTIVE)
            case USBP_EP5:
                USBP_DmaChan[epNumber] = USBP_ep5_DmaInitialize(USBP_DMA_BYTES_PER_BURST,
                                                                                        USBP_DMA_REQUEST_PER_BURST, src, dst);
            break;
    #endif /* (USBP_DMA5_ACTIVE) */

    #if (USBP_DMA6_ACTIVE)
        case USBP_EP6:
            USBP_DmaChan[epNumber] = USBP_ep6_DmaInitialize(USBP_DMA_BYTES_PER_BURST,
                                                                                    USBP_DMA_REQUEST_PER_BURST, src, dst);
            break;
    #endif /* (USBP_DMA6_ACTIVE) */

    #if (USBP_DMA7_ACTIVE)
        case USBP_EP7:
                USBP_DmaChan[epNumber] = USBP_ep7_DmaInitialize(USBP_DMA_BYTES_PER_BURST,
                                                                                        USBP_DMA_REQUEST_PER_BURST, src, dst);
            break;
    #endif /* (USBP_DMA7_ACTIVE) */

    #if (USBP_DMA8_ACTIVE)
        case USBP_EP8:
                USBP_DmaChan[epNumber] = USBP_ep8_DmaInitialize(USBP_DMA_BYTES_PER_BURST,
                                                                                        USBP_DMA_REQUEST_PER_BURST, src, dst);
            break;
    #endif /* (USBP_DMA8_ACTIVE) */

        default:
            /* Do nothing for endpoints other than 1-8. */
            break;
        }

        if ((epNumber > USBP_EP0) && (epNumber < USBP_MAX_EP))
        {
            USBP_DmaTd[epNumber] = CyDmaTdAllocate();

        #if (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u))
            USBP_DmaNextTd[epNumber] = CyDmaTdAllocate();
        #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u)) */
        }
    }
#endif /* (CY_PSOC4) */

    /***************************************************************************
    * Function Name: USBP_Stop_DMA
    ************************************************************************//**
    *  
    *  This function stops DMA channel associated with endpoint. It is available 
    *  when the Endpoint Buffer Management parameter is set to DMA. Call this 
    *  function when endpoint direction is changed from IN to OUT or vice versa 
    *  to trigger DMA re-configuration when USBP_LoadInEP() or 
    *  USBP_ReadOutEP() functions are called the first time. 
    *  
    *  \param epNumber: The data endpoint number for which associated DMA 
    *  channel is stopped. The range of valid values is between 1 and 8. To stop 
    *  all DMAs associated with endpoints call this function with 
    *  USBP_MAX_EP argument.
    *
    * \reentrant
    *  No.
    *
    ***************************************************************************/
    void USBP_Stop_DMA(uint8 epNumber) 
    {
        uint8 i;

        i = (epNumber < USBP_MAX_EP) ? epNumber : USBP_EP1;

        do
        {
        #if (CY_PSOC4)
            if (0u != USBP_DmaChan[i])
            {
                USBP_CyDmaChDisable(USBP_DmaChan[i]);
            }
        #else
            if(USBP_DmaTd[i] != DMA_INVALID_TD)
            {
                (void) CyDmaChDisable(USBP_DmaChan[i]);
                CyDmaTdFree(USBP_DmaTd[i]);
                USBP_DmaTd[i] = DMA_INVALID_TD;
            }

            #if (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u))
                if(USBP_DmaNextTd[i] != DMA_INVALID_TD)
                {
                    CyDmaTdFree(USBP_DmaNextTd[i]);
                    USBP_DmaNextTd[i] = DMA_INVALID_TD;
                }
            #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u)) */

        #endif /* (CY_PSOC4) */
            i++;
        }
        while ((i < USBP_MAX_EP) && (epNumber == USBP_MAX_EP));
    }
#endif /* (USBP_EP_MANAGEMENT_DMA) */


#if (CY_PSOC3 || CY_PSOC5)
#if (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u))
    /***************************************************************************
    * Function Name: USBP_LoadNextInEP
    ************************************************************************//**
    *
    * Summary:
    *  This internal function is used for IN endpoint DMA reconfiguration in
    *  Auto DMA mode.
    *
    * Parameters:
    *  epNumber: Contains the data endpoint number.
    *  mode:   0 - Configure DMA to send the the rest of data.
    *          1 - Configure DMA to repeat 2 last bytes of the first burst.
    *
    ***************************************************************************/
    void USBP_LoadNextInEP(uint8 epNumber, uint8 mode) 
    {
        reg16 *convert;

        if (mode == 0u)
        {
            /* Configure DMA to send rest of data. */
            /* CyDmaTdSetConfiguration API is optimized to change transfer length only and configure TD. */
            convert = (reg16 *) &CY_DMA_TDMEM_STRUCT_PTR[USBP_DmaTd[epNumber]].TD0[0u];
            /* Set transfer length. */
            CY_SET_REG16(convert, USBP_inLength[epNumber] - USBP_DMA_BYTES_PER_BURST);
            /* CyDmaTdSetAddress API is optimized to change source address only. */
            convert = (reg16 *) &CY_DMA_TDMEM_STRUCT_PTR[USBP_DmaTd[epNumber]].TD1[0u];
            CY_SET_REG16(convert, LO16((uint32)USBP_inDataPointer[epNumber] +
                                            USBP_DMA_BYTES_PER_BURST));
            USBP_inBufFull[epNumber] = 1u;
        }
        else
        {
            /* Configure DMA to repeat 2 last bytes of the first burst. */
            /* CyDmaTdSetConfiguration API is optimized to change transfer length only and configure TD. */
            convert = (reg16 *) &CY_DMA_TDMEM_STRUCT_PTR[USBP_DmaTd[epNumber]].TD0[0u];
            /* Set transfer length. */
            CY_SET_REG16(convert, USBP_DMA_BYTES_REPEAT);
            /* CyDmaTdSetAddress API is optimized to change source address only. */
            convert = (reg16 *) &CY_DMA_TDMEM_STRUCT_PTR[USBP_DmaTd[epNumber]].TD1[0u];
            CY_SET_REG16(convert,  LO16((uint32)USBP_inDataPointer[epNumber] +
                                   (USBP_DMA_BYTES_PER_BURST - USBP_DMA_BYTES_REPEAT)));
        }

        /* CyDmaChSetInitialTd API is optimized to initialize TD. */
        CY_DMA_CH_STRUCT_PTR[USBP_DmaChan[epNumber]].basic_status[1u] = USBP_DmaTd[epNumber];
    }
#endif /* (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u)) */
#endif /* (CY_PSOC3 || CY_PSOC5) */


/*******************************************************************************
* Function Name: USBP_LoadInEP
****************************************************************************//**
*
*  This function performs different functionality depending on the Component’s
*  configured Endpoint Buffer Management. This parameter is defined in
*  the Descriptor Root in Component Configure window.
*
*  *Manual (Static/Dynamic Allocation):*
*  This function loads and enables the specified USB data endpoint for an IN
*  data transfer.
*
*  *DMA with Manual Buffer Management:*
*  Configures DMA for a data transfer from system RAM to endpoint buffer.
*  Generates request for a transfer.
*
*  *DMA with Automatic Buffer Management:*
*  Configures DMA. This is required only once, so it is done only when parameter
*  pData is not NULL. When the pData pointer is NULL, the function skips this
*  task. Sets Data ready status: This generates the first DMA transfer and
*  prepares data in endpoint buffer.
*
*  \param epNumber Contains the data endpoint number.
*            Valid values are between 1 and 8.
*  \param *pData A pointer to a data array from which the data for the endpoint space
*          is loaded.
*  \param length The number of bytes to transfer from the array and then send as
*          a result of an IN request. Valid values are between 0 and 512
*          (1023 for DMA with Automatic Buffer Management mode). The value 512
*          is applicable if only one endpoint is used.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_LoadInEP(uint8 epNumber, const uint8 pData[], uint16 length)
                                                                        
{
    if ((epNumber > USBP_EP0) && (epNumber < USBP_MAX_EP))
    {
    #if (!USBP_EP_MANAGEMENT_DMA_AUTO)
        /* Limit length to available buffer USB IP buffer size.*/
        if (length > (USBP_EPX_DATA_BUF_MAX - USBP_EP[epNumber].buffOffset))
        {
            length = USBP_EPX_DATA_BUF_MAX - USBP_EP[epNumber].buffOffset;
        }
    #endif /* (!USBP_EP_MANAGEMENT_DMA_AUTO) */

        /* Set count and data toggle. */
        USBP_SIE_EP_BASE.sieEp[epNumber].epCnt0 = (uint8) HI8(length) | USBP_EP[epNumber].epToggle;
        USBP_SIE_EP_BASE.sieEp[epNumber].epCnt1 = (uint8) LO8(length);

    #if (USBP_EP_MANAGEMENT_MANUAL)
        if (NULL != pData)
        {
			/* Copy data using arbiter data register. */
            uint16 i;
            for (i = 0u; i < length; ++i)
            {
                USBP_ARB_EP_BASE.arbEp[epNumber].rwDr = pData[i];
            }
        }

        /* IN endpoint buffer is full - read to be read. */
        USBP_EP[epNumber].apiEpState = USBP_NO_EVENT_PENDING;

        /* Arm IN endpoint. */
        USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBP_EP[epNumber].epMode;

    #else

        #if (CY_PSOC3 || CY_PSOC5LP)
            /* Initialize DMA if it was not initialized. */
            if (DMA_INVALID_TD == USBP_DmaTd[epNumber])
            {
                USBP_InitEP_DMA(epNumber, pData);
            }
        #endif /* (CY_PSOC3 || CY_PSOC5LP) */

        #if (USBP_EP_MANAGEMENT_DMA_MANUAL)
            /* IN endpoint buffer will be fully loaded by DMA shortly. */
            USBP_EP[epNumber].apiEpState = USBP_NO_EVENT_PENDING;

            if ((pData != NULL) && (length > 0u))
            {
            #if (CY_PSOC4)
                uint32 channelNum = (uint32) USBP_DmaChan[epNumber];

                /* Disable DMA channel: start configuration. */
                USBP_CyDmaChDisable(channelNum);

                /* Configure source and destination. */
                USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR0, (void*) pData);
                USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR0, (void*) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr);

                /* Configure DMA descriptor. */
                --length;
                USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR0, USBP_DMA_COMMON_CFG | length |
                                                        CYDMA_BYTE | CYDMA_ELEMENT_WORD | CYDMA_INC_SRC_ADDR | CYDMA_INVALIDATE | CYDMA_PREEMPTABLE);

                /* Validate descriptor to execute on following DMA request. */
                USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR0);

                /* Enable DMA channel: configuration complete. */
                USBP_CyDmaChEnable(channelNum);
            #else
                /* Configure DMA to transfer data. */
                (void) CyDmaChDisable(USBP_DmaChan[epNumber]);
                (void) CyDmaTdSetConfiguration(USBP_DmaTd[epNumber], length, CY_DMA_DISABLE_TD, TD_TERMIN_EN | TD_INC_SRC_ADR);
                (void) CyDmaTdSetAddress(USBP_DmaTd[epNumber], LO16((uint32) pData), LO16((uint32) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr));

                /* Enable DMA channel. */
                (void) CyDmaChSetInitialTd(USBP_DmaChan[epNumber], USBP_DmaTd[epNumber]);
                (void) CyDmaChEnable(USBP_DmaChan[epNumber], 1u);
            #endif /* (CY_PSOC4) */

                /* Generate DMA request. */
                USBP_ARB_EP_BASE.arbEp[epNumber].epCfg |=  (uint8)  USBP_ARB_EPX_CFG_DMA_REQ;
                USBP_ARB_EP_BASE.arbEp[epNumber].epCfg &=  (uint8) ~USBP_ARB_EPX_CFG_DMA_REQ;

                /* IN endpoint will be armed in ARB_ISR(source: IN_BUF_FULL) after first DMA transfer has been completed. */
            }
            else
            {
                /* When zero-length packet: arm IN endpoint directly. */
                USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBP_EP[epNumber].epMode;
            }
        #endif /* (USBP_EP_MANAGEMENT_DMA_MANUAL) */

        #if (USBP_EP_MANAGEMENT_DMA_AUTO)
            if (pData != NULL)
            {
            #if (CY_PSOC4)
                uint32 channelNum = (uint32) USBP_DmaChan[epNumber];

                /* Store address of buffer. */
                USBP_DmaEpBufferAddrBackup[epNumber] = (uint32) pData;

                /* Disable DMA channel: start configuration. */
                USBP_CyDmaChDisable(channelNum);

                /* Set destination address. */
                USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR0, (void*) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr);
                USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR1, (void*) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr);

                /* Configure DMA descriptor. */
                USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR0, USBP_DMA_COMMON_CFG  |
                                                        CYDMA_BYTE | CYDMA_ELEMENT_WORD | CYDMA_INC_SRC_ADDR | CYDMA_INVALIDATE | CYDMA_CHAIN);

                /* Configure DMA descriptor. */
                USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR1, USBP_DMA_COMMON_CFG  |
                                                        CYDMA_BYTE | CYDMA_ELEMENT_WORD | CYDMA_INC_SRC_ADDR | CYDMA_INVALIDATE | CYDMA_CHAIN);

                /* Enable interrupt from DMA channel. */
                USBP_CyDmaSetInterruptMask(channelNum);


                /* Enable DMA channel: configuration complete. */
                USBP_CyDmaChEnable(channelNum);

            #else
                (void) CyDmaChDisable(USBP_DmaChan[epNumber]);

                #if (USBP_EP_DMA_AUTO_OPT == 0u)
                    USBP_inLength[epNumber] = length;
                    USBP_inDataPointer[epNumber] = pData;

                    /* Configure DMA to send data only for first burst */
                    (void) CyDmaTdSetConfiguration(USBP_DmaTd[epNumber],
                        (length > USBP_DMA_BYTES_PER_BURST) ? USBP_DMA_BYTES_PER_BURST : length,
                        USBP_DmaNextTd[epNumber], TD_TERMIN_EN | TD_INC_SRC_ADR);
                    (void) CyDmaTdSetAddress(USBP_DmaTd[epNumber], LO16((uint32) pData),
                                                                               LO16((uint32) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr));

                    /* The second TD will be executed only when the first one fails.
                    *  The intention of this TD is to generate NRQ interrupt
                    *  and repeat 2 last bytes of the first burst.
                    */
                    (void) CyDmaTdSetConfiguration(USBP_DmaNextTd[epNumber], 1u,
                                                   USBP_DmaNextTd[epNumber],
                                                   USBP_epX_TD_TERMOUT_EN[epNumber]);

                    /* Configure DmaNextTd to clear Data Ready status. */
                    (void) CyDmaTdSetAddress(USBP_DmaNextTd[epNumber], LO16((uint32) &clearInDataRdyStatus),
                                                                                   LO16((uint32) &USBP_ARB_EP_BASE.arbEp[epNumber].epCfg));
                #else
                    /* Configure DMA to send all data. */
                    (void) CyDmaTdSetConfiguration(USBP_DmaTd[epNumber], length,
                                                   USBP_DmaTd[epNumber], TD_TERMIN_EN | TD_INC_SRC_ADR);
                    (void) CyDmaTdSetAddress(USBP_DmaTd[epNumber], LO16((uint32) pData),
                                                                               LO16((uint32) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr));
                #endif /* (USBP_EP_DMA_AUTO_OPT == 0u) */

                /* Clear any potential pending DMA requests before starting DMA channel to transfer data. */
                (void) CyDmaClearPendingDrq(USBP_DmaChan[epNumber]);
                /* Enable DMA. */
                (void) CyDmaChSetInitialTd(USBP_DmaChan[epNumber], USBP_DmaTd[epNumber]);
                (void) CyDmaChEnable(USBP_DmaChan[epNumber], 1u);
            #endif /* (CY_PSOC4) */
            }
            else
            {
                /* IN endpoint buffer (32 bytes) will shortly be preloaded by DMA. */
                USBP_EP[epNumber].apiEpState = USBP_NO_EVENT_PENDING;

                if (length > 0u)
                {
                #if (CY_PSOC4)
                    uint32 lengthDescr0, lengthDescr1;
                    uint32 channelNum = (uint32) USBP_DmaChan[epNumber];

                    /* Get number of full bursts. */
                    USBP_DmaEpBurstCnt[epNumber] = (uint8) (length / USBP_DMA_BYTES_PER_BURST);

                    /* Get number of elements in the last burst. */
                    USBP_DmaEpLastBurstEl[epNumber] = (uint8) (length % USBP_DMA_BYTES_PER_BURST);

                    /* Get total number of bursts. */
                    USBP_DmaEpBurstCnt[epNumber] += (0u != USBP_DmaEpLastBurstEl[epNumber]) ? 1u : 0u;

                    /* Adjust number of data elements transferred in last burst. */
                    USBP_DmaEpLastBurstEl[epNumber] = (0u != USBP_DmaEpLastBurstEl[epNumber]) ?
                                                                          (USBP_DmaEpLastBurstEl[epNumber] - 1u) :
                                                                          (USBP_DMA_BYTES_PER_BURST - 1u);

                    /* Get number of data elements to transfer for descriptor 0 and 1. */
                    lengthDescr0 = (1u == USBP_DmaEpBurstCnt[epNumber]) ? USBP_DmaEpLastBurstEl[epNumber] : (USBP_DMA_BYTES_PER_BURST - 1u);
                    lengthDescr1 = (2u == USBP_DmaEpBurstCnt[epNumber]) ? USBP_DmaEpLastBurstEl[epNumber] : (USBP_DMA_BYTES_PER_BURST - 1u);


                    /* Mark which descriptor is last one to execute. */
                    USBP_DmaEpLastBurstEl[epNumber] |= (0u != (USBP_DmaEpBurstCnt[epNumber] & 0x1u)) ?
                                                                            USBP_DMA_DESCR0_MASK : USBP_DMA_DESCR1_MASK;

                    /* Restore DMA settings for current transfer. */
                    USBP_CyDmaChDisable(channelNum);

                    /* Restore destination address for input endpoint. */
                    USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR0, (void*) ((uint32) USBP_DmaEpBufferAddrBackup[epNumber]));
                    USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR1, (void*) ((uint32) USBP_DmaEpBufferAddrBackup[epNumber] +
                                                                                                                   USBP_DMA_BYTES_PER_BURST));

                    /* Set number of elements to transfer. */
                    USBP_CyDmaSetNumDataElements(channelNum, USBP_DMA_DESCR0, lengthDescr0);
                    USBP_CyDmaSetNumDataElements(channelNum, USBP_DMA_DESCR1, lengthDescr1);

                    /* Validate descriptor 0 and command to start with it. */
                    USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR0);
                    USBP_CyDmaSetDescriptor0Next(channelNum);

                    /* Validate descriptor 1. */
                    if (USBP_DmaEpBurstCnt[epNumber] > 1u)
                    {
                        USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR1); 
                    }                   

                    /* Adjust burst counter taking to account: 2 valid descriptors and interrupt trigger after valid descriptor were executed. */
                    USBP_DmaEpBurstCnt[epNumber] = USBP_DMA_GET_BURST_CNT(USBP_DmaEpBurstCnt[epNumber]);

                    /* Enable DMA channel: configuration complete. */
                    USBP_CyDmaChEnable(channelNum);

                #elif (USBP_EP_DMA_AUTO_OPT == 0u)
                    USBP_inLength[epNumber]  = length;
                    USBP_inBufFull[epNumber] = 0u;

                    (void) CyDmaChDisable(USBP_DmaChan[epNumber]);
                    /* Configure DMA to send data only for first burst. */
                    (void) CyDmaTdSetConfiguration(
                        USBP_DmaTd[epNumber], (length > USBP_DMA_BYTES_PER_BURST) ?
                        USBP_DMA_BYTES_PER_BURST : length,
                        USBP_DmaNextTd[epNumber], TD_TERMIN_EN | TD_INC_SRC_ADR );
                    (void) CyDmaTdSetAddress(USBP_DmaTd[epNumber], LO16((uint32)  USBP_inDataPointer[epNumber]),
                                                                               LO16((uint32) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr));
                    /* Clear Any potential pending DMA requests before starting DMA channel to transfer data. */
                    (void) CyDmaClearPendingDrq(USBP_DmaChan[epNumber]);
                    /* Enable DMA. */
                    (void) CyDmaChSetInitialTd(USBP_DmaChan[epNumber], USBP_DmaTd[epNumber]);
                    (void) CyDmaChEnable(USBP_DmaChan[epNumber], 1u);
                #endif /* (CY_PSOC4) */

                #if !defined (USBP_MANUAL_IN_EP_ARM)
                    /* Set IN data ready to generate DMA request to load data into endpoint buffer. */
                    USBP_ARB_EP_BASE.arbEp[epNumber].epCfg |= USBP_ARB_EPX_CFG_IN_DATA_RDY;
                #endif  /* (USBP_MANUAL_IN_EP_ARM) */

                    /* IN endpoint will be armed in ARB_ISR(source: IN_BUF_FULL) after first DMA transfer has been completed. */
                }
                else
                {
                    /* When zero-length packet: arm IN endpoint directly. */
                    USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBP_EP[epNumber].epMode;
                }
            }
        #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
    #endif /* (USBP_EP_MANAGEMENT_MANUAL) */
    }
}


/*******************************************************************************
* Function Name: USBP_ReadOutEP
****************************************************************************//**
*
*   This function performs different functionality depending on the Component’s
*   configured Endpoint Buffer Management. This parameter is defined in the
*   Descriptor Root in Component Configure window.
*
*   *Manual (Static/Dynamic Allocation):*
*   This function moves the specified number of bytes from endpoint buffer to
*   system RAM. The number of bytes actually transferred from endpoint buffer to
*   system RAM is the lesser of the actual number of bytes sent by the host or
*   the number of bytes requested by the length parameter.
*
*   *DMA with Manual Buffer Management:*
*   Configure DMA to transfer data from endpoint buffer to system RAM. Generate
*   a DMA request. The firmware must wait until the DMA completes the data
*   transfer after calling the USBP_ReadOutEP() API. For example,
*   by checking EPstate:
*
*   \snippet /USBFS_sut_02.cydsn/main.c checking EPstatey
*
*   The USBFS_EnableOutEP() has to be called to allow host to write data into
*   the endpoint buffer after DMA has completed transfer data from OUT endpoint
*   buffer to SRAM.
*
*   *DMA with Automatic Buffer Management:*
*   Configure DMA. This is required only once and automatically generates DMA
*   requests as data arrives
*
*  \param epNumber: Contains the data endpoint number.
*            Valid values are between 1 and 8.
*  \param pData: A pointer to a data array from which the data for the endpoint
*         space is loaded.
*  \param length: The number of bytes to transfer from the USB Out endpoint and
*          loads it into data array. Valid values are between 0 and 1023. The
*          function moves fewer than the requested number of bytes if the host
*          sends fewer bytes than requested.
*
* \return
*  Number of bytes received, 0 for an invalid endpoint.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint16 USBP_ReadOutEP(uint8 epNumber, uint8 pData[], uint16 length)
                                                                        
{
    if ((pData != NULL) && (epNumber > USBP_EP0) && (epNumber < USBP_MAX_EP))
    {
    #if (!USBP_EP_MANAGEMENT_DMA_AUTO)
        /* Adjust requested length to available data. */
        length = (length > USBP_GetEPCount(epNumber)) ? USBP_GetEPCount(epNumber) : length;
    #endif /* (!USBP_EP_MANAGEMENT_DMA_AUTO) */

    #if (USBP_EP_MANAGEMENT_MANUAL)
        {
            /* Copy data using arbiter data register. */
            uint16 i;
            for (i = 0u; i < length; ++i)
            {
                pData[i] = (uint8) USBP_ARB_EP_BASE.arbEp[epNumber].rwDr;
            }
        }

        /* Arm OUT endpoint after data has been copied from endpoint buffer. */
        USBP_EnableOutEP(epNumber);
    #else

        #if (CY_PSOC3 || CY_PSOC5LP)
            /* Initialize DMA if it was not initialized. */
            if (DMA_INVALID_TD == USBP_DmaTd[epNumber])
            {
                USBP_InitEP_DMA(epNumber, pData);
            }
        #endif /* (CY_PSOC3 || CY_PSOC5LP) */

        #if (USBP_EP_MANAGEMENT_DMA_MANUAL)
        #if (CY_PSOC4)
        {
            uint32 channelNum = (uint32) USBP_DmaChan[epNumber];

            /* Disable DMA channel: start configuration. */
            USBP_CyDmaChDisable(channelNum);

            /* Configure source and destination. */
            USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR0, (void*) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr);
            USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR0, (void*) pData);

            /* Configure DMA descriptor. */
            USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR0, USBP_DMA_COMMON_CFG | (uint16)(length - 1u) |
                                                    CYDMA_BYTE | CYDMA_WORD_ELEMENT | CYDMA_INC_DST_ADDR | CYDMA_INVALIDATE | CYDMA_PREEMPTABLE);

            /* Validate descriptor to execute on following DMA request. */
            USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR0);

            /* Enable DMA channel: configuration complete. */
            USBP_CyDmaChEnable(channelNum);
        }
        #else
            /* Configure DMA to transfer data. */
            (void) CyDmaChDisable(USBP_DmaChan[epNumber]);
            (void) CyDmaTdSetConfiguration(USBP_DmaTd[epNumber], length, CY_DMA_DISABLE_TD, TD_TERMIN_EN | TD_INC_DST_ADR);
            (void) CyDmaTdSetAddress(USBP_DmaTd[epNumber], LO16((uint32) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr), LO16((uint32)pData));

            /* Enable DMA channel. */
            (void) CyDmaChSetInitialTd(USBP_DmaChan[epNumber], USBP_DmaTd[epNumber]);
            (void) CyDmaChEnable(USBP_DmaChan[epNumber], 1u);
        #endif /* (CY_PSOC4) */

            /* Generate DMA request. */
            USBP_ARB_EP_BASE.arbEp[epNumber].epCfg |=  (uint8)  USBP_ARB_EPX_CFG_DMA_REQ;
            USBP_ARB_EP_BASE.arbEp[epNumber].epCfg &=  (uint8) ~USBP_ARB_EPX_CFG_DMA_REQ;

            /* OUT endpoint has to be armed again by user when DMA transfers have been completed.
            * NO_EVENT_PENDING: notifies that data has been copied from endpoint buffer.
            */

        #endif /* (USBP_EP_MANAGEMENT_DMA_MANUAL) */

        #if (USBP_EP_MANAGEMENT_DMA_AUTO)
        #if (CY_PSOC4)
        {
            uint32 channelNum = (uint32) USBP_DmaChan[epNumber];
            uint32 lengthDescr0, lengthDescr1;

            /* Get number of full bursts. */
            USBP_DmaEpBurstCnt[epNumber] = (uint8) (length / USBP_DMA_BYTES_PER_BURST);

            /* Get number of elements in the last burst. */
            USBP_DmaEpLastBurstEl[epNumber] = (uint8) (length % USBP_DMA_BYTES_PER_BURST);

            /* Get total number of bursts. */
            USBP_DmaEpBurstCnt[epNumber] += (0u != USBP_DmaEpLastBurstEl[epNumber]) ? 1u : 0u;

            /* Adjust number of the data elements transfered in last burst. */
            USBP_DmaEpLastBurstEl[epNumber] = (0u != USBP_DmaEpLastBurstEl[epNumber]) ?
                                                                  (USBP_DmaEpLastBurstEl[epNumber] - 1u) :
                                                                  (USBP_DMA_BYTES_PER_BURST - 1u);

            /* Get number of data elements to transfer for descriptor 0 and 1. */
            lengthDescr0 = (1u == USBP_DmaEpBurstCnt[epNumber]) ? USBP_DmaEpLastBurstEl[epNumber] : (USBP_DMA_BYTES_PER_BURST - 1u);
            lengthDescr1 = (2u == USBP_DmaEpBurstCnt[epNumber]) ? USBP_DmaEpLastBurstEl[epNumber] : (USBP_DMA_BYTES_PER_BURST - 1u);

            /* Mark if revert number of data elements in descriptor after transfer completion. */
            USBP_DmaEpLastBurstEl[epNumber] |= (USBP_DmaEpBurstCnt[epNumber] > 2u) ? USBP_DMA_DESCR_REVERT : 0u;

            /* Mark last descriptor to be executed. */
            USBP_DmaEpLastBurstEl[epNumber] |= (0u != (USBP_DmaEpBurstCnt[epNumber] & 0x1u)) ?
                                                                    USBP_DMA_DESCR0_MASK : USBP_DMA_DESCR1_MASK;

            /* Store address of buffer and burst counter for endpoint. */
            USBP_DmaEpBufferAddrBackup[epNumber] = (uint32) pData;
            USBP_DmaEpBurstCntBackup[epNumber]   = USBP_DmaEpBurstCnt[epNumber];

            /* Adjust burst counter taking to account: 2 valid descriptors and interrupt trigger after valid descriptor were executed. */
            USBP_DmaEpBurstCnt[epNumber] = USBP_DMA_GET_BURST_CNT(USBP_DmaEpBurstCnt[epNumber]);

            /* Disable DMA channel: start configuration. */
            USBP_CyDmaChDisable(channelNum);

            /* Set destination address. */
            USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR0, (void*) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr);
            USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR1, (void*) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr);

            USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR0, (void*) ((uint32) pData));
            USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR1, (void*) ((uint32) pData + USBP_DMA_BYTES_PER_BURST));

            /* Configure DMA descriptor. */
            USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR0, USBP_DMA_COMMON_CFG  | lengthDescr0 |
                                                    CYDMA_BYTE | CYDMA_WORD_ELEMENT | CYDMA_INC_DST_ADDR | CYDMA_INVALIDATE | CYDMA_CHAIN);

            /* Configure DMA descriptor. */
            USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR1, USBP_DMA_COMMON_CFG  | lengthDescr1 |
                                                    CYDMA_BYTE | CYDMA_WORD_ELEMENT | CYDMA_INC_DST_ADDR | CYDMA_INVALIDATE | CYDMA_CHAIN);

            /* Enable interrupt from DMA channel. */
            USBP_CyDmaSetInterruptMask(channelNum);

            /* Validate DMA descriptor 0 and 1. */
            USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR0);

            if (USBP_DmaEpBurstCntBackup[epNumber] > 1u)
            {
                USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR1);
            }

            /* Enable DMA channel: configuration complete. */
            USBP_CyDmaChEnable(channelNum);
        }
        #else
            (void) CyDmaChDisable(USBP_DmaChan[epNumber]);
            (void) CyDmaTdSetConfiguration(USBP_DmaTd[epNumber], length,  USBP_DmaTd[epNumber], TD_TERMIN_EN | TD_INC_DST_ADR);
            (void) CyDmaTdSetAddress(USBP_DmaTd[epNumber], LO16((uint32) &USBP_ARB_EP_BASE.arbEp[epNumber].rwDr), LO16((uint32) pData));

            /* Clear Any potential pending DMA requests before starting DMA channel to transfer data. */
            (void) CyDmaClearPendingDrq(USBP_DmaChan[epNumber]);

            /* Enable DMA channel. */
            (void) CyDmaChSetInitialTd(USBP_DmaChan[epNumber], USBP_DmaTd[epNumber]);
            (void) CyDmaChEnable(USBP_DmaChan[epNumber], 1u);
        #endif /* (CY_PSOC4) */

            /* OUT endpoint has to be armed again by user when DMA transfers have been completed.
            * NO_EVENT_PENDING: notifies that data has been copied from endpoint buffer.
            */

        #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
    #endif /* (USBP_EP_MANAGEMENT_MANUAL) */
    }
    else
    {
        length = 0u;
    }

    return (length);
}


#if (USBP_16BITS_EP_ACCESS_ENABLE)
/*******************************************************************************
* Function Name: USBP_LoadInEP16
****************************************************************************//**
*
*  This function performs different functionality depending on the Component’s
*  configured Endpoint Buffer Management. This parameter is defined in
*  the Descriptor Root in Component Configure window.
*
*  *Manual (Static/Dynamic Allocation):*
*  This function loads and enables the specified USB data endpoint for an IN
*  data transfer.
*
*  *DMA with Manual Buffer Management:*
*  Configures DMA for a data transfer from system RAM to endpoint buffer.
*  Generates request for a transfer.
*
*  *DMA with Automatic Buffer Management:*
*  Configures DMA. This is required only once, so it is done only when parameter
*  pData is not NULL. When the pData pointer is NULL, the function skips this
*  task. Sets Data ready status: This generates the first DMA transfer and
*  prepares data in endpoint buffer.
*
*  \param epNumber Contains the data endpoint number.
*        Valid values are between 1 and 8.
*  \param *pData A pointer to a data array from which the data for the endpoint
*        space is loaded. It shall be ensured that this pointer address is even
*        to ensure the 16-bit transfer is aligned to even address. Else, a hard
*        fault condition can occur.
*  \param length The number of bytes to transfer from the array and then send as
*        a result of an IN request. Valid values are between 0 and 512 (1023 for
*        DMA with Automatic Buffer Management mode). The value 512 is applicable
*        if only one endpoint is used.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_LoadInEP16(uint8 epNumber, const uint8 pData[], uint16 length)
{
    /* Check array alignment on half-word boundary. */
    CYASSERT(0u == (((uint32) pData) & 0x01u));

    if ((epNumber > USBP_EP0) && (epNumber < USBP_MAX_EP))
    {
    #if (!USBP_EP_MANAGEMENT_DMA_AUTO)
        /* Limit length to available buffer USB IP buffer size. */
        if (length > (USBP_EPX_DATA_BUF_MAX - USBP_EP[epNumber].buffOffset))
        {
            length = USBP_EPX_DATA_BUF_MAX - USBP_EP[epNumber].buffOffset;
        }
    #endif /* (!USBP_EP_MANAGEMENT_DMA_AUTO) */

        /* Set count and data toggle. */
        USBP_SIE_EP_BASE.sieEp[epNumber].epCnt0 = (uint32) HI8(length) | USBP_EP[epNumber].epToggle;
        USBP_SIE_EP_BASE.sieEp[epNumber].epCnt1 = (uint32) LO8(length);

        /* Adjust requested length: 2 bytes are handled at one data register access. */
        length += (length & 0x01u);

    #if (USBP_EP_MANAGEMENT_MANUAL)
        if (NULL != pData)
        {
            /* Convert uint8 array to uint16. */
            const uint16 *dataBuf = (uint16 *) pData;

            /* Copy data using 16-bits arbiter data register. */
            uint16 i;
            for (i = 0u; i < (length >> 1u); ++i)
            {
                USBP_ARB_EP16_BASE.arbEp[epNumber].rwDr16 = dataBuf[i];
            }
        }

        /* IN endpoint buffer is full - read to be read. */
        USBP_EP[epNumber].apiEpState = USBP_NO_EVENT_PENDING;

        /* Arm IN endpoint. */
        USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBP_EP[epNumber].epMode;

    #else

        #if (USBP_EP_MANAGEMENT_DMA_MANUAL)
            /* IN endpoint buffer will be fully loaded by DMA shortly. */
            USBP_EP[epNumber].apiEpState = USBP_NO_EVENT_PENDING;

            if ((pData != NULL) && (length > 0u))
            {
                uint32 channelNum = (uint32) USBP_DmaChan[epNumber];

                /* Disable DMA channel: start configuration. */
                USBP_CyDmaChDisable(channelNum);

                /* Configure source and destination. */
                USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR0, (void*) pData);
                USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR0, (void*) &USBP_ARB_EP16_BASE.arbEp[epNumber].rwDr16);

                /* Configure DMA descriptor. */
                length = (length >> 1u) - 1u;
                USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR0, USBP_DMA_COMMON_CFG | length |
                                                        CYDMA_HALFWORD | CYDMA_ELEMENT_WORD | CYDMA_INC_SRC_ADDR | CYDMA_INVALIDATE | CYDMA_PREEMPTABLE);

                /* Validate descriptor to execute on following DMA request. */
                USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR0);

                /* Enable DMA channel: configuration complete. */
                USBP_CyDmaChEnable(channelNum);

                /* Generate DMA request. */
                USBP_ARB_EP_BASE.arbEp[epNumber].epCfg |=  (uint32)  USBP_ARB_EPX_CFG_DMA_REQ;
                USBP_ARB_EP_BASE.arbEp[epNumber].epCfg &=  (uint32) ~USBP_ARB_EPX_CFG_DMA_REQ;

                /* IN endpoint will be armed in ARB_ISR(source: IN_BUF_FULL) after first DMA transfer has been completed. */
            }
            else
            {
                /* When zero-length packet: arm IN endpoint directly. */
                USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBP_EP[epNumber].epMode;
            }
        #endif /* (USBP_EP_MANAGEMENT_DMA_MANUAL) */

        #if (USBP_EP_MANAGEMENT_DMA_AUTO)
            if (pData != NULL)
            {
                uint32 channelNum = (uint32) USBP_DmaChan[epNumber];

                /* Store address of buffer. */
                USBP_DmaEpBufferAddrBackup[epNumber] = (uint32) pData;

                /* Disable DMA channel: start configuration. */
                USBP_CyDmaChDisable(channelNum);

                /* Set destination address. */
                USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR0, (void*) &USBP_ARB_EP16_BASE.arbEp[epNumber].rwDr16);
                USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR1, (void*) &USBP_ARB_EP16_BASE.arbEp[epNumber].rwDr16);

                /* Configure DMA descriptor. */
                USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR0, USBP_DMA_COMMON_CFG  |
                                                        CYDMA_HALFWORD | CYDMA_ELEMENT_WORD | CYDMA_INC_SRC_ADDR | CYDMA_INVALIDATE | CYDMA_CHAIN);

                /* Configure DMA descriptor. */
                USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR1, USBP_DMA_COMMON_CFG  |
                                                        CYDMA_HALFWORD | CYDMA_ELEMENT_WORD | CYDMA_INC_SRC_ADDR | CYDMA_INVALIDATE | CYDMA_CHAIN);

                /* Enable interrupt from DMA channel. */
                USBP_CyDmaSetInterruptMask(channelNum);

                /* Enable DMA channel: configuration complete. */
                USBP_CyDmaChEnable(channelNum);
            }
            else
            {
                /* IN endpoint buffer (32 bytes) will shortly be preloaded by DMA. */
                USBP_EP[epNumber].apiEpState = USBP_NO_EVENT_PENDING;

                if (length > 0u)
                {
                    uint32 lengthDescr0, lengthDescr1;
                    uint32 channelNum = (uint32) USBP_DmaChan[epNumber];

                    /* Get number of full bursts. */
                    USBP_DmaEpBurstCnt[epNumber] = (uint8) (length / USBP_DMA_BYTES_PER_BURST);

                    /* Get number of elements in the last burst. */
                    USBP_DmaEpLastBurstEl[epNumber] = (uint8) (length % USBP_DMA_BYTES_PER_BURST);

                    /* Get total number of bursts. */
                    USBP_DmaEpBurstCnt[epNumber] += (0u != USBP_DmaEpLastBurstEl[epNumber]) ? 1u : 0u;

                    /* Adjust number of data elements transferred in last burst. */
                    USBP_DmaEpLastBurstEl[epNumber] = (0u != USBP_DmaEpLastBurstEl[epNumber]) ?
                                                                          ((USBP_DmaEpLastBurstEl[epNumber] >> 1u) - 1u) :
                                                                           (USBP_DMA_HALFWORDS_PER_BURST - 1u);

                    /* Get number of data elements to transfer for descriptor 0 and 1. */
                    lengthDescr0 = (1u == USBP_DmaEpBurstCnt[epNumber]) ? USBP_DmaEpLastBurstEl[epNumber] : (USBP_DMA_HALFWORDS_PER_BURST - 1u);
                    lengthDescr1 = (2u == USBP_DmaEpBurstCnt[epNumber]) ? USBP_DmaEpLastBurstEl[epNumber] : (USBP_DMA_HALFWORDS_PER_BURST - 1u);

                    /* Mark which descriptor is last one to execute. */
                    USBP_DmaEpLastBurstEl[epNumber] |= (0u != (USBP_DmaEpBurstCnt[epNumber] & 0x1u)) ?
                                                                            USBP_DMA_DESCR0_MASK : USBP_DMA_DESCR1_MASK;

                    /* Restore DMA settings for current transfer. */
                    USBP_CyDmaChDisable(channelNum);

                    /* Restore destination address for input endpoint. */
                    USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR0, (void*) ((uint32) USBP_DmaEpBufferAddrBackup[epNumber]));
                    USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR1, (void*) ((uint32) USBP_DmaEpBufferAddrBackup[epNumber] +
                                                                                                                   USBP_DMA_BYTES_PER_BURST));

                    /* Set number of elements to transfer. */
                    USBP_CyDmaSetNumDataElements(channelNum, USBP_DMA_DESCR0, lengthDescr0);
                    USBP_CyDmaSetNumDataElements(channelNum, USBP_DMA_DESCR1, lengthDescr1);

                    /* Validate descriptor 0 and command to start with it. */
                    USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR0);
                    USBP_CyDmaSetDescriptor0Next(channelNum);

                    /* Validate descriptor 1. */
                    if (USBP_DmaEpBurstCnt[epNumber] > 1u)
                    {
                        USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR1);
                    }

                    /* Adjust burst counter taking to account: 2 valid descriptors and interrupt trigger after valid descriptor were executed. */
                    USBP_DmaEpBurstCnt[epNumber] = USBP_DMA_GET_BURST_CNT(USBP_DmaEpBurstCnt[epNumber]);

                    /* Enable DMA channel: configuration complete. */
                    USBP_CyDmaChEnable(channelNum);

                #if !defined (USBP_MANUAL_IN_EP_ARM)
                    /* Set IN data ready to generate DMA request to load data into endpoint buffer. */
                    USBP_ARB_EP_BASE.arbEp[epNumber].epCfg |= USBP_ARB_EPX_CFG_IN_DATA_RDY;
                #endif  /* (USBP_MANUAL_IN_EP_ARM) */

                    /* IN endpoint will be armed in ARB_ISR(source: IN_BUF_FULL) after first DMA transfer has been completed. */
                }
                else
                {
                    /* When zero-length packet: arm IN endpoint directly. */
                    USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBP_EP[epNumber].epMode;
                }
            }
        #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
    #endif /* (USBP_EP_MANAGEMENT_MANUAL) */
    }
}


/*******************************************************************************
* Function Name: USBP_ReadOutEP16
****************************************************************************//**
*
*   This function performs different functionality depending on the Component’s
*   configured Endpoint Buffer Management. This parameter is defined in the
*   Descriptor Root in Component Configure window.
*
*   *Manual (Static/Dynamic Allocation):*
*   This function moves the specified number of bytes from endpoint buffer to
*   system RAM. The number of bytes actually transferred from endpoint buffer to
*   system RAM is the lesser of the actual number of bytes sent by the host or
*   the number of bytes requested by the length parameter.
*
*   *DMA with Manual Buffer Management:*
*   Configure DMA to transfer data from endpoint buffer to system RAM. Generate
*   a DMA request. The firmware must wait until the DMA completes the data
*   transfer after calling the USBP_ReadOutEP() API. For example,
*   by checking EPstate:
*
*   \snippet /USBFS_sut_02.cydsn/main.c checking EPstatey
*
*   The USBFS_EnableOutEP() has to be called to allow host to write data into
*   the endpoint buffer after DMA has completed transfer data from OUT endpoint
*   buffer to SRAM.
*
*   *DMA with Automatic Buffer Management:*
*   Configure DMA. This is required only once and automatically generates DMA
*   requests as data arrives
*
*  \param epNumber: Contains the data endpoint number.
*         Valid values are between 1 and 8.
*  \param pData: A pointer to a data array into which the data for the endpoint
*         space is copied. It shall be ensured that this pointer address is
*         even to ensure the 16-bit transfer is aligned to even address. Else,
*         a hard fault condition can occur.
*  \param length: The number of bytes to transfer from the USB Out endpoint and
*         loads it into data array. Valid values are between 0 and 1023. The
*         function moves fewer than the requested number of bytes if the host
*         sends fewer bytes than requested.
*
* \return
*  Number of bytes received, 0 for an invalid endpoint.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint16 USBP_ReadOutEP16(uint8 epNumber, uint8 pData[], uint16 length)
{
    uint32 adjLength;

    /* Check array alignment on half-word boundary */
    CYASSERT(0u == (((uint32) pData) & 0x01u));

    if ((pData != NULL) && (epNumber > USBP_EP0) && (epNumber < USBP_MAX_EP))
    {
    #if (!USBP_EP_MANAGEMENT_DMA_AUTO)
        /* Adjust requested length to available data. */
        length = (length > USBP_GetEPCount(epNumber)) ? USBP_GetEPCount(epNumber) : length;
    #endif /* (!USBP_EP_MANAGEMENT_DMA_AUTO) */

    /* Adjust requested length: 2 bytes are handled at one data register access. */
    adjLength =  length + ((uint32)length & 1u);

    #if (USBP_EP_MANAGEMENT_MANUAL)
        {
            /* Convert uint8 array to uint16. */
            uint16 *dataBuf = (uint16 *) pData;

            /* Copy data using 16-bits arbiter data register. */
            uint16 i;
            for (i = 0u; i < (adjLength >> 1u); ++i)
            {
                dataBuf[i] = (uint16) USBP_ARB_EP16_BASE.arbEp[epNumber].rwDr16;
            }
        }

        /* Arm OUT endpoint after data has been read from endpoint buffer. */
        USBP_EnableOutEP(epNumber);
    #else

        #if (USBP_EP_MANAGEMENT_DMA_MANUAL)
        {
            uint32 channelNum = (uint32) USBP_DmaChan[epNumber];

            /* Disable DMA channel: start configuration. */
            USBP_CyDmaChDisable(channelNum);

            /* Configure source and destination. */
            USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR0, (void*) &USBP_ARB_EP16_BASE.arbEp[epNumber].rwDr16);
            USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR0, (void*) pData);

            /* Configure DMA descriptor. */
            USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR0, USBP_DMA_COMMON_CFG | (uint16)((adjLength >> 1u) - 1u) |
                                                    CYDMA_HALFWORD | CYDMA_WORD_ELEMENT | CYDMA_INC_DST_ADDR | CYDMA_INVALIDATE | CYDMA_PREEMPTABLE);

            /* Validate descriptor to execute on following DMA request. */
            USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR0);

            /* Enable DMA channel: configuration complete. */
            USBP_CyDmaChEnable(channelNum);

            /* Generate DMA request. */
            USBP_ARB_EP_BASE.arbEp[epNumber].epCfg |=  (uint32)  USBP_ARB_EPX_CFG_DMA_REQ;
            USBP_ARB_EP_BASE.arbEp[epNumber].epCfg &=  (uint32) ~USBP_ARB_EPX_CFG_DMA_REQ;

            /* OUT endpoint has to be armed again by user when DMA transfers have been completed.
            * NO_EVENT_PENDING: notifies that data has been copied from endpoint buffer.
            */
        }
        #endif /* (USBP_EP_MANAGEMENT_DMA_MANUAL) */

        #if (USBP_EP_MANAGEMENT_DMA_AUTO)
        {
            uint32 lengthDescr0, lengthDescr1;
            uint32 channelNum = (uint32) USBP_DmaChan[epNumber];

            /* Get number of full bursts. */
            USBP_DmaEpBurstCnt[epNumber] = (uint8) (adjLength / USBP_DMA_BYTES_PER_BURST);

            /* Get number of elements in last burst. */
            USBP_DmaEpLastBurstEl[epNumber] = (uint8) (adjLength % USBP_DMA_BYTES_PER_BURST);

            /* Get total number of bursts. */
            USBP_DmaEpBurstCnt[epNumber] += (0u != USBP_DmaEpLastBurstEl[epNumber]) ? 1u : 0u;

            /* Adjust number of data elements transferred in last burst. */
            USBP_DmaEpLastBurstEl[epNumber] = (0u != USBP_DmaEpLastBurstEl[epNumber]) ?
                                                                  ((USBP_DmaEpLastBurstEl[epNumber] >> 1u) - 1u) :
                                                                   (USBP_DMA_HALFWORDS_PER_BURST - 1u);

            /* Get number of data elements to transfer for descriptor 0 and 1. */
            lengthDescr0 = (1u == USBP_DmaEpBurstCnt[epNumber]) ? USBP_DmaEpLastBurstEl[epNumber] : (USBP_DMA_HALFWORDS_PER_BURST - 1u);
            lengthDescr1 = (2u == USBP_DmaEpBurstCnt[epNumber]) ? USBP_DmaEpLastBurstEl[epNumber] : (USBP_DMA_HALFWORDS_PER_BURST - 1u);

            /* Mark last descriptor to be executed. */
            USBP_DmaEpLastBurstEl[epNumber] |= (0u != (USBP_DmaEpBurstCnt[epNumber] & 0x1u)) ?
                                                                    USBP_DMA_DESCR0_MASK : USBP_DMA_DESCR1_MASK;

            /* Mark if revert number of data elements in descriptor after transfer completion. */
            USBP_DmaEpLastBurstEl[epNumber] |= (USBP_DmaEpBurstCnt[epNumber] > 2u) ? USBP_DMA_DESCR_REVERT : 0u;

            /* Mark that 16-bits access to data register is performed. */
            USBP_DmaEpLastBurstEl[epNumber] |= USBP_DMA_DESCR_16BITS;

            /* Store address of buffer and burst counter for endpoint. */
            USBP_DmaEpBufferAddrBackup[epNumber] = (uint32) pData;
            USBP_DmaEpBurstCntBackup[epNumber]   = USBP_DmaEpBurstCnt[epNumber];

            /* Adjust burst counter taking to account: 2 valid descriptors and interrupt trigger after valid descriptor were executed. */
            USBP_DmaEpBurstCnt[epNumber] = USBP_DMA_GET_BURST_CNT(USBP_DmaEpBurstCnt[epNumber]);

            /* Disable DMA channel: start configuration. */
            USBP_CyDmaChDisable(channelNum);

            /* Set destination address. */
            USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR0, (void*) &USBP_ARB_EP16_BASE.arbEp[epNumber].rwDr16);
            USBP_CyDmaSetSrcAddress(channelNum, USBP_DMA_DESCR1, (void*) &USBP_ARB_EP16_BASE.arbEp[epNumber].rwDr16);

            USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR0, (void*) ((uint32) pData));
            USBP_CyDmaSetDstAddress(channelNum, USBP_DMA_DESCR1, (void*) ((uint32) pData + USBP_DMA_BYTES_PER_BURST));

            /* Configure DMA descriptor 0. */
            USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR0, USBP_DMA_COMMON_CFG  | lengthDescr0 |
                                                    CYDMA_HALFWORD | CYDMA_WORD_ELEMENT | CYDMA_INC_DST_ADDR | CYDMA_INVALIDATE | CYDMA_CHAIN);

            /* Configure DMA descriptor 1. */
            USBP_CyDmaSetConfiguration(channelNum, USBP_DMA_DESCR1, USBP_DMA_COMMON_CFG  | lengthDescr1 |
                                                    CYDMA_HALFWORD | CYDMA_WORD_ELEMENT | CYDMA_INC_DST_ADDR | CYDMA_INVALIDATE | CYDMA_CHAIN);

            /* Enable interrupt from DMA channel. */
            USBP_CyDmaSetInterruptMask(channelNum);

            /* Validate DMA descriptor 0 and 1. */
            USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR0);
            
            if (USBP_DmaEpBurstCntBackup[epNumber] > 1u)
            {
                USBP_CyDmaValidateDescriptor(channelNum, USBP_DMA_DESCR1);
            }

            /* Enable DMA channel: configuration complete. */
            USBP_CyDmaChEnable(channelNum);

            /* OUT endpoint has to be armed again by user when DMA transfers have been completed.
            * NO_EVENT_PENDING: notifies that data has been copied from endpoint buffer.
            */
        }
        #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
    #endif /* (USBP_EP_MANAGEMENT_MANUAL) */
    }
    else
    {
        length = 0u;
    }

    return (length);
}
#endif /* (USBP_16BITS_EP_ACCESS_ENABLE) */


/*******************************************************************************
* Function Name: USBP_EnableOutEP
****************************************************************************//**
*
*  This function enables the specified endpoint for OUT transfers. Do not call
*  this function for IN endpoints.
*
*  \param epNumber: Contains the data endpoint number. Valid values are between
*  1 and 8.
*
* \globalvars
*
*  \ref USBP_EP[epNumber].apiEpState - set to NO_EVENT_PENDING
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_EnableOutEP(uint8 epNumber) 
{
    if ((epNumber > USBP_EP0) && (epNumber < USBP_MAX_EP))
    {
        USBP_EP[epNumber].apiEpState = USBP_NO_EVENT_PENDING;

        /* Enable OUT endpoint to be written by Host. */
        USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBP_EP[epNumber].epMode;
        
    }
}


/*******************************************************************************
* Function Name: USBP_DisableOutEP
****************************************************************************//**
*
*  This function disables the specified USBFS OUT endpoint. Do not call this
*  function for IN endpoints.
*
*  \param epNumber: Contains the data endpoint number.
*            Valid values are between 1 and 8.
*
*******************************************************************************/
void USBP_DisableOutEP(uint8 epNumber) 
{
    if ((epNumber > USBP_EP0) && (epNumber < USBP_MAX_EP))
    {
        /* Set NAK response for OUT endpoint. */
        USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBP_MODE_NAK_OUT;
    }
}


/*******************************************************************************
* Function Name: USBP_Force
****************************************************************************//**
*
*  This function forces a USB J, K, or SE0 state on the D+/D– lines. It provides
*  the necessary mechanism for a USB device application to perform a USB Remote
*  Wakeup. For more information, see the USB 2.0 Specification for details on
*  Suspend and Resume.
*
*  \param state A byte indicating which of the four bus states to enable.
*        Symbolic names  and their associated values are listed here:
*    State                      |Description
*    ---------------------------|----------------------------------------------
*    USBP_FORCE_J   | Force a J State onto the D+/D– lines
*    USBP_FORCE_K   | Force a K State onto the D+/D– lines
*    USBP_FORCE_SE0 | Force a Single Ended 0 onto the D+/D– lines
*    USBP_FORCE_NONE| Return bus to SIE control
*
*
*******************************************************************************/
void USBP_Force(uint8 bState) 
{
    /* This registers is used only for manual control of SIE (no masking is
    * needed before write into it).
    */
    USBP_USBIO_CR0_REG = bState;
}


/*******************************************************************************
* Function Name: USBP_GetEPAckState
****************************************************************************//**
*
*  This function determines whether an ACK transaction occurred on this endpoint
*  by reading the ACK bit in the control register of the endpoint. It does not
*  clear the ACK bit.
*
*  \param epNumber Contains the data endpoint number.
*            Valid values are between 1 and 8.
*
*  \return
*  If an ACKed transaction occurred, this function returns a non-zero value.
*  Otherwise, it returns zero.
*
*******************************************************************************/
uint8 USBP_GetEPAckState(uint8 epNumber) 
{
    uint8 cr = 0u;

    if ((epNumber > USBP_EP0) && (epNumber < USBP_MAX_EP))
    {
        cr = USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 & USBP_MODE_ACKD;
    }

    return ((uint8) cr);
}


/*******************************************************************************
* Function Name: USBP_SetPowerStatus
****************************************************************************//**
*
*  This function sets the current power status. The device replies to USB
*  GET_STATUS requests based on this value. This allows the device to properly
*  report its status for USB Chapter 9 compliance. Devices can change their
*  power source from self powered to bus powered at any time and report their
*  current power source as part of the device status. You should call this
*  function any time your device changes from self powered to bus powered or
*  vice versa, and set the status appropriately.
*
*  \param powerStatus: Contains the desired power status, one for self powered
*        or zero for bus powered. Symbolic names and their associated values are
*        given here:
*  Power Status                                |Description
*  --------------------------------------------|---------------------------
*  USBP_DEVICE_STATUS_BUS_POWERED  | Set the device to bus powered
*  USBP_DEVICE_STATUS_SELF_POWERED | Set the device to self powered
*
* \globalvars
*
*  \ref USBP_deviceStatus - set power status
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_SetPowerStatus(uint8 powerStatus) 
{
    if (powerStatus != USBP_DEVICE_STATUS_BUS_POWERED)
    {
        USBP_deviceStatus |= (uint8)  USBP_DEVICE_STATUS_SELF_POWERED;
    }
    else
    {
        USBP_deviceStatus &= (uint8) ~USBP_DEVICE_STATUS_SELF_POWERED;
    }
}


#if (USBP_VBUS_MONITORING_ENABLE)
    /***************************************************************************
    * Function Name: USBP_VBusPresent
    ************************************************************************//**
    *
    *  Determines VBUS presence for self-powered devices. This function is
    *  available when the VBUS Monitoring option is enabled in the Advanced tab.
    *
    * \return
    *  The return value can be the following:
    *  Return Value | Description
    *  -------------|-----------------
    *  1            | VBUS is present
    *  0            | VBUS is absent
    *
    *
    ***************************************************************************/
    uint8 USBP_VBusPresent(void) 
    {
        return ((0u != (USBP_VBUS_STATUS_REG & USBP_VBUS_VALID)) ? (uint8) 1u : (uint8) 0u);
    }
#endif /* (USBP_VBUS_MONITORING_ENABLE) */


/*******************************************************************************
* Function Name: USBP_RWUEnabled
****************************************************************************//**
*
*  This function returns the current remote wakeup status.
*  If the device supports remote wakeup, the application should use this
*  function to determine if remote wakeup was enabled by the host. When the
*  device is suspended and it determines the conditions to initiate a remote
*  wakeup are met, the application should use the USBFS_Force() function to
*  force the appropriate J and K states onto the USB bus, signaling a remote
*  wakeup.
*
*
* \return
*  Returns non-zero value if remote wakeup is enabled and zero otherwise.
*
* \globalvars
*  USBP_deviceStatus - checked to determine remote status
*
*******************************************************************************/
uint8 USBP_RWUEnabled(void) 
{
    uint8 result = USBP_FALSE;

    if (0u != (USBP_deviceStatus & USBP_DEVICE_STATUS_REMOTE_WAKEUP))
    {
        result = USBP_TRUE;
    }

    return (result);
}


/*******************************************************************************
* Function Name: USBP_GetDeviceAddress
****************************************************************************//**
*
*  This function returns the currently assigned address for the USB device.
*
* \return
*  Returns the currently assigned address.
*  Returns 0 if the device has not yet been assigned an address.
*
*******************************************************************************/
uint8 USBP_GetDeviceAddress(void) 
{
    return (uint8)(USBP_CR0_REG & USBP_CR0_DEVICE_ADDRESS_MASK);
}


/*******************************************************************************
* Function Name: USBP_EnableSofInt
****************************************************************************//**
*
*  This function enables interrupt generation when a Start-of-Frame (SOF)
*  packet is received from the host.
*
*******************************************************************************/
void USBP_EnableSofInt(void) 
{
#if (CY_PSOC4)
    /* Enable SOF interrupt interrupt source. */
    USBP_INTR_SIE_MASK_REG |= (uint32) USBP_INTR_SIE_SOF_INTR;
#else
    /* Enable SOF interrupt if it is present. */
    #if (USBP_SOF_ISR_ACTIVE)
        CyIntEnable(USBP_SOF_VECT_NUM);
    #endif /* (USBP_SOF_ISR_ACTIVE) */
#endif /* (CY_PSOC4) */
}


/*******************************************************************************
* Function Name: USBP_DisableSofInt
****************************************************************************//**
*
*  This function disables interrupt generation when a Start-of-Frame (SOF)
*  packet is received from the host.
*
*******************************************************************************/
void USBP_DisableSofInt(void) 
{
#if (CY_PSOC4)
    /* Disable SOF interrupt interrupt source. */
    USBP_INTR_SIE_MASK_REG &= (uint32) ~USBP_INTR_SIE_SOF_INTR;
#else
    /* Disable SOF interrupt if it is present. */
    #if (USBP_SOF_ISR_ACTIVE)
        CyIntDisable(USBP_SOF_VECT_NUM);
    #endif /* (USBP_SOF_ISR_ACTIVE) */
#endif /* (CY_PSOC4) */
}


#if (USBP_BATT_CHARG_DET_ENABLE)
    /***************************************************************************
    * Function Name: USBP_DetectPortType
    ************************************************************************//**
    *
    *   This function implements the USB  Battery Charger Detection (BCD)
    *   algorithm to determine the type of USB host downstream port. This API
    *   is available only for PSoC 4 devices, and should be called when the VBUS
    *   voltage transition (OFF to ON) is detected on the bus. If the USB device
    *   functionality is enabled, this API first calls USBFS_Stop() API
    *   internally to disable the USB device functionality, and then proceeds to
    *   implement the BCD algorithm to detect the USB host port type.
    *   The USBFS_Start() API should be called after this API if the USB
    *   communication needs to be initiated with the host.
    *   *Note* This API is generated only if the “Enable Battery Charging 
    *   Detection” option is enabled in the “Advanced” tab of the component GUI.
    *   *Note* API implements the steps 2-4 of the BCD algorithm which are 
    *   - Data Contact Detect
    *   - Primary Detection 
    *   - Secondary Detection 
    * 
    *   The first step of BCD algorithm, namely, VBUS detection shall be handled 
    *   at the application firmware level.
    *
    * \return
    *   The return value can be the following:
    *   Return Value                      |Description
    *   ----------------------------------|-------------------------------------
    *   USBP_BCD_PORT_SDP     | Standard downstream port detected
    *   USBP_BCD_PORT_CDP     | Charging downstream port detected
    *   USBP_BCD_PORT_DCP     | Dedicated charging port detected
    *   USBP_BCD_PORT_UNKNOWN | Unable to detect charging port type (proprietary charger type)
    *   USBP_BCD_PORT_ERR     | Error condition in detection process
    *
    *
    * \sideeffects
    *
    *  USB device functionality is disabled by this API if not already disabled.
    *
    ***************************************************************************/
    uint8 USBP_Bcd_DetectPortType(void)
    {
        uint32 bkPwrCtrl;
        uint32 cr1RegVal;
        uint32 secondaryDetection = 0u;
        uint8 result = USBP_BCD_PORT_UNKNOWN;

        /*Check USB Started and Stop it*/
        if(0u != USBP_initVar)
        {
            USBP_Stop();
        }
        /*Initialize USBFS IP for Charger detection*/

        /*Enable clock to USB IP. */
        USBP_USB_CLK_EN_REG = USBP_USB_CLK_CSR_CLK_EN;

        /* Enable USBIO control on drive mode of D+ and D- pins. */
        USBP_USBIO_CR1_REG &= ~ (uint32) USBP_USBIO_CR1_IOMODE;

        /* Select VBUS detection source and clear PHY isolate. The application
        *  level must ensure that VBUS is valid. There is no need to wait 2us
        *  before VBUS is valid.
        */
        bkPwrCtrl = USBP_POWER_CTRL_REG;
        USBP_POWER_CTRL_REG = USBP_DEFAULT_POWER_CTRL_VBUS\
                            & (~USBP_POWER_CTRL_ENABLE_VBUS_PULLDOWN)\
                            & (~USBP_POWER_CTRL_ENABLE_DM_PULLDOWN);


        /* Enable PHY detector and single-ended and differential receivers.
         * Enable charger detection.  */
        USBP_POWER_CTRL_REG |= USBP_DEFAULT_POWER_CTRL_PHY\
                                         | USBP_POWER_CTRL_ENABLE_CHGDET;

        /* Suspend clear sequence. */
        USBP_POWER_CTRL_REG &= (uint32) ~USBP_POWER_CTRL_SUSPEND;
        CyDelayUs(USBP_WAIT_SUSPEND_DEL_DISABLE);
        USBP_POWER_CTRL_REG &= (uint32) ~USBP_POWER_CTRL_SUSPEND_DEL;

        /* Data connection detection
        * Realization with delay as Hard IP does not support DCD 300 ms.
        */
        #if defined (USBP_NO_DCD)
        CyDelay(USBP_BCD_TIMEOUT);
        #else
        /* DCD implementation:*/

        {
            uint16 timeout = USBP_BCD_TIMEOUT;
            uint8 connectionApproved = 0u;
            uint8 connected = 0u;

            /*   BCD spec 1.2: Turns on Idp_src and D- pull-down resistor */
            USBP_POWER_CTRL_REG |= USBP_POWER_CTRL_ENABLE_DM_PULLDOWN;
            USBP_CHGDET_CTRL_REG |= USBP_CHGDET_CTRL_DCD_SRC_EN;

            /* BCD spec 1.2: Waits for D+ to be low for a time of Tdcd_dbnc*/
            while ((0u != timeout) && (0u == connectionApproved))
            {
                if (0u == (USBP_USBIO_CR1_REG & USBP_USBIO_CR1_DP0))
                {
                    connected++;
                }
                else
                {
                    connected = 0u;
                }
                connectionApproved = (USBP_BCD_TDCD_DBNC < connected) ? 1u:0u;
                CyDelay(1u);
                timeout--;
            }

            /*   BCD spec 1.2: Turns off Idp_src. */
            USBP_CHGDET_CTRL_REG &= ~USBP_CHGDET_CTRL_DCD_SRC_EN;
        }
        #endif /*(USBP_NO_DCD)*/

        /* Primary detection: enable VDP_SRC on D+ and IDM_SINK on D-. */
        USBP_CHGDET_CTRL_REG = USBP_CHGDET_CTRL_PRIMARY;
        CyDelay(USBP_BCD_PRIMARY_WAIT);
        cr1RegVal = USBP_USBIO_CR1_REG;

        /* Check is it SDP or DCP/CDP, read comparator 2 output. */
        if (0u == (USBP_CHGDET_CTRL_REG & USBP_CHGDET_CTRL_COMP_OUT))
        {
            /* Check status of D- line. */
            if (0u == (cr1RegVal & USBP_USBIO_CR1_DM0))
            {
                result = USBP_BCD_PORT_SDP;
            }
            else
            {
                /* ERROR: such combination is impossible. Abort charger
                 * detection.
                */
                result = USBP_BCD_PORT_ERR;
            }
        }
        else
        {
                /* Need Secondary detection. Charging port: DCP or proprietary*/
                secondaryDetection = 1u;
        }

        /* Secondary detection: Set CHGDET_CTRL register to enable VDM_SRC on D- and IDP_SINK on D+. */

        if (0u != secondaryDetection)
        {
            USBP_CHGDET_CTRL_REG = USBP_CHGDET_CTRL_DEFAULT \
                                                | USBP_CHGDET_CTRL_SECONDARY;
            CyDelay(USBP_BCD_SECONDARY_WAIT);
            cr1RegVal = USBP_USBIO_CR1_REG;

            /* Check is it SDP or DCP/CDP, read comparator 1 output. */
            if (0u == (USBP_CHGDET_CTRL_REG & USBP_CHGDET_CTRL_COMP_OUT))
            {
                /* Check status of D+ line. */
                if (0u == (cr1RegVal & USBP_USBIO_CR1_DP0))
                {
                    result = USBP_BCD_PORT_CDP;
                }
                else
                {
                    /* ERROR: such combination is impossible. Abort charger
                     * detection.
                    */
                    result = USBP_BCD_PORT_ERR;
                }
            }
            else
            {
                /* Check status of D+ line. */
                if (0u == (cr1RegVal & USBP_USBIO_CR1_DP0))
                {
                    result = USBP_BCD_PORT_DCP;
                }
                else
                {
                    /* It is may be proprietary charger. Proprietary charge is
                     * not supported byHardware IP block.
                    */
                    result = USBP_BCD_PORT_UNKNOWN;
                }
            }
        }

        /* Restore CHGDET_CTRL. */
        USBP_CHGDET_CTRL_REG = 0u;

        /*Revert registers back*/
        USBP_POWER_CTRL_REG = bkPwrCtrl;
        USBP_USBIO_CR1_REG |= (uint32) USBP_USBIO_CR1_IOMODE;
        USBP_USB_CLK_EN_REG = ~USBP_USB_CLK_CSR_CLK_EN;

        return (result);
    }
#endif  /* (USBP_BATT_CHARG_DET_ENABLE) */


#if (USBP_LPM_ACTIVE)
    /***************************************************************************
    * Function Name: USBP_Lpm_GetBeslValue
    ************************************************************************//**
    *
    *  This function returns the Best Effort Service Latency (BESL) value
    *  sent by the host as part of the LPM token transaction.
    *
    * \return
    *  4-bit BESL value received in the LPM token packet from the host
    *
    *
    ***************************************************************************/
    uint32 USBP_Lpm_GetBeslValue(void)
    {
        return (uint32) (USBP_LPM_STAT_REG & USBP_LPM_STAT_LPM_BESL_MASK);
    }


    /***************************************************************************
    * Function Name: USBP_Lpm_RemoteWakeUpAllowed
    ************************************************************************//**
    *
    *  This function returns the remote wakeup permission set for the device by
    *  the host as part of the LPM token transaction.
    *
    * \return
    *   0 - remote wakeup not allowed, 1 - remote wakeup allowed
    *
    *
    ***************************************************************************/
    uint32 USBP_Lpm_RemoteWakeUpAllowed(void)
    {
        return (uint32) (USBP_LPM_STAT_REG & USBP_LPM_STAT_LPM_REMOTE_WAKE);
    }


    /***************************************************************************
    * Function Name: USBP_Lpm_SetResponse
    ************************************************************************//**
    *
    *  This function configures the response in the handshake packet the device
    *  has to send when an LPM token packet is received.
    *
    * \param response
    *   type of response to return for an LPM token packet
    *   Allowed response values:
    *       - USBP_LPM_REQ_ACK - next LPM request will be
    *                                           responded with ACK
    *       - USBP_LPM_REQ_NACK - next LPM request will be
    *                                           responded with NACK
    *       - USBP_LPM_REQ_NYET - next LPM request will be
    *                                           responded with NYET
    *
    ***************************************************************************/
    void USBP_Lpm_SetResponse(uint32 response)
    {
        uint32 lpmCtrl = USBP_LPM_CTRL_REG & (uint32) ~USBP_LPM_CTRL_ACK_NYET_MASK;

        USBP_LPM_CTRL_REG = lpmCtrl | ((uint32) response & USBP_LPM_CTRL_ACK_NYET_MASK);
    }


    /***************************************************************************
    * Function Name: USBP_Lpm_GetResponse
    ************************************************************************//**
    *
    *  This function returns the currently configured response value that the
    *  device will send as part of the handshake packet when an LPM token
    *  packet is received.
    *
    * \return
    *   type of handshake response that will be returned by the device
    *   for an LPM token packet
    *   Possible response values:
    *       - USBP_LPM_REQ_ACK - next LPM request will be responded
    *                                        with ACK
    *       - USBP_LPM_REQ_NACK - next LPM request will be responded
    *                                        with NACK
    *       - USBP_LPM_REQ_NYET - next LPM request will be responded
    *                                        with NYET
    *
    ***************************************************************************/
    uint32 USBP_Lpm_GetResponse(void)
    {

        return  ((uint32) USBP_LPM_CTRL_REG & (uint32)USBP_LPM_CTRL_ACK_NYET_MASK);
    }


#endif /* (USBP_LPM_ACTIVE) */


/* [] END OF FILE */
