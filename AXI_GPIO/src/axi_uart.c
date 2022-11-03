// --------------------------------------------------------------------
//
// Permission:
//
//   MicroPhase grants permission to use and modify this code for use
//   in synthesis for all MicroPhase Development Boards.
//   Other use of this code, including the selling
//   ,duplication, or modification of any portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design reference
//   which illustrates how these types of functions can be implemented.
//   It is the user's responsibility to verify their design for
//   consistency and functionality through the use of formal
//   verification methods.  MicroPhase provides no warranty regarding the use
//   or functionality of this code.
//
// --------------------------------------------------------------------
//
//                     MicroPhase Technologies Inc
//                     Shanghai, China
//
//                     web: http://www.microphase.cn/
//                     email: support@microphase.cn
//
// --------------------------------------------------------------------
// --------------------------------------------------------------------
//
// Major Functions:
//
// --------------------------------------------------------------------
// --------------------------------------------------------------------
//
//  Revision History:
//  Date          By            Revision    Change	 Description
//---------------------------------------------------------------------
//2020-02-22      	Wang 		1.0          		Original
//2020-                         1.1
// --------------------------------------------------------------------
// --------------------------------------------------------------------
#include "axi_uart.h"
/*
int led_val;								//LED显示状态
int key_flag=0;								//中断标志*/
//PS端GPIO
u8 SendBuffer[TEST_BUFFER_SIZE];
u8 ReceiveBuffer[TEST_BUFFER_SIZE];

/*
 * The following counters are used to determine when the entire buffer has
 * been sent and received.
 */
static volatile int TotalReceivedCount;
static volatile int TotalSentCount;


char q[] = "world";
//PL端AXI_GPIO
XUartLite UartLite;
//中断控制
XScuGic 		 scugic;					//中断控制器  驱动实例
XScuGic_Config * scugic_cfg; 				//中断控制器  配置信息
/*******************/

void axi_uart_init(){
	int Status;	
	Status = XUartLite_Initialize(&UartLite, AXI_UART_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}	
	Status = XUartLite_SelfTest(&UartLite);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}	
//	Status = UartLiteSetupIntrSystem(&scugic,&UartLite,AXI_UART_ID);

	scugic_cfg = XScuGic_LookupConfig(INTC_DEVICE_ID);
    XScuGic_CfgInitialize(&scugic, scugic_cfg, scugic_cfg->CpuBaseAddress);
    XScuGic_SetPriorityTriggerType(&scugic, UART_INT_ID, 0xA2, 0x03);
    //关联中断源AXI_GPIO和中断处理函数axi_gpio_intr_handler
    XScuGic_Connect(&scugic, UART_INT_ID, (Xil_ExceptionHandler)axi_uart_intr_handler, &UartLite);
    //使能AXI GPIO中断
    XScuGic_Enable(&scugic, UART_INT_ID);

    //设置并打开中断异常处理功能
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
    		(Xil_ExceptionHandler)XScuGic_InterruptHandler, &scugic);
    Xil_ExceptionEnable();
    //
	//XUartLite_SetSendHandler(&UartLite, SendHandler, &UartLite);
	//XUartLite_SetRecvHandler(&UartLite, RecvHandler, &UartLite);

	/*
	 * Enable the interrupt of the UartLite so that interrupts will occur.
	 */
	/*XUartLite_EnableInterrupt(&UartLite);


    //初始化PL端  AXI GPIO驱动
    XGpio_Initialize(&axi_gpio, AXI_GPIO_ID);
    //初始化中断控制器驱动
	scugic_cfg = XScuGic_LookupConfig(SCUGIC_ID);
    XScuGic_CfgInitialize(&scugic, scugic_cfg, scugic_cfg->CpuBaseAddress);
    //配置PL AXI GPIO
    XGpio_SetDataDirection(&axi_gpio, KEY_CHANNEL, 1); //设置PL AXI GPIO通道1为输入
    XGpio_InterruptEnable(&axi_gpio, KEY_MASK);		//使能通道1中断
    XGpio_InterruptGlobalEnable(&axi_gpio);			//使能AXI GPIO全局中断
    //设置中断优先级和触发类型(高电平触发)
    XScuGic_SetPriorityTriggerType(&scugic, GPIO_INT_ID, 0xA0, 0x1);
    //关联中断源AXI_GPIO和中断处理函数axi_gpio_intr_handler
    XScuGic_Connect(&scugic, GPIO_INT_ID, axi_gpio_intr_handler, &axi_gpio);
    //使能AXI GPIO中断
    XScuGic_Enable(&scugic, GPIO_INT_ID);

    //设置并打开中断异常处理功能
    Xil_ExceptionInit();
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
    		(Xil_ExceptionHandler)XScuGic_InterruptHandler, &scugic);
    Xil_ExceptionEnable();
*/

	
}


