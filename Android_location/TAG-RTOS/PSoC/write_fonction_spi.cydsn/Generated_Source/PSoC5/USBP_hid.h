/***************************************************************************//**
* \file USBP_hid.h
* \version 3.20
*
* \brief
*  This file provides function prototypes and constants for the USBFS component 
*  HID class.
*
* Related Document:
*  Device Class Definition for Human Interface Devices (HID) Version 1.11
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USBP_hid_H)
#define CY_USBFS_USBP_hid_H

#include "USBP.h"

/***************************************
* Prototypes of the USBP_hid API.
***************************************/
/**
* \addtogroup group_hid
* @{
*/
uint8 USBP_UpdateHIDTimer(uint8 interface) ;
uint8 USBP_GetProtocol(uint8 interface)    ;
/** @} hid */

/***************************************
*Renamed Functions for backward compatible
***************************************/

#define USBP_bGetProtocol   USBP_GetProtocol


/***************************************
*  Constants for USBP_hid API.
***************************************/

#define USBP_PROTOCOL_BOOT              (0x00u)
#define USBP_PROTOCOL_REPORT            (0x01u)

/* Request Types (HID Chapter 7.2) */
#define USBP_HID_GET_REPORT             (0x01u)
#define USBP_HID_GET_IDLE               (0x02u)
#define USBP_HID_GET_PROTOCOL           (0x03u)
#define USBP_HID_SET_REPORT             (0x09u)
#define USBP_HID_SET_IDLE               (0x0Au)
#define USBP_HID_SET_PROTOCOL           (0x0Bu)

/* Descriptor Types (HID Chapter 7.1) */
#define USBP_DESCR_HID_CLASS            (0x21u)
#define USBP_DESCR_HID_REPORT           (0x22u)
#define USBP_DESCR_HID_PHYSICAL         (0x23u)

/* Report Request Types (HID Chapter 7.2.1) */
#define USBP_HID_GET_REPORT_INPUT       (0x01u)
#define USBP_HID_GET_REPORT_OUTPUT      (0x02u)
#define USBP_HID_GET_REPORT_FEATURE     (0x03u)

#endif /*  CY_USBFS_USBP_hid_H */


/* [] END OF FILE */
