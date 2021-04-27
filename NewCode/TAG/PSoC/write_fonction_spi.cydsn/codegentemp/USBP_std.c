/***************************************************************************//**
* \file USBP_std.c
* \version 3.20
*
* \brief
*  This file contains the USB Standard request handler.
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "USBP_pvt.h"

/***************************************
*   Static data allocation
***************************************/

#if defined(USBP_ENABLE_FWSN_STRING)
    static volatile uint8* USBP_fwSerialNumberStringDescriptor;
    static volatile uint8  USBP_snStringConfirm = USBP_FALSE;
#endif  /* (USBP_ENABLE_FWSN_STRING) */

#if defined(USBP_ENABLE_FWSN_STRING)
    /***************************************************************************
    * Function Name: USBP_SerialNumString
    ************************************************************************//**
    *
    *  This function is available only when the User Call Back option in the 
    *  Serial Number String descriptor properties is selected. Application 
    *  firmware can provide the source of the USB device serial number string 
    *  descriptor during run time. The default string is used if the application 
    *  firmware does not use this function or sets the wrong string descriptor.
    *
    *  \param snString:  Pointer to the user-defined string descriptor. The 
    *  string descriptor should meet the Universal Serial Bus Specification 
    *  revision 2.0 chapter 9.6.7
    *  Offset|Size|Value|Description
    *  ------|----|------|---------------------------------
    *  0     |1   |N     |Size of this descriptor in bytes
    *  1     |1   |0x03  |STRING Descriptor Type
    *  2     |N-2 |Number|UNICODE encoded string
    *  
    * *For example:* uint8 snString[16]={0x0E,0x03,'F',0,'W',0,'S',0,'N',0,'0',0,'1',0};
    *
    * \reentrant
    *  No.
    *
    ***************************************************************************/
    void  USBP_SerialNumString(uint8 snString[]) 
    {
        USBP_snStringConfirm = USBP_FALSE;
        
        if (snString != NULL)
        {
            /* Check descriptor validation */
            if ((snString[0u] > 1u) && (snString[1u] == USBP_DESCR_STRING))
            {
                USBP_fwSerialNumberStringDescriptor = snString;
                USBP_snStringConfirm = USBP_TRUE;
            }
        }
    }
#endif  /* USBP_ENABLE_FWSN_STRING */


/*******************************************************************************
* Function Name: USBP_HandleStandardRqst
****************************************************************************//**
*
*  This Routine dispatches standard requests
*
*
* \return
*  TRUE if request handled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_HandleStandardRqst(void) 
{
    uint8 requestHandled = USBP_FALSE;
    uint8 interfaceNumber;
    uint8 configurationN;
    uint8 bmRequestType = USBP_bmRequestTypeReg;

#if defined(USBP_ENABLE_STRINGS)
    volatile uint8 *pStr = 0u;
    #if defined(USBP_ENABLE_DESCRIPTOR_STRINGS)
        uint8 nStr;
        uint8 descrLength;
    #endif /* (USBP_ENABLE_DESCRIPTOR_STRINGS) */
