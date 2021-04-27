/***************************************************************************//**
* \file  USBP.h
* \version 3.20
*
* \brief
*  This file provides function prototypes and constants for the USBFS component. 
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USBP_H)
#define CY_USBFS_USBP_H

#include "cydevice_trm.h"
#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h"
#include "cyapicallbacks.h"

/*  User supplied definitions. */
/* `#START USER_DEFINITIONS` Place your declaration here */

/* `#END` */

/***************************************
* Enumerated Types and Parameters
***************************************/

/* USB IP memory management options. */
#define USBP__EP_MANUAL    (0u)
#define USBP__EP_DMAMANUAL (1u)
#define USBP__EP_DMAAUTO   (2u)

/* USB IP memory allocation options. */
#define USBP__MA_STATIC    (0u)
#define USBP__MA_DYNAMIC   (1u)


/***************************************
*    Initial Parameter Constants
***************************************/

#define USBP_NUM_DEVICES                       (1u)
#define USBP_DYNAMIC_ALLOCATION                
#define USBP_ENABLE_MIDI_CLASS                 (0u)
#define USBP_ENABLE_MSC_CLASS                  (0u)
#define USBP_BOS_ENABLE                        (0u)
#define USBP_ENABLE_DESCRIPTOR_STRINGS         
#define USBP_ENABLE_STRINGS                    
#define USBP_MAX_REPORTID_NUMBER               (0u)

#define USBP_MON_VBUS               (0u)
#define USBP_EXTERN_VBUS            (0u)
#define USBP_POWER_PAD_VBUS         (0u)
#define USBP_EXTERN_VND             (0u)
#define USBP_EXTERN_CLS             (0u)
#define USBP_MAX_INTERFACES_NUMBER  (1u)
#define USBP_EP_MM                  (0u)
#define USBP_EP_MA                  (1u)
#define USBP_ENABLE_BATT_CHARG_DET  (0u)
#define USBP_GEN_16BITS_EP_ACCESS   (0u)

/* Enable Class APIs: MIDI, CDC, MSC. */         
#define USBP_ENABLE_CDC_CLASS_API   (0u != (0u))

/* General parameters */
#define USBP_EP_ALLOC_STATIC            (USBP_EP_MA == USBP__MA_STATIC)
#define USBP_EP_ALLOC_DYNAMIC           (USBP_EP_MA == USBP__MA_DYNAMIC)
#define USBP_EP_MANAGEMENT_MANUAL       (USBP_EP_MM == USBP__EP_MANUAL)
#define USBP_EP_MANAGEMENT_DMA          (USBP_EP_MM != USBP__EP_MANUAL)
#define USBP_EP_MANAGEMENT_DMA_MANUAL   (USBP_EP_MM == USBP__EP_DMAMANUAL)
#define USBP_EP_MANAGEMENT_DMA_AUTO     (USBP_EP_MM == USBP__EP_DMAAUTO)
#define USBP_BATT_CHARG_DET_ENABLE      (CY_PSOC4 && (0u != USBP_ENABLE_BATT_CHARG_DET))
#define USBP_16BITS_EP_ACCESS_ENABLE    (CY_PSOC4 && (0u != USBP_GEN_16BITS_EP_ACCESS))
#define USBP_VBUS_MONITORING_ENABLE     (0u != USBP_MON_VBUS)
#define USBP_VBUS_MONITORING_INTERNAL   (0u == USBP_EXTERN_VBUS)
#define USBP_VBUS_POWER_PAD_ENABLE      (0u != USBP_POWER_PAD_VBUS)

/* Control endpoints availability */
#define USBP_SOF_ISR_REMOVE       (0u)
#define USBP_BUS_RESET_ISR_REMOVE (0u)
#define USBP_EP0_ISR_REMOVE       (0u)
#define USBP_ARB_ISR_REMOVE       (0u)
#define USBP_DP_ISR_REMOVE        (0u)
#define USBP_LPM_REMOVE           (1u)
#define USBP_SOF_ISR_ACTIVE       ((0u == USBP_SOF_ISR_REMOVE) ? 1u: 0u)
#define USBP_BUS_RESET_ISR_ACTIVE ((0u == USBP_BUS_RESET_ISR_REMOVE) ? 1u: 0u)
#define USBP_EP0_ISR_ACTIVE       ((0u == USBP_EP0_ISR_REMOVE) ? 1u: 0u)
#define USBP_ARB_ISR_ACTIVE       ((0u == USBP_ARB_ISR_REMOVE) ? 1u: 0u)
#define USBP_DP_ISR_ACTIVE        ((0u == USBP_DP_ISR_REMOVE) ? 1u: 0u)
#define USBP_LPM_ACTIVE           ((CY_PSOC4 && (0u == USBP_LPM_REMOVE)) ? 1u: 0u)

/* Data endpoints availability */
#define USBP_EP1_ISR_REMOVE     (0u)
#define USBP_EP2_ISR_REMOVE     (0u)
#define USBP_EP3_ISR_REMOVE     (1u)
#define USBP_EP4_ISR_REMOVE     (1u)
#define USBP_EP5_ISR_REMOVE     (1u)
#define USBP_EP6_ISR_REMOVE     (1u)
#define USBP_EP7_ISR_REMOVE     (1u)
#define USBP_EP8_ISR_REMOVE     (1u)
#define USBP_EP1_ISR_ACTIVE     ((0u == USBP_EP1_ISR_REMOVE) ? 1u: 0u)
#define USBP_EP2_ISR_ACTIVE     ((0u == USBP_EP2_ISR_REMOVE) ? 1u: 0u)
#define USBP_EP3_ISR_ACTIVE     ((0u == USBP_EP3_ISR_REMOVE) ? 1u: 0u)
#define USBP_EP4_ISR_ACTIVE     ((0u == USBP_EP4_ISR_REMOVE) ? 1u: 0u)
#define USBP_EP5_ISR_ACTIVE     ((0u == USBP_EP5_ISR_REMOVE) ? 1u: 0u)
#define USBP_EP6_ISR_ACTIVE     ((0u == USBP_EP6_ISR_REMOVE) ? 1u: 0u)
#define USBP_EP7_ISR_ACTIVE     ((0u == USBP_EP7_ISR_REMOVE) ? 1u: 0u)
#define USBP_EP8_ISR_ACTIVE     ((0u == USBP_EP8_ISR_REMOVE) ? 1u: 0u)

#define USBP_EP_DMA_AUTO_OPT    ((CY_PSOC4) ? (1u) : (0u))
#define USBP_DMA1_REMOVE        (1u)
#define USBP_DMA2_REMOVE        (1u)
#define USBP_DMA3_REMOVE        (1u)
#define USBP_DMA4_REMOVE        (1u)
#define USBP_DMA5_REMOVE        (1u)
#define USBP_DMA6_REMOVE        (1u)
#define USBP_DMA7_REMOVE        (1u)
#define USBP_DMA8_REMOVE        (1u)
#define USBP_DMA1_ACTIVE        ((0u == USBP_DMA1_REMOVE) ? 1u: 0u)
#define USBP_DMA2_ACTIVE        ((0u == USBP_DMA2_REMOVE) ? 1u: 0u)
#define USBP_DMA3_ACTIVE        ((0u == USBP_DMA3_REMOVE) ? 1u: 0u)
#define USBP_DMA4_ACTIVE        ((0u == USBP_DMA4_REMOVE) ? 1u: 0u)
#define USBP_DMA5_ACTIVE        ((0u == USBP_DMA5_REMOVE) ? 1u: 0u)
#define USBP_DMA6_ACTIVE        ((0u == USBP_DMA6_REMOVE) ? 1u: 0u)
#define USBP_DMA7_ACTIVE        ((0u == USBP_DMA7_REMOVE) ? 1u: 0u)
#define USBP_DMA8_ACTIVE        ((0u == USBP_DMA8_REMOVE) ? 1u: 0u)


/***************************************
*    Data Structures Definition
***************************************/

typedef struct
{
    uint8  attrib;
    uint8  apiEpState;
    uint8  hwEpState;
    uint8  epToggle;
    uint8  addr;
    uint8  epMode;
    uint16 buffOffset;
    uint16 bufferSize;
    uint8  interface;
} T_USBP_EP_CTL_BLOCK;

typedef struct
{
    uint8  interface;
    uint8  altSetting;
    uint8  addr;
    uint8  attributes;
    uint16 bufferSize;
    uint8  bMisc;
} T_USBP_EP_SETTINGS_BLOCK;

typedef struct
{
    uint8  status;
    uint16 length;
} T_USBP_XFER_STATUS_BLOCK;

typedef struct
{
    uint16  count;
    volatile uint8 *pData;
    T_USBP_XFER_STATUS_BLOCK *pStatusBlock;
} T_USBP_TD;

typedef struct
{
    uint8   c;
    const void *p_list;
} T_USBP_LUT;

/* Resume/Suspend API Support */
typedef struct
{
    uint8 enableState;
    uint8 mode;
#if (CY_PSOC4)
    uint8 intrSeiMask;
#endif /* (CY_PSOC4) */
} USBP_BACKUP_STRUCT;

/* Number of endpoint 0 data registers. */
#define USBP_EP0_DR_MAPPED_REG_CNT  (8u)

/* Structure to access data registers for EP0. */
typedef struct
{
    uint8 epData[USBP_EP0_DR_MAPPED_REG_CNT];
} USBP_ep0_data_struct;

/* Number of SIE endpoint registers group. */
#define USBP_SIE_EP_REG_SIZE   (USBP_USB__SIE_EP1_CR0 - \
                                            USBP_USB__SIE_EP1_CNT0)

/* Size of gap between SIE endpoint registers groups. */
#define USBP_SIE_GAP_CNT        (((USBP_USB__SIE_EP2_CNT0 - \
                                             (USBP_USB__SIE_EP1_CNT0 + \
                                              USBP_SIE_EP_REG_SIZE)) / sizeof(reg8)) - 1u)

/* Structure to access to SIE registers for endpoint. */
typedef struct
{
    uint8 epCnt0;
    uint8 epCnt1;
    uint8 epCr0;
    uint8 gap[USBP_SIE_GAP_CNT];
} USBP_sie_ep_struct;

/* Number of ARB endpoint registers group. */
#define USBP_ARB_EP_REG_SIZE    (USBP_USB__ARB_RW1_DR - \
                                             USBP_USB__ARB_EP1_CFG)

/* Size of gap between ARB endpoint registers groups. */
#define USBP_ARB_GAP_CNT        (((USBP_USB__ARB_EP2_CFG - \
                                             (USBP_USB__ARB_EP1_CFG + \
                                              USBP_ARB_EP_REG_SIZE)) / sizeof(reg8)) - 1u)

/* Structure to access to ARB registers for endpoint. */
typedef struct
{
    uint8 epCfg;
    uint8 epIntEn;
    uint8 epSr;
    uint8 reserved;
    uint8 rwWa;
    uint8 rwWaMsb;
    uint8 rwRa;
    uint8 rwRaMsb;
    uint8 rwDr;
    uint8 gap[USBP_ARB_GAP_CNT];
} USBP_arb_ep_struct;

#if (CY_PSOC4)
    /* Number of ARB endpoint registers group (16-bits access). */
    #define USBP_ARB_EP_REG16_SIZE      (USBP_USB__ARB_RW1_DR16 - \
                                                     USBP_USB__ARB_RW1_WA16)

    /* Size of gap between ARB endpoint registers groups (16-bits access). */
    #define USBP_ARB_EP_REG16_GAP_CNT   (((USBP_USB__ARB_RW2_WA16 - \
                                                     (USBP_USB__ARB_RW1_WA16 + \
                                                      USBP_ARB_EP_REG16_SIZE)) / sizeof(reg8)) - 1u)

    /* Structure to access to ARB registers for endpoint (16-bits access). */
    typedef struct
    {
        uint8 rwWa16;
        uint8 reserved0;
        uint8 rwRa16;
        uint8 reserved1;
        uint8 rwDr16;
        uint8 gap[USBP_ARB_EP_REG16_GAP_CNT];
    } USBP_arb_ep_reg16_struct;
#endif /* (CY_PSOC4) */

/* Number of endpoint (takes to account that endpoints numbers are 1-8). */
#define USBP_NUMBER_EP  (9u)

/* Consoled SIE register groups for endpoints 1-8. */
typedef struct
{
    USBP_sie_ep_struct sieEp[USBP_NUMBER_EP];
} USBP_sie_eps_struct;

/* Consolidate ARB register groups for endpoints 1-8.*/
typedef struct
{
    USBP_arb_ep_struct arbEp[USBP_NUMBER_EP];
} USBP_arb_eps_struct;

#if (CY_PSOC4)
    /* Consolidate ARB register groups for endpoints 1-8 (16-bits access). */
    typedef struct
    {
        USBP_arb_ep_reg16_struct arbEp[USBP_NUMBER_EP];
    } USBP_arb_eps_reg16_struct;
#endif /* (CY_PSOC4) */


/***************************************
*       Function Prototypes
***************************************/
/**
* \addtogroup group_general
* @{
*/
void   USBP_InitComponent(uint8 device, uint8 mode) ;
void   USBP_Start(uint8 device, uint8 mode)         ;
void   USBP_Init(void)                              ;
void   USBP_Stop(void)                              ;
uint8  USBP_GetConfiguration(void)                  ;
uint8  USBP_IsConfigurationChanged(void)            ;
uint8  USBP_GetInterfaceSetting(uint8 interfaceNumber) ;
uint8  USBP_GetEPState(uint8 epNumber)              ;
uint16 USBP_GetEPCount(uint8 epNumber)              ;
void   USBP_LoadInEP(uint8 epNumber, const uint8 pData[], uint16 length)
                                                                ;
uint16 USBP_ReadOutEP(uint8 epNumber, uint8 pData[], uint16 length)
                                                                ;
void   USBP_EnableOutEP(uint8 epNumber)             ;
void   USBP_DisableOutEP(uint8 epNumber)            ;
void   USBP_Force(uint8 bState)                     ;
uint8  USBP_GetEPAckState(uint8 epNumber)           ;
void   USBP_SetPowerStatus(uint8 powerStatus)       ;
void   USBP_TerminateEP(uint8 epNumber)             ;

uint8 USBP_GetDeviceAddress(void) ;

void USBP_EnableSofInt(void)  ;
void USBP_DisableSofInt(void) ;


#if defined(USBP_ENABLE_FWSN_STRING)
    void   USBP_SerialNumString(uint8 snString[]) ;
#endif  /* USBP_ENABLE_FWSN_STRING */

#if (USBP_VBUS_MONITORING_ENABLE)
    uint8  USBP_VBusPresent(void) ;
#endif /*  (USBP_VBUS_MONITORING_ENABLE) */

#if (USBP_16BITS_EP_ACCESS_ENABLE)
    /* PSoC4 specific functions for 16-bit data register access. */
    void   USBP_LoadInEP16 (uint8 epNumber, const uint8 pData[], uint16 length);
    uint16 USBP_ReadOutEP16(uint8 epNumber,       uint8 pData[], uint16 length);
#endif /* (USBP_16BITS_EP_ACCESS_ENABLE) */

#if (USBP_BATT_CHARG_DET_ENABLE)
        uint8 USBP_Bcd_DetectPortType(void);
#endif /* (USBP_BATT_CHARG_DET_ENABLE) */

#if (USBP_EP_MANAGEMENT_DMA)
    void USBP_InitEP_DMA(uint8 epNumber, const uint8 *pData) ;   
    void USBP_Stop_DMA(uint8 epNumber) ;
/** @} general */ 
#endif /* (USBP_EP_MANAGEMENT_DMA) */

/**
* \addtogroup group_power
* @{
*/
uint8  USBP_CheckActivity(void) ;
void   USBP_Suspend(void)       ;
void   USBP_Resume(void)        ;
uint8  USBP_RWUEnabled(void)    ;

#if (USBP_LPM_ACTIVE)
    uint32 USBP_Lpm_GetBeslValue(void);
    uint32 USBP_Lpm_RemoteWakeUpAllowed(void);
    void   USBP_Lpm_SetResponse(uint32 response);
    uint32 USBP_Lpm_GetResponse(void);
#endif /* (USBP_LPM_ACTIVE) */

/** @} power */


#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_USBP) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
/**
* \addtogroup group_bootloader
* @{
*/
    void USBP_CyBtldrCommStart(void)        ;
    void USBP_CyBtldrCommStop(void)         ;
    void USBP_CyBtldrCommReset(void)        ;
    cystatus USBP_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 *count, uint8 timeOut) CYSMALL
                                                        ;
    cystatus USBP_CyBtldrCommRead       (uint8 pData[], uint16 size, uint16 *count, uint8 timeOut) CYSMALL
                                                        ;
/** @} bootloader */

    #define USBP_BTLDR_OUT_EP   (0x01u)
    #define USBP_BTLDR_IN_EP    (0x02u)

    #define USBP_BTLDR_SIZEOF_WRITE_BUFFER  (64u)   /* Endpoint 1 (OUT) buffer size. */
    #define USBP_BTLDR_SIZEOF_READ_BUFFER   (64u)   /* Endpoint 2 (IN)  buffer size. */
    #define USBP_BTLDR_MAX_PACKET_SIZE      USBP_BTLDR_SIZEOF_WRITE_BUFFER

    #define USBP_BTLDR_WAIT_1_MS            (1u)    /* Time Out quantity equal 1mS */

    /* Map-specific USB bootloader communication functions to common bootloader functions */
    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_USBP)
        #define CyBtldrCommStart        USBP_CyBtldrCommStart
        #define CyBtldrCommStop         USBP_CyBtldrCommStop
        #define CyBtldrCommReset        USBP_CyBtldrCommReset
        #define CyBtldrCommWrite        USBP_CyBtldrCommWrite
        #define CyBtldrCommRead         USBP_CyBtldrCommRead
    #endif /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_USBP) */
