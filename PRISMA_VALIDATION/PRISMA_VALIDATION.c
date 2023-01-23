#include <stdio.h>
#include "fsl_debug_console.h"
#include "queue.h"
#include <string.h>
/*  SDK Included Files */
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_i2c_edma.h"
#include "fsl_dmamux.h"
#include "fsl_common.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

#include "../PRISMA_SCREEN_INCLUDES/PRISMA_SCREEN.c"




/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* I2C source clock */
#define I2C_MASTER_CLK_SRC          I2C0_CLK_SRC
#define I2C_MASTER_CLK_FREQ         CLOCK_GetFreq(I2C0_CLK_SRC)
#define EXAMPLE_I2C_MASTER_BASEADDR I2C0
#define DMA_REQUEST_SRC             kDmaRequestMux0I2C0
#define EXAMPLE_I2C_DMAMUX_BASEADDR DMAMUX0
#define EXAMPLE_I2C_DMA_BASEADDR    DMA0
#define I2C_DMA_CHANNEL             0U
#define I2C_MASTER_SLAVE_ADDR_7BIT 0x7EU
#define EXAMPLE_I2C_SLAVE_BASEADDR I2C0
#define I2C_BAUDRATE               100000U
#define I2C_DATA_LENGTH            8U
#define I2C_SLAVE_HOLD_TIME_NS 4000U
#define I2C_SLAVE_CLK_SRC          I2C0_CLK_SRC
#define I2C_SLAVE_CLK_FREQ         CLOCK_GetFreq(I2C0_CLK_SRC)


