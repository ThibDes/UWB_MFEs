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

uint8_t prio_matrice[NUMBER_OF_ANCHOR+1][NUMBER_OF_ANCHOR+1] = {{10,0,0,0},
                                                                {0,10,0,0},
                                                                {0,0,10,0},
                                                                {0,0,0,4},
                                                                }; //each anchor + usb_out + usb_in
uint8_t prio1 = 18;
uint8_t prio2 = 19;
uint8_t prio3 = 20;
uint8_t prio4 = 17;
uint8_t usb_out_prio = 16;
uint8_t usb_in_prio = 21;

uint16_t number_bytes_receive=0;
uint8_t RxPhoneData[8];
uint8_t TxPhoneData[63];
uint64_t all_clock_time[12]={0};
uint8_t found = 0;

// Boolean variables
uint8_t TxOk = 0;
uint8_t RxOk = 0;
uint8_t RxError = 0;
uint8_t inter = 0;

enum STATE state;

/* 		Tasks 		 */
static void freeRTOSInit(void);
static void state_machine_task(void *arg);
static void usb_out(void *arg);
static void usb_in(void *arg);

/*      Tasks handle    */
TaskHandle_t taskHandler1; 
TaskHandle_t taskHandler2; 
TaskHandle_t taskHandler3;
TaskHandle_t taskHandler4;
TaskHandle_t usbOutHandler;
TaskHandle_t usbInHandler;

SemaphoreHandle_t semaphoreBuf1;

//fonction
static void interrupt_routine(void);
static void change_priorities(int slave);

/* Interrupt prototypes */
CY_ISR_PROTO(ISR);

CY_ISR(ISR){
    IRQ_ClearPending();    /* Clear pending Interrupt */
    pin_1_ClearInterrupt();    /* Clear pin Interrupt */
	inter = 1;
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

static void interrupt_routine(void){
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
    if((StatusRegister & RX_FINISHED_MASK) == RX_FINISHED_MASK){
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
    inter=0;
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
            all_clock_time[i]=0;
        }
        USBP_LoadInEP(1,TxPhoneData,63);
        change_priorities(4);
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
        }else if(RxPhoneData[0]==0x01){
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
        }
        vTaskPrioritySet(usbOutHandler, usb_out_prio);
        vTaskPrioritySet(usbInHandler, usb_out_prio - 1);
    }else if(num == 4){
        vTaskPrioritySet(usbOutHandler, usb_out_prio - 2);
        vTaskPrioritySet(usbInHandler, usb_out_prio + 10);
    }else{
        vTaskPrioritySet(taskHandler1, uxTaskPriorityGet(taskHandler1)-prio_matrice[num][0]);
        vTaskPrioritySet(taskHandler2, uxTaskPriorityGet(taskHandler2)-prio_matrice[num][1]);
        vTaskPrioritySet(taskHandler3, uxTaskPriorityGet(taskHandler3)-prio_matrice[num][2]);
        if(NUMBER_OF_ANCHOR > 3){
            vTaskPrioritySet(taskHandler4, uxTaskPriorityGet(taskHandler4)-prio_matrice[num][3]);
            vTaskPrioritySet(usbOutHandler, uxTaskPriorityGet(usbOutHandler)-prio_matrice[num][4]);
        }else{
            vTaskPrioritySet(usbOutHandler, uxTaskPriorityGet(usbOutHandler)-prio_matrice[num][3]);
        }
    }
    taskEXIT_CRITICAL();
}

int main(void){
	LED_1_Write(LIGHT_OFF);
    LED_2_Write(LIGHT_OFF);
    LED_3_Write(LIGHT_OFF);
    LED_4_Write(LIGHT_OFF);
    
    //temp to accomodate smartphone's software
    TxPhoneData[60]=1;
    TxPhoneData[61]=2;
    TxPhoneData[62]=3;
    
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
    int slaveNummer = 1;
    xTaskCreate(state_machine_task,"slave_1", 1024, (void*) slaveNummer, 1, &taskHandler1);
    slaveNummer = 2;
    xTaskCreate(state_machine_task,"slave_2", 1024, (void*) slaveNummer, 1, &taskHandler2);
    slaveNummer = 3;
    xTaskCreate(state_machine_task,"slave_3", 1024, (void*) slaveNummer, 1, &taskHandler3);
    xTaskCreate(usb_out, "usb_out", 1024, NULL, usb_out_prio, &usbOutHandler);
    xTaskCreate(usb_in, "usb_in", 1024, NULL, usb_in_prio, &usbInHandler);
    semaphoreBuf1 = xSemaphoreCreateBinary();
    vTaskStartScheduler();
    for(;;){
    }
}
    
