/***************************************************************************//**
* \file USBP_hid.c
* \version 3.20
*
* \brief
*  This file contains the USB HID Class request handler. 
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

#include "USBP_hid.h"
#include "USBP_pvt.h"
#include "cyapicallbacks.h"


#if defined(USBP_ENABLE_HID_CLASS)

/***************************************
*    HID Variables
***************************************/
/** This variable is initialized in the USBFS_InitComponent() API to the 
 * PROTOCOL_REPORT value. It is controlled by the host using the 
 * HID_SET_PROTOCOL request. The value is returned to the user code by the 
 * USBFS_GetProtocol() API.*/
volatile uint8 USBP_hidProtocol[USBP_MAX_INTERFACES_NUMBER];

/** This variable controls the HID report rate. It is controlled by the host 
 * using the HID_SET_IDLE request and used by the USBFS_UpdateHIDTimer() API to 
 * reload timer.*/
volatile uint8 USBP_hidIdleRate[USBP_MAX_INTERFACES_NUMBER];

/** This variable contains the timer counter, which is decremented and reloaded 
 * by the USBFS_UpdateHIDTimer() API.*/
volatile uint8 USBP_hidIdleTimer[USBP_MAX_INTERFACES_NUMBER]; /* HID device idle rate value */


/***************************************
* Custom Declarations
***************************************/

/* `#START HID_CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


/*******************************************************************************
* Function Name: USBP_UpdateHIDTimer
****************************************************************************//**
*
*  This function updates the HID Report idle timer and returns the status and 
*  reloads the timer if it expires.
*
*  \param interface Contains the interface number.
*
* \return
*  Returns the state of the HID timer. Symbolic names and their associated values are given here:
*  Return Value               |Notes
*  ---------------------------|------------------------------------------------
*  USBFS_IDLE_TIMER_EXPIRED   | The timer expired.
*  USBFS_IDLE_TIMER_RUNNING   | The timer is running.
*  USBFS_IDLE_TIMER_IDEFINITE | The report is sent when data or state changes.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_UpdateHIDTimer(uint8 interface) 
{
    uint8 stat = USBP_IDLE_TIMER_INDEFINITE;

    if(USBP_hidIdleRate[interface] != 0u)
    {
        if(USBP_hidIdleTimer[interface] > 0u)
        {
            USBP_hidIdleTimer[interface]--;
            stat = USBP_IDLE_TIMER_RUNNING;
        }
        else
        {
            USBP_hidIdleTimer[interface] = USBP_hidIdleRate[interface];
            stat = USBP_IDLE_TIMER_EXPIRED;
        }
    }

    return((uint8)stat);
}


/*******************************************************************************
* Function Name: USBP_GetProtocol
****************************************************************************//**
*
*  This function returns the HID protocol value for the selected interface.
*
*  \param interface:  Contains the interface number.
*
*  \return
*  Returns the protocol value. 
*
*******************************************************************************/
uint8 USBP_GetProtocol(uint8 interface) 
{
    return(USBP_hidProtocol[interface]);
}


