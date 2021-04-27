/* Includes ------------------------------------------------------------------*/

#include <project.h>

/* RTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "main.h"
#include "stdio.h"
#include "dwm1000.h"
#include <string.h>
#include <inttypes.h>
#include <math.h>

uint8_t prio_matrice[NUMBER_OF_ANCHOR+1][NUMBER_OF_ANCHOR+1] = {{0, 0, 0, 0},
                                                                {0, 0, 0, 0},
                                                                {0, 0, 0, 0},
                                                                {0, 0, 0, 0},
                                                                }; //each anchor + usb_out + usb_in
#define ISR_PRIORITY                    (21u)
#define INTERRUPT_PRIORITY              (22u)
uint8_t prio1 = 20;
uint8_t prio2 = 20;
uint8_t prio3 = 20;
uint8_t prio4 = 20;
uint8_t usb_out_prio = 29;
uint8_t usb_in_prio = 30;

uint16_t number_bytes_receive=0;
uint8_t RxPhoneData[8];
uint8_t TxPhoneData[63];
uint64_t all_clock_time[12]={0};
uint8_t found = 0;

// Boolean variables
uint8_t TxOk = 0;
static volatile uint8_t RxState = 0;

enum STATE state;

/* 		Tasks 		 */
static void freeRTOSInit(void);
static void state_machine_task(void *arg);
static void interrupt_routine(void *arg);
static void usb_out(void *arg);
static void usb_in(void *arg);

/* 		Queues 		 */
static xQueueHandle irq_queue = NULL;
static xQueueHandle Rx_q = NULL;
static xQueueHandle Tx_q = NULL;

/*      Tasks handle    */
TaskHandle_t taskHandler1; 
TaskHandle_t taskHandler2; 
TaskHandle_t taskHandler3;
TaskHandle_t taskHandler4;
TaskHandle_t usbOutHandler;
TaskHandle_t usbInHandler;

SemaphoreHandle_t semaphoreBuf1;

//fonction
static void change_priorities(int slave);

/* Interrupt prototypes */
CY_ISR_PROTO(ISR);
CY_ISR(ISR){
    IRQ_ClearPending();    /* Clear pending Interrupt */
    pin_1_ClearInterrupt();    /* Clear pin Interrupt */
	uint32_t gpio_num = (uint32_t) ISR;
    xQueueSendFromISR(irq_queue, (void *) &gpio_num, NULL);
}
    
static void freeRTOSInit( void ){
/* Port layer functions that need to be copied into the vector table. */
extern void xPortPendSVHandler( void );
extern void xPortSysTickHandler( void );
extern void vPortSVCHandler( void );
extern cyisraddress CyRamVectors[];

	/* Install the OS Interrupt Handlers. */
	CyRamVectors[ 11 ] = ( cyisraddress ) vPortSVCHandler;
	CyRamVectors[ 14 ] = ( cyisraddress ) xPortPendSVHandler;
	CyRamVectors[ 15 ] = ( cyisraddress ) xPortSysTickHandler;
}

static void interrupt_routine(void *arg){	

    (void) arg;
    uint32_t io_num;

    for (;;) 
    {	
        if (xQueueReceive(irq_queue, &io_num, 30)) {
            uint8_t RxBuffer[4];
			uint32_t StatusRegister;
			uint8_t ack[4] = {0};
			DWM_ReadSPI_ext(SYS_STATUS, NO_SUB,  RxBuffer, 4);
			StatusRegister  = (RxBuffer[3] << 24) | (RxBuffer[2] << 16) | (RxBuffer[1] << 8) | RxBuffer[0];
			if (StatusRegister == 0xFFFFFFFF) {
				DWM_ReadSPI_ext(SYS_STATUS, NO_SUB,  RxBuffer, 4);
				StatusRegister  = (RxBuffer[3] << 24) | (RxBuffer[2] << 16) | (RxBuffer[1] << 8) | RxBuffer[0];
			}
			if (StatusRegister & TX_OK_MASK){
				TxOk = 1;
				xQueueSend(Tx_q,(void*) &TxOk,portMAX_DELAY);
				setBit(ack, 4, TX_OK_BIT, 1);
			}
			// check if RX finished

			if ((StatusRegister & RX_FINISHED_MASK) == RX_FINISHED_MASK) {
				ack[0] |= RX_FINISHED_MASK;
				ack[1] |= RX_FINISHED_MASK >> 8;
				ack[2] |= RX_FINISHED_MASK >> 16;
				ack[3] |= RX_FINISHED_MASK >> 24;
				RxState = 1;
			}
			if (StatusRegister & RX_ERROR_MASK) {
				ack[0] |= RX_ERROR_MASK;
				ack[1] |= RX_ERROR_MASK >> 8;
				ack[2] |= RX_ERROR_MASK >> 16;
				ack[3] |= RX_ERROR_MASK >> 24;
				RxState = 2;
			}
			if(RxState == 1 || RxState == 2){
                xQueueSend(Rx_q,(void*) &RxState,portMAX_DELAY);
            }
			DWM_WriteSPI_ext(SYS_STATUS ,NO_SUB, ack, 4);
         }
    }
     vTaskDelete(NULL);
}

