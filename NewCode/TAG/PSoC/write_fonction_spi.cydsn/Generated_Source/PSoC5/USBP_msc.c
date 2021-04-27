/***************************************************************************//**
* \file USBP_cdc.c
* \version 3.20
*
* \brief
*  This file contains the USB MSC Class request handler and global API for MSC 
*  class.
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

#include "USBP_msc.h"
#include "USBP_pvt.h"
#include "cyapicallbacks.h"

#if (USBP_HANDLE_MSC_REQUESTS)

/***************************************
*          Internal variables
***************************************/

static uint8 USBP_lunCount = USBP_MSC_LUN_NUMBER;


/*******************************************************************************
* Function Name: USBP_DispatchMSCClassRqst
****************************************************************************//**
*   
*  \internal 
*  This routine dispatches MSC class requests.
*
* \return
*  Status of request processing: handled or not handled.
*
* \globalvars
*  USBP_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_DispatchMSCClassRqst(void) 
{
    uint8 requestHandled = USBP_FALSE;
    
    /* Get request data. */
    uint16 value  = USBP_GET_UINT16(USBP_wValueHiReg,  USBP_wValueLoReg);
    uint16 dataLength = USBP_GET_UINT16(USBP_wLengthHiReg, USBP_wLengthLoReg);
       
    /* Check request direction: D2H or H2D. */
    if (0u != (USBP_bmRequestTypeReg & USBP_RQST_DIR_D2H))
    {
        /* Handle direction from device to host. */
        
        if (USBP_MSC_GET_MAX_LUN == USBP_bRequestReg)
        {
            /* Check request fields. */
            if ((value  == USBP_MSC_GET_MAX_LUN_WVALUE) &&
                (dataLength == USBP_MSC_GET_MAX_LUN_WLENGTH))
            {
                /* Reply to Get Max LUN request: setup control read. */
                USBP_currentTD.pData = &USBP_lunCount;
                USBP_currentTD.count =  USBP_MSC_GET_MAX_LUN_WLENGTH;
                
                requestHandled  = USBP_InitControlRead();
            }
        }
    }
    else
    {
        /* Handle direction from host to device. */
        
        if (USBP_MSC_RESET == USBP_bRequestReg)
        {
            /* Check request fields. */
            if ((value  == USBP_MSC_RESET_WVALUE) &&
                (dataLength == USBP_MSC_RESET_WLENGTH))
            {
                /* Handle to Bulk-Only Reset request: no data control transfer. */
                USBP_currentTD.count = USBP_MSC_RESET_WLENGTH;
                
            #ifdef USBP_DISPATCH_MSC_CLASS_MSC_RESET_RQST_CALLBACK
                USBP_DispatchMSCClass_MSC_RESET_RQST_Callback();
            #endif /* (USBP_DISPATCH_MSC_CLASS_MSC_RESET_RQST_CALLBACK) */
                
                requestHandled = USBP_InitNoDataControlTransfer();
            }
        }
    }
    
    return (requestHandled);
}


/*******************************************************************************
* Function Name: USBP_MSC_SetLunCount
****************************************************************************//**
*
*  This function sets the number of logical units supported in the application. 
*  The default number of logical units is set in the component customizer.
*
*  \param lunCount: Count of the logical units. Valid range is between 1 and 16.
*
*
* \globalvars
*  USBP_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_MSC_SetLunCount(uint8 lunCount) 
{
    USBP_lunCount = (lunCount - 1u);
}


/*******************************************************************************
* Function Name: USBP_MSC_GetLunCount
****************************************************************************//**
*
*  This function returns the number of logical units.
*
* \return
*   Number of the logical units.
*
* \globalvars
*  USBP_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_MSC_GetLunCount(void) 
{
    return (USBP_lunCount + 1u);
}   

#endif /* (USBP_HANDLE_MSC_REQUESTS) */


/* [] END OF FILE */