/*******************************************************************************
* Function Name: USBP_DispatchHIDClassRqst
****************************************************************************//**
*
*  This routine dispatches class requests
*
* \return
*  Results of HID Class request handling: 
*  - USBP_TRUE  - request was handled without errors
*  - USBP_FALSE - error occurs during handling of request  
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_DispatchHIDClassRqst(void) 
{
    uint8 requestHandled = USBP_FALSE;

    uint8 interfaceNumber = (uint8) USBP_wIndexLoReg;
    
    /* Check request direction: D2H or H2D. */
    if (0u != (USBP_bmRequestTypeReg & USBP_RQST_DIR_D2H))
    {
        /* Handle direction from device to host. */
        
        switch (USBP_bRequestReg)
        {
            case USBP_GET_DESCRIPTOR:
                if (USBP_wValueHiReg == USBP_DESCR_HID_CLASS)
                {
                    USBP_FindHidClassDecriptor();
                    if (USBP_currentTD.count != 0u)
                    {
                        requestHandled = USBP_InitControlRead();
                    }
                }
                else if (USBP_wValueHiReg == USBP_DESCR_HID_REPORT)
                {
                    USBP_FindReportDescriptor();
                    if (USBP_currentTD.count != 0u)
                    {
                        requestHandled = USBP_InitControlRead();
                    }
                }
                else
                {   
                    /* Do not handle this request. */
                }
                break;
                
            case USBP_HID_GET_REPORT:
                USBP_FindReport();
                if (USBP_currentTD.count != 0u)
                {
                    requestHandled = USBP_InitControlRead();
                }
                break;

            case USBP_HID_GET_IDLE:
                /* This function does not support multiple reports per interface*/
                /* Validate interfaceNumber and Report ID (should be 0): Do not support Idle per Report ID */
                if ((interfaceNumber < USBP_MAX_INTERFACES_NUMBER) && (USBP_wValueLoReg == 0u)) 
                {
                    USBP_currentTD.count = 1u;
                    USBP_currentTD.pData = &USBP_hidIdleRate[interfaceNumber];
                    requestHandled  = USBP_InitControlRead();
                }
                break;
                
            case USBP_HID_GET_PROTOCOL:
                /* Validate interfaceNumber */
                if( interfaceNumber < USBP_MAX_INTERFACES_NUMBER)
                {
                    USBP_currentTD.count = 1u;
                    USBP_currentTD.pData = &USBP_hidProtocol[interfaceNumber];
                    requestHandled  = USBP_InitControlRead();
                }
                break;
                
            default:    /* requestHandled is initialized as FALSE by default */
                break;
        }
    }
    else
    {   
        /* Handle direction from host to device. */
        
        switch (USBP_bRequestReg)
        {
            case USBP_HID_SET_REPORT:
                USBP_FindReport();
                if (USBP_currentTD.count != 0u)
                {
                    requestHandled = USBP_InitControlWrite();
                }
                break;
                
            case USBP_HID_SET_IDLE:
                /* This function does not support multiple reports per interface */
                /* Validate interfaceNumber and Report ID (should be 0): Do not support Idle per Report ID */
                if ((interfaceNumber < USBP_MAX_INTERFACES_NUMBER) && (USBP_wValueLoReg == 0u))
                {
                    USBP_hidIdleRate[interfaceNumber] = (uint8)USBP_wValueHiReg;
                    /* With regards to HID spec: "7.2.4 Set_Idle Request"
                    *  Latency. If the current period has gone past the
                    *  newly proscribed time duration, then a report
                    *  will be generated immediately.
                    */
                    if(USBP_hidIdleRate[interfaceNumber] <
                       USBP_hidIdleTimer[interfaceNumber])
                    {
                        /* Set the timer to zero and let the UpdateHIDTimer() API return IDLE_TIMER_EXPIRED status*/
                        USBP_hidIdleTimer[interfaceNumber] = 0u;
                    }
                    /* If the new request is received within 4 milliseconds
                    *  (1 count) of the end of the current period, then the
                    *  new request will have no effect until after the report.
                    */
                    else if(USBP_hidIdleTimer[interfaceNumber] <= 1u)
                    {
                        /* Do nothing.
                        *  Let the UpdateHIDTimer() API continue to work and
                        *  return IDLE_TIMER_EXPIRED status
                        */
                    }
                    else
                    {   /* Reload the timer*/
                        USBP_hidIdleTimer[interfaceNumber] =
                        USBP_hidIdleRate[interfaceNumber];
                    }
                    requestHandled = USBP_InitNoDataControlTransfer();
                }
                break;

            case USBP_HID_SET_PROTOCOL:
                /* Validate interfaceNumber and protocol (must be 0 or 1) */
                if ((interfaceNumber < USBP_MAX_INTERFACES_NUMBER) && (USBP_wValueLoReg <= 1u))
                {
                    USBP_hidProtocol[interfaceNumber] = (uint8)USBP_wValueLoReg;
                    requestHandled = USBP_InitNoDataControlTransfer();
                }
                break;
            
            default:    
                /* Unknown class request is not handled. */
                break;
        }
    }

    return (requestHandled);
}


/*******************************************************************************
* Function Name: USB_FindHidClassDescriptor
****************************************************************************//**
*
*  This routine find Hid Class Descriptor pointer based on the Interface number
*  and Alternate setting then loads the currentTD structure with the address of
*  the buffer and the size.
*  The HID Class Descriptor resides inside the config descriptor.
*
* \return
*  currentTD
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_FindHidClassDecriptor(void) 
{
    const T_USBP_LUT CYCODE *pTmp;
    volatile uint8 *pDescr;
    uint8 interfaceN;

    pTmp = USBP_GetConfigTablePtr(USBP_configuration - 1u);
    
    interfaceN = (uint8) USBP_wIndexLoReg;
    /* Third entry in the LUT starts the Interface Table pointers */
    /* Now use the request interface number*/
    pTmp = &pTmp[interfaceN + 2u];
    
    /* USB_DEVICEx_CONFIGURATIONy_INTERFACEz_TABLE */
    pTmp = (const T_USBP_LUT CYCODE *) pTmp->p_list;
    
    /* Now use Alternate setting number */
    pTmp = &pTmp[USBP_interfaceSetting[interfaceN]];
    
    /* USB_DEVICEx_CONFIGURATIONy_INTERFACEz_ALTERNATEi_HID_TABLE */
    pTmp = (const T_USBP_LUT CYCODE *) pTmp->p_list;
    
    /* Fifth entry in the LUT points to Hid Class Descriptor in Configuration Descriptor */
    pTmp = &pTmp[4u];
    pDescr = (volatile uint8 *)pTmp->p_list;
    
    /* The first byte contains the descriptor length */
    USBP_currentTD.count = *pDescr;
    USBP_currentTD.pData = pDescr;
}