static void state_machine_task(void *arg){	
	uint8_t slaveNummer = (uint8_t) arg;
    uint8_t master_first_message = 0x10 | slaveNummer;
    uint8_t master_second_message	= 0x20 | slaveNummer;
    uint8_t slave_standard_message	= 0x0A | slaveNummer<<4;
    
    uint8_t RxData[15];
    uint8_t TxData[128];
    uint64_t t1, t2, t3, t4, t5, t6;
    uint8_t t1_8[5];
    uint8_t t2_8[5];
    uint8_t t3_8[5];
    uint8_t t4_8[5];
    uint8_t t5_8[5];
    uint8_t t6_8[5];
    uint64_t TroundMaster;
    uint64_t TreplySlave;
    uint64_t TroundSlave;
    uint64_t TreplyMaster;
    
	for(;;){
        state = STATE_INIT;
        Timer_1_Enable();
        rst_in_Write(1u);   //reset timer
        CyDelay(1u);
        rst_in_Write(0u);
        while(Timer_1_ReadCounter()>0x001F){
            switch(state){
    			case STATE_INIT :
                        idle();
    					RxError = 0;
    					TxOk = 0;
    					RxOk = 0;
                        TxData[0] = master_first_message;
                        DWM_SendData(TxData,1,1);
    					state = STATE_WAIT_FIRST_SEND;
    			break;
                        
    			case STATE_WAIT_FIRST_SEND :
                        if(inter==1){
                            interrupt_routine();
                        }
    					if (TxOk){
    							DWM_ReadSPI_ext(TX_TIME, NO_SUB, t1_8, 5);//get tx time (T1)
    							TxOk = 0;
                                idle();
                                DWM_Enable_Rx();
                                state = STATE_WAIT_RESPONSE;
    						}
    			break;
                        
    			case STATE_WAIT_RESPONSE:
                        if(inter==1){
                            interrupt_routine();
                        }
    					if (RxError){
    						RxError = 0;
                            DWM_Reset_Rx();
    						state = STATE_INIT;
    					}
    					else if (RxOk){
                            
    						DWM_ReceiveData(RxData); 	// Read Rx buffer
    						if (RxData[0] == slave_standard_message){   // Check RxFrame
                                DWM_ReadSPI_ext(RX_TIME,NO_SUB, t4_8,5);// get T4
    							state = STATE_SECOND_SEND;
    						}
    						else state = STATE_INIT;
    						RxOk = 0;
    					}
    			break;
                        
    			case STATE_SECOND_SEND:	
                        idle();
        				TxData[0] = master_second_message;
        				DWM_SendData(TxData, 1, 1);
        				state = STATE_WAIT_SECOND_SEND;
    			break;
                        
    			case STATE_WAIT_SECOND_SEND:
                        if(inter==1){
                            interrupt_routine();
                        }
    					if (TxOk){
                                LED_1_Write(LIGHT_ON);
    							DWM_ReadSPI_ext(TX_TIME, NO_SUB, t5_8, 5);	//get tx time (T5)
    							state = STATE_GET_TIMES;
                                idle();
                                DWM_Enable_Rx();
    							TxOk = 0;
    					}
    			break;
                        
    			case STATE_GET_TIMES :
                        if(inter==1){
                            interrupt_routine();
                        }
    					if (RxError){
                            
    						state = STATE_INIT;
                            DWM_Reset_Rx();
    						RxError = 0;
    					}
    					else if (RxOk){
                            LED_1_Write(LIGHT_OFF);
                            LED_2_Write(LIGHT_ON);
                            LED_3_Write(LIGHT_OFF);
                            LED_4_Write(LIGHT_OFF);
    
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
    							RxOk = 0;
    						}
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
                        change_priorities(slaveNummer-1);
                    }else if(found == 2){
                        TxPhoneData[61] = slaveNummer;
                        change_priorities(slaveNummer-1);
                    }else if(found == 3){
                        found = 0;
                        TxPhoneData[62] = slaveNummer;
                        change_priorities(slaveNummer-1);
                    }
    			break;
    		}
        }        
	}
}