#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/

#define USBP_EP0                        (0u)
#define USBP_EP1                        (1u)
#define USBP_EP2                        (2u)
#define USBP_EP3                        (3u)
#define USBP_EP4                        (4u)
#define USBP_EP5                        (5u)
#define USBP_EP6                        (6u)
#define USBP_EP7                        (7u)
#define USBP_EP8                        (8u)
#define USBP_MAX_EP                     (9u)

#define USBP_TRUE                       (1u)
#define USBP_FALSE                      (0u)

#define USBP_NO_EVENT_ALLOWED           (2u)
#define USBP_EVENT_PENDING              (1u)
#define USBP_NO_EVENT_PENDING           (0u)

#define USBP_IN_BUFFER_FULL             USBP_NO_EVENT_PENDING
#define USBP_IN_BUFFER_EMPTY            USBP_EVENT_PENDING
#define USBP_OUT_BUFFER_FULL            USBP_EVENT_PENDING
#define USBP_OUT_BUFFER_EMPTY           USBP_NO_EVENT_PENDING

#define USBP_FORCE_J                    (0xA0u)
#define USBP_FORCE_K                    (0x80u)
#define USBP_FORCE_SE0                  (0xC0u)
#define USBP_FORCE_NONE                 (0x00u)

#define USBP_IDLE_TIMER_RUNNING         (0x02u)
#define USBP_IDLE_TIMER_EXPIRED         (0x01u)
#define USBP_IDLE_TIMER_INDEFINITE      (0x00u)

#define USBP_DEVICE_STATUS_BUS_POWERED  (0x00u)
#define USBP_DEVICE_STATUS_SELF_POWERED (0x01u)

#define USBP_3V_OPERATION               (0x00u)
#define USBP_5V_OPERATION               (0x01u)
#define USBP_DWR_POWER_OPERATION        (0x02u)

#define USBP_MODE_DISABLE               (0x00u)
#define USBP_MODE_NAK_IN_OUT            (0x01u)
#define USBP_MODE_STATUS_OUT_ONLY       (0x02u)
#define USBP_MODE_STALL_IN_OUT          (0x03u)
#define USBP_MODE_RESERVED_0100         (0x04u)
#define USBP_MODE_ISO_OUT               (0x05u)
#define USBP_MODE_STATUS_IN_ONLY        (0x06u)
#define USBP_MODE_ISO_IN                (0x07u)
#define USBP_MODE_NAK_OUT               (0x08u)
#define USBP_MODE_ACK_OUT               (0x09u)
#define USBP_MODE_RESERVED_1010         (0x0Au)
#define USBP_MODE_ACK_OUT_STATUS_IN     (0x0Bu)
#define USBP_MODE_NAK_IN                (0x0Cu)
#define USBP_MODE_ACK_IN                (0x0Du)
#define USBP_MODE_RESERVED_1110         (0x0Eu)
#define USBP_MODE_ACK_IN_STATUS_OUT     (0x0Fu)
#define USBP_MODE_MASK                  (0x0Fu)
#define USBP_MODE_STALL_DATA_EP         (0x80u)

#define USBP_MODE_ACKD                  (0x10u)
#define USBP_MODE_OUT_RCVD              (0x20u)
#define USBP_MODE_IN_RCVD               (0x40u)
#define USBP_MODE_SETUP_RCVD            (0x80u)

#define USBP_RQST_TYPE_MASK             (0x60u)
#define USBP_RQST_TYPE_STD              (0x00u)
#define USBP_RQST_TYPE_CLS              (0x20u)
#define USBP_RQST_TYPE_VND              (0x40u)
#define USBP_RQST_DIR_MASK              (0x80u)
#define USBP_RQST_DIR_D2H               (0x80u)
#define USBP_RQST_DIR_H2D               (0x00u)
#define USBP_RQST_RCPT_MASK             (0x03u)
#define USBP_RQST_RCPT_DEV              (0x00u)
#define USBP_RQST_RCPT_IFC              (0x01u)
#define USBP_RQST_RCPT_EP               (0x02u)
#define USBP_RQST_RCPT_OTHER            (0x03u)

#if (USBP_LPM_ACTIVE)
    #define USBP_LPM_REQ_ACK            (0x01u << USBP_LPM_CTRL_LPM_ACK_RESP_POS)
    #define USBP_LPM_REQ_NACK           (0x00u)
    #define USBP_LPM_REQ_NYET           (0x01u << USBP_LPM_CTRL_NYET_EN_POS)
#endif /*(USBP_LPM_ACTIVE)*/

/* USB Class Codes */
#define USBP_CLASS_DEVICE               (0x00u)     /* Use class code info from Interface Descriptors */
#define USBP_CLASS_AUDIO                (0x01u)     /* Audio device */
#define USBP_CLASS_CDC                  (0x02u)     /* Communication device class */
#define USBP_CLASS_HID                  (0x03u)     /* Human Interface Device */
#define USBP_CLASS_PDC                  (0x05u)     /* Physical device class */
#define USBP_CLASS_IMAGE                (0x06u)     /* Still Imaging device */
#define USBP_CLASS_PRINTER              (0x07u)     /* Printer device  */
#define USBP_CLASS_MSD                  (0x08u)     /* Mass Storage device  */
#define USBP_CLASS_HUB                  (0x09u)     /* Full/Hi speed Hub */
#define USBP_CLASS_CDC_DATA             (0x0Au)     /* CDC data device */
#define USBP_CLASS_SMART_CARD           (0x0Bu)     /* Smart Card device */
#define USBP_CLASS_CSD                  (0x0Du)     /* Content Security device */
#define USBP_CLASS_VIDEO                (0x0Eu)     /* Video device */
#define USBP_CLASS_PHD                  (0x0Fu)     /* Personal Health care device */
#define USBP_CLASS_WIRELESSD            (0xDCu)     /* Wireless Controller */
#define USBP_CLASS_MIS                  (0xE0u)     /* Miscellaneous */
#define USBP_CLASS_APP                  (0xEFu)     /* Application Specific */
#define USBP_CLASS_VENDOR               (0xFFu)     /* Vendor specific */

/* Standard Request Types (Table 9-4) */
#define USBP_GET_STATUS                 (0x00u)
#define USBP_CLEAR_FEATURE              (0x01u)
#define USBP_SET_FEATURE                (0x03u)
#define USBP_SET_ADDRESS                (0x05u)
#define USBP_GET_DESCRIPTOR             (0x06u)
#define USBP_SET_DESCRIPTOR             (0x07u)
#define USBP_GET_CONFIGURATION          (0x08u)
#define USBP_SET_CONFIGURATION          (0x09u)
#define USBP_GET_INTERFACE              (0x0Au)
#define USBP_SET_INTERFACE              (0x0Bu)
#define USBP_SYNCH_FRAME                (0x0Cu)

/* Vendor Specific Request Types */
/* Request for Microsoft OS String Descriptor */
#define USBP_GET_EXTENDED_CONFIG_DESCRIPTOR (0x01u)

/* Descriptor Types (Table 9-5) */
#define USBP_DESCR_DEVICE                   (1u)
#define USBP_DESCR_CONFIG                   (2u)
#define USBP_DESCR_STRING                   (3u)
#define USBP_DESCR_INTERFACE                (4u)
#define USBP_DESCR_ENDPOINT                 (5u)
#define USBP_DESCR_DEVICE_QUALIFIER         (6u)
#define USBP_DESCR_OTHER_SPEED              (7u)
#define USBP_DESCR_INTERFACE_POWER          (8u)
#if (USBP_BOS_ENABLE)
    #define USBP_DESCR_BOS                  (15u)
#endif /* (USBP_BOS_ENABLE) */
/* Device Descriptor Defines */
#define USBP_DEVICE_DESCR_LENGTH            (18u)
#define USBP_DEVICE_DESCR_SN_SHIFT          (16u)

/* Config Descriptor Shifts and Masks */
#define USBP_CONFIG_DESCR_LENGTH                (0u)
#define USBP_CONFIG_DESCR_TYPE                  (1u)
#define USBP_CONFIG_DESCR_TOTAL_LENGTH_LOW      (2u)
#define USBP_CONFIG_DESCR_TOTAL_LENGTH_HI       (3u)
#define USBP_CONFIG_DESCR_NUM_INTERFACES        (4u)
#define USBP_CONFIG_DESCR_CONFIG_VALUE          (5u)
#define USBP_CONFIG_DESCR_CONFIGURATION         (6u)
#define USBP_CONFIG_DESCR_ATTRIB                (7u)
#define USBP_CONFIG_DESCR_ATTRIB_SELF_POWERED   (0x40u)
#define USBP_CONFIG_DESCR_ATTRIB_RWU_EN         (0x20u)

#if (USBP_BOS_ENABLE)
    /* Config Descriptor BOS */
    #define USBP_BOS_DESCR_LENGTH               (0u)
    #define USBP_BOS_DESCR_TYPE                 (1u)
    #define USBP_BOS_DESCR_TOTAL_LENGTH_LOW     (2u)
    #define USBP_BOS_DESCR_TOTAL_LENGTH_HI      (3u)
    #define USBP_BOS_DESCR_NUM_DEV_CAPS         (4u)
#endif /* (USBP_BOS_ENABLE) */

/* Feature Selectors (Table 9-6) */
#define USBP_DEVICE_REMOTE_WAKEUP           (0x01u)
#define USBP_ENDPOINT_HALT                  (0x00u)
#define USBP_TEST_MODE                      (0x02u)

/* USB Device Status (Figure 9-4) */
#define USBP_DEVICE_STATUS_BUS_POWERED      (0x00u)
#define USBP_DEVICE_STATUS_SELF_POWERED     (0x01u)
#define USBP_DEVICE_STATUS_REMOTE_WAKEUP    (0x02u)

/* USB Endpoint Status (Figure 9-4) */
#define USBP_ENDPOINT_STATUS_HALT           (0x01u)

/* USB Endpoint Directions */
#define USBP_DIR_IN                         (0x80u)
#define USBP_DIR_OUT                        (0x00u)
#define USBP_DIR_UNUSED                     (0x7Fu)

/* USB Endpoint Attributes */
#define USBP_EP_TYPE_CTRL                   (0x00u)
#define USBP_EP_TYPE_ISOC                   (0x01u)
#define USBP_EP_TYPE_BULK                   (0x02u)
#define USBP_EP_TYPE_INT                    (0x03u)
#define USBP_EP_TYPE_MASK                   (0x03u)

#define USBP_EP_SYNC_TYPE_NO_SYNC           (0x00u)
#define USBP_EP_SYNC_TYPE_ASYNC             (0x04u)
#define USBP_EP_SYNC_TYPE_ADAPTIVE          (0x08u)
#define USBP_EP_SYNC_TYPE_SYNCHRONOUS       (0x0Cu)
#define USBP_EP_SYNC_TYPE_MASK              (0x0Cu)

#define USBP_EP_USAGE_TYPE_DATA             (0x00u)
#define USBP_EP_USAGE_TYPE_FEEDBACK         (0x10u)
#define USBP_EP_USAGE_TYPE_IMPLICIT         (0x20u)
#define USBP_EP_USAGE_TYPE_RESERVED         (0x30u)
#define USBP_EP_USAGE_TYPE_MASK             (0x30u)

/* Point Status defines */
#define USBP_EP_STATUS_LENGTH               (0x02u)

/* Point Device defines */
#define USBP_DEVICE_STATUS_LENGTH           (0x02u)

#define USBP_STATUS_LENGTH_MAX \
                 ((USBP_EP_STATUS_LENGTH > USBP_DEVICE_STATUS_LENGTH) ? \
                        USBP_EP_STATUS_LENGTH : USBP_DEVICE_STATUS_LENGTH)

/* Transfer Completion Notification */
#define USBP_XFER_IDLE                      (0x00u)
#define USBP_XFER_STATUS_ACK                (0x01u)
#define USBP_XFER_PREMATURE                 (0x02u)
#define USBP_XFER_ERROR                     (0x03u)

/* Driver State defines */
#define USBP_TRANS_STATE_IDLE               (0x00u)
#define USBP_TRANS_STATE_CONTROL_READ       (0x02u)
#define USBP_TRANS_STATE_CONTROL_WRITE      (0x04u)
#define USBP_TRANS_STATE_NO_DATA_CONTROL    (0x06u)

/* String Descriptor defines */
#define USBP_STRING_MSOS                    (0xEEu)
#define USBP_MSOS_DESCRIPTOR_LENGTH         (18u)
#define USBP_MSOS_CONF_DESCR_LENGTH         (40u)

/* Return values */
#define USBP_BCD_PORT_SDP       (1u) /* Standard downstream port detected */
#define USBP_BCD_PORT_CDP       (2u) /* Charging downstream port detected */
#define USBP_BCD_PORT_DCP       (3u) /* Dedicated charging port detected */
#define USBP_BCD_PORT_UNKNOWN   (0u) /* Unable to detect charging port */
#define USBP_BCD_PORT_ERR       (4u) /* Error condition in detection process*/


/* Timeouts for BCD */
#define USBP_BCD_TIMEOUT                (400u)  /* Copied from PBK#163 TIMEOUT (300 ms) */
#define USBP_BCD_TDCD_DBNC              (10u)  /*BCD v1.2: DCD debounce time 10 ms*/
#define USBP_BCD_PRIMARY_WAIT           (40u)   /* Copied from PBK#163 TIMEOUT (40 ms)  */
#define USBP_BCD_SECONDARY_WAIT         (47u)   /* Copied from PBK#163 TIMEOUT (40 ms)  */
#define USBP_BCD_SUSPEND_DISABLE_WAIT   (2u)    /* Copied from PBK#163 TIMEOUT (2 us)   */

/* Wait cycles required before clearing SUSPEND_DEL in POWER_CTRL: 2us */
#define USBP_WAIT_SUSPEND_DEL_DISABLE   (2u)

/* Wait cycles required for USB regulator stabilization after it is enabled : 50ns */
#define USBP_WAIT_VREF_STABILITY        (0u)

#if (CY_PSOC3 || CY_PSOC5LP)
/* Wait cycles required for USB reference restore: 40us */
#define USBP_WAIT_VREF_RESTORE          (40u)

/* Wait cycles required for stabilization after register is written : 50ns */
#define USBP_WAIT_REG_STABILITY_50NS    (0u)
#define USBP_WAIT_REG_STABILITY_1US     (1u)

/* Wait cycles required after CR0 register write: 1 cycle */
#define USBP_WAIT_CR0_REG_STABILITY     (1u)

/* Wait cycles required after PD_PULLUP_N bit is set in PM_USB_CR0: 2us */
#define USBP_WAIT_PD_PULLUP_N_ENABLE    (2u)
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

#if (CY_PSOC4)
    #if (USBP_EP_MANAGEMENT_DMA)
        #define USBP_DMA_DESCR0         (0u)
        #define USBP_DMA_DESCR1         (1u)
    #endif /* (USBP_EP_MANAGEMENT_DMA) */

    #if (USBP_EP_MANAGEMENT_DMA_AUTO)
        /* BUF_SIZE-BYTES_PER_BURST examples: 0x55 - 32 bytes, 0x44 - 16 bytes, 0x33 - 8 bytes, etc. */
        #define USBP_DMA_BUF_SIZE             (0x55u)
        #define USBP_DMA_BYTES_PER_BURST      (32u)
        #define USBP_DMA_HALFWORDS_PER_BURST  (16u)
        #define USBP_DMA_BURST_BYTES_MASK     (USBP_DMA_BYTES_PER_BURST - 1u)

        #define USBP_DMA_DESCR0_MASK        (0x00u)
        #define USBP_DMA_DESCR1_MASK        (0x80u)
        #define USBP_DMA_DESCR_REVERT       (0x40u)
        #define USBP_DMA_DESCR_16BITS       (0x20u)
        #define USBP_DMA_DESCR_SHIFT        (7u)

        #define USBP_DMA_GET_DESCR_NUM(desrc)
        #define USBP_DMA_GET_BURST_CNT(dmaBurstCnt) \
                    (((dmaBurstCnt) > 2u)? ((dmaBurstCnt) - 2u) : 0u)

        #define USBP_DMA_GET_MAX_ELEM_PER_BURST(dmaLastBurstEl) \
                    ((0u != ((dmaLastBurstEl) & USBP_DMA_DESCR_16BITS)) ? \
                                (USBP_DMA_HALFWORDS_PER_BURST - 1u) : (USBP_DMA_BYTES_PER_BURST - 1u))
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
#else
    #if (USBP_EP_MANAGEMENT_DMA_MANUAL)
        #define USBP_DMA_BYTES_PER_BURST    (0u)
        #define USBP_DMA_REQUEST_PER_BURST  (0u)
    #endif /* (USBP_EP_MANAGEMENT_DMA_MANUAL) */

    #if (USBP_EP_MANAGEMENT_DMA_AUTO)
        #define USBP_DMA_BYTES_PER_BURST    (32u)
        #define USBP_DMA_BYTES_REPEAT       (2u)

        /* BUF_SIZE-BYTES_PER_BURST examples: 0x55 - 32 bytes, 0x44 - 16 bytes, 0x33 - 8 bytes, etc. */
        #define USBP_DMA_BUF_SIZE           (0x55u)
        #define USBP_DMA_REQUEST_PER_BURST  (1u)

        #define USBP_EP17_SR_MASK           (0x7Fu)
        #define USBP_EP8_SR_MASK            (0x03u)
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */
#endif /* (CY_PSOC4) */

