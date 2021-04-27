/***************************************************************************//**
* \file USBP_drv.c
* \version 3.20
*
* \brief
*  This file contains the Endpoint 0 Driver for the USBFS Component.  
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


/***************************************
* Global data allocation
***************************************/

volatile T_USBP_EP_CTL_BLOCK USBP_EP[USBP_MAX_EP];

/** Contains the current configuration number, which is set by the host using a 
 * SET_CONFIGURATION request. This variable is initialized to zero in 
 * USBFS_InitComponent() API and can be read by the USBFS_GetConfiguration() 
 * API.*/
volatile uint8 USBP_configuration;

/** Contains the current interface number.*/
volatile uint8 USBP_interfaceNumber;

/** This variable is set to one after SET_CONFIGURATION and SET_INTERFACE 
 *requests. It can be read by the USBFS_IsConfigurationChanged() API */
volatile uint8 USBP_configurationChanged;

/** Contains the current device address.*/
volatile uint8 USBP_deviceAddress;

/** This is a two-bit variable that contains power status in the bit 0 
 * (DEVICE_STATUS_BUS_POWERED or DEVICE_STATUS_SELF_POWERED) and remote wakeup 
 * status (DEVICE_STATUS_REMOTE_WAKEUP) in the bit 1. This variable is 
 * initialized to zero in USBFS_InitComponent() API, configured by the 
 * USBFS_SetPowerStatus() API. The remote wakeup status cannot be set using the 
 * API SetPowerStatus(). */
volatile uint8 USBP_deviceStatus;

volatile uint8 USBP_interfaceSetting[USBP_MAX_INTERFACES_NUMBER];
volatile uint8 USBP_interfaceSetting_last[USBP_MAX_INTERFACES_NUMBER];
volatile uint8 USBP_interfaceStatus[USBP_MAX_INTERFACES_NUMBER];

/** Contains the started device number. This variable is set by the 
 * USBFS_Start() or USBFS_InitComponent() APIs.*/
volatile uint8 USBP_device;

/** Initialized class array for each interface. It is used for handling Class 
 * specific requests depend on interface class. Different classes in multiple 
 * alternate settings are not supported.*/
const uint8 CYCODE *USBP_interfaceClass;


/***************************************
* Local data allocation
***************************************/

volatile uint8  USBP_ep0Toggle;
volatile uint8  USBP_lastPacketSize;

/** This variable is used by the communication functions to handle the current 
* transfer state.
* Initialized to TRANS_STATE_IDLE in the USBFS_InitComponent() API and after a 
* complete transfer in the status stage.
* Changed to the TRANS_STATE_CONTROL_READ or TRANS_STATE_CONTROL_WRITE in setup 
* transaction depending on the request type.
*/
volatile uint8  USBP_transferState;
volatile T_USBP_TD USBP_currentTD;
volatile uint8  USBP_ep0Mode;
volatile uint8  USBP_ep0Count;
volatile uint16 USBP_transferByteCount;


