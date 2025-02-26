#ifndef __ESP12_H
#define __ESP12_H

#include "io_bit.h"
#include "usart2.h"

#if 1
	#define SSID "XYD-edu118"								//热点名称
	#define PWSD "13397813910"								//热点密码
#else
	#define SSID "LAPTOP"									//热点名称
	#define PWSD "12345678"									//热点密码
#endif



#define API_TYPE  "SSL"
#define API_PORT  "443"

//疫苗api
#define Vaccine_API_IP    	"113.96.232.215"
#define Vaccine_API_ADDR  	"GET https://api.inews.qq.com/newsqa/v1/automation/modules/list?modules=VaccineTopData\r\n\r\n"

//疫情api
#define Epidemic_API_IP 	"81.68.90.103"
#define Epidemic_API_ADDR	"GET https://lab.isaaclin.cn/nCoV/api/overall\r\n\r\n"

//当前天气api
#define Weather_API_IP		"183.47.126.125"

#define Weather_Current_API_ADDR	"GET /weather/common?source=xw&refer=h5&weather_type=observe|air|tips&province=%E5%B9%BF%E8%A5%BF&city=%E5%8D%97%E5%AE%81&county=%E8%A5%BF%E4%B9%A1%E5%A1%98&callback=reqwest HTTP/1.1\r\nHost: wis.qq.com\r\n\r\n"
																		//https://wis.qq.com/weather/common?source=xw&refer=h5&weather_type=observe|air|tips&province=%E5%B9%BF%E8%A5%BF&city=%E5%8D%97%E5%AE%81&county=%E8%A5%BF%E4%B9%A1%E5%A1%98&callback=reqwest%20HTTP/1.1\r\nHost:%20wis.qq.com
#define Weather_Future_API_ADDR		"GET /weather/common?source=xw&refer=h5&weather_type=forecast_24h&province=%E5%B9%BF%E8%A5%BF&city=%E5%8D%97%E5%AE%81&county=%E8%A5%BF%E4%B9%A1%E5%A1%98&callback=reqwest HTTP/1.1\r\nHost: wis.qq.com\r\n\r\n"
																		//https://wis.qq.com/weather/common?source=xw&refer=h5&weather_type=forecast_24h&province=%E5%B9%BF%E8%A5%BF&city=%E5%8D%97%E5%AE%81&county=%E8%A5%BF%E4%B9%A1%E5%A1%98&callback=reqwest HTTP/1.1\r\nHost: wis.qq.com


//当前时间
#define Time_API_IP		"14.17.27.98"
#define Time_API_ADDR 	"GET /checktime?otype=json HTTP/1.1\r\nHost: vv.video.qq.com\r\n\r\n"

#define WifiUsart usart2							//与esp8266通信的串口的数据结构体
#define WIFI_USART USART2

#define NON_BLOCKING 1
#define WIFI_DEGBUG 1
#define MESSAGE_MAX 50					//账号密码总长
#define UNIQUE_ID_ADDRESS 0x1FFFF7E8	//AT32唯一ID地址

#define WIFI_SECTOR_NUM	2047			//扇区编号
#define WIFI_FLASH_ADDR 2047*4096		//账号密码存放地址

typedef struct {
	unsigned char (*parse)(void);
	unsigned char state;
	unsigned char previous;
	unsigned char next;
	unsigned char wish_ack;
	unsigned char response;
	unsigned short timeout;
	unsigned short interval;
	unsigned short err_count;
}__esp12_state;
extern __esp12_state esp12_state;

typedef enum {
	__NULL = 0,
	__IDLE,				//Detect the data issued by the platform when idle
	AT_AT,				//Tests AT startup
	ECHO,				//Configures echoing of AT commands
	AT_CWMODE1,			//Sets the Wi-Fi Mode(Station/SoftAP/Station+SoftAP)
	AT_CIFSR,			//Gets the Local IP Address
	AT_CWJAP,			//Connects to an AP
	AT_CIPSNTPCFG,		//Configures the time domain and SNTP server
	AT_GETSNTP,
	
	AT_MQTTCLEAN,		//Close the MQTT Connection
	AT_MQTTUSERCFG,		//Set MQTT User Config
	AT_MQTTCLIENTID,	//MQTT client configuration
	AT_MQTTCONN,		//Connect to MQTT Broker
	AT_MQTTSUB,			//Subscribe to MQTT Topic
	
	AT_RST,				//Restarts a module
	FINISH,				//Initialization completed
	DOWN_DATA,			//Platform Downlink Data
	
	AT_CWMODE2 = 20,	//Setting the site ap mode 
	AT_CIPMUX,			//Open multiple links
	AT_CWSAP,			//Configuration of the ESP12 SoftAP
	AT_CIPSERVER,		//Deletes/Creates TCP or SSL Server
	WAIT_CONF,			//Wait for the user to configure WiFi
	
	AT_CIPCLOSE,
	AT_CIPSTART,
	AT_CIPMODE,
	AT_CIPSEND,
	
	HTTP_GET,
	HTTP_RESPONSE,
	
	WAIT = 50,			//Waiting for Device Response
	AT_OK,				//Response OK
	AT_ERR,				//Response error
	AT_IP,				//Check if IP is obtained
	AT_IPOK,			//The IP has been obtained
	AT_IPERR,			//IP not obtained
	AT_SNTPOK,			//
	AT_SNTPERR,			//
	DISCONNECT,			//Hot spot disconnection
	MQTTDISCONNECTED,	//MQTT disconnected
	UNKNOWN,			//Unknown
}ESP12_STATE;

typedef enum {
	TRY_COUNT = 3,		//Retransmission times
	
	WAIT_100 = 10,		//Waiting time unit：10ms，10*10=100ms
	WAIT_200 = 20,		//The following up
	WAIT_300 = 30,
	WAIT_400 = 40, 
	WAIT_500 = 50,
	WAIT_1000 = 100,
	WAIT_2000 = 200,
	WAIT_4000 = 400,
	WAIT_10000 = 1000,
	WAIT_20000 = 2000,
}AT_TIME_OUT;

typedef enum {
	REQ_NULL = 0, 
	CURR_WEATHER,
	FUTURE_WEATHER,
	EPIDEMIC,
	VACCINE,
}REQ_NUM;

typedef enum {
	N_DISCONNECT = 0,
	N_CONNECTED,
	N_CONFIGURED,
}NETWORK_STA;



void Esp12_Init(void);
#if NON_BLOCKING == 1
void TaskEsp12Msg_Handle(void);
void Esp12_DetectingNetworkRequests(void);
void Esp12_RevampState(char *cmd, unsigned char sta, unsigned char pre, unsigned char nex, unsigned char ack, unsigned short to, unsigned short inte);
unsigned char Esp12_GetRequestPlan(void);
unsigned char Esp12_SetRequestPlan(unsigned char value);
void Esp12_GetCurrentSNTP(void);
NETWORK_STA Esp12_GetNetWorkState(void);
void Esp12_SetNetWorkState(NETWORK_STA sta);
#else
void Esp12_ConnectWifi(void);
unsigned char Esp12_SendCmd(char *cmd, char *ack, unsigned short waittime);
void BuildSSL_Connect(char *type, char *ip, char *port);
unsigned char GetNcov_Api(char *api_addr, unsigned char (*parse_fun)(void));
#endif

#endif
