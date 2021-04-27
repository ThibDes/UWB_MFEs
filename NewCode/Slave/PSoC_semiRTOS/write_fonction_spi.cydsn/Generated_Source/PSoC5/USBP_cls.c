/***************************************************************************//**
* \file USBP_cls.c
* \version 3.20
*
* \brief
*  This file contains the USB Class request handler.
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USBP_pvt.h"
#include "cyapicallbacks.h"

#if(USBP_EXTERN_CLS == USBP_FALSE)

/***************************************
* User Implemented Class Driver Declarations.
***************************************/
/* `#START USER_DEFINED_CLASS_DECLARATIONS` Place your declaration here */

/* `#END` */


/*******************************************************************************
* Function Name: USBP_DispatchClassRqst
****************************************************************************//**
*  This routine dispatches class specific requests depend on interface class.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_DispatchClassRqst(void) 
{
    uint8 interfaceNumber;
    uint8 requestHandled = USBP_FALSE;

    /* Get interface to which request is intended. */
    switch (USBP_bmRequestTypeReg & USBP_RQST_RCPT_MASK)
    {
        case USBP_RQST_RCPT_IFC:
            /* Class-specific request directed to interface: wIndexLoReg 
            * contains interface number.
            */
            interfaceNumber = (uint8) USBP_wIndexLoReg;
            break;
        
        case USBP_RQST_RCPT_EP:
            /* Class-specific request directed to endpoint: wIndexLoReg contains 
            * endpoint number. Find interface related to endpoint. 
            */
            interfaceNumber = USBP_EP[USBP_wIndexLoReg & USBP_DIR_UNUSED].interface;
            break;
            
        default:
            /* Default interface is zero. */
            interfaceNumber = 0u;
            break;
    }
    
    /* Check that interface is within acceptable range */
    if (interfaceNumber <= USBP_MAX_INTERFACES_NUMBER)
    {
    #if (defined(USBP_ENABLE_HID_CLASS)   || \
         defined(USBP_ENABLE_AUDIO_CLASS) || \
         defined(USBP_ENABLE_CDC_CLASS)   || \
         USBP_ENABLE_MSC_CLASS)

        /* Handle class request depends on interface type. */
        switch (USBP_interfaceClass[interfaceNumber])
        {
        #if defined(USBP_ENABLE_HID_CLASS)
            case USBP_CLASS_HID:
                requestHandled = USBP_DispatchHIDClassRqst();
                break;
        #endif /* (USBP_ENABLE_HID_CLASS) */
                
        #if defined(USBP_ENABLE_AUDIO_CLASS)
            case USBP_CLASS_AUDIO:
                requestHandled = USBP_DispatchAUDIOClassRqst();
                break;
        #endif /* (USBP_CLASS_AUDIO) */
                
        #if defined(USBP_ENABLE_CDC_CLASS)
            case USBP_CLASS_CDC:
                requestHandled = USBP_DispatchCDCClassRqst();
                break;
        #endif /* (USBP_ENABLE_CDC_CLASS) */
            
        #if (USBP_ENABLE_MSC_CLASS)
            case USBP_CLASS_MSD:
            #if (USBP_HANDLE_MSC_REQUESTS)
                /* MSC requests are handled by the component. */
                requestHandled = USBP_DispatchMSCClassRqst();
            #elif defined(USBP_DISPATCH_MSC_CLASS_RQST_CALLBACK)
                /* MSC requests are handled by user defined callbcak. */
                requestHandled = USBP_DispatchMSCClassRqst_Callback();
            #else
                /* MSC requests are not handled. */
                requestHandled = USBP_FALSE;
            #endif /* (USBP_HANDLE_MSC_REQUESTS) */
                break;
        #endif /* (USBP_ENABLE_MSC_CLASS) */
            
            default:
                /* Request is not handled: unknown class request type. */
                requestHandled = USBP_FALSE;
                break;
        }
    #endif /* Class support is enabled */
    }
    
    /* `#START USER_DEFINED_CLASS_CODE` Place your Class request here */

    /* `#END` */

#ifdef USBP_DISPATCH_CLASS_RQST_CALLBACK
    if (USBP_FALSE == requestHandled)
    {
        requestHandled = USBP_DispatchClassRqst_Callback(interfaceNumber);
    }
#endif /* (USBP_DISPATCH_CLASS_RQST_CALLBACK) */

    return (requestHandled);
}


/*******************************************************************************
* Additional user functions supporting Class Specific Requests
********************************************************************************/

/* `#START CLASS_SPECIFIC_FUNCTIONS` Place any additional functions here */

/* `#END` */

#endif /* USBP_EXTERN_CLS */


/* [] END OF FILE */
