/***************************************************************************//**
* \file USBP_audio.h
* \version 3.20
*
* \brief
*  This file provides function prototypes and constants for the USBFS component 
*  Audio class.
*
* Related Document:
*  Universal Serial Bus Device Class Definition for Audio Devices Release 1.0
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_USBP_audio_H)
#define CY_USBFS_USBP_audio_H

#include "USBP.h"


/***************************************
* Custom Declarations
***************************************/

/* `#START CUSTOM_CONSTANTS` Place your declaration here */

/* `#END` */


/***************************************
*  Constants for USBP_audio API.
***************************************/

/* Audio Class-Specific Request Codes (AUDIO Table A-9) */
#define USBP_REQUEST_CODE_UNDEFINED     (0x00u)
#define USBP_SET_CUR                    (0x01u)
#define USBP_GET_CUR                    (0x81u)
#define USBP_SET_MIN                    (0x02u)
#define USBP_GET_MIN                    (0x82u)
#define USBP_SET_MAX                    (0x03u)
#define USBP_GET_MAX                    (0x83u)
#define USBP_SET_RES                    (0x04u)
#define USBP_GET_RES                    (0x84u)
#define USBP_SET_MEM                    (0x05u)
#define USBP_GET_MEM                    (0x85u)
#define USBP_GET_STAT                   (0xFFu)

/* point Control Selectors (AUDIO Table A-19) */
#define USBP_EP_CONTROL_UNDEFINED       (0x00u)
#define USBP_SAMPLING_FREQ_CONTROL      (0x01u)
#define USBP_PITCH_CONTROL              (0x02u)

/* Feature Unit Control Selectors (AUDIO Table A-11) */
#define USBP_FU_CONTROL_UNDEFINED       (0x00u)
#define USBP_MUTE_CONTROL               (0x01u)
#define USBP_VOLUME_CONTROL             (0x02u)
#define USBP_BASS_CONTROL               (0x03u)
#define USBP_MID_CONTROL                (0x04u)
#define USBP_TREBLE_CONTROL             (0x05u)
#define USBP_GRAPHIC_EQUALIZER_CONTROL  (0x06u)
#define USBP_AUTOMATIC_GAIN_CONTROL     (0x07u)
#define USBP_DELAY_CONTROL              (0x08u)
#define USBP_BASS_BOOST_CONTROL         (0x09u)
#define USBP_LOUDNESS_CONTROL           (0x0Au)

#define USBP_SAMPLE_FREQ_LEN            (3u)
#define USBP_VOLUME_LEN                 (2u)

#if !defined(USER_SUPPLIED_DEFAULT_VOLUME_VALUE)
    #define USBP_VOL_MIN_MSB            (0x80u)
    #define USBP_VOL_MIN_LSB            (0x01u)
    #define USBP_VOL_MAX_MSB            (0x7Fu)
    #define USBP_VOL_MAX_LSB            (0xFFu)
    #define USBP_VOL_RES_MSB            (0x00u)
    #define USBP_VOL_RES_LSB            (0x01u)
#endif /* USER_SUPPLIED_DEFAULT_VOLUME_VALUE */


/***************************************
* External data references
***************************************/
/**
* \addtogroup group_audio
* @{
*/
extern volatile uint8 USBP_currentSampleFrequency[USBP_MAX_EP][USBP_SAMPLE_FREQ_LEN];
extern volatile uint8 USBP_frequencyChanged;
extern volatile uint8 USBP_currentMute;
extern volatile uint8 USBP_currentVolume[USBP_VOLUME_LEN];
/** @} audio */

extern volatile uint8 USBP_minimumVolume[USBP_VOLUME_LEN];
extern volatile uint8 USBP_maximumVolume[USBP_VOLUME_LEN];
extern volatile uint8 USBP_resolutionVolume[USBP_VOLUME_LEN];

#endif /*  CY_USBFS_USBP_audio_H */


/* [] END OF FILE */
