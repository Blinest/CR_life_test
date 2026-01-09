

//单片机头文件
#include "stm32f1xx_hal.h"

//网络设备驱动
#include "esp8266.h"

//硬件驱动
#include "Common/utils.h"
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>

#define ESP8266_WIFI_INFO		"AT+CWJAP=\"vivo\",\"12345678\"\r\n"//热点的AP频率要设置成2.4GHz

//#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"    //旧版OneNET地址
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"  	//新版OneNET地址
//#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"192.168.11.125\",8080\r\n"   //调试地址

volatile unsigned char esp8266_buf[128];
volatile unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;


//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	函数名称：	ESP8266_WaitReceive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP8266_WaitReceive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;

	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数

		return REV_OK;								//返回接收完成标志
	}

	esp8266_cntPre = esp8266_cnt;					//置为相同

	return REV_WAIT;								//返回接收未完成标志

}

//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令，使用串口2发送
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	unsigned char timeOut = 300;  // 增加超时时间到3秒

	ESP8266_Clear();  // 发送命令前先清空缓存
	Usart_SendString(&huart2, (unsigned char *)cmd, strlen((const char *)cmd));  // 发送命令

	while(timeOut--)
	{
		delay_ms(10);  // 等待10ms
		
		// 每次循环都检查缓冲区，因为ESP8266的响应可能分多次接收
		if(esp8266_cnt > 0)  // 如果缓冲区有数据
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)  // 检查是否包含响应关键词
			{
				ESP8266_Clear();  // 清空缓存
				return 0;  // 成功
			}
		}
	}

	ESP8266_Clear();  // 超时，清空缓存
	return 1;  // 失败

}

//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	0-成功	1-失败
//
//	说明：
//==========================================================
_Bool ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[200];
	_Bool result = 1;

	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		Usart_SendString(&huart2, data, len);		//发送设备连接请求数据
		// 等待发送完成响应
		if(!ESP8266_SendCmd("", "SEND OK")) {			// 等待"SEND OK"响应
			result = 0;
		}
	}

	return result;
}

//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;

	do
	{
		if(ESP8266_WaitReceive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD,");				//搜索“IPD”头
			if(ptrIPD != NULL)
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;

			}
		}
		delay_ms(5);													//延时等待
	} while(timeOut--);

	return NULL;														//超时还未找到，返回空指针

}

//==========================================================
//	函数名称：	ESP8266_Init
//
//	函数功能：	初始化ESP8266
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：
//==========================================================
void ESP8266_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	// 使能 GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//ESP8266复位引脚
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	delay_ms(500);  // 增加复位低电平时间
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	delay_ms(1000); // 增加复位后等待时间，确保ESP8266完全启动

	// 清空缓冲区
	ESP8266_Clear();
	// 发送AT指令测试
	UsartPrintf(&huart2, " 1. AT\r\n");
	while(ESP8266_SendCmd("AT\r\n", "OK"))
		delay_ms(500);

	// 设置工作模式为STA模式
	UsartPrintf(&huart2, "2. CWMODE\r\n");
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
		delay_ms(500);

	// 连接WiFi
	UsartPrintf(&huart2, "4. CWJAP\r\n");
	while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
		delay_ms(500);

	// 连接OneNet服务器
	UsartPrintf(&huart2, "5. CIPSTART\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_ms(3000);

	UsartPrintf(&huart2, "6. ESP8266 Init OK\r\n");
}

//==========================================================
//	函数名称：	ESP8266_USART2_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：
//==========================================================
void ESP8266_USART2_IRQHandler(void)
{
	if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
	{
		if (esp8266_cnt >= sizeof(esp8266_buf)) esp8266_cnt = 0; // 防止溢出
		// 读取数据
		esp8266_buf[esp8266_cnt++] = (uint8_t)(huart2.Instance->DR & 0xFF);
		// 清除中断标志
		__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
	}
}
