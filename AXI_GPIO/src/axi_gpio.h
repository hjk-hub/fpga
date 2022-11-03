#ifndef _AXI_GPIO_H_
#define _AXI_GPIO_H_
/******************************* Include Files ********************************/
#include "stdio.h"
#include "xparameters.h"
#include "xgpiops.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "sleep.h"
///
//器件ID宏定义
#define SCUGIC_ID	XPAR_SCUGIC_0_DEVICE_ID		//中断控制器
// #define GPIOPS_ID   XPAR_XGPIOPS_0_DEVICE_ID   	//PS端  GPIO器件
#define AXI_GPIO_ID XPAR_AXI_GPIO_0_DEVICE_ID	//PL端  AXI GPIO器件
#define GPIO_INT_ID XPAR_FABRIC_GPIO_0_VEC_ID	//PL端  AXI GPIO中断
//连接端口宏定义
// #define MIO_LED1     0   						//PS LED1 连接到 MIO50
#define KEY_CHANNEL  1							//PL_KEY1使用AXI GPIO通道1
#define KEY_MASK     XGPIO_IR_CH1_MASK 			//通道1的位定义

//函数声明
void axi_gpio_intr_handler(void *CallbackRef);		//中断服务函数
void axi_gpio_init();	

extern int led_val;								//LED显示状态
extern int key_flag;								//中断标志

#endif /* XDDRPS_H_ */