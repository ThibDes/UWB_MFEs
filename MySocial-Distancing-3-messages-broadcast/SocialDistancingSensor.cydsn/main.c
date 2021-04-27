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
#include "project.h"
#include "dwm1000.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define LIGHT_OFF                       (0u)
#define LIGHT_ON                        (1u)
#define DEFAULT_PRIORITY                    (3u)

static volatile uint8_t interruptFlag = 0;
CY_ISR_PROTO(ISR);
CY_ISR(ISR){
    IRQ_ClearPending();    /* Clear pending Interrupt */
    pin_1_ClearInterrupt();    /* Clear pin Interrupt */
    interruptFlag = 1;
}

enum STATE_e  {
    STATE_INIT, 
    STATE_INITIATE_CONTACT,
    STATE_INITIATE_CONTACT_PENDING,
    STATE_PREPARE_TO_LISTEN, 
    STATE_LISTENING, 
    STATE_WAIT_RANDOM, 
    STATE_ANSWER, 
    STATE_ANSWER_PENDING, 
    STATE_PREPARE_WAIT_FOR_RESULTS, 
    STATE_WAIT_FOR_RESULTS, 
    STATE_PROCESS_RESULTS, 
    STATE_SEND_RESULTS, 
    STATE_SEND_RESULTS_PENDING, 
}; 
enum Message_ID_e {
    INITIAL_MSG = 0x10,
    ANSWER_MSG = 0x11,
}; 
struct Message_s{
    uint8_t id;
    uint8_t from;
    uint8_t to;
    int data;
};
struct Message_s msg2;

uint8_t deviceID = 0;
uint8_t remoteDeviceID;

enum STATE_e state = STATE_INIT;     //STATE_INIT is only used her to create the state parameter.

#define DATA_LEN (15u) 
static uint8_t RxData[DATA_LEN];
static uint8_t TxData[128];

uint8_t RxOk = 0u;
uint8_t TxOk = 0u;
uint8_t RxError = 0u;
uint8_t cycleTimeoutFlag = 0u;
uint16_t cycleTimeout = 0u;
uint16_t messageTimeout = 0u;
uint16_t responseTime = 0u;
uint8_t alertOn = 0u;
uint16 alertTimer = 0u;

#define ALERT_PERIOD (1000u)
#define CYCLE_PERIOD (200u)      
#define MESSAGE_TIMEOUT (5u)

void interrupt_routine(void);
void socialDistancindLoop(void);
struct Message_s readRxData(void);
void sendTxData(struct Message_s, uint16_t);

CY_ISR( isr_timeout_Handler ){           // éteint les luières après un certain temps (ALERT_PERIOD)
    /* Clear the interrupt */
    Timer_ReadStatusRegister();
   
    if (cycleTimeout == 0u){
        cycleTimeoutFlag = 1u;
        cycleTimeout = CYCLE_PERIOD;
    }else{
        cycleTimeout--;
    }
   
    if (alertOn){
        LED1_Write(alertOn >= 1u);
        LED2_Write(alertOn >= 2u);
        LED3_Write(alertOn >= 3u);
        LED4_Write(alertOn >= 4u);    //write 1 in led if alert on is bigger= than... 
        alertTimer = ALERT_PERIOD;
        alertOn = 0;
    }
   
    if (alertTimer == 1u){
        LED1_Write(LIGHT_OFF);
        LED2_Write(LIGHT_OFF);
        LED3_Write(LIGHT_OFF);
        LED4_Write(LIGHT_OFF);
    }
   
    messageTimeout = messageTimeout > 1u ? messageTimeout-1u : 0u;
    responseTime = responseTime > 1u ? responseTime-1u : 0u;
    alertTimer = alertTimer > 1u ? alertTimer-1u : 0u;
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    LED1_Write(LIGHT_OFF);
    LED2_Write(LIGHT_OFF);
    LED3_Write(LIGHT_OFF);
    LED4_Write(LIGHT_OFF);
   
    SPIM_Start();
    /* Sets up the GPIO interrupt and enables it */
    IRQ_StartEx(ISR);
    IRQ_SetPriority(DEFAULT_PRIORITY);
    
     /* initialisation of the DecaWave */
    CyDelay(10u); //time for the DW to go from Wakeup to init and then IDLE
    DWM_Init();
    LED4_Write(LIGHT_ON);
    CyDelay(1000u); 
    LED4_Write(LIGHT_OFF);
    
    Timer_Start();
    isr_timeout_StartEx(isr_timeout_Handler);
    
    socialDistancindLoop();
}

void setRandomSeed(){
    int16 temp;
    DieTemp_GetTemp(&temp);
    srand( temp );
}