#endif /* (USBP_ENABLE_STRINGS) */
    
    static volatile uint8 USBP_tBuffer[USBP_STATUS_LENGTH_MAX];
    const T_USBP_LUT CYCODE *pTmp;

    USBP_currentTD.count = 0u;

    if (USBP_RQST_DIR_D2H == (bmRequestType & USBP_RQST_DIR_MASK))
    {
        /* Control Read */
        switch (USBP_bRequestReg)
        {
            case USBP_GET_DESCRIPTOR:
                if (USBP_DESCR_DEVICE ==USBP_wValueHiReg)
                {
                    pTmp = USBP_GetDeviceTablePtr();
                    USBP_currentTD.pData = (volatile uint8 *)pTmp->p_list;
                    USBP_currentTD.count = USBP_DEVICE_DESCR_LENGTH;
                    
                    requestHandled  = USBP_InitControlRead();
                }
                else if (USBP_DESCR_CONFIG == USBP_wValueHiReg)
                {
                    pTmp = USBP_GetConfigTablePtr((uint8) USBP_wValueLoReg);
                    
                    /* Verify that requested descriptor exists */
                    if (pTmp != NULL)
                    {
                        USBP_currentTD.pData = (volatile uint8 *)pTmp->p_list;
                        USBP_currentTD.count = (uint16)((uint16)(USBP_currentTD.pData)[USBP_CONFIG_DESCR_TOTAL_LENGTH_HI] << 8u) | \
                                                                            (USBP_currentTD.pData)[USBP_CONFIG_DESCR_TOTAL_LENGTH_LOW];
                        requestHandled  = USBP_InitControlRead();
                    }
                }
                
            #if(USBP_BOS_ENABLE)
                else if (USBP_DESCR_BOS == USBP_wValueHiReg)
                {
                    pTmp = USBP_GetBOSPtr();
                    
                    /* Verify that requested descriptor exists */
                    if (pTmp != NULL)
                    {
                        USBP_currentTD.pData = (volatile uint8 *)pTmp;
                        USBP_currentTD.count = ((uint16)((uint16)(USBP_currentTD.pData)[USBP_BOS_DESCR_TOTAL_LENGTH_HI] << 8u)) | \
                                                                             (USBP_currentTD.pData)[USBP_BOS_DESCR_TOTAL_LENGTH_LOW];
                        requestHandled  = USBP_InitControlRead();
                    }
                }
            #endif /*(USBP_BOS_ENABLE)*/
            
            #if defined(USBP_ENABLE_STRINGS)
                else if (USBP_DESCR_STRING == USBP_wValueHiReg)
                {
                /* Descriptor Strings */
                #if defined(USBP_ENABLE_DESCRIPTOR_STRINGS)
                    nStr = 0u;
                    pStr = (volatile uint8 *) &USBP_STRING_DESCRIPTORS[0u];
                    
                    while ((USBP_wValueLoReg > nStr) && (*pStr != 0u))
                    {
                        /* Read descriptor length from 1st byte */
                        descrLength = *pStr;
                        /* Move to next string descriptor */
                        pStr = &pStr[descrLength];
                        nStr++;
                    }
                #endif /* (USBP_ENABLE_DESCRIPTOR_STRINGS) */
                
                /* Microsoft OS String */
                #if defined(USBP_ENABLE_MSOS_STRING)
                    if (USBP_STRING_MSOS == USBP_wValueLoReg)
                    {
                        pStr = (volatile uint8 *)& USBP_MSOS_DESCRIPTOR[0u];
                    }
                #endif /* (USBP_ENABLE_MSOS_STRING) */
                
                /* SN string */
                #if defined(USBP_ENABLE_SN_STRING)
                    if ((USBP_wValueLoReg != 0u) && 
                        (USBP_wValueLoReg == USBP_DEVICE0_DESCR[USBP_DEVICE_DESCR_SN_SHIFT]))
                    {
                    #if defined(USBP_ENABLE_IDSN_STRING)
                        /* Read DIE ID and generate string descriptor in RAM */
                        USBP_ReadDieID(USBP_idSerialNumberStringDescriptor);
                        pStr = USBP_idSerialNumberStringDescriptor;
                    #elif defined(USBP_ENABLE_FWSN_STRING)
                        
                        if(USBP_snStringConfirm != USBP_FALSE)
                        {
                            pStr = USBP_fwSerialNumberStringDescriptor;
                        }
                        else
                        {
                            pStr = (volatile uint8 *)&USBP_SN_STRING_DESCRIPTOR[0u];
                        }
                    #else
                        pStr = (volatile uint8 *)&USBP_SN_STRING_DESCRIPTOR[0u];
                    #endif  /* (USBP_ENABLE_IDSN_STRING) */
                    }
                #endif /* (USBP_ENABLE_SN_STRING) */
                
                    if (*pStr != 0u)
                    {
                        USBP_currentTD.count = *pStr;
                        USBP_currentTD.pData = pStr;
                        requestHandled  = USBP_InitControlRead();
                    }
                }
            #endif /*  USBP_ENABLE_STRINGS */
                else
                {
                    requestHandled = USBP_DispatchClassRqst();
                }
                break;
                
            case USBP_GET_STATUS:
                switch (bmRequestType & USBP_RQST_RCPT_MASK)
                {
                    case USBP_RQST_RCPT_EP:
                        USBP_currentTD.count = USBP_EP_STATUS_LENGTH;
                        USBP_tBuffer[0u]     = USBP_EP[USBP_wIndexLoReg & USBP_DIR_UNUSED].hwEpState;
                        USBP_tBuffer[1u]     = 0u;
                        USBP_currentTD.pData = &USBP_tBuffer[0u];
                        
                        requestHandled  = USBP_InitControlRead();
                        break;
                    case USBP_RQST_RCPT_DEV:
                        USBP_currentTD.count = USBP_DEVICE_STATUS_LENGTH;
                        USBP_tBuffer[0u]     = USBP_deviceStatus;
                        USBP_tBuffer[1u]     = 0u;
                        USBP_currentTD.pData = &USBP_tBuffer[0u];
                        
                        requestHandled  = USBP_InitControlRead();
                        break;
                    default:    /* requestHandled is initialized as FALSE by default */
                        break;
                }
                break;
                
            case USBP_GET_CONFIGURATION:
                USBP_currentTD.count = 1u;
                USBP_currentTD.pData = (volatile uint8 *) &USBP_configuration;
                requestHandled  = USBP_InitControlRead();
                break;
                
            case USBP_GET_INTERFACE:
                USBP_currentTD.count = 1u;
                USBP_currentTD.pData = (volatile uint8 *) &USBP_interfaceSetting[USBP_wIndexLoReg];
                requestHandled  = USBP_InitControlRead();
                break;
                
            default: /* requestHandled is initialized as FALSE by default */
                break;
        }
    }
    else
    {
        /* Control Write */
        switch (USBP_bRequestReg)
        {
            case USBP_SET_ADDRESS:
                /* Store address to be set in USBP_NoDataControlStatusStage(). */
                USBP_deviceAddress = (uint8) USBP_wValueLoReg;
                requestHandled = USBP_InitNoDataControlTransfer();
                break;
                
            case USBP_SET_CONFIGURATION:
                configurationN = USBP_wValueLoReg;
                
                /* Verify that configuration descriptor exists */
                if(configurationN > 0u)
                {
                    pTmp = USBP_GetConfigTablePtr((uint8) configurationN - 1u);
                }
                
                /* Responds with a Request Error when configuration number is invalid */
                if (((configurationN > 0u) && (pTmp != NULL)) || (configurationN == 0u))
                {
                    /* Set new configuration if it has been changed */
                    if(configurationN != USBP_configuration)
                    {
                        USBP_configuration = (uint8) configurationN;
                        USBP_configurationChanged = USBP_TRUE;
                        USBP_Config(USBP_TRUE);
                    }
                    requestHandled = USBP_InitNoDataControlTransfer();
                }
                break;
                
            case USBP_SET_INTERFACE:
                if (0u != USBP_ValidateAlternateSetting())
                {
                    /* Get interface number from the request. */
                    interfaceNumber = USBP_wIndexLoReg;
                    USBP_interfaceNumber = (uint8) USBP_wIndexLoReg;
                     
                    /* Check if alternate settings is changed for interface. */
                    if (USBP_interfaceSettingLast[interfaceNumber] != USBP_interfaceSetting[interfaceNumber])
                    {
                        USBP_configurationChanged = USBP_TRUE;
                    
                        /* Change alternate setting for the endpoints. */
                    #if (USBP_EP_MANAGEMENT_MANUAL && USBP_EP_ALLOC_DYNAMIC)
                        USBP_Config(USBP_FALSE);
                    #else
                        USBP_ConfigAltChanged();
                    #endif /* (USBP_EP_MANAGEMENT_MANUAL && USBP_EP_ALLOC_DYNAMIC) */
                    }
                    
                    requestHandled = USBP_InitNoDataControlTransfer();
                }
                break;
                
            case USBP_CLEAR_FEATURE:
                switch (bmRequestType & USBP_RQST_RCPT_MASK)
                {
                    case USBP_RQST_RCPT_EP:
                        if (USBP_wValueLoReg == USBP_ENDPOINT_HALT)
                        {
                            requestHandled = USBP_ClearEndpointHalt();
                        }
                        break;
                    case USBP_RQST_RCPT_DEV:
                        /* Clear device REMOTE_WAKEUP */
                        if (USBP_wValueLoReg == USBP_DEVICE_REMOTE_WAKEUP)
                        {
                            USBP_deviceStatus &= (uint8)~USBP_DEVICE_STATUS_REMOTE_WAKEUP;
                            requestHandled = USBP_InitNoDataControlTransfer();
                        }
                        break;
                    case USBP_RQST_RCPT_IFC:
                        /* Validate interfaceNumber */
                        if (USBP_wIndexLoReg < USBP_MAX_INTERFACES_NUMBER)
                        {
                            USBP_interfaceStatus[USBP_wIndexLoReg] &= (uint8) ~USBP_wValueLoReg;
                            requestHandled = USBP_InitNoDataControlTransfer();
                        }
                        break;
                    default:    /* requestHandled is initialized as FALSE by default */
                        break;
                }
                break;
                
            case USBP_SET_FEATURE:
                switch (bmRequestType & USBP_RQST_RCPT_MASK)
                {
                    case USBP_RQST_RCPT_EP:
                        if (USBP_wValueLoReg == USBP_ENDPOINT_HALT)
                        {
                            requestHandled = USBP_SetEndpointHalt();
                        }
                        break;
                        
                    case USBP_RQST_RCPT_DEV:
                        /* Set device REMOTE_WAKEUP */
                        if (USBP_wValueLoReg == USBP_DEVICE_REMOTE_WAKEUP)
                        {
                            USBP_deviceStatus |= USBP_DEVICE_STATUS_REMOTE_WAKEUP;
                            requestHandled = USBP_InitNoDataControlTransfer();
                        }
                        break;
                        
                    case USBP_RQST_RCPT_IFC:
                        /* Validate interfaceNumber */
                        if (USBP_wIndexLoReg < USBP_MAX_INTERFACES_NUMBER)
                        {
                            USBP_interfaceStatus[USBP_wIndexLoReg] &= (uint8) ~USBP_wValueLoReg;
                            requestHandled = USBP_InitNoDataControlTransfer();
                        }
                        break;
                    
                    default:    /* requestHandled is initialized as FALSE by default */
                        break;
                }
                break;
                
            default:    /* requestHandled is initialized as FALSE by default */
                break;
        }
    }
    
    return (requestHandled);
}


