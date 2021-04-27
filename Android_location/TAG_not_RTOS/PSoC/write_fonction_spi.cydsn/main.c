/* Includes ------------------------------------------------------------------*/

#include <project.h>

/* USER CODE BEGIN Includes */
#include "main.h"
#include "stdio.h"
#include "dwm1000.h"
#include <string.h>
#include <inttypes.h>
#include <math.h>

#define NESTED_ISR                          (1u)
#define DEFAULT_PRIORITY                    (3u)
#define HIGHER_PRIORITY                     (2u)

#define LIGHT_OFF                       (0u)
#define LIGHT_ON                        (1u)


/* Private variables ---------------------------------------------------------*/
static uint8_t RxData[15];
static uint8_t TxData[128];
static uint8_t RxPhoneData[8];
static uint8_t TxPhoneData[63];
    
static uint16_t number_bytes_receive=0;

uint8_t master_first_message;
uint8_t master_second_message;
uint8_t slave_standard_message;

static uint64_t t1, t2, t3, t4, t5, t6;
static uint64_t all_clock_time[12]={0};
static uint8_t t1_8[5];
static uint8_t t2_8[5];
static uint8_t t3_8[5];
static uint8_t t4_8[5];
static uint8_t t5_8[5];
static uint8_t t6_8[5];

static volatile uint64_t TroundMaster;
static volatile uint64_t TreplySlave;
static volatile uint64_t TroundSlave;
static volatile uint64_t TreplyMaster;

// Boolean variables
static volatile uint8_t TxOk = 0;
static volatile uint8_t RxOk = 0;
static volatile uint8_t RxError = 0;
static volatile uint8_t inter = 0;

static volatile enum STATE state;

//fonction
void interrupt_routine(void);
uint8_t find_slave_nummer(uint8_t All_ancle[], uint8_t SlaveNummer); 
    
/* Interrupt prototypes */
CY_ISR_PROTO(ISR);