void interrupt_routine(void){
    uint8_t RxBuffer[4];
    uint32_t StatusRegister;
    uint8_t ack[4] = {0};
    // Getting status Register
    DWM_ReadSPI_ext(SYS_STATUS, NO_SUB,  RxBuffer, 4);
    StatusRegister  = (RxBuffer[3] << 24) | (RxBuffer[2] << 16) | (RxBuffer[1] << 8) | RxBuffer[0];
    if (StatusRegister == 0xFFFFFFFF) {
        // Read again
        DWM_ReadSPI_ext(SYS_STATUS, NO_SUB,  RxBuffer, 4);
        StatusRegister  = (RxBuffer[3] << 24) | (RxBuffer[2] << 16) | (RxBuffer[1] << 8) | RxBuffer[0];
    }   
    if (StatusRegister & TX_OK_MASK){
        TxOk = 1;
        setBit(ack, 4, TX_OK_BIT, 1);
    }
    // check if RX finished
    if ((StatusRegister & RX_FINISHED_MASK) == RX_FINISHED_MASK) {
        ack[0] |= RX_FINISHED_MASK;
        ack[1] |= RX_FINISHED_MASK >> 8;
        ack[2] |= RX_FINISHED_MASK >> 16;
        ack[3] |= RX_FINISHED_MASK >> 24;
        RxOk = 1;
    }
    if (StatusRegister & RX_ERROR_MASK) {
        ack[0] |= RX_ERROR_MASK;
        ack[1] |= RX_ERROR_MASK >> 8;
        ack[2] |= RX_ERROR_MASK >> 16;
        ack[3] |= RX_ERROR_MASK >> 24;
        RxError = 1;
    }
    // clear IRQ flags on DW
    DWM_WriteSPI_ext(SYS_STATUS, NO_SUB, ack, 4);
    interruptFlag = 0;
}

struct Message_s readRxData(){
    struct Message_s msg;
   
    /* RxData is shifted by 1 byte and byte 0 is duplicated so message starts
       at byte 1. */
    DWM_ReceiveData(RxData);
    memcpy(&msg, &RxData[1], sizeof(msg2));
   
    return msg;
}

void sendTxData(struct Message_s msg, uint16_t timeout){
    memcpy(&TxData, &msg, sizeof(msg));
    DWM_SendData(TxData, sizeof(msg), 1);
    messageTimeout = timeout;
}


