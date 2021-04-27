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

#define NESTED_ISR                          (3u)
#define DEFAULT_PRIORITY                    (1u)
#define HIGHER_PRIORITY                     (2u)

#define LIGHT_OFF (0u)
#define LIGHT_ON (1u)
uint32_t StatusRegister;
static volatile enum STATE state;

/* Private variables */
static uint8_t RxData[128];
static uint8_t TxData[128];

static uint8_t t2_8[5];
static uint8_t t3_8[5];
static uint8_t t6_8[5];

// Boolean variables
uint8_t TxOk = 0;
uint8_t RxOk = 0;
uint8_t RxError = 0;
uint8_t inter = 0;

/* 		Tasks 		 */
static void freeRTOSInit(void);
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
	inter = 1;
}

int main( void )
{	
    
    LED_1_Write(LIGHT_OFF);
    LED_2_Write(LIGHT_OFF);
    LED_3_Write(LIGHT_OFF);
    LED_4_Write(LIGHT_OFF);
    
    /* Enable global interrupts */
    CyGlobalIntEnable;
    Timer_1_Start();
    freeRTOSInit();
    SPIM_Start();

    IRQ_StartEx(ISR);
    IRQ_SetPriority(DEFAULT_PRIORITY);
	/* initialisation of the DecaWave */
	CyDelay(10u); //time for the DW to go from Wakeup to init and then IDLE
	DWM_Init();
    
    xTaskCreate(state_machine_task,"state_machine_task", 1024, NULL, DEFAULT_PRIORITY, NULL);
    
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

static void state_machine_task(void *arg)
{	
    (void)arg;
    
	for(;;){
        state = STATE_INIT;
        Timer_1_Enable();   //Timer
        rst_in_Write(1u);   //reset timer
        CyDelay(1u);
        rst_in_Write(0u);
        while(Timer_1_ReadCounter()>0x001F){
    		switch (state){
    			case STATE_INIT :
					TxOk = 0;
					RxOk = 0;
                    idle();
    				DWM_Enable_Rx();
    				state = STATE_WAIT_RECEIVE;
    			break;

    			case STATE_WAIT_RECEIVE :
                    if(inter==1){
                        interrupt_routine();
                    }
                    if (RxError){
						RxError = 0;
                        DWM_Reset_Rx();
						state = STATE_INIT;
					}else if (RxOk){
    					// Read Rx buffer
    					DWM_ReceiveData(RxData);
    					// Check RxFrame
    					if (RxData[0] == MASTER_FIRST_MESSAGE){
    						state = STATE_MESSAGE_1;
                            LED_1_Write(LIGHT_ON);
    					}
    					else if (RxData[0] == MASTER_SECOND_MESSAGE){
    						state = STATE_MESSAGE_2;
                            LED_2_Write(LIGHT_ON);
    					}
    					else {
                            state = STATE_INIT;
    					}
                        RxOk = 0;
                        TxOk = 0;
                    }
    			break;

    			case STATE_MESSAGE_1:
                    idle();
    				DWM_ReadSPI_ext(RX_TIME,NO_SUB, t2_8,5);
                    TxData[0] = SLAVE_STANDARD_MESSAGE; 
    				DWM_SendData(TxData, 1, 1);
    				state = STATE_SEND_RESPONSE;
    			break;

    			case STATE_SEND_RESPONSE :
                    if(inter==1){
                        interrupt_routine();
                    }
					if (TxOk){
                    DWM_ReadSPI_ext(TX_TIME, NO_SUB, t3_8, 5);
                    TxOk = 0;
                    idle();
                    DWM_Enable_Rx();
					state = STATE_WAIT_RECEIVE;
                    }
    			break;

    			case STATE_MESSAGE_2 :
    				//get T6
    				DWM_ReadSPI_ext(RX_TIME, NO_SUB, t6_8,5);
                    state = STATE_SEND_TIMES;
    			break;
                    
                case STATE_SEND_TIMES :
                    idle();
                    for (int i=0; i<5; i++){
    					TxData[i] = t2_8[i];
    					TxData[i+5] = t3_8[i];
    					TxData[i+10] = t6_8[i];
    				}
    				DWM_SendData(TxData, 15, 1);
                    state = STATE_END_CYCLE;
    			break;

                case STATE_END_CYCLE:
                    if(inter==1){
                        interrupt_routine();
                    }
					if (TxOk){
    					state = STATE_INIT;
                    }
    			break;
    		}
        }
	}
    vTaskDelete(NULL);
}