static void usb_out(void *arg){

    (void) *arg;
    for(;;){
        for(int i=0;i<12;i++){                  // Preparation of data
            TxPhoneData[5*i]=all_clock_time[i];       
            TxPhoneData[5*i+1]=all_clock_time[i]>>8;       
            TxPhoneData[5*i+2]=all_clock_time[i]>>16;       
            TxPhoneData[5*i+3]=all_clock_time[i]>>24;       
            TxPhoneData[5*i+4]=all_clock_time[i]>>32;
            //all_clock_time[i]=0;
        }
        USBP_LoadInEP(1,TxPhoneData,63);
        change_priorities(-2);
    }
}

static void usb_in(void *arg){
    (void) *arg;
    for(;;){
        USBP_EnableOutEP(2);
        while(USBP_bGetEPAckState(2)==USBP_OUT_BUFFER_EMPTY)
        number_bytes_receive=USBP_GetEPCount(2);
        USBP_ReadOutEP(2,RxPhoneData,number_bytes_receive);
        USBP_DisableOutEP(2);
        if(RxPhoneData[0]==0x04 && RxPhoneData[1]==0x06){   //test connection
            TxPhoneData[0]=0x02;
            TxPhoneData[1]=0x03;
            TxPhoneData[2]=0x46;
            TxPhoneData[3]=0x37;
            USBP_LoadInEP(1,TxPhoneData,4);
        }else{
            change_priorities(-1);
        }
    }
}

static void change_priorities(int num){
    taskENTER_CRITICAL();
    if(num == -1){
        vTaskPrioritySet(taskHandler1, prio1);
        vTaskPrioritySet(taskHandler2, prio2);
        vTaskPrioritySet(taskHandler3, prio3);
        if(NUMBER_OF_ANCHOR > 3){
            vTaskPrioritySet(taskHandler4, prio4);
            vTaskResume(taskHandler4);
        }
        vTaskPrioritySet(usbOutHandler, usb_out_prio);
        vTaskPrioritySet(usbInHandler, usb_out_prio - 1);
        vTaskSuspend(usbOutHandler);
        vTaskSuspend(usbInHandler);
    }else if(num == -2){
        vTaskPrioritySet(usbInHandler, usb_out_prio + 3);
        vTaskResume(usbInHandler);
    }else{
        vTaskPrioritySet(taskHandler1, uxTaskPriorityGet(taskHandler1)-prio_matrice[num][0]);
        vTaskPrioritySet(taskHandler2, uxTaskPriorityGet(taskHandler2)-prio_matrice[num][1]);
        vTaskPrioritySet(taskHandler3, uxTaskPriorityGet(taskHandler3)-prio_matrice[num][2]);
        if(NUMBER_OF_ANCHOR > 3){
            vTaskPrioritySet(taskHandler4, uxTaskPriorityGet(taskHandler4)-prio_matrice[num][3]);
        }
    }
    taskEXIT_CRITICAL();
}