/*******************************************************************************
* Function Name: USBP_ep_0_Interrupt
****************************************************************************//**
*
*  This Interrupt Service Routine handles Endpoint 0 (Control Pipe) traffic.
*  It dispatches setup requests and handles the data and status stages.
*
*
*******************************************************************************/
CY_ISR(USBP_EP_0_ISR)
{
    uint8 tempReg;
    uint8 modifyReg;

#ifdef USBP_EP_0_ISR_ENTRY_CALLBACK
    USBP_EP_0_ISR_EntryCallback();
#endif /* (USBP_EP_0_ISR_ENTRY_CALLBACK) */
    
    tempReg = USBP_EP0_CR_REG;
    if ((tempReg & USBP_MODE_ACKD) != 0u)
    {
        modifyReg = 1u;
        if ((tempReg & USBP_MODE_SETUP_RCVD) != 0u)
        {
            if ((tempReg & USBP_MODE_MASK) != USBP_MODE_NAK_IN_OUT)
            {
                /* Mode not equal to NAK_IN_OUT: invalid setup */
                modifyReg = 0u;
            }
            else
            {
                USBP_HandleSetup();
                
                if ((USBP_ep0Mode & USBP_MODE_SETUP_RCVD) != 0u)
                {
                    /* SETUP bit set: exit without mode modificaiton */
                    modifyReg = 0u;
                }
            }
        }
        else if ((tempReg & USBP_MODE_IN_RCVD) != 0u)
        {
            USBP_HandleIN();
        }
        else if ((tempReg & USBP_MODE_OUT_RCVD) != 0u)
        {
            USBP_HandleOUT();
        }
        else
        {
            modifyReg = 0u;
        }
        
        /* Modify the EP0_CR register */
        if (modifyReg != 0u)
        {
            
            tempReg = USBP_EP0_CR_REG;
            
            /* Make sure that SETUP bit is cleared before modification */
            if ((tempReg & USBP_MODE_SETUP_RCVD) == 0u)
            {
                /* Update count register */
                tempReg = (uint8) USBP_ep0Toggle | USBP_ep0Count;
                USBP_EP0_CNT_REG = tempReg;
               
                /* Make sure that previous write operaiton was successful */
                if (tempReg == USBP_EP0_CNT_REG)
                {
                    /* Repeat until next successful write operation */
                    do
                    {
                        /* Init temporary variable */
                        modifyReg = USBP_ep0Mode;
                        
                        /* Unlock register */
                        tempReg = (uint8) (USBP_EP0_CR_REG & USBP_MODE_SETUP_RCVD);
                        
                        /* Check if SETUP bit is not set */
                        if (0u == tempReg)
                        {
                            /* Set the Mode Register  */
                            USBP_EP0_CR_REG = USBP_ep0Mode;
                            
                            /* Writing check */
                            modifyReg = USBP_EP0_CR_REG & USBP_MODE_MASK;
                        }
                    }
                    while (modifyReg != USBP_ep0Mode);
                }
            }
        }
    }

    USBP_ClearSieInterruptSource(USBP_INTR_SIE_EP0_INTR);
	
#ifdef USBP_EP_0_ISR_EXIT_CALLBACK
    USBP_EP_0_ISR_ExitCallback();
#endif /* (USBP_EP_0_ISR_EXIT_CALLBACK) */
}


/*******************************************************************************
* Function Name: USBP_HandleSetup
****************************************************************************//**
*
*  This Routine dispatches requests for the four USB request types
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_HandleSetup(void) 
{
    uint8 requestHandled;
    
    /* Clear register lock by SIE (read register) and clear setup bit 
    * (write any value in register).
    */
    requestHandled = (uint8) USBP_EP0_CR_REG;
    USBP_EP0_CR_REG = (uint8) requestHandled;
    requestHandled = (uint8) USBP_EP0_CR_REG;

    if ((requestHandled & USBP_MODE_SETUP_RCVD) != 0u)
    {
        /* SETUP bit is set: exit without mode modification. */
        USBP_ep0Mode = requestHandled;
    }
    else
    {
        /* In case the previous transfer did not complete, close it out */
        USBP_UpdateStatusBlock(USBP_XFER_PREMATURE);

        /* Check request type. */
        switch (USBP_bmRequestTypeReg & USBP_RQST_TYPE_MASK)
        {
            case USBP_RQST_TYPE_STD:
                requestHandled = USBP_HandleStandardRqst();
                break;
                
            case USBP_RQST_TYPE_CLS:
                requestHandled = USBP_DispatchClassRqst();
                break;
                
            case USBP_RQST_TYPE_VND:
                requestHandled = USBP_HandleVendorRqst();
                break;
                
            default:
                requestHandled = USBP_FALSE;
                break;
        }
        
        /* If request is not recognized. Stall endpoint 0 IN and OUT. */
        if (requestHandled == USBP_FALSE)
        {
            USBP_ep0Mode = USBP_MODE_STALL_IN_OUT;
        }
    }
}


/*******************************************************************************
* Function Name: USBP_HandleIN
****************************************************************************//**
*
*  This routine handles EP0 IN transfers.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_HandleIN(void) 
{
    switch (USBP_transferState)
    {
        case USBP_TRANS_STATE_IDLE:
            break;
        
        case USBP_TRANS_STATE_CONTROL_READ:
            USBP_ControlReadDataStage();
            break;
            
        case USBP_TRANS_STATE_CONTROL_WRITE:
            USBP_ControlWriteStatusStage();
            break;
            
        case USBP_TRANS_STATE_NO_DATA_CONTROL:
            USBP_NoDataControlStatusStage();
            break;
            
        default:    /* there are no more states */
            break;
    }
}


/*******************************************************************************
* Function Name: USBP_HandleOUT
****************************************************************************//**
*
*  This routine handles EP0 OUT transfers.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_HandleOUT(void) 
{
    switch (USBP_transferState)
    {
        case USBP_TRANS_STATE_IDLE:
            break;
        
        case USBP_TRANS_STATE_CONTROL_READ:
            USBP_ControlReadStatusStage();
            break;
            
        case USBP_TRANS_STATE_CONTROL_WRITE:
            USBP_ControlWriteDataStage();
            break;
            
        case USBP_TRANS_STATE_NO_DATA_CONTROL:
            /* Update the completion block */
            USBP_UpdateStatusBlock(USBP_XFER_ERROR);
            
            /* We expect no more data, so stall INs and OUTs */
            USBP_ep0Mode = USBP_MODE_STALL_IN_OUT;
            break;
            
        default:    
            /* There are no more states */
            break;
    }
}