#if defined(USBP_ENABLE_IDSN_STRING)
    /***************************************************************************
    * Function Name: USBP_ReadDieID
    ************************************************************************//**
    *
    *  This routine read Die ID and generate Serial Number string descriptor.
    *
    *  \param descr:  pointer on string descriptor. This string size has to be equal or
    *          greater than USBP_IDSN_DESCR_LENGTH.
    *
    *
    * \reentrant
    *  No.
    *
    ***************************************************************************/
    void USBP_ReadDieID(uint8 descr[]) 
    {
        const char8 CYCODE hex[] = "0123456789ABCDEF";
        uint8 i;
        uint8 j = 0u;
        uint8 uniqueId[8u];

        if (NULL != descr)
        {
            /* Initialize descriptor header. */
            descr[0u] = USBP_IDSN_DESCR_LENGTH;
            descr[1u] = USBP_DESCR_STRING;
            
            /* Unique ID size is 8 bytes. */
            CyGetUniqueId((uint32 *) uniqueId);

            /* Fill descriptor with unique device ID. */
            for (i = 2u; i < USBP_IDSN_DESCR_LENGTH; i += 4u)
            {
                descr[i]      = (uint8) hex[(uniqueId[j] >> 4u)];
                descr[i + 1u] = 0u;
                descr[i + 2u] = (uint8) hex[(uniqueId[j] & 0x0Fu)];
                descr[i + 3u] = 0u;
                ++j;
            }
        }
    }
#endif /* (USBP_ENABLE_IDSN_STRING) */


