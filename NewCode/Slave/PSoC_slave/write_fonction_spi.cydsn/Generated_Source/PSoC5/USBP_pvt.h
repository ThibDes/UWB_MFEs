/***************************************************************************//**
* \file .h
* \version 3.20
*
* \brief
*  This file provides private function prototypes and constants for the 
*  USBFS component. It is not intended to be used in the user project.
*
********************************************************************************
* \copyright
* Copyright 2013-2016, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USBP_pvt_H)
#define CY_USBFS_USBP_pvt_H

#include "USBP.h"
   
#ifdef USBP_ENABLE_AUDIO_CLASS
    #include "USBP_audio.h"
#endif /* USBP_ENABLE_AUDIO_CLASS */

#ifdef USBP_ENABLE_CDC_CLASS
    #include "USBP_cdc.h"
#endif /* USBP_ENABLE_CDC_CLASS */

#if (USBP_ENABLE_MIDI_CLASS)
    #include "USBP_midi.h"
#endif /* (USBP_ENABLE_MIDI_CLASS) */

#if (USBP_ENABLE_MSC_CLASS)
    #include "USBP_msc.h"
#endif /* (USBP_ENABLE_MSC_CLASS) */

#if (USBP_EP_MANAGEMENT_DMA)
    #if (CY_PSOC4)
        #include <CyDMA.h>
    #else
        #include <CyDmac.h>
        #if ((USBP_EP_MANAGEMENT_DMA_AUTO) && (USBP_EP_DMA_AUTO_OPT == 0u))
            #include "USBP_EP_DMA_Done_isr.h"
            #include "USBP_EP8_DMA_Done_SR.h"
            #include "USBP_EP17_DMA_Done_SR.h"
        #endif /* ((USBP_EP_MANAGEMENT_DMA_AUTO) && (USBP_EP_DMA_AUTO_OPT == 0u)) */
    #endif /* (CY_PSOC4) */
#endif /* (USBP_EP_MANAGEMENT_DMA) */

#if (USBP_DMA1_ACTIVE)
    #include "USBP_ep1_dma.h"
    #define USBP_EP1_DMA_CH     (USBP_ep1_dma_CHANNEL)
#endif /* (USBP_DMA1_ACTIVE) */

#if (USBP_DMA2_ACTIVE)
    #include "USBP_ep2_dma.h"
    #define USBP_EP2_DMA_CH     (USBP_ep2_dma_CHANNEL)
#endif /* (USBP_DMA2_ACTIVE) */

#if (USBP_DMA3_ACTIVE)
    #include "USBP_ep3_dma.h"
    #define USBP_EP3_DMA_CH     (USBP_ep3_dma_CHANNEL)
#endif /* (USBP_DMA3_ACTIVE) */

#if (USBP_DMA4_ACTIVE)
    #include "USBP_ep4_dma.h"
    #define USBP_EP4_DMA_CH     (USBP_ep4_dma_CHANNEL)
#endif /* (USBP_DMA4_ACTIVE) */

#if (USBP_DMA5_ACTIVE)
    #include "USBP_ep5_dma.h"
    #define USBP_EP5_DMA_CH     (USBP_ep5_dma_CHANNEL)
#endif /* (USBP_DMA5_ACTIVE) */

#if (USBP_DMA6_ACTIVE)
    #include "USBP_ep6_dma.h"
    #define USBP_EP6_DMA_CH     (USBP_ep6_dma_CHANNEL)
#endif /* (USBP_DMA6_ACTIVE) */

#if (USBP_DMA7_ACTIVE)
    #include "USBP_ep7_dma.h"
    #define USBP_EP7_DMA_CH     (USBP_ep7_dma_CHANNEL)
#endif /* (USBP_DMA7_ACTIVE) */

#if (USBP_DMA8_ACTIVE)
    #include "USBP_ep8_dma.h"
    #define USBP_EP8_DMA_CH     (USBP_ep8_dma_CHANNEL)
#endif /* (USBP_DMA8_ACTIVE) */


/***************************************
*     Private Variables
***************************************/

/* Generated external references for descriptors. */
extern const uint8 CYCODE USBP_DEVICE0_DESCR[18u];
extern const uint8 CYCODE USBP_DEVICE0_CONFIGURATION0_DESCR[32u];
extern const T_USBP_EP_SETTINGS_BLOCK CYCODE USBP_DEVICE0_CONFIGURATION0_EP_SETTINGS_TABLE[2u];
extern const uint8 CYCODE USBP_DEVICE0_CONFIGURATION0_INTERFACE_CLASS[1u];
extern const T_USBP_LUT CYCODE USBP_DEVICE0_CONFIGURATION0_TABLE[4u];
extern const T_USBP_LUT CYCODE USBP_DEVICE0_TABLE[3u];
extern const T_USBP_LUT CYCODE USBP_TABLE[1u];
extern const uint8 CYCODE USBP_STRING_DESCRIPTORS[59u];