/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t g_master_txBuff[I2C_DATA_LENGTH];
uint8_t g_master_rxBuff[I2C_DATA_LENGTH];
i2c_master_edma_handle_t g_m_dma_handle;
edma_handle_t edmaHandle;
volatile bool g_MasterCompletionFlag = false;
uint8_t g_slave_buff[I2C_DATA_LENGTH];
i2c_slave_handle_t g_s_handle;
volatile bool g_SlaveCompletionFlag = false;
i2c_master_edma_handle_t g_m_dma_handle;
edma_handle_t edmaHandle;
uint8_t g_slave_txbuff[6];
/*
 * @brief   Application entry point.
 */static void i2c_slave_callback(I2C_Type *base, i2c_slave_transfer_t *xfer,
		 void *userData) {
	 switch (xfer->event) {
	 /*  Address match event */
	 case kI2C_SlaveAddressMatchEvent:
		 xfer->data = NULL;
		 xfer->dataSize = 0;
		 break;
		 /*  Transmit request */
	 case kI2C_SlaveTransmitEvent:
		 /*  Update information for transmit process */
		 xfer->data = g_slave_txbuff;
		 xfer->dataSize = 8;
		 break;
		 /*  Receive request */
	 case kI2C_SlaveReceiveEvent:
		 /*  Update information for received process */
		 xfer->data = g_slave_buff;
		 xfer->dataSize = I2C_DATA_LENGTH;
		 break;
		 /*  Transfer done */
	 case kI2C_SlaveCompletionEvent:
		 g_SlaveCompletionFlag = true;
		 xfer->data = NULL;
		 xfer->dataSize = 0;
		 break;
	 default:
		 g_SlaveCompletionFlag = false;
		 break;
	 }
 }

 int Auto_up_validation(int *start_colour, int *end_colour,int *resolution,int *send_status_flag) {
	 if (start_colour[0] < end_colour[0]) {
		 if(resolution[0] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else if (start_colour[0] == end_colour[0]) {
		 if(resolution[0] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP, TRY A VALID START AND END COLOUR FOR RED(%d > %d IS NOT POSSIBLE)\r\n",start_colour[0],end_colour[0]);
		 delay(20);
	 }
	 if (start_colour[1] < end_colour[1]) {
		 if(resolution[1] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }
	 } else if (start_colour[1] == end_colour[1]) {
		 if(resolution[1] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP, TRY A VALID START AND END COLOUR FOR GREEN(%d > %d IS NOT POSSIBLE)\r\n",start_colour[1],end_colour[1]);
		 delay(20);
	 }

	 if (start_colour[2] < end_colour[2]) {
		 if(resolution[2] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }

	 } else if (start_colour[2] == end_colour[2]) {
		 if(resolution[2] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP, TRY A VALID START AND END COLOUR FOR BLUE(%d > %d IS NOT POSSIBLE)\r\n",start_colour[2],end_colour[2]);
		 delay(20);
	 }
	 return 1;
 }
 int Auto_down_validation(int *start_colour, int *end_colour,int *resolution,int *send_status_flag) {
	 if (start_colour[0] > end_colour[0]) {
		 if(resolution[0] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }

	 } else if (start_colour[0] == end_colour[0]) {
		 if(resolution[0] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-DOWN, TRY A VALID START AND END COLOUR FOR RED(%d > %d IS NOT POSSIBLE)",start_colour[0],end_colour[0]);
		 delay(20);
	 }
	 if (start_colour[1] > end_colour[1]) {
		 if(resolution[1] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }

	 } else if (start_colour[1] == end_colour[1]) {
		 if(resolution[1] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-DOWN, TRY A VALID START AND END COLOUR FOR GREEN(%d > %d IS NOT POSSIBLE)\r\n",start_colour[1],end_colour[1]);
		 delay(20);
	 }

	 if (start_colour[2] > end_colour[2]) {
		 if(resolution[2] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }

	 } else if (start_colour[2] == end_colour[2]) {
		 if(resolution[2] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-DOWN, TRY A VALID START AND END COLOUR FOR BLUE(%d > %d IS NOT POSSIBLE)\r\n",start_colour[2],end_colour[2]);
		 delay(20);
	 }
	 return 1;
 }
 int Auto_up_down_validation(int *start_colour, int *end_colour,int *resolution,int *send_status_flag) {
	 if (start_colour[0] < end_colour[0]) {
		 if(resolution[0] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else if (start_colour[0] == end_colour[0]) {
		 if(resolution[0] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF RED IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[0],end_colour[0],resolution[0]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP/DOWN, TRY A VALID START AND END COLOUR FOR RED(%d > %d IS NOT POSSIBLE)",start_colour[0],end_colour[0]);
		 delay(20);
	 }
	 if (start_colour[1] < end_colour[1]) {
		 if(resolution[1] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }

	 } else if (start_colour[1] == end_colour[1]) {
		 if(resolution[1] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF GREEN IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[1],end_colour[1],resolution[1]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP/DOWN, TRY A VALID START AND END COLOUR FOR GREEN(%d > %d IS NOT POSSIBLE)\r\n",start_colour[1],end_colour[1]);
		 delay(20);
	 }

	 if (start_colour[2] < end_colour[2]) {
		 if(resolution[2] > 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }

	 } else if (start_colour[2] == end_colour[2]) {
		 if(resolution[2] == 0) {
			 *send_status_flag = *send_status_flag + 1;
		 } else {
			 PRINTF("THE START COLOUR OF BLUE IS %d >>>>>> END COLOUR IS  %d >>>>>> RESOLUTION IS  %d >>>>>> IS INVALID <<<<<<\r\n",start_colour[2],end_colour[2],resolution[2]);
			 delay(20);
		 }
	 } else {
		 PRINTF("MODE IS AUTO-UP/DOWN,TRY A VALID START AND END COLOUR FOR BLUE(%d > %d IS NOT POSSIBLE)\r\n",start_colour[2],end_colour[2]);
		 delay(20);
	 }

	 return 1;
 }
 int manual_validation(int *start_colour, int *end_colour,int *resolution) {
	 return 1;
 }

 static void i2c_master_callback(I2C_Type *base, i2c_master_edma_handle_t *handle, status_t status, void *userData)
 {
	 /* Signal transfer success when received success status. */
	 if (status == kStatus_Success)
	 {
		 g_MasterCompletionFlag = true;
	 }
 }

 int Master_Handshake (void)
 {
	 i2c_master_config_t masterConfig;
	 uint32_t sourceClock;
	 i2c_master_transfer_t masterXfer;
	 edma_config_t config;
	 /*Init EDMA for example*/
	 DMAMUX_Init(EXAMPLE_I2C_DMAMUX_BASEADDR);
	 EDMA_GetDefaultConfig(&config);
	 EDMA_Init(EXAMPLE_I2C_DMA_BASEADDR, &config);
	 /* Set up i2c master to send data to slave*/
	 /* First data in txBuff is data length of the transmiting data. */
	 g_master_txBuff[0] = 'M';
	 g_master_txBuff[1] = 'A';
	 g_master_txBuff[2] = 'S';
	 g_master_txBuff[3] = 'T';
	 g_master_txBuff[4] = 'E';
	 g_master_txBuff[5] = 'R';
	 g_master_txBuff[6] = '\0';
	 /*
	  * masterConfig->baudRate_Bps = 100000U;
	  * masterConfig->enableStopHold = false;
	  * masterConfig->glitchFilterWidth = 0U;
	  * masterConfig->enableMaster = true;
	  */
	 I2C_MasterGetDefaultConfig(&masterConfig);
	 masterConfig.baudRate_Bps = I2C_BAUDRATE;
	 sourceClock = I2C_MASTER_CLK_FREQ;
	 I2C_MasterInit(EXAMPLE_I2C_MASTER_BASEADDR, &masterConfig, sourceClock);
	 memset(&g_m_dma_handle, 0, sizeof(g_m_dma_handle));
	 memset(&masterXfer, 0, sizeof(masterXfer));
	 /* subAddress = 0x01, data = g_master_txBuff - write to slave.
          start + slaveaddress(w) + subAddress + length of data buffer + data buffer + stop*/
	 uint8_t deviceAddress     = 0x01U;
	 masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT;
	 masterXfer.direction      = kI2C_Write;
	 masterXfer.subaddress     = (uint32_t)deviceAddress;
	 masterXfer.subaddressSize = 1;
	 masterXfer.data           = g_master_txBuff;
	 masterXfer.dataSize       = I2C_DATA_LENGTH;
	 masterXfer.flags          = kI2C_TransferDefaultFlag;
	 DMAMUX_SetSource(EXAMPLE_I2C_DMAMUX_BASEADDR, I2C_DMA_CHANNEL, DMA_REQUEST_SRC);
	 EDMA_CreateHandle(&edmaHandle, EXAMPLE_I2C_DMA_BASEADDR, I2C_DMA_CHANNEL);
	 DMAMUX_EnableChannel(EXAMPLE_I2C_DMAMUX_BASEADDR, I2C_DMA_CHANNEL);
	 I2C_MasterCreateEDMAHandle(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dma_handle, i2c_master_callback, NULL, &edmaHandle);
	 int i = 0;
	 status_t i2c_stat = 1;
	 while(i2c_stat && (i < 5))
	 {
		 //            for (uint32_t i = 0U; i < I2C_DATA_LENGTH - 1U; i++)
			 //            {
		 //                PRINTF("%c  ", g_master_txBuff[i]);
		 //            }
		 //            PRINTF("\r\n\r\n");
		 i2c_stat =  I2C_MasterTransferEDMA(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dma_handle, &masterXfer);
		 i++;
	 }
	 /*  Wait for transfer completed. */
	 while (!g_MasterCompletionFlag)
	 {
	 }
	 g_MasterCompletionFlag = false;

	 for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
	 {
		 g_master_rxBuff[i] = 0;
	 }

	 /* subAddress = 0x01, data = g_master_rxBuff - read from slave.
          start + slaveaddress(w) + subAddress + repeated start + slaveaddress(r) + rx data buffer + stop */
	 masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT;
	 masterXfer.direction      = kI2C_Read;
	 masterXfer.subaddress     = (uint32_t)deviceAddress;
	 masterXfer.subaddressSize = 1;
	 masterXfer.data           = g_master_rxBuff;
	 masterXfer.dataSize       = I2C_DATA_LENGTH - 1;
	 masterXfer.flags          = kI2C_TransferDefaultFlag;
	 masterXfer.flags          = kI2C_TransferDefaultFlag;
	 I2C_MasterTransferEDMA(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dma_handle, &masterXfer);
	 /*  Reset master completion flag to false. */
	 g_MasterCompletionFlag = false;
	 /*  Wait for transfer completed. */
	 while (!g_MasterCompletionFlag)
	 {
	 }
	 g_MasterCompletionFlag = false;

	 uint8_t string[] = "SLAVE";
	 if(strcmp((char *)g_master_rxBuff,(char *) string) == 0)
	 {
		 PRINTF("Handshake successful");
		 PRINTF("Send connection status to master and slave");
		 return 0;
	 }
	 else
	 {
		 PRINTF("Handshake failed");
		 return -1;
	 }
	 PRINTF("\r\n\r\n");
	 PRINTF("\r\nEnd of I2C example .\r\n");
	 while (1)
	 {
	 }
 }



 //I2C Slave Handshake////////////////////////////////////////////////////////////////////////////////////
 int Slave_Handshake(void)
 {
	 i2c_slave_config_t slaveConfig;
	 PRINTF("\r\nI2C board2board EDMA example -- Slave transfer.\r\n\r\n");
	 /*1.Set up i2c slave first*/
	 /*
	  * slaveConfig->addressingMode = kI2C_Address7bit;
	  * slaveConfig->enableGeneralCall = false;
	  * slaveConfig->enableWakeUp = false;
	  * slaveConfig->enableBaudRateCtl = false;
	  * slaveConfig->enableSlave = true;
	  * slaveConfig->sclStopHoldTime_ns = 4000;
	  */
	 I2C_SlaveGetDefaultConfig(&slaveConfig);
	 slaveConfig.addressingMode = kI2C_Address7bit;
	 slaveConfig.slaveAddress = I2C_MASTER_SLAVE_ADDR_7BIT;
	 slaveConfig.upperAddress = 0; /*  not used for this example */
	 slaveConfig.sclStopHoldTime_ns = I2C_SLAVE_HOLD_TIME_NS;
	 I2C_SlaveInit(EXAMPLE_I2C_SLAVE_BASEADDR, &slaveConfig, I2C_SLAVE_CLK_FREQ);
	 for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++) {
		 g_slave_buff[i] = 0;
	 }
	 g_slave_txbuff[0] = 'S';
	 g_slave_txbuff[1] = 'L';
	 g_slave_txbuff[2] = 'A';
	 g_slave_txbuff[3] = 'V';
	 g_slave_txbuff[4] = 'E';
	 g_slave_txbuff[5] = '\0';
	 memset(&g_s_handle, 0, sizeof(g_s_handle));
	 I2C_SlaveTransferCreateHandle(EXAMPLE_I2C_SLAVE_BASEADDR, &g_s_handle,
			 i2c_slave_callback, NULL);
	 /* Set up slave transfer. */
	 I2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE_BASEADDR, &g_s_handle,
			 kI2C_SlaveCompletionEvent | kI2C_SlaveAddressMatchEvent);
	 /*  wait for transfer completed. */
	 while (!g_SlaveCompletionFlag) {
	 }
	 g_SlaveCompletionFlag = false;
	 /* Wait for master receive completed.*/
	 while (!g_SlaveCompletionFlag) {
	 }
	 g_SlaveCompletionFlag = false;
	 uint8_t string[] = "MASTER";
	 if(strcmp((char *)g_slave_buff,(char *) string) == 0)
	 {
		 PRINTF("Handshake successful");
		 PRINTF("Send connection status to master and slave");
		 return 0;
	 }
	 else
	 {
		 PRINTF("Handshake failed");
		 return -1;
	 }
	 PRINTF("\r\nEnd of I2C example .\r\n");
	 while (1) {
	 }
 }
 ////I2C Data Master////////////////////////////////////////////////////////////////////////////////////////
 void Master_Data(uint8_t *g_master_txBuff)
 {
	 i2c_master_config_t masterConfig;
	 uint32_t sourceClock;
	 i2c_master_transfer_t masterXfer;
	 edma_config_t config;
	 BOARD_InitBootPins();
	 BOARD_InitBootClocks();
	 BOARD_InitDebugConsole();
	 /*Init EDMA for example*/
	 DMAMUX_Init(EXAMPLE_I2C_DMAMUX_BASEADDR);
	 EDMA_GetDefaultConfig(&config);
	 EDMA_Init(EXAMPLE_I2C_DMA_BASEADDR, &config);
	 PRINTF("\r\nI2C board2board EDMA example -- Master transfer.\r\n");
	 PRINTF("Master will send data :");
	 for (uint32_t i = 0U; i <= I2C_DATA_LENGTH; i++)
	 {
		 PRINTF("%d  ", g_master_txBuff[i]);
	 }
	 PRINTF("\r\n\r\n");
	 /*
	  * masterConfig->baudRate_Bps = 100000U;
	  * masterConfig->enableStopHold = false;
	  * masterConfig->glitchFilterWidth = 0U;
	  * masterConfig->enableMaster = true;
	  */
	 I2C_MasterGetDefaultConfig(&masterConfig);
	 masterConfig.baudRate_Bps = I2C_BAUDRATE;
	 sourceClock = I2C_MASTER_CLK_FREQ;
	 I2C_MasterInit(EXAMPLE_I2C_MASTER_BASEADDR, &masterConfig, sourceClock);
	 memset(&g_m_dma_handle, 0, sizeof(g_m_dma_handle));
	 memset(&masterXfer, 0, sizeof(masterXfer));
	 /* subAddress = 0x01, data = g_master_txBuff - write to slave.
  start + slaveaddress(w) + subAddress + length of data buffer + data buffer + stop*/
	 uint8_t deviceAddress     = 0x01U;
	 masterXfer.slaveAddress   = I2C_MASTER_SLAVE_ADDR_7BIT;
	 masterXfer.direction      = kI2C_Write;
	 masterXfer.subaddress     = (uint32_t)deviceAddress;
	 masterXfer.subaddressSize = 1;
	 masterXfer.data           = g_master_txBuff;
	 masterXfer.dataSize       = I2C_DATA_LENGTH;
	 masterXfer.flags          = kI2C_TransferDefaultFlag;
	 DMAMUX_SetSource(EXAMPLE_I2C_DMAMUX_BASEADDR, I2C_DMA_CHANNEL, DMA_REQUEST_SRC);
	 EDMA_CreateHandle(&edmaHandle, EXAMPLE_I2C_DMA_BASEADDR, I2C_DMA_CHANNEL);
	 DMAMUX_EnableChannel(EXAMPLE_I2C_DMAMUX_BASEADDR, I2C_DMA_CHANNEL);
	 I2C_MasterCreateEDMAHandle(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dma_handle, i2c_master_callback, NULL, &edmaHandle);
	 I2C_MasterTransferEDMA(EXAMPLE_I2C_MASTER_BASEADDR, &g_m_dma_handle, &masterXfer);
	 /*  Wait for transfer completed. */
	 while (!g_MasterCompletionFlag)
	 {
	 }
	 g_MasterCompletionFlag = false;
 }
 //////I2C Slave data/////////////////////////////////////////////////////////////////////////////////
 void Slave_Data(uint8_t *g_slave_buff)
 {
	 i2c_slave_config_t slaveConfig;
	 I2C_SlaveGetDefaultConfig(&slaveConfig);
	 slaveConfig.addressingMode = kI2C_Address7bit;
	 slaveConfig.slaveAddress = I2C_MASTER_SLAVE_ADDR_7BIT;
	 slaveConfig.upperAddress = 0; /*  not used for this example */
	 slaveConfig.sclStopHoldTime_ns = I2C_SLAVE_HOLD_TIME_NS;
	 I2C_SlaveInit(EXAMPLE_I2C_SLAVE_BASEADDR, &slaveConfig, I2C_SLAVE_CLK_FREQ);
	 for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++) {
		 g_slave_buff[i] = 0;
	 }
	 I2C_SlaveTransferCreateHandle(EXAMPLE_I2C_SLAVE_BASEADDR, &g_s_handle,
			 i2c_slave_callback, NULL);
	 /* Set up slave transfer. */
	 I2C_SlaveTransferNonBlocking(EXAMPLE_I2C_SLAVE_BASEADDR, &g_s_handle,
			 kI2C_SlaveCompletionEvent | kI2C_SlaveAddressMatchEvent);
	 /*  wait for transfer completed. */
	 while (!g_SlaveCompletionFlag) {
	 }
	 g_SlaveCompletionFlag = false;
	 PRINTF("Slave received these:");
	 for (int i = 0; i < 16; i++)
	 {
		 PRINTF("%d \r\n",g_slave_buff[i]);
	 }
	 while (1) {
	 }
 }

