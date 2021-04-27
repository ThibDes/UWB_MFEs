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


#define SYS_CFG_PHR_MODE_00		0x00000000UL   	/* Standard Frame mode */
#define SYS_CFG_PHR_MODE_11		0x00030000UL   	/* Long Frames mode */

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

enum STATE_e {
    STATE_INIT,
   
    /* Social */
    STATE_INITIATE_CONTACT,
    STATE_INITIATE_CONTACT_PENDING,
    STATE_PREPARE_TO_LISTEN,
    STATE_LISTENING,
    STATE_SEND_FINAL,
    STATE_SEND_FINAL_PENDING,
    STATE_PREPARE_WAIT_FOR_RESULTS,
    STATE_WAIT_FOR_RESULTS, // DEL
    STATE_PROCESS_RESULTS,
    STATE_SEND_DISTANCE,
    STATE_SEND_DISTANCE_PENDING,

    STATE_WAIT_RANDOM,
    STATE_ANSWER,
    STATE_ANSWER_PENDING,
    STATE_WAIT_FOR_FINAL,
    STATE_SEND_RESULTS,
    STATE_SEND_RESULTS_PENDING,
};

enum Message_ID_e {
    INITIAL_MSG = 0x10,
    ANSWER_MSG = 0x11,
    FINAL_MSG = 0x12,
    RESULTS_MSG = 0x13,
    DISTANCE_MSG = 0x14,
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


enum STATE_e state = STATE_INIT;


static uint8_t RxData[(NUMBER_OF_ANCHOR - 1) * DATA_LEN];
static uint8_t TxData[128];

uint8_t RxOk = 0u;
uint8_t TxOk = 0u;
uint8_t RxError = 0u;
uint8_t cycleTimeoutFlag = 0u;
uint16_t cycleTimeout = 0u;
//uint16_t cyclePeriode  = CYCLE_PERIOD * NUMBER_OF_ANCHOR;
uint16_t messageTimeout = 0u;
uint16_t responseTime = 0u;

uint8_t alertOn = 0u;
uint16 alertTimer = 0u;
#define ALERT_PERIOD (3000u)


#define MESSAGE_TIMEOUT (5u)

#define SAFE_DISTANCE_CM (100u)
#define SAFE_DISTANCE_STEP_CM (15u)

#define TICK2S ((1.0f / (128 * 499.2f * 1e6))) 
#define LIGHT_SPEED (299792458.0f) // m/s
#define FREQUENCY_CONSTANT ((pow(2.0,-17)/(2*(1024.0f)/(998.4f*1e6)))/(6.4896f*1e9));

#define SIGN_MASK 0b11111111111011111111111111111111
#define MASK21    0b11111111111000000000000000000000
                           
void interrupt_routine(void);

void socialDistancindLoop(void);
void setRandomSeed(void);

struct Message_s readRxData(void);
void sendTxData(struct Message_s, uint16_t);

CY_ISR( isr_timeout_Handler ){           // éteint les lumières après un certain temps (ALERT_PERIOD)
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
   
    setRandomSeed();
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
    /* twr */
    uint64_t t1 = 0u, t2 = 0u, t3 = 0u, t4 = 0u;
    uint8_t t1_8[5], t2_8[5], t3_8[5], t4_8[5];
    uint8_t CFO_Rx_S_8[3], CFO_Rx_M_8[3];
    uint32_t CFO_Rx_S = 0;
    uint32_t CFO_Rx_M = 0;
    
    double TroundMaster;
    double TreplySlave;
    double Foffset_Master;
    double OffsetPPM_Master;
    double Foffset_Slave;
    double OffsetPPM_Slave;
            
    state = STATE_INITIATE_CONTACT; 
    deviceID = rand() & 0xFF;
    uint distancesList[NUMBER_OF_ANCHOR - 1][2]; 
    uint dist = 0; 
    for (int i = 0;i < NUMBER_OF_ANCHOR-1; i++){
        for (int j = 0; j < 2; j++){
            distancesList[i][j] = 0; 
        }        
    }
//////////////////////////////////////////   
    
    for (;;){
        switch(state){
            case STATE_INITIATE_CONTACT :
                idle();         
                //send the 1st message
                sendTxData((struct Message_s) {
                    .id = INITIAL_MSG,
                    .from = deviceID,
                    .to = deviceID,
                }, 105); // @TODO timeout = 105
               
                state = STATE_INITIATE_CONTACT_PENDING;
                break;
           
            case STATE_INITIATE_CONTACT_PENDING:
                if (messageTimeout == 0u){
                    state = STATE_PREPARE_TO_LISTEN; }  
                if (interruptFlag){
                    interrupt_routine(); }
                if (TxOk){
                    //save t1
                    TxOk = 0;
                    DWM_ReadSPI_ext(TX_TIME, NO_SUB, t1_8, 5);
                    state = STATE_PREPARE_TO_LISTEN;
                }
                break;
               
            case STATE_PREPARE_TO_LISTEN:
                remoteDeviceID = 0;
                idle();
                DWM_Enable_Rx();
                state = STATE_LISTENING;
                break;

            case STATE_LISTENING:    

                if (cycleTimeoutFlag){
                    cycleTimeoutFlag = 0u;
                    state = STATE_INITIATE_CONTACT;
                }
               
                if (interruptFlag){
                    interrupt_routine();
                    }

                if (RxError){
                    RxError = 0;
                    DWM_Reset_Rx();
                    state = STATE_PREPARE_TO_LISTEN;
                }
                if (RxOk){
                    RxOk = 0;
                    struct Message_s msg = readRxData();
                    if (msg.id == INITIAL_MSG){   
                        // save t2 and CFO_Rx_s
                        DWM_ReadSPI_ext(DRX_CONF,RF_CONF,CFO_Rx_S_8,3);
                        DWM_ReadSPI_ext(RX_TIME,NO_SUB, t2_8,5);
                        /*for (int i = 0; i<3; i++){
                            CFO_Rx_S = (CFO_Rx_S << 8) | CFO_Rx_S_8[2-i];}
                        if (CFO_Rx_S==0){
                            alertOn = 1;
                        }*/
                        remoteDeviceID = msg.from;
                        //responseTime = 0; 
                        responseTime = (rand() % (8u - 1u));
                        state = STATE_WAIT_RANDOM;
                    }
                    else if(msg.id == ANSWER_MSG && messageTimeout > 0u && msg.to == deviceID ){
                        // save t4 and CFO_Rx_m
                        DWM_ReadSPI_ext(DRX_CONF,RF_CONF,CFO_Rx_M_8,3);
                        DWM_ReadSPI_ext(RX_TIME,NO_SUB, t4_8,5);
                        remoteDeviceID = msg.from;
                        uint flag = 1; // used to check if the remote device ID already exists in the distancesList
                        if (msg.data != 0 && msg.data < 10000){ //as dist has been casted in uint, if it is equal to 0 we had a computation error 
                            for (int i = 0 ; i<NUMBER_OF_ANCHOR-1; i++){
                                if (distancesList[i][0] == (uint) remoteDeviceID){
                                    distancesList[i][1] = (uint) msg.data; 
                                    flag = 0; 
                                    }
                                }
                                if (flag){
                                    uint index = 0; 
                                    for (int i = NUMBER_OF_ANCHOR-2 ; i> -1; i--){
                                        if (distancesList[i][0] == 0){
                                            index = i;
                                        }
                                    }
                                    distancesList[index][0] = (uint) remoteDeviceID; 
                                    distancesList[index][1] = (uint) msg.data; 
                                }
                            }
                        state = STATE_SEND_RESULTS;
                        messageTimeout = MESSAGE_TIMEOUT; //added
                        uint distMin = 10000; 
                        for (int i = 0 ; i<NUMBER_OF_ANCHOR-2 ; i++){
                            if (distMin > distancesList[i][1] && distancesList[i][1]>0){
                            distMin = distancesList[i][1]; 
                            }
                        }
                        if (distMin > 0 && distMin < 10000){
                                if (distMin < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM * 3){ //3
                                    alertOn = 1;
                                }
                                if (distMin < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM * 2){
                                    alertOn = 2;
                                }
                                if (distMin < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM){
                                    alertOn = 3;
                                }
                                if (distMin < SAFE_DISTANCE_CM){
                                    alertOn = 4;
                                }
                            }                              
                        }                                      
                    else{
                        state = STATE_PREPARE_TO_LISTEN; // @TODO Verif
                        break; 
                    }
                }             
                break;
                
            case STATE_ANSWER:
                idle();
                sendTxData((struct Message_s) {
                    .id = ANSWER_MSG,
                    .from = deviceID,
                    .to = remoteDeviceID,
                    .data = (int) dist, 
                }, MESSAGE_TIMEOUT);  // MESSAGE_TIMEOUT*NUMBER_OF_ANCHOR*50; //Wait enough time to recieve all the results 
                state = STATE_ANSWER_PENDING;
                break;
           
            case STATE_ANSWER_PENDING:
                if (messageTimeout == 0u){
                    state = STATE_PREPARE_TO_LISTEN; }
                if (interruptFlag){
                    interrupt_routine(); }
                if (TxOk){
                    TxOk = 0;
                    DWM_ReadSPI_ext(TX_TIME, NO_SUB, t3_8, 5);                    
                    state = STATE_PREPARE_WAIT_FOR_RESULTS;
                    messageTimeout = MESSAGE_TIMEOUT;  
                }
                break;
           
            case STATE_PREPARE_WAIT_FOR_RESULTS:
                idle();
                DWM_Enable_Rx();
                state = STATE_WAIT_FOR_RESULTS;
                break;
               
            case STATE_WAIT_FOR_RESULTS:                
                if (interruptFlag){ interrupt_routine(); }
                if (messageTimeout == 0u){
                    state = STATE_PREPARE_TO_LISTEN; }  
                if (RxError){
                    RxError = 0;
                    state = STATE_PREPARE_TO_LISTEN;  
                    DWM_Reset_Rx();
                }
                if (RxOk){
                    DWM_ReceiveData2(RxData);
                    //struct Message_s msg = readRxData();
                    /* Results messages are identified with their length instead,
                       of their ID because there isn't enough space left is the
                       data field for an ID */
                    uint8_t len;
                    uint flag = 0; 
                    DWM_ReadSPI_ext(RX_FINFO, NO_SUB, &len, 1);
                    if (len == DATA_LEN + 2){
                        if (RxData[0] == deviceID){
                            flag = 1; 
                            for (int j=0;j<5;j++){
                                t1_8[j] = RxData[j+1]; // the +1 is here to do not take the deviceID
                                t4_8[j] = RxData[j+6];
                            }
                            for (int j= 0; j<3; j++){
                                CFO_Rx_M_8[j] = RxData[j+11];                        
                            }
                        }
                        if (flag){
                            state = STATE_PROCESS_RESULTS;
                            RxOk = 0;
                        }
                    }
                    else{
                        state = STATE_PREPARE_TO_LISTEN;
                    }
                }
                break;

            case STATE_PROCESS_RESULTS:
                t1 = t2 = t3 = t4 = CFO_Rx_S = CFO_Rx_M = 0;
                for (int i=0;i<5;i++){
                    t1 = (t1 << 8) | t1_8[4-i];
                    t2 = (t2 << 8) | t2_8[4-i];
                    t3 = (t3 << 8) | t3_8[4-i];
                    t4 = (t4 << 8) | t4_8[4-i];
                    }
                for (int i = 0; i<3; i++){
                    CFO_Rx_S = (CFO_Rx_S << 8) | CFO_Rx_S_8[2-i];
                    CFO_Rx_M = (CFO_Rx_M << 8) | CFO_Rx_M_8[2-i];
                    }  
                int sign_M = 1; 
                int sign_S = 1;     
                if (~(CFO_Rx_M | SIGN_MASK) == 0){ //if yes, the number is negative 
                    CFO_Rx_M = ~(CFO_Rx_M-1);
                    CFO_Rx_M = CFO_Rx_M-MASK21;
                    sign_M = -1; 
                }
                if (~(CFO_Rx_S | SIGN_MASK) == 0){ //if yes, the number is negative 
                    CFO_Rx_S = ~(CFO_Rx_S-1);
                    CFO_Rx_S = CFO_Rx_S-MASK21;
                    sign_S = -1; 
                }
                TroundMaster = (t4-t1);
                TreplySlave = (t3-t2);
                float  Epsil_CFO = sign_M*FREQUENCY_CONSTANT;
                Epsil_CFO = Epsil_CFO*(CFO_Rx_M+CFO_Rx_S)/2;
                
                if (CFO_Rx_S==0){
                     Epsil_CFO = Epsil_CFO*(CFO_Rx_M);
                }
                float K = 1+Epsil_CFO; 
                float TOF = (TroundMaster-K*TreplySlave)/2;
                float distance = LIGHT_SPEED * TOF * TICK2S; 
                if (CFO_Rx_S==0){
                    distance = -1;
                    }
                dist = (uint) (distance * 100.0 ) ;
                uint flag = 1;  
                if (dist != 0 && dist < 1000){ //as dist has been casted in uint, if it is equal to 0 we had a computation error 
                    for (int i = 0 ; i<NUMBER_OF_ANCHOR-1; i++){
                        if (distancesList[i][0] == (uint) remoteDeviceID){
                            distancesList[i][1] = dist; 
                            flag = 0; 
                        }
                    }
                    if (flag){
                        uint index = 0; 
                        for (int i = NUMBER_OF_ANCHOR-2 ; i> -1; i--){
                            if (distancesList[i][0] == 0){
                                index = i;
                            }
                        }
                        distancesList[index][0] = (uint) remoteDeviceID; 
                        distancesList[index][1] = dist; 
                    }
                }
                uint distMin = dist; 
                for (int i = 0 ; i<NUMBER_OF_ANCHOR-2 ; i++){
                    if (dist > distancesList[i][1] && distancesList[i][1]>0){
                    distMin = distancesList[i][1]; 
                    }
                }
                
                if (distMin > 0 && distMin < 10000){
                        if (distMin < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM * 24){ //3
                            alertOn = 1;
                        }
                        if (distMin < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM * 2){
                            alertOn = 2;
                        }
                        if (distMin < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM){
                            alertOn = 3;
                        }
                        if (distMin < SAFE_DISTANCE_CM){
                            alertOn = 4;
                        }
                }
                state = STATE_INITIATE_CONTACT;
                break;
                
            case STATE_WAIT_RANDOM:
                if (responseTime == 0u && remoteDeviceID != 0u){
                     state = STATE_ANSWER;
                }
                if (remoteDeviceID == 0u){
                    state = STATE_INITIATE_CONTACT; 
                }
                break;
               
            case STATE_SEND_RESULTS:
                idle();
                TxData[0] = remoteDeviceID; 
                for (int i = 1 ; i < 6; i++){
                    TxData[i] = t1_8[i-1];
                    }
                for (int i = 6 ; i < 11; i++){
                    TxData[i] = t4_8[i-6];
                    }
                for (int i = 11 ; i < 14; i++){
                    TxData[i] = CFO_Rx_M_8[i-11];
                    }                  
                
                DWM_SendData(TxData,DATA_LEN, 1);      //MODIFY DATA LEN !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
                
                //erases the id to avoid sending of old info
                state = STATE_SEND_RESULTS_PENDING;
                messageTimeout = MESSAGE_TIMEOUT;
                break;
               
            case STATE_SEND_RESULTS_PENDING:
                if (messageTimeout == 0u){ state = STATE_PREPARE_TO_LISTEN; }
                if (interruptFlag){ interrupt_routine(); }
                if (TxOk){
                    TxOk = 0;
                    state = STATE_PREPARE_TO_LISTEN;  
                    messageTimeout = MESSAGE_TIMEOUT; // @TODO Fix me
                }
                break;

            default:
                break;
        }
    }

}

/* [] END OF FILE */