/*******************************************************************************
* Function Name: USBP_ConfigReg
****************************************************************************//**
*
*  This routine configures hardware registers from the variables.
*  It is called from USBP_Config() function and from RestoreConfig
*  after Wakeup.
*
*******************************************************************************/
void USBP_ConfigReg(void) 
{
    uint8 ep;

#if (USBP_EP_MANAGEMENT_DMA_AUTO)
    uint8 epType = 0u;
#endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */

    /* Go thought all endpoints and set hardware configuration */
    for (ep = USBP_EP1; ep < USBP_MAX_EP; ++ep)
    {
        USBP_ARB_EP_BASE.arbEp[ep].epCfg = USBP_ARB_EPX_CFG_DEFAULT;
        
    #if (USBP_EP_MANAGEMENT_DMA)
        /* Enable arbiter endpoint interrupt sources */
        USBP_ARB_EP_BASE.arbEp[ep].epIntEn = USBP_ARB_EPX_INT_MASK;
    #endif /* (USBP_EP_MANAGEMENT_DMA) */
    
        if (USBP_EP[ep].epMode != USBP_MODE_DISABLE)
        {
            if (0u != (USBP_EP[ep].addr & USBP_DIR_IN))
            {
                USBP_SIE_EP_BASE.sieEp[ep].epCr0 = USBP_MODE_NAK_IN;
                
            #if (USBP_EP_MANAGEMENT_DMA_AUTO && CY_PSOC4)
                /* Clear DMA_TERMIN for IN endpoint. */
                USBP_ARB_EP_BASE.arbEp[ep].epIntEn &= (uint32) ~USBP_ARB_EPX_INT_DMA_TERMIN;
            #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO && CY_PSOC4) */
            }
            else
            {
                USBP_SIE_EP_BASE.sieEp[ep].epCr0 = USBP_MODE_NAK_OUT;

            #if (USBP_EP_MANAGEMENT_DMA_AUTO)
                /* (CY_PSOC4): DMA_TERMIN for OUT endpoint is set above. */
                
                /* Prepare endpoint type mask. */
                epType |= (uint8) (0x01u << (ep - USBP_EP1));
            #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
            }
        }
        else
        {
            USBP_SIE_EP_BASE.sieEp[ep].epCr0 = USBP_MODE_STALL_DATA_EP;
        }
        
    #if (!USBP_EP_MANAGEMENT_DMA_AUTO)
        #if (CY_PSOC4)
            USBP_ARB_EP16_BASE.arbEp[ep].rwRa16  = (uint32) USBP_EP[ep].buffOffset;
            USBP_ARB_EP16_BASE.arbEp[ep].rwWa16  = (uint32) USBP_EP[ep].buffOffset;
        #else
            USBP_ARB_EP_BASE.arbEp[ep].rwRa    = LO8(USBP_EP[ep].buffOffset);
            USBP_ARB_EP_BASE.arbEp[ep].rwRaMsb = HI8(USBP_EP[ep].buffOffset);
            USBP_ARB_EP_BASE.arbEp[ep].rwWa    = LO8(USBP_EP[ep].buffOffset);
            USBP_ARB_EP_BASE.arbEp[ep].rwWaMsb = HI8(USBP_EP[ep].buffOffset);
        #endif /* (CY_PSOC4) */
    #endif /* (!USBP_EP_MANAGEMENT_DMA_AUTO) */
    }

#if (USBP_EP_MANAGEMENT_DMA_AUTO)
     /* BUF_SIZE depend on DMA_THRESS value:0x55-32 bytes  0x44-16 bytes 0x33-8 bytes 0x22-4 bytes 0x11-2 bytes */
    USBP_BUF_SIZE_REG = USBP_DMA_BUF_SIZE;

    /* Configure DMA burst threshold */
#if (CY_PSOC4)
    USBP_DMA_THRES16_REG   = USBP_DMA_BYTES_PER_BURST;
#else
    USBP_DMA_THRES_REG     = USBP_DMA_BYTES_PER_BURST;
    USBP_DMA_THRES_MSB_REG = 0u;
#endif /* (CY_PSOC4) */
    USBP_EP_ACTIVE_REG = USBP_DEFAULT_ARB_INT_EN;
    USBP_EP_TYPE_REG   = epType;
    
    /* Cfg_cmp bit set to 1 once configuration is complete. */
    /* Lock arbiter configtuation */
    USBP_ARB_CFG_REG |= (uint8)  USBP_ARB_CFG_CFG_CMP;
    /* Cfg_cmp bit set to 0 during configuration of PFSUSB Registers. */
    USBP_ARB_CFG_REG &= (uint8) ~USBP_ARB_CFG_CFG_CMP;

#endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */

    /* Enable interrupt SIE interurpt source from EP0-EP1 */
    USBP_SIE_EP_INT_EN_REG = (uint8) USBP_DEFAULT_SIE_EP_INT_EN;
}


/*******************************************************************************
* Function Name: USBP_EpStateInit
****************************************************************************//**
*
*  This routine initialize state of Data end points based of its type: 
*   IN  - USBP_IN_BUFFER_EMPTY (USBP_EVENT_PENDING)
*   OUT - USBP_OUT_BUFFER_EMPTY (USBP_NO_EVENT_PENDING)
*
*******************************************************************************/
void USBP_EpStateInit(void) 
{
    uint8 i;

    for (i = USBP_EP1; i < USBP_MAX_EP; i++)
    { 
        if (0u != (USBP_EP[i].addr & USBP_DIR_IN))
        {
            /* IN Endpoint */
            USBP_EP[i].apiEpState = USBP_EVENT_PENDING;
        }
        else
        {
            /* OUT Endpoint */
            USBP_EP[i].apiEpState = USBP_NO_EVENT_PENDING;
        }
    }
                    
}


