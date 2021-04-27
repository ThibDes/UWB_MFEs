#include <project.h>

/* RTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "main.h"
#include "stdio.h"
#include "dwm1000.h"
#include "timers.h"
#include <string.h>
#include <inttypes.h>
#include <math.h>

#define NESTED_ISR                          (2u)
#define INTERRUPT_PRIORITY                    (4u)
#define LOOP_PRIORITY                     (3u)

#define LIGHT_OFF (0u)
#define LIGHT_ON (1u)
uint32_t StatusRegister;
static volatile enum STATE state;
static volatile uint8_t inter = 0;

/* Private variables */
static uint8_t RxData[128];
static uint8_t TxData[128];

static uint8_t t2_8[5];
static uint8_t t3_8[5];
static uint8_t t6_8[5];

// Boolean variables
static volatile uint8_t TxOk = 0;
static volatile uint8_t RxState = 0;

/* 		Queues 		 */
static xQueueHandle irq_queue = NULL;
static xQueueHandle Rx_q = NULL;
static xQueueHandle Tx_q = NULL;

/* 		Tasks 		 */
static void freeRTOSInit(void);
static void interrupt_routine(void *arg);
static void state_machine_task(void *arg);

uint32_t byte2long(uint8_t data[]){
	uint32_t res;
	res = data[0]|data[1]<<8|data[2]<<16|data[3]<<24;
	return res;
}

CY_ISR_PROTO(ISR);
CY_ISR(ISR){
    IRQ_ClearPending();    /* Clear pending Interrupt */
    pin_1_ClearInterrupt();    /* Clear pin Interrupt */
	uint32_t gpio_num = (uint32_t) ISR;
    xQueueSendFromISR(irq_queue, (void *) &gpio_num, NULL);
}

int main( void )
{	
    
    LED_1_Write(LIGHT_OFF);
    LED_2_Write(LIGHT_OFF);
    LED_3_Write(LIGHT_OFF);
    LED_4_Write(LIGHT_OFF);
    
    //show the device number
    if(MASTER_FIRST_MESSAGE == 0x11){
        LED_1_Write(LIGHT_ON);
    }else if(MASTER_FIRST_MESSAGE == 0x12){
        LED_2_Write(LIGHT_ON);
    }else if(MASTER_FIRST_MESSAGE == 0x13){
        LED_3_Write(LIGHT_ON);
    }
    /* Enable global interrupts */
    CyGlobalIntEnable;
    freeRTOSInit();
    SPIM_Start();
    IRQ_StartEx(ISR);
    IRQ_SetPriority(NESTED_ISR);
	/* initialisation of the DecaWave */
	CyDelay(10u); //time for the DW to go from Wakeup to init and then IDLE
	DWM_Init();
    
    irq_queue = xQueueCreate(10, sizeof(uint32_t));
    Rx_q = xQueueCreate(1, sizeof(uint8_t));
    Tx_q = xQueueCreate(1, sizeof(uint8_t));
    xTaskCreate(interrupt_routine, "interrupt_routine",1024, NULL, INTERRUPT_PRIORITY, NULL);
    xTaskCreate(state_machine_task,"state_machine_task", 1024, NULL, LOOP_PRIORITY, NULL);
    
    vTaskStartScheduler();     
    for(;;)
    {
    }
}

static void freeRTOSInit( void )
{
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
        if (xQueueReceive(irq_queue, &io_num, portMAX_DELAY)) {	
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

static void state_machine_task(void *arg)
{	
    (void)arg;
    
	uint32_t RxFlag,TxFlag;
    state = STATE_INIT;
	for(;;){
        
		switch (state){
			case STATE_INIT :
        		TxOk = 0;
                RxState = 0;
                RxFlag=0;
				TxFlag=0;
                xQueueReset(Rx_q);
				xQueueReset(Tx_q);
                idle();
				DWM_Enable_Rx();
				state = STATE_WAIT_RECEIVE;
			break;

			case STATE_WAIT_RECEIVE :
                if(xQueueReceive(Rx_q,&RxFlag,32) == pdPASS ){
                    if (RxFlag == 2){
    					DWM_Reset_Rx();
    					state = STATE_INIT;
    				} else if (RxFlag == 1){
    					// Read Rx buffer
    					DWM_ReceiveData(RxData);
    					// Check RxFrame
    					if (RxData[0] == MASTER_FIRST_MESSAGE){
                            DWM_ReadSPI_ext(RX_TIME,NO_SUB, t2_8,5);
    						state = STATE_MESSAGE_1;
    					}
    					else if (RxData[0] == MASTER_SECOND_MESSAGE){
    						DWM_ReadSPI_ext(RX_TIME, NO_SUB, t6_8,5);
                            state = STATE_MESSAGE_2;
    					}
    					else {
                            state = STATE_INIT;
    					}
                        RxFlag = 0;
    					RxState = 0;
                    }
                }else{
                    state = STATE_INIT;
                }
			break;

			case STATE_MESSAGE_1:
                idle();
                TxData[0] = SLAVE_STANDARD_MESSAGE; 
				DWM_SendData(TxData, 1, 1);
				state = STATE_SEND_RESPONSE;
			break;

			case STATE_SEND_RESPONSE :
                if (xQueueReceive(Tx_q,&TxFlag,portMAX_DELAY)){
                    DWM_ReadSPI_ext(TX_TIME, NO_SUB, t3_8, 5);
					TxFlag =0;
                    TxOk = 0;
                    idle();
                    DWM_Enable_Rx();
					state = STATE_WAIT_RECEIVE;
                }
			break;
                
            case STATE_MESSAGE_2 :
                idle();
                for (int i=0; i<5; i++){
					TxData[i] = t2_8[i];
					TxData[i+5] = t3_8[i];
					TxData[i+10] = t6_8[i];
				}
				DWM_SendData(TxData, 15, 1);
                state = STATE_END_CYCLE;
			break;

			case STATE_END_CYCLE :
                if (xQueueReceive(Tx_q,&TxFlag,portMAX_DELAY)){
                    state = STATE_INIT;
				}
			break;
		}
	}
    vTaskDelete(NULL);
}