int main(void){
	LED_1_Write(LIGHT_OFF);
    LED_2_Write(LIGHT_OFF);
    LED_3_Write(LIGHT_OFF);
    LED_4_Write(LIGHT_OFF);
    
    CyGlobalIntEnable;
    
    USBP_Start(0, USBP_DWR_VDDD_OPERATION);
    
    freeRTOSInit();
    Timer_1_Start();
    
    /* initialisation of the DecaWave */
    SPIM_Start();
	CyDelay(10u); //time for the DW to go from Wakeup to init and then IDLE
	DWM_Init();
    
    /* Sets up the GPIO interrupt and enables it */
    IRQ_StartEx(ISR);
    IRQ_SetPriority(ISR_PRIORITY);
    while(!USBP_bGetConfiguration());
    //initialize tasks (one for each slave + usb_out, usb_in)
    irq_queue = xQueueCreate(10, sizeof(uint32_t));
    Rx_q = xQueueCreate(1, sizeof(uint8_t));
    Tx_q = xQueueCreate(1, sizeof(uint8_t));
    semaphoreBuf1 = xSemaphoreCreateMutex();
    xTaskCreate(interrupt_routine, "interrupt_routine",1024, NULL, INTERRUPT_PRIORITY, NULL);
    int slaveNummer = 1;
    xTaskCreate(state_machine_task,"slave_1", 1024, (void*) slaveNummer, 1, &taskHandler1);
    slaveNummer = 2;
    xTaskCreate(state_machine_task,"slave_2", 1024, (void*) slaveNummer, 1, &taskHandler2);
    slaveNummer = 3;
    xTaskCreate(state_machine_task,"slave_3", 1024, (void*) slaveNummer, 1, &taskHandler3);
    xTaskCreate(usb_out, "usb_out", 1024, NULL, usb_out_prio, &usbOutHandler);
    xTaskCreate(usb_in, "usb_in", 1024, NULL, usb_in_prio, &usbInHandler);
    vTaskStartScheduler();
    for(;;){
    }
}
    
