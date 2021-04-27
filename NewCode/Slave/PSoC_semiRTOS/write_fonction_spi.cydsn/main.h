/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/


/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */
// board switching
#define SLAVE1_BOARD
//#define SLAVE2_BOARD
//#define SLAVE3_BOARD
//#define SLAVE4_BOARD


// Antenna calibration
//#define ANTENNA_DELAY        65610U //Offset for error at -50cm  //0x8066 // precis 10cm
#define ANTENNA_DELAY          32926u

#ifdef SLAVE1_BOARD
#define SLAVE_BOARD
#define MASTER_FIRST_MESSAGE	0x11
#define MASTER_SECOND_MESSAGE	0x21
#define SLAVE_STANDARD_MESSAGE	0x1A
#endif

#ifdef SLAVE2_BOARD
#define SLAVE_BOARD
#define MASTER_FIRST_MESSAGE	0x12
#define MASTER_SECOND_MESSAGE	0x22
#define SLAVE_STANDARD_MESSAGE	0x2A
#endif

#ifdef SLAVE3_BOARD
#define SLAVE_BOARD
#define MASTER_FIRST_MESSAGE	0x13
#define MASTER_SECOND_MESSAGE	0x23
#define SLAVE_STANDARD_MESSAGE	0x3A
#endif

#ifdef SLAVE4_BOARD
#define SLAVE_BOARD
#define MASTER_FIRST_MESSAGE	0x14
#define MASTER_SECOND_MESSAGE	0x24
#define SLAVE_STANDARD_MESSAGE	0x4A
#endif

#define TX_OK_MASK		0x00000080U // TX OK
#define RX_FINISHED_MASK	0x00006400U // RX FINISHED
#define RX_ERROR_MASK		0x04279000U // RX ERROR
// State Machine Slave
enum STATE{
	STATE_INIT,
	STATE_WAIT_RECEIVE,
	STATE_MESSAGE_1,	
	STATE_MESSAGE_2,
    STATE_SEND_TIMES,
	STATE_END_CYCLE,
	STATE_SEND_RESPONSE,
};

#endif /* __MAIN_H */