/*******************************************************************************
* Function Name: USBP_Config
****************************************************************************//**
*
*  This routine configures endpoints for the entire configuration by scanning
*  the configuration descriptor.
*
*  \param clearAltSetting: It configures the bAlternateSetting 0 for each interface.
*
* USBP_interfaceClass - Initialized class array for each interface.
*   It is used for handling Class specific requests depend on interface class.
*   Different classes in multiple Alternate settings does not supported.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_Config(uint8 clearAltSetting) 
{
    uint8 ep;
    uint8 curEp;
    uint8 i;
    uint8 epType;
    const uint8 *pDescr;
    
    #if (!USBP_EP_MANAGEMENT_DMA_AUTO)
        uint16 buffCount = 0u;
    #endif /* (!USBP_EP_MANAGEMENT_DMA_AUTO) */

    const T_USBP_LUT CYCODE *pTmp;
    const T_USBP_EP_SETTINGS_BLOCK CYCODE *pEP;

    /* Clear endpoints settings */
    for (ep = 0u; ep < USBP_MAX_EP; ++ep)
    {
        USBP_EP[ep].attrib     = 0u;
        USBP_EP[ep].hwEpState  = 0u;
        USBP_EP[ep].epToggle   = 0u;
        USBP_EP[ep].bufferSize = 0u;
        USBP_EP[ep].interface  = 0u;
        USBP_EP[ep].apiEpState = USBP_NO_EVENT_PENDING;
        USBP_EP[ep].epMode     = USBP_MODE_DISABLE;   
    }

    /* Clear Alternate settings for all interfaces. */
    if (0u != clearAltSetting)
    {
        for (i = 0u; i < USBP_MAX_INTERFACES_NUMBER; ++i)
        {
            USBP_interfaceSetting[i]     = 0u;
            USBP_interfaceSettingLast[i] = 0u;
        }
    }

    /* Init Endpoints and Device Status if configured */
    if (USBP_configuration > 0u)
    {
        #if defined(USBP_ENABLE_CDC_CLASS)
            uint8 cdcComNums = 0u;
        #endif  /* (USBP_ENABLE_CDC_CLASS) */  

        pTmp = USBP_GetConfigTablePtr(USBP_configuration - 1u);
        
        /* Set Power status for current configuration */
        pDescr = (const uint8 *)pTmp->p_list;
        if ((pDescr[USBP_CONFIG_DESCR_ATTRIB] & USBP_CONFIG_DESCR_ATTRIB_SELF_POWERED) != 0u)
        {
            USBP_deviceStatus |= (uint8)  USBP_DEVICE_STATUS_SELF_POWERED;
        }
        else
        {
            USBP_deviceStatus &= (uint8) ~USBP_DEVICE_STATUS_SELF_POWERED;
        }
        
        /* Move to next element */
        pTmp = &pTmp[1u];
        ep = pTmp->c;  /* For this table, c is the number of endpoints configurations  */

        #if (USBP_EP_MANAGEMENT_MANUAL && USBP_EP_ALLOC_DYNAMIC)
            /* Configure for dynamic EP memory allocation */
            /* p_list points the endpoint setting table. */
            pEP = (T_USBP_EP_SETTINGS_BLOCK *) pTmp->p_list;
            
            for (i = 0u; i < ep; i++)
            {     
                /* Compare current Alternate setting with EP Alt */
                if (USBP_interfaceSetting[pEP->interface] == pEP->altSetting)
                {                                                          
                    curEp  = pEP->addr & USBP_DIR_UNUSED;
                    epType = pEP->attributes & USBP_EP_TYPE_MASK;
                    
                    USBP_EP[curEp].addr       = pEP->addr;
                    USBP_EP[curEp].attrib     = pEP->attributes;
                    USBP_EP[curEp].bufferSize = pEP->bufferSize;

                    if (0u != (pEP->addr & USBP_DIR_IN))
                    {
                        /* IN Endpoint */
                        USBP_EP[curEp].epMode     = USBP_GET_ACTIVE_IN_EP_CR0_MODE(epType);
                        USBP_EP[curEp].apiEpState = USBP_EVENT_PENDING;
                    
                    #if (defined(USBP_ENABLE_MIDI_STREAMING) && (USBP_MIDI_IN_BUFF_SIZE > 0))
                        if ((pEP->bMisc == USBP_CLASS_AUDIO) && (epType == USBP_EP_TYPE_BULK))
                        {
                            USBP_midi_in_ep = curEp;
                        }
                    #endif  /* (USBP_ENABLE_MIDI_STREAMING) */
                    }
                    else
                    {
                        /* OUT Endpoint */
                        USBP_EP[curEp].epMode     = USBP_GET_ACTIVE_OUT_EP_CR0_MODE(epType);
                        USBP_EP[curEp].apiEpState = USBP_NO_EVENT_PENDING;
                        
                    #if (defined(USBP_ENABLE_MIDI_STREAMING) && (USBP_MIDI_OUT_BUFF_SIZE > 0))
                        if ((pEP->bMisc == USBP_CLASS_AUDIO) && (epType == USBP_EP_TYPE_BULK))
                        {
                            USBP_midi_out_ep = curEp;
                        }
                    #endif  /* (USBP_ENABLE_MIDI_STREAMING) */
                    }

                #if(defined (USBP_ENABLE_CDC_CLASS))
                    if((pEP->bMisc == USBP_CLASS_CDC_DATA) ||(pEP->bMisc == USBP_CLASS_CDC))
                    {
                        cdcComNums = USBP_Cdc_EpInit(pEP, curEp, cdcComNums);
                    }
                #endif  /* (USBP_ENABLE_CDC_CLASS) */
                }
                
                pEP = &pEP[1u];
            }
            
        #else
            for (i = USBP_EP1; i < USBP_MAX_EP; ++i)
            {
                /* p_list points the endpoint setting table. */
                pEP = (const T_USBP_EP_SETTINGS_BLOCK CYCODE *) pTmp->p_list;
                /* Find max length for each EP and select it (length could be different in different Alt settings) */
                /* but other settings should be correct with regards to Interface alt Setting */
                
                for (curEp = 0u; curEp < ep; ++curEp)
                {
                    if (i == (pEP->addr & USBP_DIR_UNUSED))
                    {
                        /* Compare endpoint buffers size with current size to find greater. */
                        if (USBP_EP[i].bufferSize < pEP->bufferSize)
                        {
                            USBP_EP[i].bufferSize = pEP->bufferSize;
                        }
                        
                        /* Compare current Alternate setting with EP Alt */
                        if (USBP_interfaceSetting[pEP->interface] == pEP->altSetting)
                        {                            
                            USBP_EP[i].addr = pEP->addr;
                            USBP_EP[i].attrib = pEP->attributes;
                            
                            epType = pEP->attributes & USBP_EP_TYPE_MASK;
                            
                            if (0u != (pEP->addr & USBP_DIR_IN))
                            {
                                /* IN Endpoint */
                                USBP_EP[i].epMode     = USBP_GET_ACTIVE_IN_EP_CR0_MODE(epType);
                                USBP_EP[i].apiEpState = USBP_EVENT_PENDING;
                                
                            #if (defined(USBP_ENABLE_MIDI_STREAMING) && (USBP_MIDI_IN_BUFF_SIZE > 0))
                                if ((pEP->bMisc == USBP_CLASS_AUDIO) && (epType == USBP_EP_TYPE_BULK))
                                {
                                    USBP_midi_in_ep = i;
                                }
                            #endif  /* (USBP_ENABLE_MIDI_STREAMING) */
                            }
                            else
                            {
                                /* OUT Endpoint */
                                USBP_EP[i].epMode     = USBP_GET_ACTIVE_OUT_EP_CR0_MODE(epType);
                                USBP_EP[i].apiEpState = USBP_NO_EVENT_PENDING;
                                
                            #if (defined(USBP_ENABLE_MIDI_STREAMING) && (USBP_MIDI_OUT_BUFF_SIZE > 0))
                                if ((pEP->bMisc == USBP_CLASS_AUDIO) && (epType == USBP_EP_TYPE_BULK))
                                {
                                    USBP_midi_out_ep = i;
                                }
                            #endif  /* (USBP_ENABLE_MIDI_STREAMING) */
                            }

                        #if (defined(USBP_ENABLE_CDC_CLASS))
                            if((pEP->bMisc == USBP_CLASS_CDC_DATA) ||(pEP->bMisc == USBP_CLASS_CDC))
                            {
                                cdcComNums = USBP_Cdc_EpInit(pEP, i, cdcComNums);
                            }
                        #endif  /* (USBP_ENABLE_CDC_CLASS) */

                            #if (USBP_EP_MANAGEMENT_DMA_AUTO)
                                break;  /* Use first EP setting in Auto memory management */
                            #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
                        }
                    }
                    
                    pEP = &pEP[1u];
                }
            }
        #endif /*  (USBP_EP_MANAGEMENT_MANUAL && USBP_EP_ALLOC_DYNAMIC) */

        /* Init class array for each interface and interface number for each EP.
        *  It is used for handling Class specific requests directed to either an
        *  interface or the endpoint.
        */
        /* p_list points the endpoint setting table. */
        pEP = (const T_USBP_EP_SETTINGS_BLOCK CYCODE *) pTmp->p_list;
        for (i = 0u; i < ep; i++)
        {
            /* Configure interface number for each EP */
            USBP_EP[pEP->addr & USBP_DIR_UNUSED].interface = pEP->interface;
            pEP = &pEP[1u];
        }
        
        /* Init pointer on interface class table */
        USBP_interfaceClass = USBP_GetInterfaceClassTablePtr();
        
    /* Set the endpoint buffer addresses */
    #if (!USBP_EP_MANAGEMENT_DMA_AUTO)
        buffCount = 0u;
        for (ep = USBP_EP1; ep < USBP_MAX_EP; ++ep)
        {
            USBP_EP[ep].buffOffset = buffCount;        
            buffCount += USBP_EP[ep].bufferSize;
            
        #if (USBP_GEN_16BITS_EP_ACCESS)
            /* Align EP buffers to be event size to access 16-bits DR register. */
            buffCount += (0u != (buffCount & 0x01u)) ? 1u : 0u;
        #endif /* (USBP_GEN_16BITS_EP_ACCESS) */            
        }
    #endif /* (!USBP_EP_MANAGEMENT_DMA_AUTO) */

        /* Configure hardware registers */
        USBP_ConfigReg();
    }
}