/* DIE ID string descriptor defines */
#if defined(USBP_ENABLE_IDSN_STRING)
    #define USBP_IDSN_DESCR_LENGTH      (0x22u)
#endif /* (USBP_ENABLE_IDSN_STRING) */


/***************************************
*     Vars with External Linkage
***************************************/

/**
* \addtogroup group_globals
* @{
*/
extern uint8 USBP_initVar;
extern volatile uint8 USBP_device;
extern volatile uint8 USBP_transferState;
extern volatile uint8 USBP_configuration;
extern volatile uint8 USBP_configurationChanged;
extern volatile uint8 USBP_deviceStatus;
/** @} globals */

/**
* \addtogroup group_hid
* @{
*/
/* HID Variables */
#if defined(USBP_ENABLE_HID_CLASS)
    extern volatile uint8 USBP_hidProtocol [USBP_MAX_INTERFACES_NUMBER]; 
    extern volatile uint8 USBP_hidIdleRate [USBP_MAX_INTERFACES_NUMBER];
    extern volatile uint8 USBP_hidIdleTimer[USBP_MAX_INTERFACES_NUMBER];
#endif /* (USBP_ENABLE_HID_CLASS) */
/** @} hid */


/***************************************
*              Registers
***************************************/

/* Common registers for all PSoCs: 3/4/5LP */
#define USBP_ARB_CFG_PTR        ( (reg8 *) USBP_USB__ARB_CFG)
#define USBP_ARB_CFG_REG        (*(reg8 *) USBP_USB__ARB_CFG)

#define USBP_ARB_EP1_CFG_PTR    ( (reg8 *) USBP_USB__ARB_EP1_CFG)
#define USBP_ARB_EP1_CFG_REG    (*(reg8 *) USBP_USB__ARB_EP1_CFG)
#define USBP_ARB_EP1_INT_EN_PTR ( (reg8 *) USBP_USB__ARB_EP1_INT_EN)
#define USBP_ARB_EP1_INT_EN_REG (*(reg8 *) USBP_USB__ARB_EP1_INT_EN)
#define USBP_ARB_EP1_SR_PTR     ( (reg8 *) USBP_USB__ARB_EP1_SR)
#define USBP_ARB_EP1_SR_REG     (*(reg8 *) USBP_USB__ARB_EP1_SR)
#define USBP_ARB_EP1_CFG_IND    USBP_USB__ARB_EP1_CFG
#define USBP_ARB_EP1_INT_EN_IND USBP_USB__ARB_EP1_INT_EN
#define USBP_ARB_EP1_SR_IND     USBP_USB__ARB_EP1_SR
#define USBP_ARB_EP_BASE        (*(volatile USBP_arb_eps_struct CYXDATA *) \
                                            (USBP_USB__ARB_EP1_CFG - sizeof(USBP_arb_ep_struct)))

#define USBP_ARB_EP2_CFG_PTR    ( (reg8 *) USBP_USB__ARB_EP2_CFG)
#define USBP_ARB_EP2_CFG_REG    (*(reg8 *) USBP_USB__ARB_EP2_CFG)
#define USBP_ARB_EP2_INT_EN_PTR ( (reg8 *) USBP_USB__ARB_EP2_INT_EN)
#define USBP_ARB_EP2_INT_EN_REG (*(reg8 *) USBP_USB__ARB_EP2_INT_EN)
#define USBP_ARB_EP2_SR_PTR     ( (reg8 *) USBP_USB__ARB_EP2_SR)
#define USBP_ARB_EP2_SR_REG     (*(reg8 *) USBP_USB__ARB_EP2_SR)

#define USBP_ARB_EP3_CFG_PTR    ( (reg8 *) USBP_USB__ARB_EP3_CFG)
#define USBP_ARB_EP3_CFG_REG    (*(reg8 *) USBP_USB__ARB_EP3_CFG)
#define USBP_ARB_EP3_INT_EN_PTR ( (reg8 *) USBP_USB__ARB_EP3_INT_EN)
#define USBP_ARB_EP3_INT_EN_REG (*(reg8 *) USBP_USB__ARB_EP3_INT_EN)
#define USBP_ARB_EP3_SR_PTR     ( (reg8 *) USBP_USB__ARB_EP3_SR)
#define USBP_ARB_EP3_SR_REG     (*(reg8 *) USBP_USB__ARB_EP3_SR)

#define USBP_ARB_EP4_CFG_PTR    ( (reg8 *) USBP_USB__ARB_EP4_CFG)
#define USBP_ARB_EP4_CFG_REG    (*(reg8 *) USBP_USB__ARB_EP4_CFG)
#define USBP_ARB_EP4_INT_EN_PTR ( (reg8 *) USBP_USB__ARB_EP4_INT_EN)
#define USBP_ARB_EP4_INT_EN_REG (*(reg8 *) USBP_USB__ARB_EP4_INT_EN)
#define USBP_ARB_EP4_SR_PTR     ( (reg8 *) USBP_USB__ARB_EP4_SR)
#define USBP_ARB_EP4_SR_REG     (*(reg8 *) USBP_USB__ARB_EP4_SR)

#define USBP_ARB_EP5_CFG_PTR    ( (reg8 *) USBP_USB__ARB_EP5_CFG)
#define USBP_ARB_EP5_CFG_REG    (*(reg8 *) USBP_USB__ARB_EP5_CFG)
#define USBP_ARB_EP5_INT_EN_PTR ( (reg8 *) USBP_USB__ARB_EP5_INT_EN)
#define USBP_ARB_EP5_INT_EN_REG (*(reg8 *) USBP_USB__ARB_EP5_INT_EN)
#define USBP_ARB_EP5_SR_PTR     ( (reg8 *) USBP_USB__ARB_EP5_SR)
#define USBP_ARB_EP5_SR_REG     (*(reg8 *) USBP_USB__ARB_EP5_SR)

#define USBP_ARB_EP6_CFG_PTR    ( (reg8 *) USBP_USB__ARB_EP6_CFG)
#define USBP_ARB_EP6_CFG_REG    (*(reg8 *) USBP_USB__ARB_EP6_CFG)
#define USBP_ARB_EP6_INT_EN_PTR ( (reg8 *) USBP_USB__ARB_EP6_INT_EN)
#define USBP_ARB_EP6_INT_EN_REG (*(reg8 *) USBP_USB__ARB_EP6_INT_EN)
#define USBP_ARB_EP6_SR_PTR     ( (reg8 *) USBP_USB__ARB_EP6_SR)
#define USBP_ARB_EP6_SR_REG     (*(reg8 *) USBP_USB__ARB_EP6_SR)

#define USBP_ARB_EP7_CFG_PTR    ( (reg8 *) USBP_USB__ARB_EP7_CFG)
#define USBP_ARB_EP7_CFG_REG    (*(reg8 *) USBP_USB__ARB_EP7_CFG)
#define USBP_ARB_EP7_INT_EN_PTR ( (reg8 *) USBP_USB__ARB_EP7_INT_EN)
#define USBP_ARB_EP7_INT_EN_REG (*(reg8 *) USBP_USB__ARB_EP7_INT_EN)
#define USBP_ARB_EP7_SR_PTR     ( (reg8 *) USBP_USB__ARB_EP7_SR)
#define USBP_ARB_EP7_SR_REG     (*(reg8 *) USBP_USB__ARB_EP7_SR)

#define USBP_ARB_EP8_CFG_PTR    ( (reg8 *) USBP_USB__ARB_EP8_CFG)
#define USBP_ARB_EP8_CFG_REG    (*(reg8 *) USBP_USB__ARB_EP8_CFG)
#define USBP_ARB_EP8_INT_EN_PTR ( (reg8 *) USBP_USB__ARB_EP8_INT_EN)
#define USBP_ARB_EP8_INT_EN_REG (*(reg8 *) USBP_USB__ARB_EP8_INT_EN)
#define USBP_ARB_EP8_SR_PTR     ( (reg8 *) USBP_USB__ARB_EP8_SR)
#define USBP_ARB_EP8_SR_REG     (*(reg8 *) USBP_USB__ARB_EP8_SR)

#define USBP_ARB_INT_EN_PTR     ( (reg8 *) USBP_USB__ARB_INT_EN)
#define USBP_ARB_INT_EN_REG     (*(reg8 *) USBP_USB__ARB_INT_EN)
#define USBP_ARB_INT_SR_PTR     ( (reg8 *) USBP_USB__ARB_INT_SR)
#define USBP_ARB_INT_SR_REG     (*(reg8 *) USBP_USB__ARB_INT_SR)

#define USBP_ARB_RW1_DR_PTR     ( (reg8 *) USBP_USB__ARB_RW1_DR)
#define USBP_ARB_RW1_RA_PTR     ( (reg8 *) USBP_USB__ARB_RW1_RA)

#define USBP_ARB_RW1_RA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW1_RA_MSB)
#define USBP_ARB_RW1_WA_PTR     ( (reg8 *) USBP_USB__ARB_RW1_WA)
#define USBP_ARB_RW1_WA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW1_WA_MSB)
#define USBP_ARB_RW1_DR_IND     USBP_USB__ARB_RW1_DR
#define USBP_ARB_RW1_RA_IND     USBP_USB__ARB_RW1_RA
#define USBP_ARB_RW1_RA_MSB_IND USBP_USB__ARB_RW1_RA_MSB
#define USBP_ARB_RW1_WA_IND     USBP_USB__ARB_RW1_WA
#define USBP_ARB_RW1_WA_MSB_IND USBP_USB__ARB_RW1_WA_MSB

#define USBP_ARB_RW2_DR_PTR     ( (reg8 *) USBP_USB__ARB_RW2_DR)
#define USBP_ARB_RW2_RA_PTR     ( (reg8 *) USBP_USB__ARB_RW2_RA)
#define USBP_ARB_RW2_RA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW2_RA_MSB)
#define USBP_ARB_RW2_WA_PTR     ( (reg8 *) USBP_USB__ARB_RW2_WA)
#define USBP_ARB_RW2_WA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW2_WA_MSB)

#define USBP_ARB_RW3_DR_PTR     ( (reg8 *) USBP_USB__ARB_RW3_DR)
#define USBP_ARB_RW3_RA_PTR     ( (reg8 *) USBP_USB__ARB_RW3_RA)
#define USBP_ARB_RW3_RA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW3_RA_MSB)
#define USBP_ARB_RW3_WA_PTR     ( (reg8 *) USBP_USB__ARB_RW3_WA)
#define USBP_ARB_RW3_WA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW3_WA_MSB)

#define USBP_ARB_RW4_DR_PTR     ( (reg8 *) USBP_USB__ARB_RW4_DR)
#define USBP_ARB_RW4_RA_PTR     ( (reg8 *) USBP_USB__ARB_RW4_RA)
#define USBP_ARB_RW4_RA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW4_RA_MSB)
#define USBP_ARB_RW4_WA_PTR     ( (reg8 *) USBP_USB__ARB_RW4_WA)
#define USBP_ARB_RW4_WA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW4_WA_MSB)

#define USBP_ARB_RW5_DR_PTR     ( (reg8 *) USBP_USB__ARB_RW5_DR)
#define USBP_ARB_RW5_RA_PTR     ( (reg8 *) USBP_USB__ARB_RW5_RA)
#define USBP_ARB_RW5_RA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW5_RA_MSB)
#define USBP_ARB_RW5_WA_PTR     ( (reg8 *) USBP_USB__ARB_RW5_WA)
#define USBP_ARB_RW5_WA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW5_WA_MSB)

#define USBP_ARB_RW6_DR_PTR     ( (reg8 *) USBP_USB__ARB_RW6_DR)
#define USBP_ARB_RW6_RA_PTR     ( (reg8 *) USBP_USB__ARB_RW6_RA)
#define USBP_ARB_RW6_RA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW6_RA_MSB)
#define USBP_ARB_RW6_WA_PTR     ( (reg8 *) USBP_USB__ARB_RW6_WA)
#define USBP_ARB_RW6_WA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW6_WA_MSB)

#define USBP_ARB_RW7_DR_PTR     ( (reg8 *) USBP_USB__ARB_RW7_DR)
#define USBP_ARB_RW7_RA_PTR     ( (reg8 *) USBP_USB__ARB_RW7_RA)
#define USBP_ARB_RW7_RA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW7_RA_MSB)
#define USBP_ARB_RW7_WA_PTR     ( (reg8 *) USBP_USB__ARB_RW7_WA)
#define USBP_ARB_RW7_WA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW7_WA_MSB)

#define USBP_ARB_RW8_DR_PTR     ( (reg8 *) USBP_USB__ARB_RW8_DR)
#define USBP_ARB_RW8_RA_PTR     ( (reg8 *) USBP_USB__ARB_RW8_RA)
#define USBP_ARB_RW8_RA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW8_RA_MSB)
#define USBP_ARB_RW8_WA_PTR     ( (reg8 *) USBP_USB__ARB_RW8_WA)
#define USBP_ARB_RW8_WA_MSB_PTR ( (reg8 *) USBP_USB__ARB_RW8_WA_MSB)

#define USBP_BUF_SIZE_PTR       ( (reg8 *) USBP_USB__BUF_SIZE)
#define USBP_BUF_SIZE_REG       (*(reg8 *) USBP_USB__BUF_SIZE)
#define USBP_BUS_RST_CNT_PTR    ( (reg8 *) USBP_USB__BUS_RST_CNT)
#define USBP_BUS_RST_CNT_REG    (*(reg8 *) USBP_USB__BUS_RST_CNT)
#define USBP_CWA_PTR            ( (reg8 *) USBP_USB__CWA)
#define USBP_CWA_REG            (*(reg8 *) USBP_USB__CWA)
#define USBP_CWA_MSB_PTR        ( (reg8 *) USBP_USB__CWA_MSB)
#define USBP_CWA_MSB_REG        (*(reg8 *) USBP_USB__CWA_MSB)
#define USBP_CR0_PTR            ( (reg8 *) USBP_USB__CR0)
#define USBP_CR0_REG            (*(reg8 *) USBP_USB__CR0)
#define USBP_CR1_PTR            ( (reg8 *) USBP_USB__CR1)
#define USBP_CR1_REG            (*(reg8 *) USBP_USB__CR1)

#define USBP_DMA_THRES_PTR      ( (reg8 *) USBP_USB__DMA_THRES)
#define USBP_DMA_THRES_REG      (*(reg8 *) USBP_USB__DMA_THRES)
#define USBP_DMA_THRES_MSB_PTR  ( (reg8 *) USBP_USB__DMA_THRES_MSB)
#define USBP_DMA_THRES_MSB_REG  (*(reg8 *) USBP_USB__DMA_THRES_MSB)

#define USBP_EP_ACTIVE_PTR      ( (reg8 *) USBP_USB__EP_ACTIVE)
#define USBP_EP_ACTIVE_REG      (*(reg8 *) USBP_USB__EP_ACTIVE)
#define USBP_EP_TYPE_PTR        ( (reg8 *) USBP_USB__EP_TYPE)
#define USBP_EP_TYPE_REG        (*(reg8 *) USBP_USB__EP_TYPE)

#define USBP_EP0_CNT_PTR        ( (reg8 *) USBP_USB__EP0_CNT)
#define USBP_EP0_CNT_REG        (*(reg8 *) USBP_USB__EP0_CNT)
#define USBP_EP0_CR_PTR         ( (reg8 *) USBP_USB__EP0_CR)
#define USBP_EP0_CR_REG         (*(reg8 *) USBP_USB__EP0_CR)
#define USBP_EP0_DR0_PTR        ( (reg8 *) USBP_USB__EP0_DR0)
#define USBP_EP0_DR0_REG        (*(reg8 *) USBP_USB__EP0_DR0)
#define USBP_EP0_DR1_PTR        ( (reg8 *) USBP_USB__EP0_DR1)
#define USBP_EP0_DR1_REG        (*(reg8 *) USBP_USB__EP0_DR1)
#define USBP_EP0_DR2_PTR        ( (reg8 *) USBP_USB__EP0_DR2)
#define USBP_EP0_DR2_REG        (*(reg8 *) USBP_USB__EP0_DR2)
#define USBP_EP0_DR3_PTR        ( (reg8 *) USBP_USB__EP0_DR3)
#define USBP_EP0_DR3_REG        (*(reg8 *) USBP_USB__EP0_DR3)
#define USBP_EP0_DR4_PTR        ( (reg8 *) USBP_USB__EP0_DR4)
#define USBP_EP0_DR4_REG        (*(reg8 *) USBP_USB__EP0_DR4)
#define USBP_EP0_DR5_PTR        ( (reg8 *) USBP_USB__EP0_DR5)
#define USBP_EP0_DR5_REG        (*(reg8 *) USBP_USB__EP0_DR5)
#define USBP_EP0_DR6_PTR        ( (reg8 *) USBP_USB__EP0_DR6)
#define USBP_EP0_DR6_REG        (*(reg8 *) USBP_USB__EP0_DR6)
#define USBP_EP0_DR7_PTR        ( (reg8 *) USBP_USB__EP0_DR7)
#define USBP_EP0_DR7_REG        (*(reg8 *) USBP_USB__EP0_DR7)
#define USBP_EP0_DR0_IND        USBP_USB__EP0_DR0
#define USBP_EP0_DR_BASE        (*(volatile USBP_ep0_data_struct CYXDATA *) USBP_USB__EP0_DR0)

#define USBP_OSCLK_DR0_PTR      ( (reg8 *) USBP_USB__OSCLK_DR0)
#define USBP_OSCLK_DR0_REG      (*(reg8 *) USBP_USB__OSCLK_DR0)
#define USBP_OSCLK_DR1_PTR      ( (reg8 *) USBP_USB__OSCLK_DR1)
#define USBP_OSCLK_DR1_REG      (*(reg8 *) USBP_USB__OSCLK_DR1)