// main fonction 
int main(void){
    uint8_t SlaveNummer=1;
    uint8_t All_ancle[4]={1,1,1,0};
    uint8_t nbr_ancle_find=0;
    uint8_t SlaveSave;
    uint8_t next_slave=1;
    uint8_t isempty=0;

	LED_1_Write(LIGHT_OFF);
    LED_2_Write(LIGHT_OFF);
    LED_3_Write(LIGHT_OFF);
    LED_4_Write(LIGHT_OFF);
    
    /* Enable global interrupts */
    CyGlobalIntEnable;
    
    /*We need to start the USB connection and choose the power supply */
    USBP_Start(0, USBP_DWR_VDDD_OPERATION);
    
	/* We need to start Character LCD, Timer, SPI Master and Slave components */
    Timer_1_Start();
    SPIM_Start();
    
	/* initialisation of the DecaWave */
	CyDelay(10u); //time for the DW to go from Wakeup to init and then IDLE
	DWM_Init();
    
    /* Sets up the GPIO interrupt and enables it */
    IRQ_StartEx(ISR);
    IRQ_SetPriority(DEFAULT_PRIORITY);
    
    while(!USBP_bGetConfiguration());
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
        }  
        while(RxPhoneData[0]==0x01){   //Localization message
            state = STATE_INIT;
            
            //SlaveNummer=find_slave_nummer(All_ancle, SlaveNummer); 
            SlaveSave=SlaveNummer;
            while(SlaveNummer==SlaveSave){
                if(next_slave>NUMBER_OF_ANCHOR)next_slave=1;
                isempty=0;
                for(int u=0;u<NUMBER_OF_ANCHOR;u++)if(All_ancle[u]==1)isempty=1;
                
                if(All_ancle[next_slave-1]==1){
                    SlaveNummer=next_slave;
                    All_ancle[next_slave-1]=0;
                }
                else if(All_ancle[next_slave-1]!=2 && isempty==0)SlaveNummer=next_slave;
                next_slave++;
            }
            
            Timer_1_Enable();   //Timer
            rst_in_Write(1u);   //reset timer
            CyDelay(1u);
            rst_in_Write(0u);
            while(nbr_ancle_find<4 && Timer_1_ReadCounter()>0x001F){
    		    switch(state){
        			case STATE_INIT :
                            LED_1_Write(LIGHT_OFF);
                            LED_2_Write(LIGHT_OFF);
                            LED_3_Write(LIGHT_OFF);
                            LED_4_Write(LIGHT_OFF);
                            idle();
        					RxError = 0;
        					TxOk = 0;
        					RxOk = 0;
                            
        					master_first_message = 0x10 | SlaveNummer;
        					master_second_message	= 0x20 | SlaveNummer;
        				    slave_standard_message	= 0x0A | SlaveNummer<<4;
        					
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
                                    LED_1_Write(LIGHT_ON);
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
                                LED_2_Write(LIGHT_ON);
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
                                LED_3_Write(LIGHT_ON);
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
                                LED_4_Write(LIGHT_ON);
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
                        TroundMaster = (t4-t1);         //Time calculation
        				TreplySlave = (t3-t2);
        				TroundSlave = (t6-t3);
        				TreplyMaster = (t5-t4);
                        all_clock_time[4*nbr_ancle_find]=TroundMaster;      //Time are save
                        all_clock_time[4*nbr_ancle_find+1]=TroundSlave;
                        all_clock_time[4*nbr_ancle_find+2]=TreplyMaster;
                        all_clock_time[4*nbr_ancle_find+3]=TreplySlave;
                        
                        state = STATE_INIT;
        			break;
        		}
            }
            
            Timer_1_Stop();     //end timer
            if(all_clock_time[0]!=0 && nbr_ancle_find==0){           //check if one transaction is done
                if(SlaveNummer==1)LED_1_Write(LIGHT_ON);
                if(SlaveNummer==2)LED_2_Write(LIGHT_ON);
                if(SlaveNummer==3)LED_3_Write(LIGHT_ON);
                if(SlaveNummer==4)LED_4_Write(LIGHT_ON);
                nbr_ancle_find++;
                All_ancle[SlaveNummer-1]=2;
                TxPhoneData[60]=SlaveNummer;
            }
            else if(all_clock_time[4]!=0 && nbr_ancle_find==1){      //check if 2 transactions are done
                nbr_ancle_find++;
                All_ancle[SlaveNummer-1]=2;
                TxPhoneData[61]=SlaveNummer;
            }
            else if(all_clock_time[8]!=0 && nbr_ancle_find==2){      //check if 3 transactions are done
                nbr_ancle_find++;
                All_ancle[SlaveNummer-1]=2;
                TxPhoneData[62]=SlaveNummer;
            }
            
            if(nbr_ancle_find==3){                      //transactions are done
                for(int i=0;i<12;i++){                  // Preparation of data
                            TxPhoneData[5*i]=all_clock_time[i];       
                            TxPhoneData[5*i+1]=all_clock_time[i]>>8;       
                            TxPhoneData[5*i+2]=all_clock_time[i]>>16;       
                            TxPhoneData[5*i+3]=all_clock_time[i]>>24;       
                            TxPhoneData[5*i+4]=all_clock_time[i]>>32;  
                            all_clock_time[i]=0;
                }
                USBP_LoadInEP(1,TxPhoneData,63);        //Data sending
                
                RxPhoneData[0]=0x00;                    //End of the while >> All data are collected and send
                nbr_ancle_find=0;
                All_ancle[TxPhoneData[60]-1]=1;         //Initialisation for the next localisation
                All_ancle[TxPhoneData[61]-1]=1;
                All_ancle[TxPhoneData[62]-1]=1;
                LED_1_Write(LIGHT_OFF);                 //All LED are switch off
                LED_2_Write(LIGHT_OFF);
                LED_3_Write(LIGHT_OFF);
                LED_4_Write(LIGHT_OFF);
            }
            
        }
	}
}


//interrupt management

CY_ISR(ISR){
    IRQ_ClearPending();    /* Clear pending Interrupt */
    pin_1_ClearInterrupt();    /* Clear pin Interrupt */
	inter = 1;
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

/*uint8_t find_slave_nummer(uint8_t All_ancle[], uint8_t SlaveNummer){
    uint8_t Next=SlaveNummer;
    uint8_t isempty=0;
    
    for(int u=0;u<NUMBER_OF_ANCHOR;u++)if(All_ancle[u]==1)isempty=1;
    while(isempty==1){
        if(All_ancle[Next-1]==1){
            All_ancle[Next-1]=0;
            return Next;
        }
        else Next ++;
        if(Next>NUMBER_OF_ANCHOR)Next=1;
    }
    while(isempty==0){
        if(All_ancle[Next-1]!=2 && isempty==0)return Next;
        else Next++;
        if(Next>NUMBER_OF_ANCHOR)Next=1;
    }
    return 0;
}*/