/*******************************************************************************
* Function Name: USBP_ConfigAltChanged
****************************************************************************//**
*
*  This routine update configuration for the required endpoints only.
*  It is called after SET_INTERFACE request when Static memory allocation used.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_ConfigAltChanged(void) 
{
    uint8 ep;
    uint8 curEp;
    uint8 epType;
    uint8 i;
    uint8 interfaceNum;

    const T_USBP_LUT CYCODE *pTmp;
    const T_USBP_EP_SETTINGS_BLOCK CYCODE *pEP;

    /* Init Endpoints and Device Status if configured */
    if (USBP_configuration > 0u)
    {
        /* Get number of endpoints configurations (ep). */
        pTmp = USBP_GetConfigTablePtr(USBP_configuration - 1u);
        pTmp = &pTmp[1u];
        ep = pTmp->c;

        /* Get pointer to endpoints setting table (pEP). */
        pEP = (const T_USBP_EP_SETTINGS_BLOCK CYCODE *) pTmp->p_list;
        
        /* Look through all possible endpoint configurations. Find endpoints 
        * which belong to current interface and alternate settings for 
        * re-configuration.
        */
        interfaceNum = USBP_interfaceNumber;
        for (i = 0u; i < ep; i++)
        {
            /* Find endpoints which belong to current interface and alternate settings. */
            if ((interfaceNum == pEP->interface) && 
                (USBP_interfaceSetting[interfaceNum] == pEP->altSetting))
            {
                curEp  = ((uint8) pEP->addr & USBP_DIR_UNUSED);
                epType = ((uint8) pEP->attributes & USBP_EP_TYPE_MASK);
                
                /* Change the SIE mode for the selected EP to NAK ALL */
                USBP_EP[curEp].epToggle   = 0u;
                USBP_EP[curEp].addr       = pEP->addr;
                USBP_EP[curEp].attrib     = pEP->attributes;
                USBP_EP[curEp].bufferSize = pEP->bufferSize;

                if (0u != (pEP->addr & USBP_DIR_IN))
                {
                    /* IN Endpoint */
                    USBP_EP[curEp].epMode     = USBP_GET_ACTIVE_IN_EP_CR0_MODE(epType);
                    USBP_EP[curEp].apiEpState = USBP_EVENT_PENDING;
                }
                else
                {
                    /* OUT Endpoint */
                    USBP_EP[curEp].epMode     = USBP_GET_ACTIVE_OUT_EP_CR0_MODE(epType);
                    USBP_EP[curEp].apiEpState = USBP_NO_EVENT_PENDING;
                }
                
                /* Make SIE to NAK any endpoint requests */
                USBP_SIE_EP_BASE.sieEp[curEp].epCr0 = USBP_MODE_NAK_IN_OUT;

            #if (USBP_EP_MANAGEMENT_DMA_AUTO)
                /* Clear IN data ready. */
                USBP_ARB_EP_BASE.arbEp[curEp].epCfg &= (uint8) ~USBP_ARB_EPX_CFG_IN_DATA_RDY;

                /* Select endpoint number of reconfiguration */
                USBP_DYN_RECONFIG_REG = (uint8) ((curEp - 1u) << USBP_DYN_RECONFIG_EP_SHIFT);
                
                /* Request for dynamic re-configuration of endpoint. */
                USBP_DYN_RECONFIG_REG |= USBP_DYN_RECONFIG_ENABLE;
                
                /* Wait until block is ready for re-configuration */
                while (0u == (USBP_DYN_RECONFIG_REG & USBP_DYN_RECONFIG_RDY_STS))
                {
                }
                
                /* Once DYN_RECONFIG_RDY_STS bit is set, FW can change the EP configuration. */
                /* Change EP Type with new direction */
                if (0u != (pEP->addr & USBP_DIR_IN))
                {
                    /* Set endpoint type: 0 - IN and 1 - OUT. */
                    USBP_EP_TYPE_REG &= (uint8) ~(uint8)((uint8) 0x01u << (curEp - 1u));
                    
                #if (CY_PSOC4)
                    /* Clear DMA_TERMIN for IN endpoint */
                    USBP_ARB_EP_BASE.arbEp[curEp].epIntEn &= (uint32) ~USBP_ARB_EPX_INT_DMA_TERMIN;
                #endif /* (CY_PSOC4) */
                }
                else
                {
                    /* Set endpoint type: 0 - IN and 1- OUT. */
                    USBP_EP_TYPE_REG |= (uint8) ((uint8) 0x01u << (curEp - 1u));
                    
                #if (CY_PSOC4)
                    /* Set DMA_TERMIN for OUT endpoint */
                    USBP_ARB_EP_BASE.arbEp[curEp].epIntEn |= (uint32) USBP_ARB_EPX_INT_DMA_TERMIN;
                #endif /* (CY_PSOC4) */
                }
                
                /* Complete dynamic re-configuration: all endpoint related status and signals 
                * are set into the default state.
                */
                USBP_DYN_RECONFIG_REG &= (uint8) ~USBP_DYN_RECONFIG_ENABLE;

            #else
                USBP_SIE_EP_BASE.sieEp[curEp].epCnt0 = HI8(USBP_EP[curEp].bufferSize);
                USBP_SIE_EP_BASE.sieEp[curEp].epCnt1 = LO8(USBP_EP[curEp].bufferSize);
                
                #if (CY_PSOC4)
                    USBP_ARB_EP16_BASE.arbEp[curEp].rwRa16  = (uint32) USBP_EP[curEp].buffOffset;
                    USBP_ARB_EP16_BASE.arbEp[curEp].rwWa16  = (uint32) USBP_EP[curEp].buffOffset;
                #else
                    USBP_ARB_EP_BASE.arbEp[curEp].rwRa    = LO8(USBP_EP[curEp].buffOffset);
                    USBP_ARB_EP_BASE.arbEp[curEp].rwRaMsb = HI8(USBP_EP[curEp].buffOffset);
                    USBP_ARB_EP_BASE.arbEp[curEp].rwWa    = LO8(USBP_EP[curEp].buffOffset);
                    USBP_ARB_EP_BASE.arbEp[curEp].rwWaMsb = HI8(USBP_EP[curEp].buffOffset);
                #endif /* (CY_PSOC4) */                
            #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
            }
            
            pEP = &pEP[1u]; /* Get next EP element */
        }
        
        /* The main loop has to re-enable DMA and OUT endpoint */
    }
}