#define USBP_SIE_EP_INT_EN_PTR  ( (reg8 *) USBP_USB__SIE_EP_INT_EN)
#define USBP_SIE_EP_INT_EN_REG  (*(reg8 *) USBP_USB__SIE_EP_INT_EN)
#define USBP_SIE_EP_INT_SR_PTR  ( (reg8 *) USBP_USB__SIE_EP_INT_SR)
#define USBP_SIE_EP_INT_SR_REG  (*(reg8 *) USBP_USB__SIE_EP_INT_SR)

#define USBP_SIE_EP1_CNT0_PTR   ( (reg8 *) USBP_USB__SIE_EP1_CNT0)
#define USBP_SIE_EP1_CNT0_REG   (*(reg8 *) USBP_USB__SIE_EP1_CNT0)
#define USBP_SIE_EP1_CNT1_PTR   ( (reg8 *) USBP_USB__SIE_EP1_CNT1)
#define USBP_SIE_EP1_CNT1_REG   (*(reg8 *) USBP_USB__SIE_EP1_CNT1)
#define USBP_SIE_EP1_CR0_PTR    ( (reg8 *) USBP_USB__SIE_EP1_CR0)
#define USBP_SIE_EP1_CR0_REG    (*(reg8 *) USBP_USB__SIE_EP1_CR0)
#define USBP_SIE_EP1_CNT1_IND   USBP_USB__SIE_EP1_CNT1
#define USBP_SIE_EP1_CNT0_IND   USBP_USB__SIE_EP1_CNT0
#define USBP_SIE_EP1_CR0_IND    USBP_USB__SIE_EP1_CR0
#define USBP_SIE_EP_BASE        (*(volatile USBP_sie_eps_struct CYXDATA *) \
                                            (USBP_USB__SIE_EP1_CNT0 - sizeof(USBP_sie_ep_struct)))

#define USBP_SIE_EP2_CNT0_PTR   ( (reg8 *) USBP_USB__SIE_EP2_CNT0)
#define USBP_SIE_EP2_CNT0_REG   (*(reg8 *) USBP_USB__SIE_EP2_CNT0)
#define USBP_SIE_EP2_CNT1_PTR   ( (reg8 *) USBP_USB__SIE_EP2_CNT1)
#define USBP_SIE_EP2_CNT1_REG   (*(reg8 *) USBP_USB__SIE_EP2_CNT1)
#define USBP_SIE_EP2_CR0_PTR    ( (reg8 *) USBP_USB__SIE_EP2_CR0)
#define USBP_SIE_EP2_CR0_REG    (*(reg8 *) USBP_USB__SIE_EP2_CR0)

#define USBP_SIE_EP3_CNT0_PTR   ( (reg8 *) USBP_USB__SIE_EP3_CNT0)
#define USBP_SIE_EP3_CNT0_REG   (*(reg8 *) USBP_USB__SIE_EP3_CNT0)
#define USBP_SIE_EP3_CNT1_PTR   ( (reg8 *) USBP_USB__SIE_EP3_CNT1)
#define USBP_SIE_EP3_CNT1_REG   (*(reg8 *) USBP_USB__SIE_EP3_CNT1)
#define USBP_SIE_EP3_CR0_PTR    ( (reg8 *) USBP_USB__SIE_EP3_CR0)
#define USBP_SIE_EP3_CR0_REG    (*(reg8 *) USBP_USB__SIE_EP3_CR0)

#define USBP_SIE_EP4_CNT0_PTR   ( (reg8 *) USBP_USB__SIE_EP4_CNT0)
#define USBP_SIE_EP4_CNT0_REG   (*(reg8 *) USBP_USB__SIE_EP4_CNT0)
#define USBP_SIE_EP4_CNT1_PTR   ( (reg8 *) USBP_USB__SIE_EP4_CNT1)
#define USBP_SIE_EP4_CNT1_REG   (*(reg8 *) USBP_USB__SIE_EP4_CNT1)
#define USBP_SIE_EP4_CR0_PTR    ( (reg8 *) USBP_USB__SIE_EP4_CR0)
#define USBP_SIE_EP4_CR0_REG    (*(reg8 *) USBP_USB__SIE_EP4_CR0)

#define USBP_SIE_EP5_CNT0_PTR   ( (reg8 *) USBP_USB__SIE_EP5_CNT0)
#define USBP_SIE_EP5_CNT0_REG   (*(reg8 *) USBP_USB__SIE_EP5_CNT0)
#define USBP_SIE_EP5_CNT1_PTR   ( (reg8 *) USBP_USB__SIE_EP5_CNT1)
#define USBP_SIE_EP5_CNT1_REG   (*(reg8 *) USBP_USB__SIE_EP5_CNT1)
#define USBP_SIE_EP5_CR0_PTR    ( (reg8 *) USBP_USB__SIE_EP5_CR0)
#define USBP_SIE_EP5_CR0_REG    (*(reg8 *) USBP_USB__SIE_EP5_CR0)

#define USBP_SIE_EP6_CNT0_PTR   ( (reg8 *) USBP_USB__SIE_EP6_CNT0)
#define USBP_SIE_EP6_CNT0_REG   (*(reg8 *) USBP_USB__SIE_EP6_CNT0)
#define USBP_SIE_EP6_CNT1_PTR   ( (reg8 *) USBP_USB__SIE_EP6_CNT1)
#define USBP_SIE_EP6_CNT1_REG   (*(reg8 *) USBP_USB__SIE_EP6_CNT1)
#define USBP_SIE_EP6_CR0_PTR    ( (reg8 *) USBP_USB__SIE_EP6_CR0)
#define USBP_SIE_EP6_CR0_REG    (*(reg8 *) USBP_USB__SIE_EP6_CR0)

#define USBP_SIE_EP7_CNT0_PTR   ( (reg8 *) USBP_USB__SIE_EP7_CNT0)
#define USBP_SIE_EP7_CNT0_REG   (*(reg8 *) USBP_USB__SIE_EP7_CNT0)
#define USBP_SIE_EP7_CNT1_PTR   ( (reg8 *) USBP_USB__SIE_EP7_CNT1)
#define USBP_SIE_EP7_CNT1_REG   (*(reg8 *) USBP_USB__SIE_EP7_CNT1)
#define USBP_SIE_EP7_CR0_PTR    ( (reg8 *) USBP_USB__SIE_EP7_CR0)
#define USBP_SIE_EP7_CR0_REG    (*(reg8 *) USBP_USB__SIE_EP7_CR0)

#define USBP_SIE_EP8_CNT0_PTR   ( (reg8 *) USBP_USB__SIE_EP8_CNT0)
#define USBP_SIE_EP8_CNT0_REG   (*(reg8 *) USBP_USB__SIE_EP8_CNT0)
#define USBP_SIE_EP8_CNT1_PTR   ( (reg8 *) USBP_USB__SIE_EP8_CNT1)
#define USBP_SIE_EP8_CNT1_REG   (*(reg8 *) USBP_USB__SIE_EP8_CNT1)
#define USBP_SIE_EP8_CR0_PTR    ( (reg8 *) USBP_USB__SIE_EP8_CR0)
#define USBP_SIE_EP8_CR0_REG    (*(reg8 *) USBP_USB__SIE_EP8_CR0)

#define USBP_SOF0_PTR           ( (reg8 *) USBP_USB__SOF0)
#define USBP_SOF0_REG           (*(reg8 *) USBP_USB__SOF0)
#define USBP_SOF1_PTR           ( (reg8 *) USBP_USB__SOF1)
#define USBP_SOF1_REG           (*(reg8 *) USBP_USB__SOF1)

#define USBP_USB_CLK_EN_PTR     ( (reg8 *) USBP_USB__USB_CLK_EN)
#define USBP_USB_CLK_EN_REG     (*(reg8 *) USBP_USB__USB_CLK_EN)

#define USBP_USBIO_CR0_PTR      ( (reg8 *) USBP_USB__USBIO_CR0)
#define USBP_USBIO_CR0_REG      (*(reg8 *) USBP_USB__USBIO_CR0)
#define USBP_USBIO_CR1_PTR      ( (reg8 *) USBP_USB__USBIO_CR1)
#define USBP_USBIO_CR1_REG      (*(reg8 *) USBP_USB__USBIO_CR1)

#define USBP_DYN_RECONFIG_PTR   ( (reg8 *) USBP_USB__DYN_RECONFIG)
#define USBP_DYN_RECONFIG_REG   (*(reg8 *) USBP_USB__DYN_RECONFIG)

#if (CY_PSOC4)
    #define USBP_ARB_RW1_RA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW1_RA16)
    #define USBP_ARB_RW1_RA16_REG   (*(reg32 *) USBP_cy_m0s8_usb__ARB_RW1_RA16)
    #define USBP_ARB_RW1_WA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW1_WA16)
    #define USBP_ARB_RW1_WA16_REG   (*(reg32 *) USBP_cy_m0s8_usb__ARB_RW1_WA16)
    #define USBP_ARB_RW1_DR16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW1_DR16)
    #define USBP_ARB_RW1_DR16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW1_DR16)
    #define USBP_ARB_EP16_BASE      (*(volatile USBP_arb_eps_reg16_struct CYXDATA *) \
                                                (USBP_USB__ARB_RW1_WA16 - sizeof(USBP_arb_ep_reg16_struct)))

    #define USBP_ARB_RW2_DR16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW2_DR16)
    #define USBP_ARB_RW2_RA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW2_RA16)
    #define USBP_ARB_RW2_WA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW2_WA16)

    #define USBP_ARB_RW3_DR16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW3_DR16)
    #define USBP_ARB_RW3_RA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW3_RA16)
    #define USBP_ARB_RW3_WA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW3_WA16)

    #define USBP_ARB_RW4_DR16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW4_DR16)
    #define USBP_ARB_RW4_RA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW4_RA16)
    #define USBP_ARB_RW4_WA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW4_WA16)

    #define USBP_ARB_RW5_DR16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW5_DR16)
    #define USBP_ARB_RW5_RA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW5_RA16)
    #define USBP_ARB_RW5_WA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW5_WA16)

    #define USBP_ARB_RW6_DR16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW6_DR16)
    #define USBP_ARB_RW6_RA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW6_RA16)
    #define USBP_ARB_RW6_WA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW6_WA16)

    #define USBP_ARB_RW7_DR16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW7_DR16)
    #define USBP_ARB_RW7_RA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW7_RA16)
    #define USBP_ARB_RW7_WA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW7_WA16)

    #define USBP_ARB_RW8_DR16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW8_DR16)
    #define USBP_ARB_RW8_RA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW8_RA16)
    #define USBP_ARB_RW8_WA16_PTR   ( (reg32 *) USBP_cy_m0s8_usb__ARB_RW8_WA16)

    #define USBP_OSCLK_DR16_PTR     ( (reg32 *) USBP_cy_m0s8_usb__OSCLK_DR16)
    #define USBP_OSCLK_DR16_REG     (*(reg32 *) USBP_cy_m0s8_usb__OSCLK_DR16)

    #define USBP_SOF16_PTR          ( (reg32 *) USBP_cy_m0s8_usb__SOF16)
    #define USBP_SOF16_REG          (*(reg32 *) USBP_cy_m0s8_usb__SOF16)
    
    #define USBP_CWA16_PTR          ( (reg32 *) USBP_cy_m0s8_usb__CWA16)
    #define USBP_CWA16_REG          (*(reg32 *) USBP_cy_m0s8_usb__CWA16)

    #define USBP_DMA_THRES16_PTR    ( (reg32 *) USBP_cy_m0s8_usb__DMA_THRES16)
    #define USBP_DMA_THRES16_REG    (*(reg32 *) USBP_cy_m0s8_usb__DMA_THRES16)

    #define USBP_USB_CLK_EN_PTR     ( (reg32 *) USBP_cy_m0s8_usb__USB_CLK_EN)
    #define USBP_USB_CLK_EN_REG     (*(reg32 *) USBP_cy_m0s8_usb__USB_CLK_EN)

    #define USBP_USBIO_CR2_PTR      ( (reg32 *) USBP_cy_m0s8_usb__USBIO_CR2)
    #define USBP_USBIO_CR2_REG      (*(reg32 *) USBP_cy_m0s8_usb__USBIO_CR2)

    #define USBP_USB_MEM            ( (reg32 *) USBP_cy_m0s8_usb__MEM_DATA0)

    #define USBP_POWER_CTRL_REG      (*(reg32 *) USBP_cy_m0s8_usb__USB_POWER_CTRL)
    #define USBP_POWER_CTRL_PTR      ( (reg32 *) USBP_cy_m0s8_usb__USB_POWER_CTRL)

    #define USBP_CHGDET_CTRL_REG     (*(reg32 *) USBP_cy_m0s8_usb__USB_CHGDET_CTRL)
    #define USBP_CHGDET_CTRL_PTR     ( (reg32 *) USBP_cy_m0s8_usb__USB_CHGDET_CTRL)

    #define USBP_USBIO_CTRL_REG      (*(reg32 *) USBP_cy_m0s8_usb__USB_USBIO_CTRL)
    #define USBP_USBIO_CTRL_PTR      ( (reg32 *) USBP_cy_m0s8_usb__USB_USBIO_CTRL)

    #define USBP_FLOW_CTRL_REG       (*(reg32 *) USBP_cy_m0s8_usb__USB_FLOW_CTRL)
    #define USBP_FLOW_CTRL_PTR       ( (reg32 *) USBP_cy_m0s8_usb__USB_FLOW_CTRL)

    #define USBP_LPM_CTRL_REG        (*(reg32 *) USBP_cy_m0s8_usb__USB_LPM_CTRL)
    #define USBP_LPM_CTRL_PTR        ( (reg32 *) USBP_cy_m0s8_usb__USB_LPM_CTRL)

    #define USBP_LPM_STAT_REG        (*(reg32 *) USBP_cy_m0s8_usb__USB_LPM_STAT)
    #define USBP_LPM_STAT_PTR        ( (reg32 *) USBP_cy_m0s8_usb__USB_LPM_STAT)

    #define USBP_PHY_CONTROL_REG     (*(reg32 *) USBP_cy_m0s8_usb__USB_PHY_CONTROL)
    #define USBP_PHY_CONTROL_PTR     ( (reg32 *) USBP_cy_m0s8_usb__USB_PHY_CONTROL)

    #define USBP_INTR_SIE_REG        (*(reg32 *) USBP_cy_m0s8_usb__USB_INTR_SIE)
    #define USBP_INTR_SIE_PTR        ( (reg32 *) USBP_cy_m0s8_usb__USB_INTR_SIE)

    #define USBP_INTR_SIE_SET_REG    (*(reg32 *) USBP_cy_m0s8_usb__USB_INTR_SIE_SET)
    #define USBP_INTR_SIE_SET_PTR    ( (reg32 *) USBP_cy_m0s8_usb__USB_INTR_SIE_SET)

    #define USBP_INTR_SIE_MASK_REG   (*(reg32 *) USBP_cy_m0s8_usb__USB_INTR_SIE_MASK)
    #define USBP_INTR_SIE_MASK_PTR   ( (reg32 *) USBP_cy_m0s8_usb__USB_INTR_SIE_MASK)

    #define USBP_INTR_SIE_MASKED_REG (*(reg32 *) USBP_cy_m0s8_usb__USB_INTR_SIE_MASKED)
    #define USBP_INTR_SIE_MASKED_PTR ( (reg32 *) USBP_cy_m0s8_usb__USB_INTR_SIE_MASKED)

    #define USBP_INTR_LVL_SEL_REG    (*(reg32 *) USBP_cy_m0s8_usb__USB_INTR_LVL_SEL)
    #define USBP_INTR_LVL_SEL_PTR    ( (reg32 *) USBP_cy_m0s8_usb__USB_INTR_LVL_SEL)

    #define USBP_INTR_CAUSE_HI_REG   (*(reg32 *) USBP_cy_m0s8_usb__USB_INTR_CAUSE_HI)
    #define USBP_INTR_CAUSE_HI_PTR   ( (reg32 *) USBP_cy_m0s8_usb__USB_INTR_CAUSE_HI)

    #define USBP_INTR_CAUSE_LO_REG   (*(reg32 *) USBP_cy_m0s8_usb__USB_INTR_CAUSE_LO)
    #define USBP_INTR_CAUSE_LO_PTR   ( (reg32 *) USBP_cy_m0s8_usb__USB_INTR_CAUSE_LO)

    #define USBP_INTR_CAUSE_MED_REG  (*(reg32 *) USBP_cy_m0s8_usb__USB_INTR_CAUSE_MED)
    #define USBP_INTR_CAUSE_MED_PTR  ( (reg32 *) USBP_cy_m0s8_usb__USB_INTR_CAUSE_MED)

    #define USBP_DFT_CTRL_REG        (*(reg32 *) USBP_cy_m0s8_usb__USB_DFT_CTRL)
    #define USBP_DFT_CTRL_PTR        ( (reg32 *) USBP_cy_m0s8_usb__USB_DFT_CTRL)

    #if (USBP_VBUS_MONITORING_ENABLE)
        #if (USBP_VBUS_POWER_PAD_ENABLE)
            /* Vbus power pad pin is hard wired to P13[2] */
            #define USBP_VBUS_STATUS_REG    (*(reg32 *) CYREG_GPIO_PRT13_PS)
            #define USBP_VBUS_STATUS_PTR    ( (reg32 *) CYREG_GPIO_PRT13_PS)
            #define USBP_VBUS_VALID         (0x04u)
        #else
            /* Vbus valid pin is hard wired to P0[0] */
            #define USBP_VBUS_STATUS_REG    (*(reg32 *) CYREG_GPIO_PRT0_PS)
            #define USBP_VBUS_STATUS_PTR    ( (reg32 *) CYREG_GPIO_PRT0_PS)
            #define USBP_VBUS_VALID         (0x01u)
        #endif
    #endif /*(USBP_VBUS_MONITORING_ENABLE) */

    #define USBP_BURSTEND_0_TR_OUTPUT    (USBP_cy_m0s8_usb__BURSTEND0_TR_OUTPUT)
    #define USBP_BURSTEND_1_TR_OUTPUT    (USBP_cy_m0s8_usb__BURSTEND1_TR_OUTPUT)
    #define USBP_BURSTEND_2_TR_OUTPUT    (USBP_cy_m0s8_usb__BURSTEND2_TR_OUTPUT)
    #define USBP_BURSTEND_3_TR_OUTPUT    (USBP_cy_m0s8_usb__BURSTEND3_TR_OUTPUT)
    #define USBP_BURSTEND_4_TR_OUTPUT    (USBP_cy_m0s8_usb__BURSTEND4_TR_OUTPUT)
    #define USBP_BURSTEND_5_TR_OUTPUT    (USBP_cy_m0s8_usb__BURSTEND5_TR_OUTPUT)
    #define USBP_BURSTEND_6_TR_OUTPUT    (USBP_cy_m0s8_usb__BURSTEND6_TR_OUTPUT)
    #define USBP_BURSTEND_7_TR_OUTPUT    (USBP_cy_m0s8_usb__BURSTEND7_TR_OUTPUT)
    