/*******************************************************************************
* Function Name: USBP_LoadEP0
****************************************************************************//**
*
*  This routine loads the EP0 data registers for OUT transfers. It uses the
*  currentTD (previously initialized by the _InitControlWrite function and
*  updated for each OUT transfer, and the bLastPacketSize) to determine how
*  many uint8s to transfer on the current OUT.
*
*  If the number of uint8s remaining is zero and the last transfer was full,
*  we need to send a zero length packet.  Otherwise we send the minimum
*  of the control endpoint size (8) or remaining number of uint8s for the
*  transaction.
*
*
* \globalvars
*  USBP_transferByteCount - Update the transfer byte count from the
*     last transaction.
*  USBP_ep0Count - counts the data loaded to the SIE memory in
*     current packet.
*  USBP_lastPacketSize - remembers the USBFS_ep0Count value for the
*     next packet.
*  USBP_transferByteCount - sum of the previous bytes transferred
*     on previous packets(sum of USBFS_lastPacketSize)
*  USBP_ep0Toggle - inverted
*  USBP_ep0Mode  - prepare for mode register content.
*  USBP_transferState - set to TRANS_STATE_CONTROL_READ
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_LoadEP0(void) 
{
    uint8 ep0Count = 0u;

    /* Update the transfer byte count from the last transaction */
    USBP_transferByteCount += USBP_lastPacketSize;

    /* Now load the next transaction */
    while ((USBP_currentTD.count > 0u) && (ep0Count < 8u))
    {
        USBP_EP0_DR_BASE.epData[ep0Count] = (uint8) *USBP_currentTD.pData;
        USBP_currentTD.pData = &USBP_currentTD.pData[1u];
        ep0Count++;
        USBP_currentTD.count--;
    }

    /* Support zero-length packet */
    if ((USBP_lastPacketSize == 8u) || (ep0Count > 0u))
    {
        /* Update the data toggle */
        USBP_ep0Toggle ^= USBP_EP0_CNT_DATA_TOGGLE;
        /* Set the Mode Register  */
        USBP_ep0Mode = USBP_MODE_ACK_IN_STATUS_OUT;
        /* Update the state (or stay the same) */
        USBP_transferState = USBP_TRANS_STATE_CONTROL_READ;
    }
    else
    {
        /* Expect Status Stage Out */
        USBP_ep0Mode = USBP_MODE_STATUS_OUT_ONLY;
        /* Update the state (or stay the same) */
        USBP_transferState = USBP_TRANS_STATE_CONTROL_READ;
    }

    /* Save the packet size for next time */
    USBP_ep0Count =       (uint8) ep0Count;
    USBP_lastPacketSize = (uint8) ep0Count;
}


/*******************************************************************************
* Function Name: USBP_InitControlRead
****************************************************************************//**
*
*  Initialize a control read transaction. It is used to send data to the host.
*  The following global variables should be initialized before this function
*  called. To send zero length packet use InitZeroLengthControlTransfer
*  function.
*
*
* \return
*  requestHandled state.
*
* \globalvars
*  USBP_currentTD.count - counts of data to be sent.
*  USBP_currentTD.pData - data pointer.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_InitControlRead(void) 
{
    uint16 xferCount;

    if (USBP_currentTD.count == 0u)
    {
        (void) USBP_InitZeroLengthControlTransfer();
    }
    else
    {
        /* Set up the state machine */
        USBP_transferState = USBP_TRANS_STATE_CONTROL_READ;
        
        /* Set the toggle, it gets updated in LoadEP */
        USBP_ep0Toggle = 0u;
        
        /* Initialize the Status Block */
        USBP_InitializeStatusBlock();
        
        xferCount = ((uint16)((uint16) USBP_lengthHiReg << 8u) | ((uint16) USBP_lengthLoReg));

        if (USBP_currentTD.count > xferCount)
        {
            USBP_currentTD.count = xferCount;
        }
        
        USBP_LoadEP0();
    }

    return (USBP_TRUE);
}


