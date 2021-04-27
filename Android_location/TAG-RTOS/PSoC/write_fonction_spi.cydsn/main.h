/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
/* Includes ------------------------------------------------------------------*/

//static values
#define MAX_COM_AMOUNT                  (10u)
#define LIGHT_OFF                       (0u)
#define LIGHT_ON                        (1u)  
#define NUMBER_OF_ANCHOR            3

//DWM1000 status register masks
#define TX_OK_MASK			0x00000080U // TX OK
#define RX_FINISHED_MASK	0x00006400U // RX FINISHED
#define RX_ERROR_MASK		0x04279000U // RX ERROR

// Antenna calibration
#define ANTENNA_DELAY          32926u  //great average value, if the distance is really wrong, set this value on the other side too
    
// State Machine
enum STATE {
STATE_INIT,
STATE_WAIT_FIRST_SEND,
STATE_WAIT_RESPONSE,	
STATE_SECOND_SEND,
STATE_WAIT_SECOND_SEND,
STATE_GET_TIMES,
STATE_END,
};

#endif
/* [] END OF FILE */