void socialDistancindLoop(){
        
    uint64_t t1 = 0u, t2 = 0u, t3 = 0u, t4 = 0u;
    uint8_t t1_8[5], t2_8[5], t3_8[5], t4_8[5];
    uint8_t CFO_Rx_S_8[3], CFO_Rx_M_8[3]; 
    uint8_t results[NUMBER_OF_ANCHOR - 1][9]; 
    uint deviceCounter = 0; 
    deviceID = rand() & 0xFF;
    
    state = STATE_INITIATE_CONTACT; 
    
    for(;;){
        switch(state){
            case STATE_INITIATE_CONTACT : 
                idle(); 
                //send the initial message
                sendTxData((struct Message_s){.id = INITIAL_MSG, .from = deviceID, .to = deviceID}, 105); 
                state = STATE_INITIATE_CONTACT_PENDING; 
                break; 
                
            case STATE_INITIATE_CONTACT_PENDING :
                if(messageTimeout == 0u){
                    state = STATE_PREPARE_TO_LISTEN; 
                }
                if (interruptFlag){
                    interrupt_routine();
                }
                if (TxOk){
                    //save t1
                    TxOk = 0;
                    DWM_ReadSPI_ext(TX_TIME, NO_SUB, t1_8, 5);
                    state = STATE_PREPARE_TO_LISTEN;
                }
                break; 
                
            case STATE_PREPARE_TO_LISTEN : 
                remoteDeviceID = 0;
                idle();
                DWM_Enable_Rx();  //enable the RX state of the decawave
                state = STATE_LISTENING;
                break;
                
            case STATE_LISTENING :
                if (cycleTimeoutFlag){
                    cycleTimeoutFlag = 0u;
                    state = STATE_INITIATE_CONTACT;
                   }
                if (interruptFlag){
                    interrupt_routine(); }

                if (RxError){
                    RxError = 0;
                    DWM_Reset_Rx();
                    state = STATE_PREPARE_TO_LISTEN;
                }
                if (RxOk){
                    RxOk = 0;
                    struct Message_s msg = readRxData();
                    
                    if (msg.id == ANSWER_MSG && messageTimeout > 0u && msg.to == deviceID && deviceCounter < NUMBER_OF_ANCHOR){
                        // save t4 and CFO_Rx_m
                        DWM_ReadSPI_ext(DRX_CONF,RF_CONF,CFO_Rx_M_8,3);
                        DWM_ReadSPI_ext(RX_TIME,NO_SUB, t4_8,5);
                        remoteDeviceID = msg.from;
                        for (int i = 0 ; i < 1; i++){
                            results[deviceCounter][i] = remoteDeviceID; 
                        }
                        for (int i = 1 ; i < 6; i++){
                            results[deviceCounter][i] = t4_8[i-1];
                        }
                        for (int i = 6 ; i < 9; i++){
                            results[deviceCounter][i] = CFO_Rx_M_8[i-6];
                        }
                        deviceCounter = deviceCounter + 1; 
                        state = STATE_PREPARE_TO_LISTEN;
                    }
                    else if (msg.id == INITIAL_MSG){   
                        // save t2 and CFO_Rx_s
                        DWM_ReadSPI_ext(DRX_CONF,RF_CONF,CFO_Rx_S_8,3);
                        DWM_ReadSPI_ext(RX_TIME,NO_SUB, t2_8,5);
                        remoteDeviceID = msg.from;
                        responseTime = 0; 
                        state = STATE_WAIT_RANDOM;
                    }
                    else if (messageTimeout == 0u){
                        state = STATE_SEND_RESULTS; 
                    }
                }
                break; 
                
            case STATE_WAIT_RANDOM : 
                if (responseTime == 0u && remoteDeviceID != 0u){
                     state = STATE_ANSWER;
                } 
                break; 
                
            case STATE_ANSWER : 
                // send the answer message 
                idle();
                sendTxData((struct Message_s) {.id = ANSWER_MSG,.from = deviceID,.to = remoteDeviceID,}, MESSAGE_TIMEOUT);
                state = STATE_ANSWER_PENDING;
                break; 
                
            case STATE_ANSWER_PENDING :
                if (messageTimeout == 0u){
                    state = STATE_PREPARE_TO_LISTEN; }
                if (interruptFlag){
                    interrupt_routine(); }
                if (TxOk){
                    TxOk = 0;
                    DWM_ReadSPI_ext(TX_TIME, NO_SUB, t3_8, 5);
                    state = STATE_PREPARE_WAIT_FOR_RESULTS;
                    
                }
                break;
                
            case STATE_PREPARE_WAIT_FOR_RESULTS : 
                idle();
                DWM_Enable_Rx();                    
                messageTimeout = MESSAGE_TIMEOUT;
                state = STATE_WAIT_FOR_RESULTS;
                break; 
                
            case STATE_WAIT_FOR_RESULTS : 
                if (interruptFlag){ 
                    interrupt_routine();
                }
                if (messageTimeout == 0u){
                    state = STATE_PREPARE_TO_LISTEN;
                    }  
                if (RxError){
                    RxError = 0;
                    state = STATE_PREPARE_TO_LISTEN;  
                    DWM_Reset_Rx();
                    }
                if (RxOk){
                    DWM_ReceiveData(RxData);
                    uint8_t len;
                    DWM_ReadSPI_ext(RX_FINFO, NO_SUB, &len, 1);
                     if (len == DATA_LEN + 2){
                    state = STATE_PROCESS_RESULTS;}
                    }
                else{
                        state = STATE_PREPARE_TO_LISTEN;
                    }
                break;
                
            case STATE_PROCESS_RESULTS :
                alertOn = 4; 
                state = STATE_INITIATE_CONTACT; 
                break; 
                
            case STATE_SEND_RESULTS : 
                idle();
                //REPLACE BY RESULTS 
                for (int i=0;i<5;i++){
                        TxData[i] = t1_8[i];
                        TxData[i+5] = t4_8[i];
                        
                    }
                for (int i = 0; i<3;i++){
                    TxData[i+10] = CFO_Rx_M_8[i];
                }
                DWM_SendData(TxData, DATA_LEN, 1);      //MODIFY DATA LEN !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                state = STATE_SEND_RESULTS_PENDING;
                messageTimeout = MESSAGE_TIMEOUT;
                break;
                
            case STATE_SEND_RESULTS_PENDING : 
                if (messageTimeout == 0u){ state = STATE_PREPARE_TO_LISTEN; }
                if (interruptFlag){ interrupt_routine(); }
                if (TxOk){
                    TxOk = 0;
                    state = STATE_INITIATE_CONTACT;
                    messageTimeout = MESSAGE_TIMEOUT * 3; // @TODO Fix me
                    }
                break; 
                
            default: 
            break;      
        }
    }
}

/* [] END OF FILE */