/*******************************************************************************
* Function Name: USBP_InitZeroLengthControlTransfer
****************************************************************************//**
*
*  Initialize a zero length data IN transfer.
*
* \return
*  requestHandled state.
*
* \globalvars
*  USBP_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  USBP_ep0Mode  - prepare for mode register content.
*  USBP_transferState - set to TRANS_STATE_CONTROL_READ
*  USBP_ep0Count - cleared, means the zero-length packet.
*  USBP_lastPacketSize - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_InitZeroLengthControlTransfer(void)
                                                
{
    /* Update the state */
    USBP_transferState = USBP_TRANS_STATE_CONTROL_READ;
    
    /* Set the data toggle */
    USBP_ep0Toggle = USBP_EP0_CNT_DATA_TOGGLE;
    
    /* Set the Mode Register  */
    USBP_ep0Mode = USBP_MODE_ACK_IN_STATUS_OUT;
    
    /* Save the packet size for next time */
    USBP_lastPacketSize = 0u;
    
    USBP_ep0Count = 0u;

    return (USBP_TRUE);
}


/*******************************************************************************
* Function Name: USBP_ControlReadDataStage
****************************************************************************//**
*
*  Handle the Data Stage of a control read transfer.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_ControlReadDataStage(void) 

{
    USBP_LoadEP0();
}


/*******************************************************************************
* Function Name: USBP_ControlReadStatusStage
****************************************************************************//**
*
*  Handle the Status Stage of a control read transfer.
*
*
* \globalvars
*  USBP_USBFS_transferByteCount - updated with last packet size.
*  USBP_transferState - set to TRANS_STATE_IDLE.
*  USBP_ep0Mode  - set to MODE_STALL_IN_OUT.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_ControlReadStatusStage(void) 
{
    /* Update the transfer byte count */
    USBP_transferByteCount += USBP_lastPacketSize;
    
    /* Go Idle */
    USBP_transferState = USBP_TRANS_STATE_IDLE;
    
    /* Update the completion block */
    USBP_UpdateStatusBlock(USBP_XFER_STATUS_ACK);
    
    /* We expect no more data, so stall INs and OUTs */
    USBP_ep0Mode = USBP_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: USBP_InitControlWrite
****************************************************************************//**
*
*  Initialize a control write transaction
*
* \return
*  requestHandled state.
*
* \globalvars
*  USBP_USBFS_transferState - set to TRANS_STATE_CONTROL_WRITE
*  USBP_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  USBP_ep0Mode  - set to MODE_ACK_OUT_STATUS_IN
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_InitControlWrite(void) 
{
    uint16 xferCount;

    /* Set up the state machine */
    USBP_transferState = USBP_TRANS_STATE_CONTROL_WRITE;
    
    /* This might not be necessary */
    USBP_ep0Toggle = USBP_EP0_CNT_DATA_TOGGLE;
    
    /* Initialize the Status Block */
    USBP_InitializeStatusBlock();

    xferCount = ((uint16)((uint16) USBP_lengthHiReg << 8u) | ((uint16) USBP_lengthLoReg));

    if (USBP_currentTD.count > xferCount)
    {
        USBP_currentTD.count = xferCount;
    }

    /* Expect Data or Status Stage */
    USBP_ep0Mode = USBP_MODE_ACK_OUT_STATUS_IN;

    return(USBP_TRUE);
}


/*******************************************************************************
* Function Name: USBP_ControlWriteDataStage
****************************************************************************//**
*
*  Handle the Data Stage of a control write transfer
*       1. Get the data (We assume the destination was validated previously)
*       2. Update the count and data toggle
*       3. Update the mode register for the next transaction
*
*
* \globalvars
*  USBP_transferByteCount - Update the transfer byte count from the
*    last transaction.
*  USBP_ep0Count - counts the data loaded from the SIE memory
*    in current packet.
*  USBP_transferByteCount - sum of the previous bytes transferred
*    on previous packets(sum of USBFS_lastPacketSize)
*  USBP_ep0Toggle - inverted
*  USBP_ep0Mode  - set to MODE_ACK_OUT_STATUS_IN.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_ControlWriteDataStage(void) 
{
    uint8 ep0Count;
    uint8 regIndex = 0u;

    ep0Count = (USBP_EP0_CNT_REG & USBP_EPX_CNT0_MASK) - USBP_EPX_CNTX_CRC_COUNT;

    USBP_transferByteCount += (uint8)ep0Count;

    while ((USBP_currentTD.count > 0u) && (ep0Count > 0u))
    {
        *USBP_currentTD.pData = (uint8) USBP_EP0_DR_BASE.epData[regIndex];
        USBP_currentTD.pData = &USBP_currentTD.pData[1u];
        regIndex++;
        ep0Count--;
        USBP_currentTD.count--;
    }
    
    USBP_ep0Count = (uint8)ep0Count;
    
    /* Update the data toggle */
    USBP_ep0Toggle ^= USBP_EP0_CNT_DATA_TOGGLE;
    
    /* Expect Data or Status Stage */
    USBP_ep0Mode = USBP_MODE_ACK_OUT_STATUS_IN;
}