/*******************************************************************************
* Function Name: USB_FindReportDescriptor
****************************************************************************//**
*
*  This routine find Hid Report Descriptor pointer based on the Interface
*  number, then loads the currentTD structure with the address of the buffer
*  and the size.
*  Hid Report Descriptor is located after IN/OUT/FEATURE reports.
*
* \return
*  currentTD
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_FindReportDescriptor(void) 
{
    const T_USBP_LUT CYCODE *pTmp;
    volatile uint8 *pDescr;
    uint8 interfaceN;

    pTmp = USBP_GetConfigTablePtr(USBP_configuration - 1u);
    interfaceN = (uint8) USBP_wIndexLoReg;
    
    /* Third entry in the LUT starts the Interface Table pointers */
    /* Now use the request interface number */
    pTmp = &pTmp[interfaceN + 2u];
    
    /* USB_DEVICEx_CONFIGURATIONy_INTERFACEz_TABLE */
    pTmp = (const T_USBP_LUT CYCODE *) pTmp->p_list;
    
    /* Now use Alternate setting number */
    pTmp = &pTmp[USBP_interfaceSetting[interfaceN]];
    
    /* USB_DEVICEx_CONFIGURATIONy_INTERFACEz_ALTERNATEi_HID_TABLE */
    pTmp = (const T_USBP_LUT CYCODE *) pTmp->p_list;
    
    /* Fourth entry in the LUT starts the Hid Report Descriptor */
    pTmp = &pTmp[3u];
    pDescr = (volatile uint8 *)pTmp->p_list;
    
    /* The 1st and 2nd bytes of descriptor contain its length. LSB is 1st. */
    USBP_currentTD.count =  ((uint16)((uint16) pDescr[1u] << 8u) | pDescr[0u]);
    USBP_currentTD.pData = &pDescr[2u];
}


/*******************************************************************************
* Function Name: USBP_FindReport
****************************************************************************//**
*
*  This routine sets up a transfer based on the Interface number, Report Type
*  and Report ID, then loads the currentTD structure with the address of the
*  buffer and the size.  The caller has to decide if it is a control read or
*  control write.
*
* \return
*  currentTD
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_FindReport(void) 
{
    const T_USBP_LUT CYCODE *pTmp;
    T_USBP_TD *pTD;
    uint8 reportType;
    uint8 interfaceN;
 
    /* `#START HID_FINDREPORT` Place custom handling here */

    /* `#END` */
    
#ifdef USBP_FIND_REPORT_CALLBACK
    USBP_FindReport_Callback();
#endif /* (USBP_FIND_REPORT_CALLBACK) */
    
    USBP_currentTD.count = 0u;   /* Init not supported condition */
    pTmp = USBP_GetConfigTablePtr(USBP_configuration - 1u);
    reportType = (uint8) USBP_wValueHiReg;
    interfaceN = (uint8) USBP_wIndexLoReg;
    
    /* Third entry in the LUT Configuration Table starts the Interface Table pointers */
    /* Now use the request interface number */
    pTmp = &pTmp[interfaceN + 2u];
    
    /* USB_DEVICEx_CONFIGURATIONy_INTERFACEz_TABLE */
    pTmp = (const T_USBP_LUT CYCODE *) pTmp->p_list;
    if (interfaceN < USBP_MAX_INTERFACES_NUMBER)
    {
        /* Now use Alternate setting number */
        pTmp = &pTmp[USBP_interfaceSetting[interfaceN]];
        
        /* USB_DEVICEx_CONFIGURATIONy_INTERFACEz_ALTERNATEi_HID_TABLE */
        pTmp = (const T_USBP_LUT CYCODE *) pTmp->p_list;
        
        /* Validate reportType to comply with "7.2.1 Get_Report Request" */
        if ((reportType >= USBP_HID_GET_REPORT_INPUT) &&
            (reportType <= USBP_HID_GET_REPORT_FEATURE))
        {
            /* Get the entry proper TD (IN, OUT or Feature Report Table)*/
            pTmp = &pTmp[reportType - 1u];
            
            /* Get reportID */
            reportType = (uint8) USBP_wValueLoReg;
            
            /* Validate table support by the HID descriptor, compare table count with reportID */
            if (pTmp->c >= reportType)
            {
                pTD = (T_USBP_TD *) pTmp->p_list;
                pTD = &pTD[reportType];                          /* select entry depend on report ID*/
                USBP_currentTD.pData = pTD->pData;   /* Buffer pointer */
                USBP_currentTD.count = pTD->count;   /* Buffer Size */
                USBP_currentTD.pStatusBlock = pTD->pStatusBlock;
            }
        }
    }
}


/*******************************************************************************
* Additional user functions supporting HID Requests
********************************************************************************/

/* `#START HID_FUNCTIONS` Place any additional functions here */

/* `#END` */

#endif  /*  USBP_ENABLE_HID_CLASS */


/* [] END OF FILE */