static void state_machine_task(void *arg){	
	uint8_t slaveNummer = (uint8_t) arg;
    uint8_t master_first_message = 0x10 | slaveNummer;
    uint8_t master_second_message	= 0x20 | slaveNummer;
    uint8_t slave_standard_message	= 0x0A | slaveNummer<<4;
    uint32_t RxFlag,TxFlag;
    uint8_t RxData[15], TxData[128];
    uint64_t t1, t2, t3, t4, t5, t6;
    uint8_t t1_8[5], t2_8[5], t3_8[5], t4_8[5], t5_8[5], t6_8[5];
    uint64_t TroundMaster, TreplySlave, TroundSlave, TreplyMaster;
    uint8_t done = 0;
    state = STATE_INIT;
	for(;;){
        a:      //label
        CyDelay(1); //without this, semaphore can sometimes be taken by 2 anchors at the same time
        if(xSemaphoreTake(semaphoreBuf1, 5) == pdPASS){
            vTaskPrioritySet(NULL, 25);
            done = 0;
            while(done == 0){
                switch(state){
        			case STATE_INIT :
        				TxOk = 0;
                        TxOk = 0;
                        RxState = 0;
                        RxFlag=0;
        		        TxFlag=0;
                        idle();
                        xQueueReset(Rx_q);
        		        xQueueReset(Tx_q);
                        TxData[0] = master_first_message;
                        DWM_SendData(TxData,1,1);
        				state = STATE_WAIT_FIRST_SEND;
        			break;
                            
        			case STATE_WAIT_FIRST_SEND :
                        if (xQueueReceive(Tx_q,&TxFlag,portMAX_DELAY)){
        					DWM_ReadSPI_ext(TX_TIME, NO_SUB, t1_8, 5);//get tx time (T1)
        					TxFlag = 0;
                            TxOk = 0;
                            idle();
                            DWM_Enable_Rx();
                            state = STATE_WAIT_RESPONSE;
        				}
        			break;
                            
        			case STATE_WAIT_RESPONSE:
                        if(xQueueReceive(Rx_q,&RxFlag,32) == pdPASS ){  //5 ou 32 mais pas 20
                            if (RxFlag == 2){
                                idle();
        				        DWM_Reset_Rx();
        				        state = STATE_INIT;
        			        } else if (RxFlag == 1){
        					    DWM_ReceiveData(RxData); 	// Read Rx buffer
        					    if (RxData[0] == slave_standard_message){   // Check RxFrame
                                DWM_ReadSPI_ext(RX_TIME,NO_SUB, t4_8,5);// get T4
        						state = STATE_SECOND_SEND;
                                idle();
        						}else{
                                    state = STATE_INIT;
        						}
        						RxFlag = 0;
                                RxState = 0;
                            }
        				}else{
                            state = STATE_INIT;
                            taskENTER_CRITICAL();
                            xSemaphoreGive(semaphoreBuf1);
                            vTaskPrioritySet(NULL, uxTaskPriorityGet(NULL)-6);
                            taskEXIT_CRITICAL();
                            goto a;
                            
                            
                        }
        			break;
                            
        			case STATE_SECOND_SEND:	
                        
        				TxData[0] = master_second_message;
        				DWM_SendData(TxData, 1, 1);
        				state = STATE_WAIT_SECOND_SEND;
        			break;
                            
        			case STATE_WAIT_SECOND_SEND:
                        if (xQueueReceive(Tx_q,&TxFlag,portMAX_DELAY)){
        						DWM_ReadSPI_ext(TX_TIME, NO_SUB, t5_8, 5);	//get tx time (T5)
                                TxFlag =0;
                                TxOk = 0;
                                idle();
                                DWM_Enable_Rx();
                                state = STATE_GET_TIMES;
        				}
        			break;
                            
        			case STATE_GET_TIMES :
                        if(xQueueReceive(Rx_q,&RxFlag,32) == pdPASS ){ //5 ou 32 mais pas 20
                            if (RxFlag == 2){
            					DWM_Reset_Rx();
            					state = STATE_INIT;
            				} else if (RxFlag == 1){
        						DWM_ReceiveData(RxData);	//Read Rx Buffer
        						for (int i=0;i<5;i++){
        							t2_8[i] = RxData[i];
        							t3_8[i] = RxData[i+5];
        							t6_8[i] = RxData[i+10];
        						}
                               
        						// Cast all times to uint64
        						t1 = t2 = t3 = t4 = t5 = t6 = 0;
        						for (int i=0;i<5;i++){
        							t1 = (t1 << 8) | t1_8[4-i];
        							t2 = (t2 << 8) | t2_8[4-i];
        							t3 = (t3 << 8) | t3_8[4-i];
        							t4 = (t4 << 8) | t4_8[4-i];
        							t5 = (t5 << 8) | t5_8[4-i];
        							t6 = (t6 << 8) | t6_8[4-i];
        						}
        						if (t6 < t2 || t5 < t1){
        							state = STATE_INIT;
        						}
        						else{
        							state = STATE_END;
        							
        						}
                            }
                            RxFlag = 0;
                            RxState = 0;
        				}else{
                            state = STATE_INIT;
                            taskENTER_CRITICAL();
                            xSemaphoreGive(semaphoreBuf1);
                            vTaskPrioritySet(NULL, uxTaskPriorityGet(NULL)-6);
                            taskEXIT_CRITICAL();
                            goto a;
                        }
        			break;
        			
        			case STATE_END :
                        idle();
                        TroundMaster = (t4-t1);         //Time calculation
        				TreplySlave = (t3-t2);
        				TroundSlave = (t6-t3);
        				TreplyMaster = (t5-t4);
                        all_clock_time[4*found] = TroundMaster;
                        all_clock_time[4*found+1] = TroundSlave;
                        all_clock_time[4*found+2] = TreplyMaster;
                        all_clock_time[4*found+3] = TreplySlave;
                        found =found+1;
                        state = STATE_INIT;
                        if(found == 1){
                            TxPhoneData[60] = slaveNummer;
                        }else if(found == 2){
                            TxPhoneData[61] = slaveNummer;
                        }else if(found == 3){
                            found = 0;
                            TxPhoneData[62] = slaveNummer;
                            done = 1;
                            taskENTER_CRITICAL();
                            xSemaphoreGive(semaphoreBuf1);
                            vTaskResume(usbOutHandler);
                            taskEXIT_CRITICAL();
                            goto a;
                            break;
                        }
                        done = 1;
                        taskENTER_CRITICAL();
                        xSemaphoreGive(semaphoreBuf1);
                        vTaskPrioritySet(NULL, 5);
                        taskEXIT_CRITICAL();
                        goto a;
        			break;
        		}
            }
        }else{}
	}
}