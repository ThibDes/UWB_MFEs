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

#define USB_ANALYSE (0u)
#define ONLY_RECIEVER (0u)
#define LIGHT_OFF                       (0u)
#define LIGHT_ON                        (1u)
#define DEFAULT_PRIORITY                    (3u)

#define USBFS_DEVICE    (0u)

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
    STATE_PREPARE_WAIT_FOR_DISTANCE,
    STATE_WAIT_FOR_DISTANCE,
    USB_SEND,
    FIN,
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

//#define DATA_LEN (15u)
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

#define SAFE_DISTANCE_CM (100u)
#define SAFE_DISTANCE_STEP_CM (15u)

#define TICK2S ((1.0f / (128 * 499.2f * 1e6))) // Unknown factor 10 (transforme la fréquence en temps) (BECAUSE 10e6 = 1e7)
#define LIGHT_SPEED (299792458.0f) // m/s
#define FREQUENCY_CONSTANT ((pow(2.0,-17)/(2*(1024.0f)/(998.4f*1e6)))/(6.4896f*1e9));

#define SIGN_MASK 0b11111111111011111111111111111111
#define MASK21    0b11111111111000000000000000000000
                            

void interrupt_routine(void);

void socialDistancindLoop(void);
void setRandomSeed(void);

struct Message_s readRxData(void);
void sendTxData(struct Message_s, uint16_t);