#else /* (CY_PSOC3 || CY_PSOC5LP) */

    /* USBP_PM_USB_CR0 */
    #define USBP_PM_USB_CR0_PTR     ( (reg8 *) CYREG_PM_USB_CR0)
    #define USBP_PM_USB_CR0_REG     (*(reg8 *) CYREG_PM_USB_CR0)

    /* USBP_PM_ACT/STBY_CFG */
    #define USBP_PM_ACT_CFG_PTR     ( (reg8 *) USBP_USB__PM_ACT_CFG)
    #define USBP_PM_ACT_CFG_REG     (*(reg8 *) USBP_USB__PM_ACT_CFG)
    #define USBP_PM_STBY_CFG_PTR    ( (reg8 *) USBP_USB__PM_STBY_CFG)
    #define USBP_PM_STBY_CFG_REG    (*(reg8 *) USBP_USB__PM_STBY_CFG)

    #if (!CY_PSOC5LP)
        #define USBP_USBIO_CR2_PTR  (  (reg8 *) USBP_USB__USBIO_CR2)
        #define USBP_USBIO_CR2_REG  (* (reg8 *) USBP_USB__USBIO_CR2)
    #endif /* (!CY_PSOC5LP) */

    /* USBP_USB_MEM - USB IP memory buffer */
    #define USBP_USB_MEM            ((reg8 *) CYDEV_USB_MEM_BASE)

    #if (USBP_VBUS_MONITORING_ENABLE)
        #if (USBP_VBUS_MONITORING_INTERNAL)
            #define USBP_VBUS_STATUS_REG    (*(reg8 *) USBP_VBUS__PS)
            #define USBP_VBUS_STATUS_PTR    ( (reg8 *) USBP_VBUS__PS)
            #define USBP_VBUS_VALID         (USBP_VBUS__MASK)
        #else
            #define USBP_VBUS_STATUS_REG    (*(reg8 *) USBP_Vbus_ps_sts_sts_reg__STATUS_REG)
            #define USBP_VBUS_STATUS_PTR    ( (reg8 *) USBP_Vbus_ps_sts_sts_reg__STATUS_REG)
            #define USBP_VBUS_VALID         (USBP_Vbus_ps_sts_sts_reg__MASK)
        #endif /* (USBP_VBUS_MONITORING_INTERNAL) */
    #endif /*(USBP_VBUS_MONITORING_ENABLE) */
#endif /* (CY_PSOC4) */


/***************************************
*       Interrupt source constants
***************************************/

#define USBP_DP_INTC_PRIORITY       USBP_dp_int__INTC_PRIOR_NUM
#define USBP_DP_INTC_VECT_NUM       USBP_dp_int__INTC_NUMBER

#if (CY_PSOC4)
    #define USBP_DMA_AUTO_INTR_PRIO (0u)
    
    #define USBP_INTR_HI_PRIORITY   USBP_high_int__INTC_PRIOR_NUM
    #define USBP_INTR_HI_VECT_NUM   USBP_high_int__INTC_NUMBER

    #define USBP_INTR_MED_PRIORITY  USBP_med_int__INTC_PRIOR_NUM
    #define USBP_INTR_MED_VECT_NUM  USBP_med_int__INTC_NUMBER

    #define USBP_INTR_LO_PRIORITY   USBP_lo_int__INTC_PRIOR_NUM
    #define USBP_INTR_LO_VECT_NUM   USBP_lo_int__INTC_NUMBER

    /* Interrupt sources in USBP_isrCallbacks[] table */
    #define USBP_SOF_INTR_NUM       (0u)
    #define USBP_BUS_RESET_INT_NUM  (1u)
    #define USBP_EP0_INTR_NUM       (2u)
    #define USBP_LPM_INTR_NUM       (3u)
    #define USBP_ARB_EP_INTR_NUM    (4u)
    #define USBP_EP1_INTR_NUM       (5u)
    #define USBP_EP2_INTR_NUM       (6u)
    #define USBP_EP3_INTR_NUM       (7u)
    #define USBP_EP4_INTR_NUM       (8u)
    #define USBP_EP5_INTR_NUM       (9u)
    #define USBP_EP6_INTR_NUM       (10u)
    #define USBP_EP7_INTR_NUM       (11u)
    #define USBP_EP8_INTR_NUM       (12u)

#else
    #define USBP_BUS_RESET_PRIOR    USBP_bus_reset__INTC_PRIOR_NUM
    #define USBP_BUS_RESET_MASK     USBP_bus_reset__INTC_MASK
    #define USBP_BUS_RESET_VECT_NUM USBP_bus_reset__INTC_NUMBER

    #define USBP_SOF_PRIOR          USBP_sof_int__INTC_PRIOR_NUM
    #define USBP_SOF_MASK           USBP_sof_int__INTC_MASK
    #define USBP_SOF_VECT_NUM       USBP_sof_int__INTC_NUMBER

    #define USBP_EP_0_PRIOR         USBP_ep_0__INTC_PRIOR_NUM
    #define USBP_EP_0_MASK          USBP_ep_0__INTC_MASK
    #define USBP_EP_0_VECT_NUM      USBP_ep_0__INTC_NUMBER

    #define USBP_EP_1_PRIOR         USBP_ep_1__INTC_PRIOR_NUM
    #define USBP_EP_1_MASK          USBP_ep_1__INTC_MASK
    #define USBP_EP_1_VECT_NUM      USBP_ep_1__INTC_NUMBER

    #define USBP_EP_2_PRIOR         USBP_ep_2__INTC_PRIOR_NUM
    #define USBP_EP_2_MASK          USBP_ep_2__INTC_MASK
    #define USBP_EP_2_VECT_NUM      USBP_ep_2__INTC_NUMBER

    #define USBP_EP_3_PRIOR         USBP_ep_3__INTC_PRIOR_NUM
    #define USBP_EP_3_MASK          USBP_ep_3__INTC_MASK
    #define USBP_EP_3_VECT_NUM      USBP_ep_3__INTC_NUMBER

    #define USBP_EP_4_PRIOR         USBP_ep_4__INTC_PRIOR_NUM
    #define USBP_EP_4_MASK          USBP_ep_4__INTC_MASK
    #define USBP_EP_4_VECT_NUM      USBP_ep_4__INTC_NUMBER

    #define USBP_EP_5_PRIOR         USBP_ep_5__INTC_PRIOR_NUM
    #define USBP_EP_5_MASK          USBP_ep_5__INTC_MASK
    #define USBP_EP_5_VECT_NUM      USBP_ep_5__INTC_NUMBER

    #define USBP_EP_6_PRIOR         USBP_ep_6__INTC_PRIOR_NUM
    #define USBP_EP_6_MASK          USBP_ep_6__INTC_MASK
    #define USBP_EP_6_VECT_NUM      USBP_ep_6__INTC_NUMBER

    #define USBP_EP_7_PRIOR         USBP_ep_7__INTC_PRIOR_NUM
    #define USBP_EP_7_MASK          USBP_ep_7__INTC_MASK
    #define USBP_EP_7_VECT_NUM      USBP_ep_7__INTC_NUMBER

    #define USBP_EP_8_PRIOR         USBP_ep_8__INTC_PRIOR_NUM
    #define USBP_EP_8_MASK          USBP_ep_8__INTC_MASK
    #define USBP_EP_8_VECT_NUM      USBP_ep_8__INTC_NUMBER

    /* Set ARB ISR priority 0 to be highest for all EPX ISRs. */
    #define USBP_ARB_PRIOR          (0u)
    #define USBP_ARB_MASK           USBP_arb_int__INTC_MASK
    #define USBP_ARB_VECT_NUM       USBP_arb_int__INTC_NUMBER
#endif /* (CY_PSOC4) */


/***************************************
*       Endpoint 0 offsets (Table 9-2)
***************************************/
#define USBP_bmRequestTypeReg      USBP_EP0_DR_BASE.epData[0u]
#define USBP_bRequestReg           USBP_EP0_DR_BASE.epData[1u]
#define USBP_wValueLoReg           USBP_EP0_DR_BASE.epData[2u]
#define USBP_wValueHiReg           USBP_EP0_DR_BASE.epData[3u]
#define USBP_wIndexLoReg           USBP_EP0_DR_BASE.epData[4u]
#define USBP_wIndexHiReg           USBP_EP0_DR_BASE.epData[5u]
#define USBP_wLengthLoReg          USBP_EP0_DR_BASE.epData[6u]
#define USBP_wLengthHiReg          USBP_EP0_DR_BASE.epData[7u]

/* Compatibility defines */
#define USBP_lengthLoReg           USBP_EP0_DR_BASE.epData[6u]
#define USBP_lengthHiReg           USBP_EP0_DR_BASE.epData[7u]


/***************************************
*       Register Constants
***************************************/

#define USBP_3500MV     (3500u)
#if (CY_PSOC4)
    #define USBP_VDDD_MV    (CYDEV_VBUS_MV)
#else
    #define USBP_VDDD_MV    (CYDEV_VDDD_MV)
#endif /* (CY_PSOC4) */


/* USBP_USB_CLK */
#define USBP_USB_CLK_CSR_CLK_EN_POS (0u)
#define USBP_USB_CLK_CSR_CLK_EN     ((uint8) ((uint8) 0x1u << USBP_USB_CLK_CSR_CLK_EN_POS))
#define USBP_USB_CLK_ENABLE         (USBP_USB_CLK_CSR_CLK_EN)

/* USBP_CR0 */
#define USBP_CR0_DEVICE_ADDRESS_POS     (0u)
#define USBP_CR0_ENABLE_POS             (7u)
#define USBP_CR0_DEVICE_ADDRESS_MASK    ((uint8) ((uint8) 0x7Fu << USBP_CR0_DEVICE_ADDRESS_POS))
#define USBP_CR0_ENABLE                 ((uint8) ((uint8) 0x01u << USBP_CR0_ENABLE_POS))


/* USBP_CR1 */
#define USBP_CR1_REG_ENABLE_POS         (0u)
#define USBP_CR1_ENABLE_LOCK_POS        (1u)
#define USBP_CR1_BUS_ACTIVITY_POS       (2u)
#define USBP_CR1_TRIM_OFFSET_MSB_POS    (3u)
#define USBP_CR1_REG_ENABLE             ((uint8) ((uint8) 0x1u << USBP_CR1_REG_ENABLE_POS))
#define USBP_CR1_ENABLE_LOCK            ((uint8) ((uint8) 0x1u << USBP_CR1_ENABLE_LOCK_POS))
#define USBP_CR1_BUS_ACTIVITY           ((uint8) ((uint8) 0x1u << USBP_CR1_BUS_ACTIVITY_POS))
#define USBP_CR1_TRIM_OFFSET_MSB        ((uint8) ((uint8) 0x1u << USBP_CR1_TRIM_OFFSET_MSB_POS))

/* USBP_EPX_CNT */
#define USBP_EP0_CNT_DATA_TOGGLE        (0x80u)
#define USBP_EPX_CNT_DATA_TOGGLE        (0x80u)
#define USBP_EPX_CNT0_MASK              (0x0Fu)
#define USBP_EPX_CNTX_MSB_MASK          (0x07u)
#define USBP_EPX_CNTX_ADDR_SHIFT        (0x04u)
#define USBP_EPX_CNTX_ADDR_OFFSET       (0x10u)
#define USBP_EPX_CNTX_CRC_COUNT         (0x02u)
#define USBP_EPX_DATA_BUF_MAX           (512u)

/* USBP_USBIO_CR0 */

#define USBP_USBIO_CR0_TEN              (0x80u)
#define USBP_USBIO_CR0_TSE0             (0x40u)
#define USBP_USBIO_CR0_TD               (0x20u)
#define USBP_USBIO_CR0_RD               (0x01u)

/* USBP_USBIO_CR1 */
#define USBP_USBIO_CR1_DM0_POS      (0u)
#define USBP_USBIO_CR1_DP0_POS      (1u)
#define USBP_USBIO_CR1_USBPUEN_POS  (2u)
#define USBP_USBIO_CR1_IOMODE_POS   (5u)
#define USBP_USBIO_CR1_DM0          ((uint8) ((uint8) 0x1u << USBP_USBIO_CR1_DM0_POS))
#define USBP_USBIO_CR1_DP0          ((uint8) ((uint8) 0x1u << USBP_USBIO_CR1_DP0_POS))
#define USBP_USBIO_CR1_USBPUEN      ((uint8) ((uint8) 0x1u << USBP_USBIO_CR1_USBPUEN_POS))
#define USBP_USBIO_CR1_IOMODE       ((uint8) ((uint8) 0x1u << USBP_USBIO_CR1_IOMODE_POS))

/* USBP_FASTCLK_IMO_CR */
#define USBP_FASTCLK_IMO_CR_USBCLK_ON   (0x40u)
#define USBP_FASTCLK_IMO_CR_XCLKEN      (0x20u)
#define USBP_FASTCLK_IMO_CR_FX2ON       (0x10u)

/* USBP_ARB_EPX_CFG */
#define USBP_ARB_EPX_CFG_IN_DATA_RDY_POS    (0u)
#define USBP_ARB_EPX_CFG_DMA_REQ_POS        (1u)
#define USBP_ARB_EPX_CFG_CRC_BYPASS_POS     (2u)
#define USBP_ARB_EPX_CFG_RESET_POS          (3u)
#define USBP_ARB_EPX_CFG_IN_DATA_RDY        ((uint8) ((uint8) 0x1u << USBP_ARB_EPX_CFG_IN_DATA_RDY_POS))
#define USBP_ARB_EPX_CFG_DMA_REQ            ((uint8) ((uint8) 0x1u << USBP_ARB_EPX_CFG_DMA_REQ_POS))
#define USBP_ARB_EPX_CFG_CRC_BYPASS         ((uint8) ((uint8) 0x1u << USBP_ARB_EPX_CFG_CRC_BYPASS_POS))
#define USBP_ARB_EPX_CFG_RESET              ((uint8) ((uint8) 0x1u << USBP_ARB_EPX_CFG_RESET_POS))

/* USBP_ARB_EPX_INT / SR */
#define USBP_ARB_EPX_INT_IN_BUF_FULL_POS    (0u)
#define USBP_ARB_EPX_INT_DMA_GNT_POS        (1u)
#define USBP_ARB_EPX_INT_BUF_OVER_POS       (2u)
#define USBP_ARB_EPX_INT_BUF_UNDER_POS      (3u)
#define USBP_ARB_EPX_INT_ERR_INT_POS        (4u)
#define USBP_ARB_EPX_INT_IN_BUF_FULL        ((uint8) ((uint8) 0x1u << USBP_ARB_EPX_INT_IN_BUF_FULL_POS))
#define USBP_ARB_EPX_INT_DMA_GNT            ((uint8) ((uint8) 0x1u << USBP_ARB_EPX_INT_DMA_GNT_POS))
#define USBP_ARB_EPX_INT_BUF_OVER           ((uint8) ((uint8) 0x1u << USBP_ARB_EPX_INT_BUF_OVER_POS))
#define USBP_ARB_EPX_INT_BUF_UNDER          ((uint8) ((uint8) 0x1u << USBP_ARB_EPX_INT_BUF_UNDER_POS))
#define USBP_ARB_EPX_INT_ERR_INT            ((uint8) ((uint8) 0x1u << USBP_ARB_EPX_INT_ERR_INT_POS))

#if (CY_PSOC4)
#define USBP_ARB_EPX_INT_DMA_TERMIN_POS     (5u)
#define USBP_ARB_EPX_INT_DMA_TERMIN         ((uint8) ((uint8) 0x1u << USBP_ARB_EPX_INT_DMA_TERMIN_POS))
#endif /* (CY_PSOC4) */

/* Common arbiter interrupt sources for all PSoC devices. */
#define USBP_ARB_EPX_INT_COMMON    (USBP_ARB_EPX_INT_IN_BUF_FULL | \
                                                USBP_ARB_EPX_INT_DMA_GNT     | \
                                                USBP_ARB_EPX_INT_BUF_OVER    | \
                                                USBP_ARB_EPX_INT_BUF_UNDER   | \
                                                USBP_ARB_EPX_INT_ERR_INT)