/*******************************************************************************
* Function Name: USBP_ControlWriteStatusStage
****************************************************************************//**
*
*  Handle the Status Stage of a control write transfer
*
* \globalvars
*  USBP_transferState - set to TRANS_STATE_IDLE.
*  USBP_USBFS_ep0Mode  - set to MODE_STALL_IN_OUT.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_ControlWriteStatusStage(void) 
{
    /* Go Idle */
    USBP_transferState = USBP_TRANS_STATE_IDLE;
    
    /* Update the completion block */    
    USBP_UpdateStatusBlock(USBP_XFER_STATUS_ACK);
    
    /* We expect no more data, so stall INs and OUTs */
    USBP_ep0Mode = USBP_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: USBP_InitNoDataControlTransfer
****************************************************************************//**
*
*  Initialize a no data control transfer
*
* \return
*  requestHandled state.
*
* \globalvars
*  USBP_transferState - set to TRANS_STATE_NO_DATA_CONTROL.
*  USBP_ep0Mode  - set to MODE_STATUS_IN_ONLY.
*  USBP_ep0Count - cleared.
*  USBP_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 USBP_InitNoDataControlTransfer(void) 
{
    USBP_transferState = USBP_TRANS_STATE_NO_DATA_CONTROL;
    USBP_ep0Mode       = USBP_MODE_STATUS_IN_ONLY;
    USBP_ep0Toggle     = USBP_EP0_CNT_DATA_TOGGLE;
    USBP_ep0Count      = 0u;

    return (USBP_TRUE);
}


/*******************************************************************************
* Function Name: USBP_NoDataControlStatusStage
****************************************************************************//**
*  Handle the Status Stage of a no data control transfer.
*
*  SET_ADDRESS is special, since we need to receive the status stage with
*  the old address.
*
* \globalvars
*  USBP_transferState - set to TRANS_STATE_IDLE.
*  USBP_ep0Mode  - set to MODE_STALL_IN_OUT.
*  USBP_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  USBP_deviceAddress - used to set new address and cleared
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_NoDataControlStatusStage(void) 
{
    if (0u != USBP_deviceAddress)
    {
        /* Update device address if we got new address. */
        USBP_CR0_REG = (uint8) USBP_deviceAddress | USBP_CR0_ENABLE;
        USBP_deviceAddress = 0u;
    }

    USBP_transferState = USBP_TRANS_STATE_IDLE;
    
    /* Update the completion block. */
    USBP_UpdateStatusBlock(USBP_XFER_STATUS_ACK);
    
    /* Stall IN and OUT, no more data is expected. */
    USBP_ep0Mode = USBP_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: USBP_UpdateStatusBlock
****************************************************************************//**
*
*  Update the Completion Status Block for a Request.  The block is updated
*  with the completion code the USBP_transferByteCount.  The
*  StatusBlock Pointer is set to NULL.
*
*  completionCode - status.
*
*
* \globalvars
*  USBP_currentTD.pStatusBlock->status - updated by the
*    completionCode parameter.
*  USBP_currentTD.pStatusBlock->length - updated.
*  USBP_currentTD.pStatusBlock - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_UpdateStatusBlock(uint8 completionCode) 
{
    if (USBP_currentTD.pStatusBlock != NULL)
    {
        USBP_currentTD.pStatusBlock->status = completionCode;
        USBP_currentTD.pStatusBlock->length = USBP_transferByteCount;
        USBP_currentTD.pStatusBlock = NULL;
    }
}


/*******************************************************************************
* Function Name: USBP_InitializeStatusBlock
****************************************************************************//**
*
*  Initialize the Completion Status Block for a Request.  The completion
*  code is set to USB_XFER_IDLE.
*
*  Also, initializes USBP_transferByteCount.  Save some space,
*  this is the only consumer.
*
* \globalvars
*  USBP_currentTD.pStatusBlock->status - set to XFER_IDLE.
*  USBP_currentTD.pStatusBlock->length - cleared.
*  USBP_transferByteCount - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
void USBP_InitializeStatusBlock(void) 
{
    USBP_transferByteCount = 0u;
    
    if (USBP_currentTD.pStatusBlock != NULL)
    {
        USBP_currentTD.pStatusBlock->status = USBP_XFER_IDLE;
        USBP_currentTD.pStatusBlock->length = 0u;
    }
}


/* [] END OF FILE */