extern const uint8 CYCODE USBP_MSOS_DESCRIPTOR[USBP_MSOS_DESCRIPTOR_LENGTH];
extern const uint8 CYCODE USBP_MSOS_CONFIGURATION_DESCR[USBP_MSOS_CONF_DESCR_LENGTH];
#if defined(USBP_ENABLE_IDSN_STRING)
    extern uint8 USBP_idSerialNumberStringDescriptor[USBP_IDSN_DESCR_LENGTH];
#endif /* (USBP_ENABLE_IDSN_STRING) */

extern volatile uint8 USBP_interfaceNumber;
extern volatile uint8 USBP_interfaceSetting[USBP_MAX_INTERFACES_NUMBER];
extern volatile uint8 USBP_interfaceSettingLast[USBP_MAX_INTERFACES_NUMBER];
extern volatile uint8 USBP_deviceAddress;
extern volatile uint8 USBP_interfaceStatus[USBP_MAX_INTERFACES_NUMBER];
extern const uint8 CYCODE *USBP_interfaceClass;

extern volatile T_USBP_EP_CTL_BLOCK USBP_EP[USBP_MAX_EP];
extern volatile T_USBP_TD USBP_currentTD;

#if (USBP_EP_MANAGEMENT_DMA)
    #if (CY_PSOC4)
        extern const uint8 USBP_DmaChan[USBP_MAX_EP];
    #else
        extern uint8 USBP_DmaChan[USBP_MAX_EP];
        extern uint8 USBP_DmaTd  [USBP_MAX_EP];
    #endif /* (CY_PSOC4) */
#endif /* (USBP_EP_MANAGEMENT_DMA) */

#if (USBP_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)
    extern uint8  USBP_DmaEpBurstCnt   [USBP_MAX_EP];
    extern uint8  USBP_DmaEpLastBurstEl[USBP_MAX_EP];

    extern uint8  USBP_DmaEpBurstCntBackup  [USBP_MAX_EP];
    extern uint32 USBP_DmaEpBufferAddrBackup[USBP_MAX_EP];
    
    extern const uint8 USBP_DmaReqOut     [USBP_MAX_EP];    
    extern const uint8 USBP_DmaBurstEndOut[USBP_MAX_EP];
#else
    #if (USBP_EP_DMA_AUTO_OPT == 0u)
        extern uint8 USBP_DmaNextTd[USBP_MAX_EP];
        extern volatile uint16 USBP_inLength [USBP_MAX_EP];
        extern volatile uint8  USBP_inBufFull[USBP_MAX_EP];
        extern const uint8 USBP_epX_TD_TERMOUT_EN[USBP_MAX_EP];
        extern const uint8 *USBP_inDataPointer[USBP_MAX_EP];
    #endif /* (USBP_EP_DMA_AUTO_OPT == 0u) */
#endif /* CY_PSOC4 */
#endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */

extern volatile uint8 USBP_ep0Toggle;
extern volatile uint8 USBP_lastPacketSize;
extern volatile uint8 USBP_ep0Mode;
extern volatile uint8 USBP_ep0Count;
extern volatile uint16 USBP_transferByteCount;


/***************************************
*     Private Function Prototypes
***************************************/
void  USBP_ReInitComponent(void)            ;
void  USBP_HandleSetup(void)                ;
void  USBP_HandleIN(void)                   ;
void  USBP_HandleOUT(void)                  ;
void  USBP_LoadEP0(void)                    ;
uint8 USBP_InitControlRead(void)            ;
uint8 USBP_InitControlWrite(void)           ;
void  USBP_ControlReadDataStage(void)       ;
void  USBP_ControlReadStatusStage(void)     ;
void  USBP_ControlReadPrematureStatus(void) ;
uint8 USBP_InitControlWrite(void)           ;
uint8 USBP_InitZeroLengthControlTransfer(void) ;
void  USBP_ControlWriteDataStage(void)      ;
void  USBP_ControlWriteStatusStage(void)    ;
void  USBP_ControlWritePrematureStatus(void);
uint8 USBP_InitNoDataControlTransfer(void)  ;
void  USBP_NoDataControlStatusStage(void)   ;
void  USBP_InitializeStatusBlock(void)      ;
void  USBP_UpdateStatusBlock(uint8 completionCode) ;
uint8 USBP_DispatchClassRqst(void)          ;