#if (CY_PSOC4)
    #define USBP_ARB_EPX_INT_ALL    (USBP_ARB_EPX_INT_COMMON | USBP_ARB_EPX_INT_DMA_TERMIN)
#else
    #define USBP_ARB_EPX_INT_ALL    (USBP_ARB_EPX_INT_COMMON)
#endif /* (CY_PSOC4) */

/* USBP_ARB_CFG */
#define USBP_ARB_CFG_AUTO_MEM_POS   (4u)
#define USBP_ARB_CFG_DMA_CFG_POS    (5u)
#define USBP_ARB_CFG_CFG_CMP_POS    (7u)
#define USBP_ARB_CFG_AUTO_MEM       ((uint8) ((uint8) 0x1u << USBP_ARB_CFG_AUTO_MEM_POS))
#define USBP_ARB_CFG_DMA_CFG_MASK   ((uint8) ((uint8) 0x3u << USBP_ARB_CFG_DMA_CFG_POS))
#define USBP_ARB_CFG_DMA_CFG_NONE   ((uint8) ((uint8) 0x0u << USBP_ARB_CFG_DMA_CFG_POS))
#define USBP_ARB_CFG_DMA_CFG_MANUAL ((uint8) ((uint8) 0x1u << USBP_ARB_CFG_DMA_CFG_POS))
#define USBP_ARB_CFG_DMA_CFG_AUTO   ((uint8) ((uint8) 0x2u << USBP_ARB_CFG_DMA_CFG_POS))
#define USBP_ARB_CFG_CFG_CMP        ((uint8) ((uint8) 0x1u << USBP_ARB_CFG_CFG_CMP_POS))

/* USBP_DYN_RECONFIG */
#define USBP_DYN_RECONFIG_EP_SHIFT      (1u)
#define USBP_DYN_RECONFIG_ENABLE_POS    (0u)
#define USBP_DYN_RECONFIG_EPNO_POS      (1u)
#define USBP_DYN_RECONFIG_RDY_STS_POS   (4u)
#define USBP_DYN_RECONFIG_ENABLE        ((uint8) ((uint8) 0x1u << USBP_DYN_RECONFIG_ENABLE_POS))
#define USBP_DYN_RECONFIG_EPNO_MASK     ((uint8) ((uint8) 0x7u << USBP_DYN_RECONFIG_EPNO_POS))
#define USBP_DYN_RECONFIG_RDY_STS       ((uint8) ((uint8) 0x1u << USBP_DYN_RECONFIG_RDY_STS_POS))

/* USBP_ARB_INT */
#define USBP_ARB_INT_EP1_INTR_POS          (0u) /* [0] Interrupt for USB EP1 */
#define USBP_ARB_INT_EP2_INTR_POS          (1u) /* [1] Interrupt for USB EP2 */
#define USBP_ARB_INT_EP3_INTR_POS          (2u) /* [2] Interrupt for USB EP3 */
#define USBP_ARB_INT_EP4_INTR_POS          (3u) /* [3] Interrupt for USB EP4 */
#define USBP_ARB_INT_EP5_INTR_POS          (4u) /* [4] Interrupt for USB EP5 */
#define USBP_ARB_INT_EP6_INTR_POS          (5u) /* [5] Interrupt for USB EP6 */
#define USBP_ARB_INT_EP7_INTR_POS          (6u) /* [6] Interrupt for USB EP7 */
#define USBP_ARB_INT_EP8_INTR_POS          (7u) /* [7] Interrupt for USB EP8 */
#define USBP_ARB_INT_EP1_INTR              ((uint8) ((uint8) 0x1u << USBP_ARB_INT_EP1_INTR_POS))
#define USBP_ARB_INT_EP2_INTR              ((uint8) ((uint8) 0x1u << USBP_ARB_INT_EP2_INTR_POS))
#define USBP_ARB_INT_EP3_INTR              ((uint8) ((uint8) 0x1u << USBP_ARB_INT_EP3_INTR_POS))
#define USBP_ARB_INT_EP4_INTR              ((uint8) ((uint8) 0x1u << USBP_ARB_INT_EP4_INTR_POS))
#define USBP_ARB_INT_EP5_INTR              ((uint8) ((uint8) 0x1u << USBP_ARB_INT_EP5_INTR_POS))
#define USBP_ARB_INT_EP6_INTR              ((uint8) ((uint8) 0x1u << USBP_ARB_INT_EP6_INTR_POS))
#define USBP_ARB_INT_EP7_INTR              ((uint8) ((uint8) 0x1u << USBP_ARB_INT_EP7_INTR_POS))
#define USBP_ARB_INT_EP8_INTR              ((uint8) ((uint8) 0x1u << USBP_ARB_INT_EP8_INTR_POS))

/* USBP_SIE_INT */
#define USBP_SIE_INT_EP1_INTR_POS          (0u) /* [0] Interrupt for USB EP1 */
#define USBP_SIE_INT_EP2_INTR_POS          (1u) /* [1] Interrupt for USB EP2 */
#define USBP_SIE_INT_EP3_INTR_POS          (2u) /* [2] Interrupt for USB EP3 */
#define USBP_SIE_INT_EP4_INTR_POS          (3u) /* [3] Interrupt for USB EP4 */
#define USBP_SIE_INT_EP5_INTR_POS          (4u) /* [4] Interrupt for USB EP5 */
#define USBP_SIE_INT_EP6_INTR_POS          (5u) /* [5] Interrupt for USB EP6 */
#define USBP_SIE_INT_EP7_INTR_POS          (6u) /* [6] Interrupt for USB EP7 */
#define USBP_SIE_INT_EP8_INTR_POS          (7u) /* [7] Interrupt for USB EP8 */
#define USBP_SIE_INT_EP1_INTR              ((uint8) ((uint8) 0x01u << USBP_SIE_INT_EP1_INTR_POS))
#define USBP_SIE_INT_EP2_INTR              ((uint8) ((uint8) 0x01u << USBP_SIE_INT_EP2_INTR_POS))
#define USBP_SIE_INT_EP3_INTR              ((uint8) ((uint8) 0x01u << USBP_SIE_INT_EP3_INTR_POS))
#define USBP_SIE_INT_EP4_INTR              ((uint8) ((uint8) 0x01u << USBP_SIE_INT_EP4_INTR_POS))
#define USBP_SIE_INT_EP5_INTR              ((uint8) ((uint8) 0x01u << USBP_SIE_INT_EP5_INTR_POS))
#define USBP_SIE_INT_EP6_INTR              ((uint8) ((uint8) 0x01u << USBP_SIE_INT_EP6_INTR_POS))
#define USBP_SIE_INT_EP7_INTR              ((uint8) ((uint8) 0x01u << USBP_SIE_INT_EP7_INTR_POS))
#define USBP_SIE_INT_EP8_INTR              ((uint8) ((uint8) 0x01u << USBP_SIE_INT_EP8_INTR_POS))

#if (CY_PSOC4)
    /* USBP_POWER_CTRL_REG */
    #define USBP_POWER_CTRL_VBUS_VALID_OVR_POS       (0u)  /* [0] */
    #define USBP_POWER_CTRL_SUSPEND_POS              (2u)  /* [1] */
    #define USBP_POWER_CTRL_SUSPEND_DEL_POS          (3u)  /* [3] */
    #define USBP_POWER_CTRL_ISOLATE_POS              (4u)  /* [4] */
    #define USBP_POWER_CTRL_CHDET_PWR_CTL_POS        (5u)  /* [5] */
    #define USBP_POWER_CTRL_ENABLE_DM_PULLDOWN_POS   (25u) /* [25] */
    #define USBP_POWER_CTRL_ENABLE_VBUS_PULLDOWN_POS (26u) /* [26] */
    #define USBP_POWER_CTRL_ENABLE_RCVR_POS          (27u) /* [27] */
    #define USBP_POWER_CTRL_ENABLE_DPO_POS           (28u) /* [28] */
    #define USBP_POWER_CTRL_ENABLE_DMO_POS           (29u) /* [29] */
    #define USBP_POWER_CTRL_ENABLE_CHGDET_POS        (30u) /* [30] */
    #define USBP_POWER_CTRL_ENABLE_POS               (31u) /* [31] */
    #define USBP_POWER_CTRL_VBUS_VALID_OVR_MASK      ((uint32) 0x03u << USBP_POWER_CTRL_VBUS_VALID_OVR_POS)
    #define USBP_POWER_CTRL_VBUS_VALID_OVR_0         ((uint32) 0x00u << USBP_POWER_CTRL_VBUS_VALID_OVR_POS)
    #define USBP_POWER_CTRL_VBUS_VALID_OVR_1         ((uint32) 0x01u << USBP_POWER_CTRL_VBUS_VALID_OVR_POS)
    #define USBP_POWER_CTRL_VBUS_VALID_OVR_GPIO      ((uint32) 0x02u << USBP_POWER_CTRL_VBUS_VALID_OVR_POS)
    #define USBP_POWER_CTRL_VBUS_VALID_OVR_PHY       ((uint32) 0x03u << USBP_POWER_CTRL_VBUS_VALID_OVR_POS)
    #define USBP_POWER_CTRL_SUSPEND                  ((uint32) 0x01u << USBP_POWER_CTRL_SUSPEND_POS)
    #define USBP_POWER_CTRL_SUSPEND_DEL              ((uint32) 0x01u << USBP_POWER_CTRL_SUSPEND_DEL_POS)
    #define USBP_POWER_CTRL_ISOLATE                  ((uint32) 0x01u << USBP_POWER_CTRL_ISOLATE_POS)
    #define USBP_POWER_CTRL_CHDET_PWR_CTL_MASK       ((uint32) 0x03u << USBP_POWER_CTRL_CHDET_PWR_CTL_POS)
    #define USBP_POWER_CTRL_ENABLE_DM_PULLDOWN       ((uint32) 0x01u << USBP_POWER_CTRL_ENABLE_DM_PULLDOWN_POS)
    #define USBP_POWER_CTRL_ENABLE_VBUS_PULLDOWN     ((uint32) 0x01u << USBP_POWER_CTRL_ENABLE_VBUS_PULLDOWN_POS)
    #define USBP_POWER_CTRL_ENABLE_RCVR              ((uint32) 0x01u << USBP_POWER_CTRL_ENABLE_RCVR_POS)
    #define USBP_POWER_CTRL_ENABLE_DPO               ((uint32) 0x01u << USBP_POWER_CTRL_ENABLE_DPO_POS)
    #define USBP_POWER_CTRL_ENABLE_DMO               ((uint32) 0x01u << USBP_POWER_CTRL_ENABLE_DMO_POS)
    #define USBP_POWER_CTRL_ENABLE_CHGDET            ((uint32) 0x01u << USBP_POWER_CTRL_ENABLE_CHGDET_POS)
    #define USBP_POWER_CTRL_ENABLE                   ((uint32) 0x01u << USBP_POWER_CTRL_ENABLE_POS)

    /* USBP_CHGDET_CTRL_REG */
    #define USBP_CHGDET_CTRL_COMP_DP_POS        (0u)  /* [0] */
    #define USBP_CHGDET_CTRL_COMP_DM_POS        (1u)  /* [1] */
    #define USBP_CHGDET_CTRL_COMP_EN_POS        (2u)  /* [2] */
    #define USBP_CHGDET_CTRL_REF_DP_POS         (3u)  /* [3] */
    #define USBP_CHGDET_CTRL_REF_DM_POS         (4u)  /* [4] */
    #define USBP_CHGDET_CTRL_REF_EN_POS         (5u)  /* [5] */
    #define USBP_CHGDET_CTRL_DCD_SRC_EN_POS     (6u)  /* [6] */
    #define USBP_CHGDET_CTRL_ADFT_CTRL_POS      (12u) /* [12] */
    #define USBP_CHGDET_CTRL_COMP_OUT_POS       (31u) /* [31] */
    #define USBP_CHGDET_CTRL_COMP_DP            ((uint32) 0x01u << USBP_CHGDET_CTRL_COMP_DP_POS)
    #define USBP_CHGDET_CTRL_COMP_DM            ((uint32) 0x01u << USBP_CHGDET_CTRL_COMP_DM_POS)
    #define USBP_CHGDET_CTRL_COMP_EN            ((uint32) 0x01u << USBP_CHGDET_CTRL_COMP_EN_POS)
    #define USBP_CHGDET_CTRL_REF_DP             ((uint32) 0x01u << USBP_CHGDET_CTRL_REF_DP_POS)
    #define USBP_CHGDET_CTRL_REF_DM             ((uint32) 0x01u << USBP_CHGDET_CTRL_REF_DM_POS)
    #define USBP_CHGDET_CTRL_REF_EN             ((uint32) 0x01u << USBP_CHGDET_CTRL_REF_EN_POS)
    #define USBP_CHGDET_CTRL_DCD_SRC_EN         ((uint32) 0x01u << USBP_CHGDET_CTRL_DCD_SRC_EN_POS)
    #define USBP_CHGDET_CTRL_ADFT_CTRL_MASK     ((uint32) 0x03u << USBP_CHGDET_CTRL_ADFT_CTRL_POS)
    #define USBP_CHGDET_CTRL_ADFT_CTRL_NORMAL   ((uint32) 0x00u << USBP_CHGDET_CTRL_ADFT_CTRL_POS)
    #define USBP_CHGDET_CTRL_ADFT_CTRL_VBG      ((uint32) 0x01u << USBP_CHGDET_CTRL_ADFT_CTRL_POS)
    #define USBP_CHGDET_CTRL_ADFT_CTRL_DONTUSE  ((uint32) 0x02u << USBP_CHGDET_CTRL_ADFT_CTRL_POS)
    #define USBP_CHGDET_CTRL_ADFT_CTRL_ADFTIN   ((uint32) 0x03u << USBP_CHGDET_CTRL_ADFT_CTRL_POS)
    #define USBP_CHGDET_CTRL_COMP_OUT           ((uint32) 0x01u << USBP_CHGDET_CTRL_COMP_OUT_POS)

    /* USBP_LPM_CTRL */
    #define USBP_LPM_CTRL_LPM_EN_POS        (0u)
    #define USBP_LPM_CTRL_LPM_ACK_RESP_POS  (1u)
    #define USBP_LPM_CTRL_NYET_EN_POS       (2u)
    #define USBP_LPM_CTRL_SUB_RESP_POS      (4u)
    #define USBP_LPM_CTRL_LPM_EN            ((uint32) 0x01u << USBP_LPM_CTRL_LPM_EN_POS)
    #define USBP_LPM_CTRL_LPM_ACK_RESP      ((uint32) 0x01u << USBP_LPM_CTRL_LPM_ACK_RESP_POS)
    #define USBP_LPM_CTRL_NYET_EN           ((uint32) 0x01u << USBP_LPM_CTRL_NYET_EN_POS)
    #define USBP_LPM_CTRL_ACK_NYET_MASK     ((uint32) 0x03u << USBP_LPM_CTRL_LPM_ACK_RESP_POS)
    #define USBP_LPM_CTRL_SUB_RESP          ((uint32) 0x01u << USBP_LPM_CTRL_SUB_RESP_POS)

    #define USBP_LPM_STAT_LPM_BESL_POS          (0u)
    #define USBP_LPM_STAT_LPM_REMOTE_WAKE_POS   (4u)
    #define USBP_LPM_STAT_LPM_BESL_MASK         ((uint32) 0x0Fu << USBP_LPM_STAT_LPM_BESL_POS)
    #define USBP_LPM_STAT_LPM_REMOTE_WAKE       ((uint32) 0x01u << USBP_LPM_STAT_LPM_REMOTE_WAKE_POS)

    /* USBP_INTR_SIE */
    #define USBP_INTR_SIE_SOF_INTR_POS          (0u) /* [0] Interrupt for USB SOF   */
    #define USBP_INTR_SIE_BUS_RESET_INTR_POS    (1u) /* [1] Interrupt for BUS RESET */
    #define USBP_INTR_SIE_EP0_INTR_POS          (2u) /* [2] Interrupt for EP0       */
    #define USBP_INTR_SIE_LPM_INTR_POS          (3u) /* [3] Interrupt for LPM       */
    #define USBP_INTR_SIE_RESUME_INTR_POS       (4u) /* [4] Interrupt for RESUME (not used by component) */
    #define USBP_INTR_SIE_SOF_INTR              ((uint32) 0x01u << USBP_INTR_SIE_SOF_INTR_POS)
    #define USBP_INTR_SIE_BUS_RESET_INTR        ((uint32) 0x01u << USBP_INTR_SIE_BUS_RESET_INTR_POS)
    #define USBP_INTR_SIE_EP0_INTR              ((uint32) 0x01u << USBP_INTR_SIE_EP0_INTR_POS)
    #define USBP_INTR_SIE_LPM_INTR              ((uint32) 0x01u << USBP_INTR_SIE_LPM_INTR_POS)
    #define USBP_INTR_SIE_RESUME_INTR           ((uint32) 0x01u << USBP_INTR_SIE_RESUME_INTR_POS)

    /* USBP_INTR_CAUSE_LO, MED and HI */
    #define USBP_INTR_CAUSE_SOF_INTR_POS       (0u)  /* [0] Interrupt status for USB SOF    */
    #define USBP_INTR_CAUSE_BUS_RESET_INTR_POS (1u)  /* [1] Interrupt status for USB BUS RSET */
    #define USBP_INTR_CAUSE_EP0_INTR_POS       (2u)  /* [2] Interrupt status for USB EP0    */
    #define USBP_INTR_CAUSE_LPM_INTR_POS       (3u)  /* [3] Interrupt status for USB LPM    */
    #define USBP_INTR_CAUSE_RESUME_INTR_POS    (4u)  /* [4] Interrupt status for USB RESUME */
    #define USBP_INTR_CAUSE_ARB_INTR_POS       (7u)  /* [7] Interrupt status for USB ARB    */
    #define USBP_INTR_CAUSE_EP1_INTR_POS       (8u)  /* [8] Interrupt status for USB EP1    */
    #define USBP_INTR_CAUSE_EP2_INTR_POS       (9u)  /* [9] Interrupt status for USB EP2    */
    #define USBP_INTR_CAUSE_EP3_INTR_POS       (10u) /* [10] Interrupt status for USB EP3   */
    #define USBP_INTR_CAUSE_EP4_INTR_POS       (11u) /* [11] Interrupt status for USB EP4   */
    #define USBP_INTR_CAUSE_EP5_INTR_POS       (12u) /* [12] Interrupt status for USB EP5   */
    #define USBP_INTR_CAUSE_EP6_INTR_POS       (13u) /* [13] Interrupt status for USB EP6   */
    #define USBP_INTR_CAUSE_EP7_INTR_POS       (14u) /* [14] Interrupt status for USB EP7   */
    #define USBP_INTR_CAUSE_EP8_INTR_POS       (15u) /* [15] Interrupt status for USB EP8   */
    #define USBP_INTR_CAUSE_SOF_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_SOF_INTR_POS)
    #define USBP_INTR_CAUSE_BUS_RESET_INTR     ((uint32) 0x01u << USBP_INTR_CAUSE_BUS_RESET_INTR_POS)
    #define USBP_INTR_CAUSE_EP0_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_EP0_INTR_POS)
    #define USBP_INTR_CAUSE_LPM_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_LPM_INTR_POS)
    #define USBP_INTR_CAUSE_RESUME_INTR        ((uint32) 0x01u << USBP_INTR_CAUSE_RESUME_INTR_POS)
    #define USBP_INTR_CAUSE_ARB_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_ARB_INTR_POS)
    #define USBP_INTR_CAUSE_EP1_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_EP1_INTR_POS)
    #define USBP_INTR_CAUSE_EP2_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_EP2_INTR_POS)
    #define USBP_INTR_CAUSE_EP3_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_EP3_INTR_POS)
    #define USBP_INTR_CAUSE_EP4_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_EP4_INTR_POS)
    #define USBP_INTR_CAUSE_EP5_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_EP5_INTR_POS)
    #define USBP_INTR_CAUSE_EP6_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_EP6_INTR_POS)
    #define USBP_INTR_CAUSE_EP7_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_EP7_INTR_POS)
    #define USBP_INTR_CAUSE_EP8_INTR           ((uint32) 0x01u << USBP_INTR_CAUSE_EP8_INTR_POS)

    #define USBP_INTR_CAUSE_CTRL_INTR_MASK     (USBP_INTR_CAUSE_SOF_INTR       | \
                                                            USBP_INTR_CAUSE_BUS_RESET_INTR | \
                                                            USBP_INTR_CAUSE_EP0_INTR       | \
                                                            USBP_INTR_CAUSE_LPM_INTR)

    #define USBP_INTR_CAUSE_EP1_8_INTR_MASK    (USBP_INTR_CAUSE_EP1_INTR       | \
                                                            USBP_INTR_CAUSE_EP2_INTR       | \
                                                            USBP_INTR_CAUSE_EP3_INTR       | \
                                                            USBP_INTR_CAUSE_EP4_INTR       | \
                                                            USBP_INTR_CAUSE_EP5_INTR       | \
                                                            USBP_INTR_CAUSE_EP6_INTR       | \
                                                            USBP_INTR_CAUSE_EP7_INTR       | \
                                                            USBP_INTR_CAUSE_EP8_INTR)

    #define USBP_INTR_CAUSE_EP_INTR_SHIFT      (USBP_INTR_CAUSE_ARB_INTR_POS - \
                                                           (USBP_INTR_CAUSE_LPM_INTR_POS + 1u))
    #define USBP_INTR_CAUSE_SRC_COUNT          (13u)

    #define USBP_CHGDET_CTRL_PRIMARY    (USBP_CHGDET_CTRL_COMP_EN | \
                                                     USBP_CHGDET_CTRL_COMP_DM | \
                                                     USBP_CHGDET_CTRL_REF_EN  | \
                                                     USBP_CHGDET_CTRL_REF_DP)

    #define USBP_CHGDET_CTRL_SECONDARY  (USBP_CHGDET_CTRL_COMP_EN | \
                                                     USBP_CHGDET_CTRL_COMP_DP | \
                                                     USBP_CHGDET_CTRL_REF_EN  | \
                                                     USBP_CHGDET_CTRL_REF_DM)

    #define USBP_CHGDET_CTRL_DEFAULT    (0x00000900u)