/*******************************************************************************
* Function Name: USBP_GetConfigTablePtr
****************************************************************************//**
*
*  This routine returns a pointer a configuration table entry
*
*  \param confIndex:  Configuration Index
*
* \return
*  Device Descriptor pointer or NULL when descriptor does not exist.
*
*******************************************************************************/
const T_USBP_LUT CYCODE *USBP_GetConfigTablePtr(uint8 confIndex)
                                                        
{
    /* Device Table */
    const T_USBP_LUT CYCODE *pTmp;

    pTmp = (const T_USBP_LUT CYCODE *) USBP_TABLE[USBP_device].p_list;

    /* The first entry points to the Device Descriptor,
    *  the second entry point to the BOS Descriptor
    *  the rest configuration entries.
    *  Set pointer to the first Configuration Descriptor
    */
    pTmp = &pTmp[2u];
    /* For this table, c is the number of configuration descriptors  */
    if(confIndex >= pTmp->c)   /* Verify that required configuration descriptor exists */
    {
        pTmp = (const T_USBP_LUT CYCODE *) NULL;
    }
    else
    {
        pTmp = (const T_USBP_LUT CYCODE *) pTmp[confIndex].p_list;
    }

    return (pTmp);
}


#if (USBP_BOS_ENABLE)
    /*******************************************************************************
    * Function Name: USBP_GetBOSPtr
    ****************************************************************************//**
    *
    *  This routine returns a pointer a BOS table entry
    *
    *  
    *
    * \return
    *  BOS Descriptor pointer or NULL when descriptor does not exist.
    *
    *******************************************************************************/
    const T_USBP_LUT CYCODE *USBP_GetBOSPtr(void)
                                                            
    {
        /* Device Table */
        const T_USBP_LUT CYCODE *pTmp;

        pTmp = (const T_USBP_LUT CYCODE *) USBP_TABLE[USBP_device].p_list;

        /* The first entry points to the Device Descriptor,
        *  the second entry points to the BOS Descriptor
        */
        pTmp = &pTmp[1u];
        pTmp = (const T_USBP_LUT CYCODE *) pTmp->p_list;
        return (pTmp);
    }
#endif /* (USBP_BOS_ENABLE) */


/*******************************************************************************
* Function Name: USBP_GetDeviceTablePtr
****************************************************************************//**
*
*  This routine returns a pointer to the Device table
*
* \return
*  Device Table pointer
*
*******************************************************************************/
const T_USBP_LUT CYCODE *USBP_GetDeviceTablePtr(void)
                                                            
{
    /* Device Table */
    return( (const T_USBP_LUT CYCODE *) USBP_TABLE[USBP_device].p_list );
}


/*******************************************************************************
* Function Name: USB_GetInterfaceClassTablePtr
****************************************************************************//**
*
*  This routine returns Interface Class table pointer, which contains
*  the relation between interface number and interface class.
*
* \return
*  Interface Class table pointer.
*
*******************************************************************************/
const uint8 CYCODE *USBP_GetInterfaceClassTablePtr(void)
                                                        
{
    const T_USBP_LUT CYCODE *pTmp;
    const uint8 CYCODE *pInterfaceClass;
    uint8 currentInterfacesNum;

    pTmp = USBP_GetConfigTablePtr(USBP_configuration - 1u);
    if (pTmp != NULL)
    {
        currentInterfacesNum  = ((const uint8 *) pTmp->p_list)[USBP_CONFIG_DESCR_NUM_INTERFACES];
        /* Third entry in the LUT starts the Interface Table pointers */
        /* The INTERFACE_CLASS table is located after all interfaces */
        pTmp = &pTmp[currentInterfacesNum + 2u];
        pInterfaceClass = (const uint8 CYCODE *) pTmp->p_list;
    }
    else
    {
        pInterfaceClass = (const uint8 CYCODE *) NULL;
    }

    return (pInterfaceClass);
}