void USBP_Config(uint8 clearAltSetting) ;
void USBP_ConfigAltChanged(void)        ;
void USBP_ConfigReg(void)               ;
void USBP_EpStateInit(void)             ;


const T_USBP_LUT CYCODE *USBP_GetConfigTablePtr(uint8 confIndex);
const T_USBP_LUT CYCODE *USBP_GetDeviceTablePtr(void)           ;
#if (USBP_BOS_ENABLE)
    const T_USBP_LUT CYCODE *USBP_GetBOSPtr(void)               ;
#endif /* (USBP_BOS_ENABLE) */
const uint8 CYCODE *USBP_GetInterfaceClassTablePtr(void)                    ;
uint8 USBP_ClearEndpointHalt(void)                                          ;
uint8 USBP_SetEndpointHalt(void)                                            ;
uint8 USBP_ValidateAlternateSetting(void)                                   ;

void USBP_SaveConfig(void)      ;
void USBP_RestoreConfig(void)   ;

#if (CY_PSOC3 || CY_PSOC5LP)
    #if (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u))
        void USBP_LoadNextInEP(uint8 epNumber, uint8 mode)  ;
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO && (USBP_EP_DMA_AUTO_OPT == 0u)) */
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

#if defined(USBP_ENABLE_IDSN_STRING)
    void USBP_ReadDieID(uint8 descr[])  ;
#endif /* USBP_ENABLE_IDSN_STRING */

#if defined(USBP_ENABLE_HID_CLASS)
    uint8 USBP_DispatchHIDClassRqst(void) ;
#endif /* (USBP_ENABLE_HID_CLASS) */

#if defined(USBP_ENABLE_AUDIO_CLASS)
    uint8 USBP_DispatchAUDIOClassRqst(void) ;
#endif /* (USBP_ENABLE_AUDIO_CLASS) */

#if defined(USBP_ENABLE_CDC_CLASS)
    uint8 USBP_DispatchCDCClassRqst(void) ;
#endif /* (USBP_ENABLE_CDC_CLASS) */

#if (USBP_ENABLE_MSC_CLASS)
    #if (USBP_HANDLE_MSC_REQUESTS)
        uint8 USBP_DispatchMSCClassRqst(void) ;
    #endif /* (USBP_HANDLE_MSC_REQUESTS) */
#endif /* (USBP_ENABLE_MSC_CLASS */

CY_ISR_PROTO(USBP_EP_0_ISR);
CY_ISR_PROTO(USBP_BUS_RESET_ISR);

#if (USBP_SOF_ISR_ACTIVE)
    CY_ISR_PROTO(USBP_SOF_ISR);
#endif /* (USBP_SOF_ISR_ACTIVE) */

#if (USBP_EP1_ISR_ACTIVE)
    CY_ISR_PROTO(USBP_EP_1_ISR);
#endif /* (USBP_EP1_ISR_ACTIVE) */

#if (USBP_EP2_ISR_ACTIVE)
    CY_ISR_PROTO(USBP_EP_2_ISR);
#endif /* (USBP_EP2_ISR_ACTIVE) */

#if (USBP_EP3_ISR_ACTIVE)
    CY_ISR_PROTO(USBP_EP_3_ISR);
#endif /* (USBP_EP3_ISR_ACTIVE) */

#if (USBP_EP4_ISR_ACTIVE)
    CY_ISR_PROTO(USBP_EP_4_ISR);
#endif /* (USBP_EP4_ISR_ACTIVE) */

#if (USBP_EP5_ISR_ACTIVE)
    CY_ISR_PROTO(USBP_EP_5_ISR);
#endif /* (USBP_EP5_ISR_ACTIVE) */

#if (USBP_EP6_ISR_ACTIVE)
    CY_ISR_PROTO(USBP_EP_6_ISR);
#endif /* (USBP_EP6_ISR_ACTIVE) */

#if (USBP_EP7_ISR_ACTIVE)
    CY_ISR_PROTO(USBP_EP_7_ISR);
#endif /* (USBP_EP7_ISR_ACTIVE) */

#if (USBP_EP8_ISR_ACTIVE)
    CY_ISR_PROTO(USBP_EP_8_ISR);
#endif /* (USBP_EP8_ISR_ACTIVE) */

#if (USBP_EP_MANAGEMENT_DMA)
    CY_ISR_PROTO(USBP_ARB_ISR);