#else /* (CY_PSOC3 || CY_PSOC5LP) */
    #define USBP_PM_ACT_EN_FSUSB            USBP_USB__PM_ACT_MSK
    #define USBP_PM_STBY_EN_FSUSB           USBP_USB__PM_STBY_MSK
    #define USBP_PM_AVAIL_EN_FSUSBIO        (0x10u)

    #define USBP_PM_USB_CR0_REF_EN          (0x01u)
    #define USBP_PM_USB_CR0_PD_N            (0x02u)
    #define USBP_PM_USB_CR0_PD_PULLUP_N     (0x04u)
#endif /* (CY_PSOC4) */


/***************************************
*       Macros Definitions
***************************************/

#if (CY_PSOC4)
    #define USBP_ClearSieInterruptSource(intMask) \
                do{ \
                    USBP_INTR_SIE_REG = (uint32) (intMask); \
                }while(0)
#else
    #define USBP_ClearSieInterruptSource(intMask) \
                do{ /* Does nothing. */ }while(0)
#endif /* (CY_PSOC4) */

#define USBP_ClearSieEpInterruptSource(intMask) \
            do{ \
                USBP_SIE_EP_INT_SR_REG = (uint8) (intMask); \
            }while(0)

#define USBP_GET_ACTIVE_IN_EP_CR0_MODE(epType)  (((epType) == USBP_EP_TYPE_ISOC) ? \
                                                                (USBP_MODE_ISO_IN) : (USBP_MODE_ACK_IN))

#define USBP_GET_ACTIVE_OUT_EP_CR0_MODE(epType) (((epType) == USBP_EP_TYPE_ISOC) ? \
                                                                (USBP_MODE_ISO_OUT) : (USBP_MODE_ACK_OUT))

#define USBP_GET_EP_TYPE(epNumber)  (USBP_EP[epNumber].attrib & USBP_EP_TYPE_MASK)

#define USBP_GET_UINT16(hi, low)    (((uint16) ((uint16) (hi) << 8u)) | ((uint16) (low) & 0xFFu))


/***************************************
*    Initialization Register Settings
***************************************/

/* Clear device address and enable USB IP respond to USB traffic. */
#define USBP_DEFUALT_CR0    (USBP_CR0_ENABLE)

/* Arbiter configuration depends on memory management mode. */
#define USBP_DEFAULT_ARB_CFG    ((USBP_EP_MANAGEMENT_MANUAL) ? (USBP_ARB_CFG_DMA_CFG_NONE) : \
                                                ((USBP_EP_MANAGEMENT_DMA_MANUAL) ? \
                                                    (USBP_ARB_CFG_DMA_CFG_MANUAL) : \
                                                        (USBP_ARB_CFG_AUTO_MEM | USBP_ARB_CFG_DMA_CFG_AUTO)))

/* Enable arbiter interrupt for active endpoints only */
#define USBP_DEFAULT_ARB_INT_EN \
        ((uint8) ((uint8) USBP_DMA1_ACTIVE << USBP_ARB_INT_EP1_INTR_POS) | \
         (uint8) ((uint8) USBP_DMA2_ACTIVE << USBP_ARB_INT_EP2_INTR_POS) | \
         (uint8) ((uint8) USBP_DMA3_ACTIVE << USBP_ARB_INT_EP3_INTR_POS) | \
         (uint8) ((uint8) USBP_DMA4_ACTIVE << USBP_ARB_INT_EP4_INTR_POS) | \
         (uint8) ((uint8) USBP_DMA5_ACTIVE << USBP_ARB_INT_EP5_INTR_POS) | \
         (uint8) ((uint8) USBP_DMA6_ACTIVE << USBP_ARB_INT_EP6_INTR_POS) | \
         (uint8) ((uint8) USBP_DMA7_ACTIVE << USBP_ARB_INT_EP7_INTR_POS) | \
         (uint8) ((uint8) USBP_DMA8_ACTIVE << USBP_ARB_INT_EP8_INTR_POS))

/* Enable all SIE endpoints interrupts */
#define USBP_DEFAULT_SIE_EP_INT_EN  (USBP_SIE_INT_EP1_INTR | \
                                                 USBP_SIE_INT_EP2_INTR | \
                                                 USBP_SIE_INT_EP3_INTR | \
                                                 USBP_SIE_INT_EP4_INTR | \
                                                 USBP_SIE_INT_EP5_INTR | \
                                                 USBP_SIE_INT_EP6_INTR | \
                                                 USBP_SIE_INT_EP7_INTR | \
                                                 USBP_SIE_INT_EP8_INTR)

#define USBP_ARB_EPX_CFG_DEFAULT    (USBP_ARB_EPX_CFG_RESET | \
                                                 USBP_ARB_EPX_CFG_CRC_BYPASS)

/* Default EP arbiter interrupt source register */
#define USBP_ARB_EPX_INT_COMMON_MASK   (USBP_ARB_EPX_INT_IN_BUF_FULL | \
                                                    USBP_ARB_EPX_INT_BUF_OVER    | \
                                                    USBP_ARB_EPX_INT_BUF_UNDER   | \
                                                    USBP_ARB_EPX_INT_ERR_INT     | \
                                                    (USBP_EP_MANAGEMENT_DMA_MANUAL ? USBP_ARB_EPX_INT_DMA_GNT : 0u))

#define USBP_CLEAR_REG      (0u)

#if (CY_PSOC4)
    /* Set USB lock option when IMO is locked to USB traffic. */
    #define USBP_DEFUALT_CR1    ((0u != CySysClkImoGetUsbLock()) ? (USBP_CR1_ENABLE_LOCK) : (0u))

    /* Recommended value is increased from 3 to 10 due to suppress glitch on  
     * RSE0 with USB2.0 hubs (LF CLK = 32kHz equal to 350us). */
    #define USBP_DEFUALT_BUS_RST_CNT  (10u)

    /* Select VBUS sources as: valid, PHY of GPIO, and clears isolate bit. */
    /* Application level must ensure that VBUS is valid valid to use. */
    #define USBP_DEFAULT_POWER_CTRL_VBUS    (USBP_POWER_CTRL_ENABLE_VBUS_PULLDOWN | \
                                                         ((!USBP_VBUS_MONITORING_ENABLE) ? \
                                                            (USBP_POWER_CTRL_VBUS_VALID_OVR_1) : \
                                                                (USBP_VBUS_POWER_PAD_ENABLE ? \
                                                                    (USBP_POWER_CTRL_VBUS_VALID_OVR_PHY) : \
                                                                    (USBP_POWER_CTRL_VBUS_VALID_OVR_GPIO))))
    /* Enable USB IP. */
    #define USBP_DEFAULT_POWER_CTRL_PHY (USBP_POWER_CTRL_SUSPEND     | \
                                                     USBP_POWER_CTRL_SUSPEND_DEL | \
                                                     USBP_POWER_CTRL_ENABLE_RCVR | \
                                                     USBP_POWER_CTRL_ENABLE_DPO  | \
                                                     USBP_POWER_CTRL_ENABLE_DMO  | \
                                                     USBP_POWER_CTRL_ENABLE)

    /* Assign interrupt between levels lo, med, hi. */
    #define USBP_DEFAULT_INTR_LVL_SEL   ((uint32) (USBP_INTR_LVL_SEL))

    /* Enable interrupt source in the INTR_SIE. The SOF is always disabled and EP0 is enabled. */
    #define USBP_DEFAULT_INTR_SIE_MASK \
                ((uint32) ((uint32) USBP_BUS_RESET_ISR_ACTIVE << USBP_INTR_SIE_BUS_RESET_INTR_POS) | \
                 (uint32) ((uint32) USBP_SOF_ISR_ACTIVE       << USBP_INTR_SIE_SOF_INTR_POS)       | \
                 (uint32) ((uint32) USBP_LPM_ACTIVE           << USBP_INTR_SIE_LPM_INTR_POS)       | \
                 (uint32) ((uint32) USBP_INTR_SIE_EP0_INTR))

    /* Arbiter interrupt sources */
    #define USBP_ARB_EPX_INT_MASK   (USBP_ARB_EPX_INT_COMMON_MASK | \
                                                (USBP_EP_MANAGEMENT_DMA_AUTO ? USBP_ARB_EPX_INT_DMA_TERMIN : 0u))

    /* Common DMA configuration */
    #define USBP_DMA_COMMON_CFG     (CYDMA_PULSE | CYDMA_ENTIRE_DESCRIPTOR | \
                                                 CYDMA_NON_PREEMPTABLE)


#else
    #define USBP_ARB_EPX_INT_MASK   (USBP_ARB_EPX_INT_COMMON_MASK)

    #define USBP_DEFUALT_CR1        (USBP_CR1_ENABLE_LOCK)

    /* Recommended value is 3 for LF CLK = 100kHz equal to 100us. */
    #define USBP_DEFUALT_BUS_RST_CNT    (10u)
#endif /* (CY_PSOC4) */

/*
* \addtogroup group_deprecated
* @{
*/

/***************************************
* The following code is DEPRECATED and
* must not be used.
***************************************/

/* Renamed type definitions */
#define USBP_CODE CYCODE
#define USBP_FAR CYFAR
#if defined(__C51__) || defined(__CX51__)
    #define USBP_DATA data
    #define USBP_XDATA xdata
#else
    #define USBP_DATA
    #define USBP_XDATA
#endif /*  __C51__ */
#define USBP_NULL       NULL
/** @} deprecated */
/* Renamed structure fields */
#define wBuffOffset         buffOffset
#define wBufferSize         bufferSize
#define bStatus             status
#define wLength             length
#define wCount              count

/* Renamed global variable */
#define CurrentTD           USBP_currentTD
#define USBP_interfaceSetting_last       USBP_interfaceSettingLast

/* Renamed global constants */
#define USBP_DWR_VDDD_OPERATION         (USBP_DWR_POWER_OPERATION)

/* Renamed functions */
#define USBP_bCheckActivity             USBP_CheckActivity
#define USBP_bGetConfiguration          USBP_GetConfiguration
#define USBP_bGetInterfaceSetting       USBP_GetInterfaceSetting
#define USBP_bGetEPState                USBP_GetEPState
#define USBP_wGetEPCount                USBP_GetEPCount
#define USBP_bGetEPAckState             USBP_GetEPAckState
#define USBP_bRWUEnabled                USBP_RWUEnabled
#define USBP_bVBusPresent               USBP_VBusPresent

#define USBP_bConfiguration             USBP_configuration
#define USBP_bInterfaceSetting          USBP_interfaceSetting
#define USBP_bDeviceAddress             USBP_deviceAddress
#define USBP_bDeviceStatus              USBP_deviceStatus
#define USBP_bDevice                    USBP_device
#define USBP_bTransferState             USBP_transferState
#define USBP_bLastPacketSize            USBP_lastPacketSize

#define USBP_LoadEP                     USBP_LoadInEP
#define USBP_LoadInISOCEP               USBP_LoadInEP
#define USBP_EnableOutISOCEP            USBP_EnableOutEP

#define USBP_SetVector                  CyIntSetVector
#define USBP_SetPriority                CyIntSetPriority
#define USBP_EnableInt                  CyIntEnable

/* Replace with register access. */
#define USBP_bmRequestType      USBP_EP0_DR0_PTR
#define USBP_bRequest           USBP_EP0_DR1_PTR
#define USBP_wValue             USBP_EP0_DR2_PTR
#define USBP_wValueHi           USBP_EP0_DR3_PTR
#define USBP_wValueLo           USBP_EP0_DR2_PTR
#define USBP_wIndex             USBP_EP0_DR4_PTR
#define USBP_wIndexHi           USBP_EP0_DR5_PTR
#define USBP_wIndexLo           USBP_EP0_DR4_PTR
#define USBP_length             USBP_EP0_DR6_PTR
#define USBP_lengthHi           USBP_EP0_DR7_PTR
#define USBP_lengthLo           USBP_EP0_DR6_PTR

