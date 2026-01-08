//单片机头文件
#include "stm32f1xx_hal.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "BSP/Motor/motor.h"

//C库
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
//==============
#include "cJSON.h"



#define PROID		"LmSFw76394"  //产品ID

#define AUTH_INFO	"version=2018-10-31&res=products%2FLmSFw76394%2Fdevices%2FCR_life_test&et=1894109471&method=md5&sign=wgMQZNQnGh6cqgezTBVeTw%3D%3D"
#define DEVID		"CR_life_test"   //设备名称

#define DEVICE_NAME		"CR_life_test"

extern unsigned char esp8266_buf[512];

extern int motor_state, force;


//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;

	_Bool status = 1;

	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 1, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)  //修改clean_session=1
	{

		if(!ESP8266_SendData(mqttPacket._data, mqttPacket._len))	//上传平台成功
		{
			// 发送成功，继续处理响应
		}

		dataPtr = ESP8266_GetIPD(250);	//等待平台响应
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:UsartPrintf(&huart1, "Tips:	连接成功\r\n");status = 0;break;


					case 1:break;
					case 2:break;
					case 3:break;
					case 4:break;
					case 5:break;

					default:break;
				}
			}
		}

		MQTT_DeleteBuffer(&mqttPacket);		//删包
	}

	return status;

}



//访问ONENET需要提交JSON数据，就获取到的温湿度转换为JSON数据形式
unsigned char MqttOnenet_Savedata(char *t_payload)
{

  char json[]="{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"motor\":{\"value\":%d},\"force\":{\"value\":%d},\"mq2\":{\"value\":%d}}}";
	char t_json[200];
  unsigned short json_len;// 电机、拉力
	json_len = strlen(t_json)/sizeof(char);
	sprintf(t_json, json,motor,force,200);

	//sprintf(t_json, json, (int)(CO2_Data[6]*256+CO2_Data[7]),(int)humi,(int)light,(int)soilhumi,(int)temp,fansflag,lightflag,pumpflag,sg90flag);
  json_len = strlen(t_json)/sizeof(char);
	memcpy(t_payload, t_json, json_len);

    return json_len;
}


//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：
//==========================================================
void OneNet_SendData(void)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};		//协议包

	char buf[512];
	short body_len = 0, i = 0;
	memset(buf, 0, sizeof(buf));    //清空buff
	body_len=MqttOnenet_Savedata(buf);

	if(body_len)
	{

		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//封包
		{

			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];

			if(!ESP8266_SendData(mqttPacket._data, mqttPacket._len))									//上传数据到平台成功
		{
			// 发送成功
		}

			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
	}

}

//============================================
//	函数名称：	OneNET_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：
//==========================================================
void OneNET_Subscribe(void)
{

	MQTT_PACKET_STRUCTURE mqtt_packet = {NULL, 0, 0, 0};						//协议包

	char topic_buf[128];
	const char *topic = topic_buf;

	snprintf(topic_buf, sizeof(topic_buf), "$sys/%s/%s/thing/property/set", PROID, DEVICE_NAME);

	UsartPrintf(&huart1, "Subscribe Topic: %s\r\n", topic_buf);

	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, &topic, 1, &mqtt_packet) == 0)
	{
		if(!ESP8266_SendData(mqtt_packet._data, mqtt_packet._len))					//向平台发送订阅请求成功
		{
			// 订阅请求发送成功
		}

		MQTT_DeleteBuffer(&mqtt_packet);										//删包
	}

}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包

	char *req_payload = NULL;
	char *cmdid_topic = NULL;

	unsigned char qos = 0;
	static unsigned short pkt_id = 0;

	unsigned short topic_len = 0;
	unsigned short req_len = 0;

	unsigned char type = 0;

	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	cJSON *raw_json, *params_json, *fan_json;
	type = MQTT_UnPacketRecv(cmd);


	switch(type)
	{
		case MQTT_PKT_PUBLISH:

			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);	//解出topic和消息体
			if(result == 0)
			{
					UsartPrintf(&huart1,"req: %s",req_payload);

					raw_json=cJSON_Parse(req_payload);
					params_json=cJSON_GetObjectItem(raw_json,"params");
					fan_json=cJSON_GetObjectItem(params_json,"fan");


				if(fan_json != NULL){

					if(fan_json->valueint == 1)
					{

					}
					else
					{

					}
				}


					cJSON_Delete(raw_json);
					MQTT_DeleteBuffer(&mqttPacket);			//删包
			}

			break;

		case MQTT_PKT_PUBACK:			//发送Publish消息，平台回复的Ack

			break;

		default:
			result = -1;
			break;
	}

	ESP8266_Clear();									//清空缓存

	if(result == -1)
		return;

	dataPtr = strchr(req_payload, '}');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;

		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}

		num = atoi((const char *)numBuf);				//转为数值形式
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