#endif /* (USBP_EP_MANAGEMENT_DMA) */

#if (USBP_DP_ISR_ACTIVE)
    CY_ISR_PROTO(USBP_DP_ISR);
#endif /* (USBP_DP_ISR_ACTIVE) */

#if (CY_PSOC4)
    CY_ISR_PROTO(USBP_INTR_HI_ISR);
    CY_ISR_PROTO(USBP_INTR_MED_ISR);
    CY_ISR_PROTO(USBP_INTR_LO_ISR);
    #if (USBP_LPM_ACTIVE)
        CY_ISR_PROTO(USBP_LPM_ISR);
    #endif /* (USBP_LPM_ACTIVE) */
#endif /* (CY_PSOC4) */

#if (USBP_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)
    #if (USBP_DMA1_ACTIVE)
        void USBP_EP1_DMA_DONE_ISR(void);
    #endif /* (USBP_DMA1_ACTIVE) */

    #if (USBP_DMA2_ACTIVE)
        void USBP_EP2_DMA_DONE_ISR(void);
    #endif /* (USBP_DMA2_ACTIVE) */

    #if (USBP_DMA3_ACTIVE)
        void USBP_EP3_DMA_DONE_ISR(void);
    #endif /* (USBP_DMA3_ACTIVE) */

    #if (USBP_DMA4_ACTIVE)
        void USBP_EP4_DMA_DONE_ISR(void);
    #endif /* (USBP_DMA4_ACTIVE) */

    #if (USBP_DMA5_ACTIVE)
        void USBP_EP5_DMA_DONE_ISR(void);
    #endif /* (USBP_DMA5_ACTIVE) */

    #if (USBP_DMA6_ACTIVE)
        void USBP_EP6_DMA_DONE_ISR(void);
    #endif /* (USBP_DMA6_ACTIVE) */

    #if (USBP_DMA7_ACTIVE)
        void USBP_EP7_DMA_DONE_ISR(void);
    #endif /* (USBP_DMA7_ACTIVE) */

    #if (USBP_DMA8_ACTIVE)
        void USBP_EP8_DMA_DONE_ISR(void);
    #endif /* (USBP_DMA8_ACTIVE) */

#else
    #if (USBP_EP_DMA_AUTO_OPT == 0u)
        CY_ISR_PROTO(USBP_EP_DMA_DONE_ISR);
    #endif /* (USBP_EP_DMA_AUTO_OPT == 0u) */
#endif /* (CY_PSOC4) */
#endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */


/***************************************
*         Request Handlers
***************************************/

uint8 USBP_HandleStandardRqst(void) ;
uint8 USBP_DispatchClassRqst(void)  ;
uint8 USBP_HandleVendorRqst(void)   ;


/***************************************
*    HID Internal references
***************************************/

#if defined(USBP_ENABLE_HID_CLASS)
    void USBP_FindReport(void)            ;
    void USBP_FindReportDescriptor(void)  ;
    void USBP_FindHidClassDecriptor(void) ;
#endif /* USBP_ENABLE_HID_CLASS */


/***************************************
*    MIDI Internal references
***************************************/

#if defined(USBP_ENABLE_MIDI_STREAMING)
    void USBP_MIDI_IN_EP_Service(void)  ;
#endif /* (USBP_ENABLE_MIDI_STREAMING) */


/***************************************
*    CDC Internal references
***************************************/

#if defined(USBP_ENABLE_CDC_CLASS)

    typedef struct
    {
        uint8  bRequestType;
        uint8  bNotification;
        uint8  wValue;
        uint8  wValueMSB;
        uint8  wIndex;
        uint8  wIndexMSB;
        uint8  wLength;
        uint8  wLengthMSB;
        uint8  wSerialState;
        uint8  wSerialStateMSB;
    } t_USBP_cdc_notification;

    uint8 USBP_GetInterfaceComPort(uint8 interface) ;
    uint8 USBP_Cdc_EpInit( const T_USBP_EP_SETTINGS_BLOCK CYCODE *pEP, uint8 epNum, uint8 cdcComNums) ;

    extern volatile uint8  USBP_cdc_dataInEpList[USBP_MAX_MULTI_COM_NUM];
    extern volatile uint8  USBP_cdc_dataOutEpList[USBP_MAX_MULTI_COM_NUM];
    extern volatile uint8  USBP_cdc_commInEpList[USBP_MAX_MULTI_COM_NUM];
#endif /* (USBP_ENABLE_CDC_CLASS) */


#endif /* CY_USBFS_USBP_pvt_H */


/* [] END OF FILE */