/* Rename VBUS monitoring registers. */
#if (CY_PSOC3 || CY_PSOC5LP)
    #if (USBP_VBUS_MONITORING_ENABLE)
        #if (USBP_VBUS_MONITORING_INTERNAL)
            #define USBP_VBUS_DR_PTR    ( (reg8 *) USBP_VBUS__DR)
            #define USBP_VBUS_DR_REG    (*(reg8 *) USBP_VBUS__DR)
            #define USBP_VBUS_PS_PTR    ( (reg8 *) USBP_VBUS__PS)
            #define USBP_VBUS_PS_REG    (*(reg8 *) USBP_VBUS__PS)
            #define USBP_VBUS_MASK          USBP_VBUS__MASK
        #else
            #define USBP_VBUS_PS_PTR    ( (reg8 *) USBP_Vbus_ps_sts_sts_reg__STATUS_REG)
            #define USBP_VBUS_MASK      (0x01u)
        #endif /* (USBP_VBUS_MONITORING_INTERNAL) */
    #endif /*(USBP_VBUS_MONITORING_ENABLE) */
        
    /* Pointer DIE structure in flash (8 bytes): Y and X location, wafer, lot msb, lot lsb, 
    *  work week, fab/year, minor. */
    #define USBP_DIE_ID             CYDEV_FLSHID_CUST_TABLES_BASE

     #if (USBP_EP_MANAGEMENT_DMA_AUTO)
        #if (USBP_DMA1_ACTIVE)
            #define USBP_ep1_TD_TERMOUT_EN  (USBP_ep1__TD_TERMOUT_EN)
        #else
            #define USBP_ep1_TD_TERMOUT_EN  (0u)
        #endif /* (USBP_DMA1_ACTIVE) */

        #if (USBP_DMA2_ACTIVE)
            #define USBP_ep2_TD_TERMOUT_EN  (USBP_ep2__TD_TERMOUT_EN)
        #else
            #define USBP_ep2_TD_TERMOUT_EN  (0u)
        #endif /* (USBP_DMA2_ACTIVE) */

        #if (USBP_DMA3_ACTIVE)
            #define USBP_ep3_TD_TERMOUT_EN  (USBP_ep3__TD_TERMOUT_EN)
        #else
            #define USBP_ep3_TD_TERMOUT_EN  (0u)
        #endif /* (USBP_DMA3_ACTIVE) */

        #if (USBP_DMA4_ACTIVE)
            #define USBP_ep4_TD_TERMOUT_EN  (USBP_ep4__TD_TERMOUT_EN)
        #else
            #define USBP_ep4_TD_TERMOUT_EN  (0u)
        #endif /* (USBP_DMA4_ACTIVE) */

        #if (USBP_DMA5_ACTIVE)
            #define USBP_ep5_TD_TERMOUT_EN  (USBP_ep5__TD_TERMOUT_EN)
        #else
            #define USBP_ep5_TD_TERMOUT_EN  (0u)
        #endif /* (USBP_DMA5_ACTIVE) */

        #if (USBP_DMA6_ACTIVE)
            #define USBP_ep6_TD_TERMOUT_EN  (USBP_ep6__TD_TERMOUT_EN)
        #else
            #define USBP_ep6_TD_TERMOUT_EN  (0u)
        #endif /* (USBP_DMA6_ACTIVE) */

        #if (USBP_DMA7_ACTIVE)
            #define USBP_ep7_TD_TERMOUT_EN  (USBP_ep7__TD_TERMOUT_EN)
        #else
            #define USBP_ep7_TD_TERMOUT_EN  (0u)
        #endif /* (USBP_DMA7_ACTIVE) */

        #if (USBP_DMA8_ACTIVE)
            #define USBP_ep8_TD_TERMOUT_EN  (USBP_ep8__TD_TERMOUT_EN)
        #else
            #define USBP_ep8_TD_TERMOUT_EN  (0u)
        #endif /* (USBP_DMA8_ACTIVE) */
    #endif /* (USBP_EP_MANAGEMENT_DMA_AUTO) */   
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

/* Rename USB IP registers. */
#define USBP_ARB_CFG        USBP_ARB_CFG_PTR

#define USBP_ARB_EP1_CFG    USBP_ARB_EP1_CFG_PTR
#define USBP_ARB_EP1_INT_EN USBP_ARB_EP1_INT_EN_PTR
#define USBP_ARB_EP1_SR     USBP_ARB_EP1_SR_PTR

#define USBP_ARB_EP2_CFG    USBP_ARB_EP2_CFG_PTR
#define USBP_ARB_EP2_INT_EN USBP_ARB_EP2_INT_EN_PTR
#define USBP_ARB_EP2_SR     USBP_ARB_EP2_SR_PTR

#define USBP_ARB_EP3_CFG    USBP_ARB_EP3_CFG_PTR
#define USBP_ARB_EP3_INT_EN USBP_ARB_EP3_INT_EN_PTR
#define USBP_ARB_EP3_SR     USBP_ARB_EP3_SR_PTR

#define USBP_ARB_EP4_CFG    USBP_ARB_EP4_CFG_PTR
#define USBP_ARB_EP4_INT_EN USBP_ARB_EP4_INT_EN_PTR
#define USBP_ARB_EP4_SR     USBP_ARB_EP4_SR_PTR

#define USBP_ARB_EP5_CFG    USBP_ARB_EP5_CFG_PTR
#define USBP_ARB_EP5_INT_EN USBP_ARB_EP5_INT_EN_PTR
#define USBP_ARB_EP5_SR     USBP_ARB_EP5_SR_PTR

#define USBP_ARB_EP6_CFG    USBP_ARB_EP6_CFG_PTR
#define USBP_ARB_EP6_INT_EN USBP_ARB_EP6_INT_EN_PTR
#define USBP_ARB_EP6_SR     USBP_ARB_EP6_SR_PTR

#define USBP_ARB_EP7_CFG    USBP_ARB_EP7_CFG_PTR
#define USBP_ARB_EP7_INT_EN USBP_ARB_EP7_INT_EN_PTR
#define USBP_ARB_EP7_SR     USBP_ARB_EP7_SR_PTR

#define USBP_ARB_EP8_CFG    USBP_ARB_EP8_CFG_PTR
#define USBP_ARB_EP8_INT_EN USBP_ARB_EP8_INT_EN_PTR
#define USBP_ARB_EP8_SR     USBP_ARB_EP8_SR_PTR

#define USBP_ARB_INT_EN     USBP_ARB_INT_EN_PTR
#define USBP_ARB_INT_SR     USBP_ARB_INT_SR_PTR

#define USBP_ARB_RW1_DR     USBP_ARB_RW1_DR_PTR
#define USBP_ARB_RW1_RA     USBP_ARB_RW1_RA_PTR
#define USBP_ARB_RW1_RA_MSB USBP_ARB_RW1_RA_MSB_PTR
#define USBP_ARB_RW1_WA     USBP_ARB_RW1_WA_PTR
#define USBP_ARB_RW1_WA_MSB USBP_ARB_RW1_WA_MSB_PTR

#define USBP_ARB_RW2_DR     USBP_ARB_RW2_DR_PTR
#define USBP_ARB_RW2_RA     USBP_ARB_RW2_RA_PTR
#define USBP_ARB_RW2_RA_MSB USBP_ARB_RW2_RA_MSB_PTR
#define USBP_ARB_RW2_WA     USBP_ARB_RW2_WA_PTR
#define USBP_ARB_RW2_WA_MSB USBP_ARB_RW2_WA_MSB_PTR

#define USBP_ARB_RW3_DR     USBP_ARB_RW3_DR_PTR
#define USBP_ARB_RW3_RA     USBP_ARB_RW3_RA_PTR
#define USBP_ARB_RW3_RA_MSB USBP_ARB_RW3_RA_MSB_PTR
#define USBP_ARB_RW3_WA     USBP_ARB_RW3_WA_PTR
#define USBP_ARB_RW3_WA_MSB USBP_ARB_RW3_WA_MSB_PTR

#define USBP_ARB_RW4_DR     USBP_ARB_RW4_DR_PTR
#define USBP_ARB_RW4_RA     USBP_ARB_RW4_RA_PTR
#define USBP_ARB_RW4_RA_MSB USBP_ARB_RW4_RA_MSB_PTR
#define USBP_ARB_RW4_WA     USBP_ARB_RW4_WA_PTR
#define USBP_ARB_RW4_WA_MSB USBP_ARB_RW4_WA_MSB_PTR

#define USBP_ARB_RW5_DR     USBP_ARB_RW5_DR_PTR
#define USBP_ARB_RW5_RA     USBP_ARB_RW5_RA_PTR
#define USBP_ARB_RW5_RA_MSB USBP_ARB_RW5_RA_MSB_PTR
#define USBP_ARB_RW5_WA     USBP_ARB_RW5_WA_PTR
#define USBP_ARB_RW5_WA_MSB USBP_ARB_RW5_WA_MSB_PTR

#define USBP_ARB_RW6_DR     USBP_ARB_RW6_DR_PTR
#define USBP_ARB_RW6_RA     USBP_ARB_RW6_RA_PTR
#define USBP_ARB_RW6_RA_MSB USBP_ARB_RW6_RA_MSB_PTR
#define USBP_ARB_RW6_WA     USBP_ARB_RW6_WA_PTR
#define USBP_ARB_RW6_WA_MSB USBP_ARB_RW6_WA_MSB_PTR

#define USBP_ARB_RW7_DR     USBP_ARB_RW7_DR_PTR
#define USBP_ARB_RW7_RA     USBP_ARB_RW7_RA_PTR
#define USBP_ARB_RW7_RA_MSB USBP_ARB_RW7_RA_MSB_PTR
#define USBP_ARB_RW7_WA     USBP_ARB_RW7_WA_PTR
#define USBP_ARB_RW7_WA_MSB USBP_ARB_RW7_WA_MSB_PTR

#define USBP_ARB_RW8_DR     USBP_ARB_RW8_DR_PTR
#define USBP_ARB_RW8_RA     USBP_ARB_RW8_RA_PTR
#define USBP_ARB_RW8_RA_MSB USBP_ARB_RW8_RA_MSB_PTR
#define USBP_ARB_RW8_WA     USBP_ARB_RW8_WA_PTR
#define USBP_ARB_RW8_WA_MSB USBP_ARB_RW8_WA_MSB_PTR

#define USBP_BUF_SIZE       USBP_BUF_SIZE_PTR
#define USBP_BUS_RST_CNT    USBP_BUS_RST_CNT_PTR
#define USBP_CR0            USBP_CR0_PTR
#define USBP_CR1            USBP_CR1_PTR
#define USBP_CWA            USBP_CWA_PTR
#define USBP_CWA_MSB        USBP_CWA_MSB_PTR

#define USBP_DMA_THRES      USBP_DMA_THRES_PTR
#define USBP_DMA_THRES_MSB  USBP_DMA_THRES_MSB_PTR

#define USBP_EP_ACTIVE      USBP_EP_ACTIVE_PTR
#define USBP_EP_TYPE        USBP_EP_TYPE_PTR

#define USBP_EP0_CNT        USBP_EP0_CNT_PTR
#define USBP_EP0_CR         USBP_EP0_CR_PTR
#define USBP_EP0_DR0        USBP_EP0_DR0_PTR
#define USBP_EP0_DR1        USBP_EP0_DR1_PTR
#define USBP_EP0_DR2        USBP_EP0_DR2_PTR
#define USBP_EP0_DR3        USBP_EP0_DR3_PTR
#define USBP_EP0_DR4        USBP_EP0_DR4_PTR
#define USBP_EP0_DR5        USBP_EP0_DR5_PTR
#define USBP_EP0_DR6        USBP_EP0_DR6_PTR
#define USBP_EP0_DR7        USBP_EP0_DR7_PTR

#define USBP_OSCLK_DR0      USBP_OSCLK_DR0_PTR
#define USBP_OSCLK_DR1      USBP_OSCLK_DR1_PTR

#define USBP_PM_ACT_CFG     USBP_PM_ACT_CFG_PTR
#define USBP_PM_STBY_CFG    USBP_PM_STBY_CFG_PTR

#define USBP_SIE_EP_INT_EN  USBP_SIE_EP_INT_EN_PTR
#define USBP_SIE_EP_INT_SR  USBP_SIE_EP_INT_SR_PTR

#define USBP_SIE_EP1_CNT0   USBP_SIE_EP1_CNT0_PTR
#define USBP_SIE_EP1_CNT1   USBP_SIE_EP1_CNT1_PTR
#define USBP_SIE_EP1_CR0    USBP_SIE_EP1_CR0_PTR

#define USBP_SIE_EP2_CNT0   USBP_SIE_EP2_CNT0_PTR
#define USBP_SIE_EP2_CNT1   USBP_SIE_EP2_CNT1_PTR
#define USBP_SIE_EP2_CR0    USBP_SIE_EP2_CR0_PTR

#define USBP_SIE_EP3_CNT0   USBP_SIE_EP3_CNT0_PTR
#define USBP_SIE_EP3_CNT1   USBP_SIE_EP3_CNT1_PTR
#define USBP_SIE_EP3_CR0    USBP_SIE_EP3_CR0_PTR

#define USBP_SIE_EP4_CNT0   USBP_SIE_EP4_CNT0_PTR
#define USBP_SIE_EP4_CNT1   USBP_SIE_EP4_CNT1_PTR
#define USBP_SIE_EP4_CR0    USBP_SIE_EP4_CR0_PTR

#define USBP_SIE_EP5_CNT0   USBP_SIE_EP5_CNT0_PTR
#define USBP_SIE_EP5_CNT1   USBP_SIE_EP5_CNT1_PTR
#define USBP_SIE_EP5_CR0    USBP_SIE_EP5_CR0_PTR

#define USBP_SIE_EP6_CNT0   USBP_SIE_EP6_CNT0_PTR
#define USBP_SIE_EP6_CNT1   USBP_SIE_EP6_CNT1_PTR
#define USBP_SIE_EP6_CR0    USBP_SIE_EP6_CR0_PTR

#define USBP_SIE_EP7_CNT0   USBP_SIE_EP7_CNT0_PTR
#define USBP_SIE_EP7_CNT1   USBP_SIE_EP7_CNT1_PTR
#define USBP_SIE_EP7_CR0    USBP_SIE_EP7_CR0_PTR

#define USBP_SIE_EP8_CNT0   USBP_SIE_EP8_CNT0_PTR
#define USBP_SIE_EP8_CNT1   USBP_SIE_EP8_CNT1_PTR
#define USBP_SIE_EP8_CR0    USBP_SIE_EP8_CR0_PTR

#define USBP_SOF0           USBP_SOF0_PTR
#define USBP_SOF1           USBP_SOF1_PTR

#define USBP_USB_CLK_EN     USBP_USB_CLK_EN_PTR

#define USBP_USBIO_CR0      USBP_USBIO_CR0_PTR
#define USBP_USBIO_CR1      USBP_USBIO_CR1_PTR
#define USBP_USBIO_CR2      USBP_USBIO_CR2_PTR

#define USBP_DM_INP_DIS_PTR     ( (reg8 *) USBP_Dm__INP_DIS)
#define USBP_DM_INP_DIS_REG     (*(reg8 *) USBP_Dm__INP_DIS)
#define USBP_DP_INP_DIS_PTR     ( (reg8 *) USBP_Dp__INP_DIS)
#define USBP_DP_INP_DIS_REG     (*(reg8 *) USBP_Dp__INP_DIS)
#define USBP_DP_INTSTAT_PTR     ( (reg8 *) USBP_Dp__INTSTAT)
#define USBP_DP_INTSTAT_REG     (*(reg8 *) USBP_Dp__INTSTAT)
#define USBP_DM_MASK            USBP_Dm__0__MASK
#define USBP_DP_MASK            USBP_Dp__0__MASK

#define USBFS_SIE_EP_INT_EP1_MASK        (0x01u)
#define USBFS_SIE_EP_INT_EP2_MASK        (0x02u)
#define USBFS_SIE_EP_INT_EP3_MASK        (0x04u)
#define USBFS_SIE_EP_INT_EP4_MASK        (0x08u)
#define USBFS_SIE_EP_INT_EP5_MASK        (0x10u)
#define USBFS_SIE_EP_INT_EP6_MASK        (0x20u)
#define USBFS_SIE_EP_INT_EP7_MASK        (0x40u)
#define USBFS_SIE_EP_INT_EP8_MASK        (0x80u)

#define USBP_ARB_EPX_SR_IN_BUF_FULL     (0x01u)
#define USBP_ARB_EPX_SR_DMA_GNT         (0x02u)
#define USBP_ARB_EPX_SR_BUF_OVER        (0x04u)
#define USBP_ARB_EPX_SR_BUF_UNDER       (0x08u)

#define USBP_ARB_EPX_INT_EN_ALL USBP_ARB_EPX_INT_ALL

#define USBP_CR1_BUS_ACTIVITY_SHIFT     (0x02u)

#define USBP_BUS_RST_COUNT      USBP_DEFUALT_BUS_RST_CNT

#define USBP_ARB_INT_MASK       USBP_DEFAULT_ARB_INT_EN

#if (CYDEV_CHIP_DIE_EXPECT == CYDEV_CHIP_DIE_LEOPARD)
    /* CY_PSOC3 interrupt registers */
    #define USBP_USB_ISR_PRIOR  ((reg8 *) CYDEV_INTC_PRIOR0)
    #define USBP_USB_ISR_SET_EN ((reg8 *) CYDEV_INTC_SET_EN0)
    #define USBP_USB_ISR_CLR_EN ((reg8 *) CYDEV_INTC_CLR_EN0)
    #define USBP_USB_ISR_VECT   ((cyisraddress *) CYDEV_INTC_VECT_MBASE)
#elif (CYDEV_CHIP_DIE_EXPECT == CYDEV_CHIP_DIE_PANTHER)
    /* CY_PSOC5LP interrupt registers */
    #define USBP_USB_ISR_PRIOR  ((reg8 *) CYDEV_NVIC_PRI_0)
    #define USBP_USB_ISR_SET_EN ((reg8 *) CYDEV_NVIC_SETENA0)
    #define USBP_USB_ISR_CLR_EN ((reg8 *) CYDEV_NVIC_CLRENA0)
    #define USBP_USB_ISR_VECT   ((cyisraddress *) CYDEV_NVIC_VECT_OFFSET)
#endif /*  CYDEV_CHIP_DIE_EXPECT */


#endif /* (CY_USBFS_USBP_H) */


/* [] END OF FILE */