/*******************************************************************************
* Function Name: USBP_TerminateEP
****************************************************************************//**
*
*  This function terminates the specified USBFS endpoint.
*  This function should be used before endpoint reconfiguration.
*
*  \param ep Contains the data endpoint number.
*
*  \reentrant
*  No.
*
* \sideeffect
* 
* The device responds with a NAK for any transactions on the selected endpoint.
*   
*******************************************************************************/
void USBP_TerminateEP(uint8 epNumber) 
{
    /* Get endpoint number */
    epNumber &= USBP_DIR_UNUSED;

    if ((epNumber > USBP_EP0) && (epNumber < USBP_MAX_EP))
    {
        /* Set the endpoint Halt */
        USBP_EP[epNumber].hwEpState |= USBP_ENDPOINT_STATUS_HALT;

        /* Clear the data toggle */
        USBP_EP[epNumber].epToggle = 0u;
        USBP_EP[epNumber].apiEpState = USBP_NO_EVENT_ALLOWED;

        if ((USBP_EP[epNumber].addr & USBP_DIR_IN) != 0u)
        {   
            /* IN Endpoint */
            USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBP_MODE_NAK_IN;
        }
        else
        {
            /* OUT Endpoint */
            USBP_SIE_EP_BASE.sieEp[epNumber].epCr0 = USBP_MODE_NAK_OUT;
        }
    }
}


/*******************************************************************************
* Function Name: USBP_SetEndpointHalt
****************************************************************************//**
*
*  This routine handles set endpoint halt.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_SetEndpointHalt(void) 
{
    uint8 requestHandled = USBP_FALSE;
    uint8 ep;
    
    /* Set endpoint halt */
    ep = USBP_wIndexLoReg & USBP_DIR_UNUSED;

    if ((ep > USBP_EP0) && (ep < USBP_MAX_EP))
    {
        /* Set the endpoint Halt */
        USBP_EP[ep].hwEpState |= (USBP_ENDPOINT_STATUS_HALT);

        /* Clear the data toggle */
        USBP_EP[ep].epToggle = 0u;
        USBP_EP[ep].apiEpState |= USBP_NO_EVENT_ALLOWED;

        if ((USBP_EP[ep].addr & USBP_DIR_IN) != 0u)
        {
            /* IN Endpoint */
            USBP_SIE_EP_BASE.sieEp[ep].epCr0 = (USBP_MODE_STALL_DATA_EP | 
                                                            USBP_MODE_ACK_IN);
        }
        else
        {
            /* OUT Endpoint */
            USBP_SIE_EP_BASE.sieEp[ep].epCr0 = (USBP_MODE_STALL_DATA_EP | 
                                                            USBP_MODE_ACK_OUT);
        }
        requestHandled = USBP_InitNoDataControlTransfer();
    }

    return (requestHandled);
}


/*******************************************************************************
* Function Name: USBP_ClearEndpointHalt
****************************************************************************//**
*
*  This routine handles clear endpoint halt.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_ClearEndpointHalt(void) 
{
    uint8 requestHandled = USBP_FALSE;
    uint8 ep;

    /* Clear endpoint halt */
    ep = USBP_wIndexLoReg & USBP_DIR_UNUSED;

    if ((ep > USBP_EP0) && (ep < USBP_MAX_EP))
    {
        /* Clear the endpoint Halt */
        USBP_EP[ep].hwEpState &= (uint8) ~USBP_ENDPOINT_STATUS_HALT;

        /* Clear the data toggle */
        USBP_EP[ep].epToggle = 0u;
        
        /* Clear toggle bit for already armed packet */
        USBP_SIE_EP_BASE.sieEp[ep].epCnt0 &= (uint8) ~(uint8)USBP_EPX_CNT_DATA_TOGGLE;
        
        /* Return API State as it was defined before */
        USBP_EP[ep].apiEpState &= (uint8) ~USBP_NO_EVENT_ALLOWED;

        if ((USBP_EP[ep].addr & USBP_DIR_IN) != 0u)
        {
            /* IN Endpoint */
            if(USBP_EP[ep].apiEpState == USBP_IN_BUFFER_EMPTY)
            {       
                /* Wait for next packet from application */
                USBP_SIE_EP_BASE.sieEp[ep].epCr0 = USBP_MODE_NAK_IN;
            }
            else    /* Continue armed transfer */
            {
                USBP_SIE_EP_BASE.sieEp[ep].epCr0 = USBP_MODE_ACK_IN;
            }
        }
        else
        {
            /* OUT Endpoint */
            if (USBP_EP[ep].apiEpState == USBP_OUT_BUFFER_FULL)
            {       
                /* Allow application to read full buffer */
                USBP_SIE_EP_BASE.sieEp[ep].epCr0 = USBP_MODE_NAK_OUT;
            }
            else    /* Mark endpoint as empty, so it will be reloaded */
            {
                USBP_SIE_EP_BASE.sieEp[ep].epCr0 = USBP_MODE_ACK_OUT;
            }
        }
        
        requestHandled = USBP_InitNoDataControlTransfer();
    }

    return(requestHandled);
}


/*******************************************************************************
* Function Name: USBP_ValidateAlternateSetting
****************************************************************************//**
*
*  Validates (and records) a SET INTERFACE request.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_ValidateAlternateSetting(void) 
{
    uint8 requestHandled = USBP_FALSE;
    
    uint8 interfaceNum;
    uint8 curInterfacesNum;
    const T_USBP_LUT CYCODE *pTmp;
    
    /* Get interface number from the request. */
    interfaceNum = (uint8) USBP_wIndexLoReg;
    
    /* Get number of interfaces for current configuration. */
    pTmp = USBP_GetConfigTablePtr(USBP_configuration - 1u);
    curInterfacesNum  = ((const uint8 *) pTmp->p_list)[USBP_CONFIG_DESCR_NUM_INTERFACES];

    /* Validate that interface number is within range. */
    if ((interfaceNum <= curInterfacesNum) || (interfaceNum <= USBP_MAX_INTERFACES_NUMBER))
    {
        /* Save current and new alternate settings (come with request) to make 
        * desicion about following endpoint re-configuration.
        */
        USBP_interfaceSettingLast[interfaceNum] = USBP_interfaceSetting[interfaceNum];
        USBP_interfaceSetting[interfaceNum]     = (uint8) USBP_wValueLoReg;
        
        requestHandled = USBP_TRUE;
    }

    return (requestHandled);
}


/* [] END OF FILE */