/*
// AXI GPIO 中断服务函数
void axi_gpio_intr_handler(void *CallbackRef)
{
	int key_value = 1;
	XGpio *GpioPtr = (XGpio *)CallbackRef;


	XGpio_InterruptDisable(GpioPtr, KEY_MASK);				//AXI GPIO中断失能
	key_value = XGpio_DiscreteRead(GpioPtr, KEY_CHANNEL);
	usleep(100000);												//延时100ms,按键消抖
	if(key_value == 0){										//判断按键按下
		key_flag=1;
	}

	XGpio_InterruptClear(GpioPtr, KEY_MASK);				//清除中断标志
	XGpio_InterruptEnable(GpioPtr, KEY_MASK);				//AXI GPIO中断使能
}*/
/****************************************************************************/
void axi_uart_intr_handler(void *CallBackRef)
{
    u8 buff,buff1;
    static u8 i=0;
    xil_printf("test success\r\n");
    //清除中断位
    //读取中断 ID 寄存器，判断触发的是哪种中断
    buff=XUartLite_RecvByte(XPAR_AXI_UARTLITE_0_BASEADDR);
    buff1=buff+1;
    XUartLite_SendByte(XPAR_AXI_UARTLITE_0_BASEADDR,++i);
    XUartLite_SendByte(XPAR_AXI_UARTLITE_0_BASEADDR,buff1);
    XUartLite_Send(&UartLite, q, 5);
}

/****************************************************************************/
/****************************************************************************/
void RecvHandler(void *CallBackRef, unsigned int EventData)
{
	TotalReceivedCount = EventData;
}

/****************************************************************************/
int UartLiteSetupIntrSystem(XScuGic *IntcInstancePtr,
				XUartLite *UartLiteInstPtr,
				u16 UartLiteIntrId)
{
	int Status;

#ifdef XPAR_INTC_0_DEVICE_ID

#ifndef TESTAPP_GEN
	/*
	 * Initialize the interrupt controller driver so that it is ready
	 * to use.
	 */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif

	/*
	 * Connect a device driver handler that will be called when an interrupt
	 * for the device occurs, the device driver handler performs the specific
	 * interrupt processing for the device.
	 */
	Status = XIntc_Connect(IntcInstancePtr, UartLiteIntrId,
			(XInterruptHandler)XUartLite_InterruptHandler,
			(void *)UartLiteInstPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

#ifndef TESTAPP_GEN
	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specific real mode so that
	 * the UART can cause interrupts thru the interrupt controller.
	 */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif

	/*
	 * Enable the interrupt for the UartLite.
	 */
	XIntc_Enable(IntcInstancePtr, UartLiteIntrId);
#else

#ifndef TESTAPP_GEN
	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
#endif /* TESTAPP_GEN */

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, UartLiteIntrId,
					0xA1, 0x3);

	/*
	 * Connect the interrupt handler that will be called when an
	 * interrupt occurs for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, UartLiteIntrId,
				 (Xil_ExceptionHandler)axi_uart_intr_handler,
				 UartLiteInstPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable the interrupt for the Timer device.
	 */
	XScuGic_Enable(IntcInstancePtr, UartLiteIntrId);
#endif /* XPAR_INTC_0_DEVICE_ID */



#ifndef TESTAPP_GEN

	/*
	 * Initialize the exception table.
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XScuGic_InterruptHandler,
			IntcInstancePtr);

	/*
	 * Enable exceptions.
	 */
	Xil_ExceptionEnable();

#endif /* TESTAPP_GEN */

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function disables the interrupts that occur for the UartLite.
*
* @param	IntcInstancePtr is a pointer to the instance of the INTC driver.
* @param	UartLiteIntrId is the Interrupt ID and is typically
*		XPAR_<INTC_instance>_<UARTLITE_instance>_VEC_ID
*		value from xparameters.h.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/


