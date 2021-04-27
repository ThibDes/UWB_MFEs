/***************************************************************************//**
* \file USBP_cdc.h
* \version 3.20
*
* \brief
*  This file provides function prototypes and constants for the USBFS component 
*  CDC class.
*
* Related Document:
*  Universal Serial Bus Class Definitions for Communication Devices Version 1.1
*
********************************************************************************
* \copyright
* Copyright 2012-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USBP_cdc_H)
#define CY_USBFS_USBP_cdc_H

#include "USBP.h"


/*******************************************************************************
* Prototypes of the USBP_cdc API.
*******************************************************************************/
/**
* \addtogroup group_cdc
* @{
*/
#if (USBP_ENABLE_CDC_CLASS_API != 0u)
    uint8 USBP_CDC_Init(void)            ;
    void USBP_PutData(const uint8* pData, uint16 length) ;
    void USBP_PutString(const char8 string[])            ;
    void USBP_PutChar(char8 txDataByte) ;
    void USBP_PutCRLF(void)             ;
    uint16 USBP_GetCount(void)          ;
    uint8  USBP_CDCIsReady(void)        ;
    uint8  USBP_DataIsReady(void)       ;
    uint16 USBP_GetData(uint8* pData, uint16 length)     ;
    uint16 USBP_GetAll(uint8* pData)    ;
    uint8  USBP_GetChar(void)           ;
    uint8  USBP_IsLineChanged(void)     ;
    uint32 USBP_GetDTERate(void)        ;
    uint8  USBP_GetCharFormat(void)     ;
    uint8  USBP_GetParityType(void)     ;
    uint8  USBP_GetDataBits(void)       ;
    uint16 USBP_GetLineControl(void)    ;
    void USBP_SendSerialState (uint16 serialState) ;
    uint16 USBP_GetSerialState (void)   ;
    void USBP_SetComPort (uint8 comNumber) ;
    uint8 USBP_GetComPort (void)        ;
    uint8 USBP_NotificationIsReady(void) ;

#endif  /* (USBP_ENABLE_CDC_CLASS_API) */
/** @} cdc */

/*******************************************************************************
*  Constants for USBP_cdc API.
*******************************************************************************/

/* CDC Class-Specific Request Codes (CDC ver 1.2 Table 19) */
#define USBP_CDC_SET_LINE_CODING        (0x20u)
#define USBP_CDC_GET_LINE_CODING        (0x21u)
#define USBP_CDC_SET_CONTROL_LINE_STATE (0x22u)

/*PSTN Subclass Specific Notifications (CDC ver 1.2 Table 30)*/
#define USBP_SERIAL_STATE               (0x20u)

#define USBP_LINE_CODING_CHANGED        (0x01u)
#define USBP_LINE_CONTROL_CHANGED       (0x02u)

#define USBP_1_STOPBIT                  (0x00u)
#define USBP_1_5_STOPBITS               (0x01u)
#define USBP_2_STOPBITS                 (0x02u)

#define USBP_PARITY_NONE                (0x00u)
#define USBP_PARITY_ODD                 (0x01u)
#define USBP_PARITY_EVEN                (0x02u)
#define USBP_PARITY_MARK                (0x03u)
#define USBP_PARITY_SPACE               (0x04u)

#define USBP_LINE_CODING_SIZE           (0x07u)
#define USBP_LINE_CODING_RATE           (0x00u)
#define USBP_LINE_CODING_STOP_BITS      (0x04u)
#define USBP_LINE_CODING_PARITY         (0x05u)
#define USBP_LINE_CODING_DATA_BITS      (0x06u)

#define USBP_LINE_CONTROL_DTR           (0x01u)
#define USBP_LINE_CONTROL_RTS           (0x02u)

#define USBP_MAX_MULTI_COM_NUM          (2u) 

#define USBP_COM_PORT1                  (0u) 
#define USBP_COM_PORT2                  (1u) 

#define USBP_SUCCESS                    (0u)
#define USBP_FAILURE                    (1u)

#define USBP_SERIAL_STATE_SIZE          (10u)

/* SerialState constants*/
#define USBP_SERIAL_STATE_REQUEST_TYPE  (0xA1u)
#define USBP_SERIAL_STATE_LENGTH        (0x2u)

/*******************************************************************************
* External data references
*******************************************************************************/
/**
* \addtogroup group_cdc
* @{
*/
extern volatile uint8  USBP_linesCoding[USBP_MAX_MULTI_COM_NUM][USBP_LINE_CODING_SIZE];
extern volatile uint8  USBP_linesChanged[USBP_MAX_MULTI_COM_NUM];
extern volatile uint16 USBP_linesControlBitmap[USBP_MAX_MULTI_COM_NUM];
extern volatile uint16 USBP_serialStateBitmap[USBP_MAX_MULTI_COM_NUM];
extern volatile uint8  USBP_cdcDataInEp[USBP_MAX_MULTI_COM_NUM];
extern volatile uint8  USBP_cdcDataOutEp[USBP_MAX_MULTI_COM_NUM];
extern volatile uint8  USBP_cdcCommInInterruptEp[USBP_MAX_MULTI_COM_NUM];
/** @} cdc */

/*******************************************************************************
* The following code is DEPRECATED and
* must not be used.
*******************************************************************************/


#define USBP_lineCoding             USBP_linesCoding[0]
#define USBP_lineChanged            USBP_linesChanged[0]
#define USBP_lineControlBitmap      USBP_linesControlBitmap[0]
#define USBP_cdc_data_in_ep         USBP_cdcDataInEp[0]
#define USBP_cdc_data_out_ep        USBP_cdcDataOutEp[0]

#endif /* (CY_USBFS_USBP_cdc_H) */


/* [] END OF FILE */