CY_ISR( isr_timeout_Handler ){
    /* Clear the inteerrupt */
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
        LED4_Write(alertOn >= 4u);
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
    LCD_Start();    

    /* Sets up the GPIO interrupt and enables it */
    IRQ_StartEx(ISR);
    IRQ_SetPriority(DEFAULT_PRIORITY);
   
    LCD_ClearDisplay();
    LCD_Position(0u,0u);
    LCD_PrintString("Init...");
   
    /* initialisation of the DecaWave */
    CyDelay(10u); //time for the DW to go from Wakeup to init and then IDLE
    DWM_Init();
    LED4_Write(LIGHT_ON);
    CyDelay(1000u); 
    LED4_Write(LIGHT_OFF);
    LCD_ClearDisplay();
    LCD_Position(0u,0u);
    LCD_PrintString("Init..Ok");
 
    Timer_Start();
    isr_timeout_StartEx(isr_timeout_Handler);
   
    setRandomSeed();
    USBUART_Start(0, USBUART_5V_OPERATION);
    LED2_Write(LIGHT_ON); 
    if (ONLY_RECIEVER == 0 && USB_ANALYSE == 1){
    while(!USBUART_bGetConfiguration());
    }
    LED2_Write(LIGHT_OFF); ;
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
   
    /* Asymmetric Double-Sided Two-Way Ranging */
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
    //double TroundSlave;
    //double TreplyMaster;
    //double tof_num;
    //double tof_denum;
    
    
    state = STATE_INITIATE_CONTACT; 
    if (ONLY_RECIEVER == 1){
        state = STATE_PREPARE_TO_LISTEN;
    }
    deviceID = rand() & 0xFF;
    int t = 0; 
    int BR=500;
    int na = 0;
    int nb = 1; 
    int offset = 500; 
    uint8 Buffer1[BR/nb][3];
    uint8 Buffer2[BR/nb][3];
    for (int j=0; j<6; j++){
        for(int i=0; i<BR/nb; i++){
            Buffer1[i][j] = 0x00;
            Buffer2[i][j] = 0x00;
            }
    }
    
   
    uint8 BufferT1[BR/nb][5];
    uint8 BufferT2[BR/nb][5];
    uint8 BufferT3[BR/nb][5];
    uint8 BufferT4[BR/nb][5];
    for (int j=0; j<10; j++){
        for(int i=0; i<BR/nb; i++){
            BufferT1[i][j] = 0x00;
            BufferT2[i][j] = 0x00;
            BufferT3[i][j] = 0x00;
            BufferT4[i][j] = 0x00;
            }
    }
    uint8 responseTimeBUFFER[BR/nb][2];
    for (int j=0; j<4; j++){
        for(int i=0; i<BR/nb; i++){
            responseTimeBUFFER[i][j] = 0x00;           
            }
    }
    uint8_t responseTimeBuffer_8[2]; 
    uint16 responseTimeBuffer; 
    uint16 saveResponseTime = 0; 
    
    for (;;){
        switch(state){
            case STATE_INITIATE_CONTACT :
                idle();         
                
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
                    if (ONLY_RECIEVER == 1){
                    state = STATE_PREPARE_TO_LISTEN;
                    }
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
                    if (msg.id == INITIAL_MSG){                        
                        DWM_ReadSPI_ext(DRX_CONF,RF_CONF,CFO_Rx_S_8,3);
                        DWM_ReadSPI_ext(RX_TIME,NO_SUB, t2_8,5);
                        remoteDeviceID = msg.from;
                        responseTime = 0; 
                        //responseTime = saveResponseTime;
                        //responseTime = (rand() % (50u - 1u)) + 1u;/// @TODO

                        responseTimeBuffer_8[0]=(unsigned char) responseTime;
                        responseTimeBuffer_8[1]=(unsigned char) (responseTime>>8);
                        na = na+1; 
                         if ( na == nb){
                            na = 0;
                            saveResponseTime = saveResponseTime+1; 
                         }
                        
                         
                        //responseTime = 0u;
                        /*if (t >= BR/2){
                            responseTime = 0u; 
                        }*/
                        state = STATE_WAIT_RANDOM;
                    }
                    else if(msg.id == ANSWER_MSG && messageTimeout > 0u && msg.to == deviceID){
                        DWM_ReadSPI_ext(DRX_CONF,RF_CONF,CFO_Rx_M_8,3);
                        DWM_ReadSPI_ext(RX_TIME,NO_SUB, t4_8,5);
                        remoteDeviceID = msg.from;
                        //state = STATE_SEND_FINAL;
                        state = STATE_PREPARE_WAIT_FOR_RESULTS;
                        messageTimeout = MESSAGE_TIMEOUT; //added
                    }else{
                        state = STATE_LISTENING; // @TODO Verif
                    }
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
                    DWM_ReceiveData(RxData);
                    /* Results messages are identified with their length instead,
                       of their ID because there isn't enough space left is the
                       data field for an ID */
                    uint8_t len;
                DWM_ReadSPI_ext(RX_FINFO, NO_SUB, &len, 1);
                    if (len == DATA_LEN + 2){
                        // @TODO make a struct for that 1
                        for (int i=0;i<5;i++){
                            t2_8[i] = RxData[i];
                            t3_8[i] = RxData[i+5];
                           
                            //t6_8[i] = RxData[i+10];
                        }
                        for (int i= 0; i<3; i++){
                            CFO_Rx_S_8[i] = RxData[i+10];                        
                        }
                        for (int i= 0; i<2; i++){
                            responseTimeBuffer_8[i] = RxData[i+13];                        
                        }
                        //t1 = t2 = t3 = t4 = t5 = t6 = 0;
                        t1 = t2 = t3 = t4 = CFO_Rx_S = CFO_Rx_M = 0;
                        for (int i=0;i<5;i++){
                            t1 = (t1 << 8) | t1_8[4-i];
                            t2 = (t2 << 8) | t2_8[4-i];
                            t3 = (t3 << 8) | t3_8[4-i];
                            t4 = (t4 << 8) | t4_8[4-i];
                            //t5 = (t5 << 8) | t5_8[4-i];
                            //t6 = (t6 << 8) | t6_8[4-i];

                        }
                        
                        

                       
                       
                        if (t2 < t1 || t4 < t1){
                            state = STATE_PREPARE_TO_LISTEN;  
                        }
                        else{
                            state = STATE_PROCESS_RESULTS;
                            RxOk = 0; // @TODO should it be higher ?
                        }  
                    }else{
                        state = STATE_PREPARE_TO_LISTEN;
                    }
                }
                break;
            /*case STATE_SEND_FINAL:
                    sendTxData((struct Message_s) {
                        .id = FINAL_MSG,
                        .from = deviceID,
                        .to = remoteDeviceID,
                    }, MESSAGE_TIMEOUT);
                    state = STATE_SEND_FINAL_PENDING;
                break;*/
               
            /*case STATE_SEND_FINAL_PENDING:
                if (messageTimeout == 0u){ state = STATE_PREPARE_TO_LISTEN; }  
                if (interruptFlag){ interrupt_routine(); }
                if (TxOk){
                    TxOk = 0;
                    DWM_ReadSPI_ext(TX_TIME, NO_SUB, t5_8, 5);
                    idle();
                    DWM_Enable_Rx();
                    state = STATE_WAIT_FOR_RESULTS;
                    messageTimeout = MESSAGE_TIMEOUT;
                }
                break;*/

           

            case STATE_PROCESS_RESULTS:

                TroundMaster = (t4-t1);
                TreplySlave = (t3-t2);
                for (int i = 0; i<3; i++){
                    CFO_Rx_S = (CFO_Rx_S << 8) | CFO_Rx_S_8[2-i];
                    CFO_Rx_M = (CFO_Rx_M << 8) | CFO_Rx_M_8[2-i];
                    }  
                for (int i = 0; i<2; i++){
                    responseTimeBuffer = (responseTimeBuffer << 8) | responseTimeBuffer_8[1-i];                  
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
                               
                
                float  Epsil_CFO = sign_M*FREQUENCY_CONSTANT;
                Epsil_CFO = Epsil_CFO*(CFO_Rx_M+CFO_Rx_S)/2;
                float K = 1+Epsil_CFO; 
                float TOF = (TroundMaster-K*TreplySlave)/2;
                float distance = LIGHT_SPEED * TOF * TICK2S; 
                na = na +1; 
                if (USB_ANALYSE  == 1 && na == nb){
                
                //CFO_Rx_M = (CFO_Rx_M+CFO_Rx_S)/2;    
                if (offset == 0){   
                Buffer1[t][2]=(unsigned char) CFO_Rx_M;
                Buffer1[t][1]=(unsigned char) (CFO_Rx_M>>8);
                Buffer1[t][0]=(unsigned char) (CFO_Rx_M>>16);
                
                Buffer2[t][2]=(unsigned char) CFO_Rx_S;
                Buffer2[t][1]=(unsigned char) (CFO_Rx_S>>8);
                Buffer2[t][0]=(unsigned char) (CFO_Rx_S>>16);
                
                BufferT1[t][4]=(unsigned char) t1;
                BufferT1[t][3]=(unsigned char) (t1>>8);
                BufferT1[t][2]=(unsigned char) (t1>>16);
                BufferT1[t][1]=(unsigned char) (t1>>24);
                BufferT1[t][0]=(unsigned char) (t1>>32);
                
                BufferT2[t][4]=(unsigned char) t2;
                BufferT2[t][3]=(unsigned char) (t2>>8);
                BufferT2[t][2]=(unsigned char) (t2>>16);
                BufferT2[t][1]=(unsigned char) (t2>>24);
                BufferT2[t][0]=(unsigned char) (t2>>32);
                
                BufferT3[t][4]=(unsigned char) t3;
                BufferT3[t][3]=(unsigned char) (t3>>8);
                BufferT3[t][2]=(unsigned char) (t3>>16);
                BufferT3[t][1]=(unsigned char) (t3>>24);
                BufferT3[t][0]=(unsigned char) (t3>>32);
                
                BufferT4[t][4]=(unsigned char) t4;
                BufferT4[t][3]=(unsigned char) (t4>>8);
                BufferT4[t][2]=(unsigned char) (t4>>16);
                BufferT4[t][1]=(unsigned char) (t4>>24);
                BufferT4[t][0]=(unsigned char) (t4>>32);
                
                responseTimeBUFFER[t][1]=(unsigned char) responseTimeBuffer; 
                responseTimeBUFFER[t][0]=(unsigned char) (responseTimeBuffer>>8);
                t = t+1;
                
                }
                else{
                offset = offset-1;
                }
                na = 0;
                }
                

                if (distance > 0.0 && distance < 100.0){
                       
                        //if ((uint) (distance * 100.0 + OffsetPPM_Master+OffsetPPM_Slave+K+ OffsetPPM -tof_tick+m+TroundMaster-TreplySlave+n) < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM * 3){
                        if ((uint) (distance * 100.0 ) < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM * 24){
                            alertOn = 1;
                        }
                        if ((uint) (distance * 100.0) < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM * 2){
                            alertOn = 2;
                        }
                        if ((uint) (distance * 100.0) < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM){
                            alertOn = 3;
                        }
                        if ((uint) (distance * 100.0) < SAFE_DISTANCE_CM){
                            alertOn = 4;
                        }
                       
                        char str[8];
                        sprintf(str, "%d cm", (uint) (distance * 100.0));
                        LCD_ClearDisplay();
                        LCD_Position(0,0);
                        LCD_PrintString(str);
                    //}                    
                }
                
                state = STATE_SEND_DISTANCE;
                if (t>=BR/nb && USB_ANALYSE == 1){
                    state = USB_SEND;
                }
                break;
                
            case USB_SEND:
                LED1_Write(LIGHT_OFF);
                LED2_Write(LIGHT_OFF);
                LED3_Write(LIGHT_OFF);
                LED4_Write(LIGHT_OFF);
                CyDelay(400u);
                LED3_Write(LIGHT_OFF);
                CyDelay(400u);
                LED3_Write(LIGHT_ON);
                
                if (USBUART_CDCIsReady()){ 
                    LED3_Write(LIGHT_ON);
                    for (int i = 0; i<BR/nb; i++){
                    USBUART_PutData(BufferT1[i],5); 
                    CyDelay(400u); 
                    }
                    LED3_Write(LIGHT_ON);
                    for (int i = 0; i<BR/nb; i++){
                    USBUART_PutData(BufferT2[i],5); 
                    CyDelay(400u); 
                    }
                    LED3_Write(LIGHT_ON);
                    for (int i = 0; i<BR/nb; i++){
                    USBUART_PutData(BufferT3[i],5); 
                    CyDelay(400u); 
                    }
                    LED3_Write(LIGHT_ON);
                    for (int i = 0; i<BR/nb; i++){
                    USBUART_PutData(BufferT4[i],5); 
                    CyDelay(400u); 
                    }
                    LED3_Write(LIGHT_ON);
                    for (int i = 0; i<BR/nb; i++){
                    USBUART_PutData(Buffer1[i],3); 
                    CyDelay(400u); 
                    }
                    LED3_Write(LIGHT_ON);
                    for (int i = 0; i<BR/nb; i++){
                    USBUART_PutData(Buffer2[i],3); 
                    CyDelay(400u); 
                    }
                    LED3_Write(LIGHT_ON);
                    for (int i = 0; i<BR/nb; i++){
                    USBUART_PutData(responseTimeBUFFER[i],2); 
                    CyDelay(400u); 
                    }
                    
                    /*for (int i = 0; i<BR/nb; i++){
                    USBUART_PutData(Buffer2[i],3); 
                    CyDelay(400u); 
                    }*/
                    state = FIN; 
                }
                break; 
                
            case FIN:
                LED1_Write(LIGHT_OFF);
                LED2_Write(LIGHT_OFF);
                LED3_Write(LIGHT_OFF);
                LED4_Write(LIGHT_OFF);
                CyDelay(400u); 
                LED1_Write(LIGHT_ON);
                LED2_Write(LIGHT_ON);
                LED3_Write(LIGHT_ON);
                LED4_Write(LIGHT_ON);
                CyDelay(400u); 
                break; 
           
            case STATE_SEND_DISTANCE:
                sendTxData((struct Message_s) {
                    .id = DISTANCE_MSG,
                    .from = deviceID,
                    .to = remoteDeviceID,
                    .data = (int) (distance * 100),
                }, MESSAGE_TIMEOUT);
               
                state = STATE_SEND_DISTANCE_PENDING;
                break;
               
            case STATE_SEND_DISTANCE_PENDING:
                if (messageTimeout == 0u){ state = STATE_PREPARE_TO_LISTEN; }  
                if (interruptFlag){ interrupt_routine(); }
                if (TxOk){
                    TxOk = 0;
                    state = STATE_PREPARE_TO_LISTEN;
                }
                break;
           
            case STATE_WAIT_RANDOM:

                if (interruptFlag){ interrupt_routine(); }

                if (RxError){
                    RxError = 0;
                    DWM_Reset_Rx();
                    state = STATE_PREPARE_TO_LISTEN;
                }
                if (RxOk){
                    RxOk = 0;
                    struct Message_s msg = readRxData();
                    if(msg.id == ANSWER_MSG && msg.to == remoteDeviceID){
                        state = STATE_PREPARE_TO_LISTEN;
                    }
                        /*else if(msg.id == ANSWER_MSG && msg.to == deviceID){
                        DWM_ReadSPI_ext(RX_TIME,NO_SUB, t4_8,5);
                        remoteDeviceID = msg.from;
                        state = STATE_SEND_FINAL;
                    }*/
                }
                if (responseTime == 0u && remoteDeviceID != 0u){
                     state = STATE_ANSWER;
                }

                break;
               
            case STATE_ANSWER:
                idle();
                sendTxData((struct Message_s) {
                    .id = ANSWER_MSG,
                    .from = deviceID,
                    .to = remoteDeviceID,
                }, MESSAGE_TIMEOUT);
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
                    idle();
                    DWM_Enable_Rx();
                    //state = STATE_WAIT_FOR_FINAL;
                    state = STATE_SEND_RESULTS;
                    messageTimeout = MESSAGE_TIMEOUT;
                }
                break;

            /*case STATE_WAIT_FOR_FINAL:
                if (messageTimeout == 0u){
                    state = STATE_PREPARE_TO_LISTEN; }
                if (interruptFlag){
                    interrupt_routine(); }
                if (RxError){
                    RxError = 0;
                    DWM_Reset_Rx();
                    state = STATE_PREPARE_TO_LISTEN;
                }
                else if (RxOk){
                    RxOk = 0;
                    struct Message_s msg = readRxData();
                    if (msg.id == FINAL_MSG && msg.to == deviceID && msg.from == remoteDeviceID){
                        DWM_ReadSPI_ext(RX_TIME,NO_SUB, t6_8,5);
                        state = STATE_SEND_RESULTS;
                    }
                    else{
                        state = STATE_PREPARE_TO_LISTEN;
                    }
                }
                break;*/

                case STATE_SEND_RESULTS:
                    idle();
                    // @TODO make a struct for that 2
                    for (int i=0;i<5;i++){
                        TxData[i] = t2_8[i];
                        TxData[i+5] = t3_8[i];
                        //TxData[i+10] = t6_8[i];
                    }
                    for (int i = 0; i<3;i++){
                        TxData[i+10] = CFO_Rx_S_8[i];
                    }
                    for (int i = 0; i<2; i++){
                        TxData[i+13] = responseTimeBuffer_8[i];
                    }
                    DWM_SendData(TxData, DATA_LEN, 1);      //MODIFY DATA LEN !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    state = STATE_SEND_RESULTS_PENDING;
                    messageTimeout = MESSAGE_TIMEOUT;
                    break;
               
                case STATE_SEND_RESULTS_PENDING:
                    if (messageTimeout == 0u){ state = STATE_PREPARE_TO_LISTEN; }
                    if (interruptFlag){ interrupt_routine(); }
                    if (TxOk){
                        TxOk = 0;
                        state = STATE_WAIT_FOR_DISTANCE;
                        messageTimeout = MESSAGE_TIMEOUT * 3; // @TODO Fix me
                    }
                    break;
                   
                case STATE_PREPARE_WAIT_FOR_DISTANCE:
                    idle();
                    DWM_Enable_Rx();
                    state = STATE_WAIT_FOR_RESULTS;
                break;
               
                case STATE_WAIT_FOR_DISTANCE:
                    if (messageTimeout == 0u){
                        state = STATE_PREPARE_TO_LISTEN;
                    }
                    if (interruptFlag){
                        interrupt_routine(); }
                    if (RxError){
                        RxError = 0;
                        DWM_Reset_Rx();
                        state = STATE_PREPARE_TO_LISTEN;
                    }
                    else if (RxOk){
                        RxOk = 0;

                        struct Message_s msg = readRxData();
                        if (msg.id == DISTANCE_MSG && msg.to == deviceID && msg.from == remoteDeviceID){
                           
                            if ((uint) msg.data < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM * 3){
                                alertOn = 1;
                            }
                            if ((uint) msg.data < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM * 2){
                                alertOn = 2;
                            }
                            if ((uint) msg.data < SAFE_DISTANCE_CM + SAFE_DISTANCE_STEP_CM){
                                alertOn = 3;
                            }
                            if ((uint) msg.data < SAFE_DISTANCE_CM){
                                alertOn = 4;
                            }
                           
                            char str[8];
                            sprintf(str, "%d cm", msg.data);
                            LCD_ClearDisplay();
                            LCD_Position(0,0);
                            LCD_PrintString(str);
                           
                            state = STATE_PREPARE_TO_LISTEN;
                        }
                        else{
                            state = STATE_PREPARE_TO_LISTEN;
                        }

                    }
                    break;
               
            default:
                break;
        }
    }

}

/* [] END OF FILE */