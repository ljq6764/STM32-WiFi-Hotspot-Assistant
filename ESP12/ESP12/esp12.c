#include "esp12.h"
#include "systick.h"
#include "usart1.h"
#include "malloc.h"
#include "rtc.h"
#include "w25qxx.h"
#include "QMsg.h"
#include "led.h"
#include "control.h"
#include "parse.h"

CQueue ehead;

char poweron_link =0;

__esp12_state esp12_state = {
	.state = AT_AT,			//初始状态
	.previous = AT_AT,		//上一个状态
	.next = AT_AT,			//下一个状态
	.wish_ack = AT_OK,		//期望应答
	.response = AT_OK,		//实际应答
	.timeout = WAIT_100,	//超时时间
	.interval = WAIT_100,	//命令发送间隔
	.err_count = TRY_COUNT,	//最大错误次数
};

static unsigned char ssid[MESSAGE_MAX/2] = {"ww"}, pwsd[MESSAGE_MAX/2] = {"123456789"};

static unsigned char Esp12_MeasuringResponse(unsigned char ack);
static void Esp12_JumpState(unsigned char sta, unsigned char offset);
static bool Esp12_CheckFlashOrBuffer(unsigned char *ssid, unsigned char *pwsd, char mode);
static void Esp12_WaitResponse(void);
static unsigned char Esp12_DownDataHandle(void);
static unsigned short Crc16_Compute(unsigned char *ptr, unsigned short len);

void Esp12_DetectingNetworkRequests(void)
{
	if(network_flag.r.is_request != true){			//没有请求
		return;
	}

	if(network_flag.r.is_connect != N_CONNECTED){	//未联网不允许发起网络请求
		return;
	}
	
	if(network_flag.r.requestIsFinish != true){		//之前的请求是否完成
		return;
	}
	
	network_flag.r.request_number = Esp12_GetRequestPlan();		//从队列中读取计划
	if(network_flag.r.request_number == 0){						//队列为空
		network_flag.r.is_request = false;						//取消请求标记
		network_flag.r.networkIsRequest = false;				//取消网络请求标记
		network_flag.r.requestIsFinish = true;					//标记已完成请求
		return;	
	}
	
	printf("Network requests: %d\r\n", network_flag.r.request_number);
	
	network_flag.r.requestIsFinish = false;			//标记正在请求

	Esp12_RevampState("AT+CIPCLOSE\r\n", WAIT, AT_CIPCLOSE, AT_CIPSTART, AT_OK|AT_ERR, WAIT_1000, WAIT_100);	
}

void TaskEsp12Msg_Handle(void)
{
	switch(esp12_state.state){
		case __IDLE:
			if(Esp12_DownDataHandle() == DISCONNECT){			//断线时回到最初的状态
				Esp12_RevampState("AT\r\n", WAIT, AT_AT, AT_AT, AT_OK, WAIT_1000, WAIT_100);
			}
			Esp12_DetectingNetworkRequests();
			break;
		case AT_AT:
			Esp12_SetNetWorkState(N_DISCONNECT);
			Esp12_RevampState("AT\r\n", WAIT, AT_AT, ECHO, AT_OK, WAIT_1000, WAIT_100);
			break;
		case ECHO:
			Esp12_RevampState("ATE0\r\n", WAIT, ECHO, AT_CWMODE1, AT_OK, WAIT_1000, WAIT_100);
		break;
		case AT_CWMODE1:
			Esp12_RevampState("AT+CWMODE=1\r\n", WAIT, AT_CWMODE1, AT_CIFSR, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_CIFSR:
			Esp12_RevampState("AT+CIFSR\r\n", WAIT, AT_CIFSR, AT_CWJAP, AT_IP, WAIT_1000, WAIT_100);
			break;
		case AT_CWJAP:
			sprintf((char *)usart1.RxBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwsd);
			Esp12_RevampState((char *)usart1.RxBuff, WAIT, AT_CWJAP, AT_CIPSNTPCFG, AT_OK, WAIT_20000, WAIT_4000);
			break;
		case AT_CIPSNTPCFG:
			Esp12_RevampState("AT+CIPSNTPCFG=1,8\r\n", WAIT, AT_CIPSNTPCFG, AT_GETSNTP, AT_OK, WAIT_1000, WAIT_2000);
			break;
		case AT_GETSNTP:
			Esp12_RevampState("AT+CIPSNTPTIME?\r\n", WAIT, AT_GETSNTP, FINISH, AT_SNTPOK, WAIT_1000, WAIT_2000);
			break;
		case AT_CWMODE2:
			memset(ssid, 0, MESSAGE_MAX/2);			//清除本地缓冲信息
			memset(pwsd, 0, MESSAGE_MAX/2);
			memset(usart1.RxBuff, 0, sizeof(usart1.RxBuff));
			W25Qxx_EraseSector(WIFI_SECTOR_NUM);
			Esp12_RevampState("AT+CWMODE=2\r\n", WAIT, AT_CWMODE2, AT_CIPMUX, AT_OK, WAIT_1000, WAIT_100);
			Esp12_SetNetWorkState(N_CONFIGURED);
			break;
		case AT_CIPMUX:
			Esp12_RevampState("AT+CIPMUX=1\r\n", WAIT, AT_CIPMUX, AT_CWSAP, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_CWSAP:{
			unsigned char buf[12] = {0};
			memcpy(buf, (unsigned char *)UNIQUE_ID_ADDRESS, 12);
			memset(usart1.RxBuff, 0, sizeof(usart1.RxBuff));
			sprintf((char *)usart1.RxBuff, "AT+CWSAP=\"Smart-%05d\",\"12345678\",1,4\r\n", Crc16_Compute(buf, 12));
			Esp12_RevampState((char *)usart1.RxBuff, WAIT, AT_CWSAP, AT_CIPSERVER, AT_OK, WAIT_1000, WAIT_100);
		}	break;
		case AT_CIPSERVER:
			Esp12_RevampState("AT+CIPSERVER=1,8086\r\n", WAIT, AT_CIPSERVER, WAIT_CONF, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_RST:	//4S后结束复位状态，回到最开始的状态
			if(--esp12_state.timeout == 0)	
				Esp12_RevampState("AT\r\n", WAIT, AT_AT, AT_AT, AT_OK, WAIT_1000, WAIT_100);
			break;
		case WAIT_CONF:
			if(Esp12_DownDataHandle() == WAIT_CONF){
				printf("--%s %s--\r\n", ssid, pwsd);
				network_flag.r.is_connect = N_DISCONNECT;
				Esp12_RevampState(NULL, AT_AT, AT_AT, AT_AT, AT_OK, WAIT_1000, WAIT_100);	//得到账号密码后回到最开始的状态
			}
			break;
		case FINISH:	//完成连接后做一些准备工作，随后跳转空闲状态
			Esp12_SetNetWorkState(N_CONNECTED);	//标记已连接到wifi				
			memset(usart1.RxBuff, 0, sizeof(usart1.RxBuff));
			esp12_state.timeout = sprintf((char *)usart1.RxBuff, "ssid:%s pwsd:%s", ssid, pwsd);
			usart1.RxBuff[esp12_state.timeout] = 0xaa;
			W25Qxx_EraseSector(WIFI_SECTOR_NUM);
			W25Qxx_WritePage(usart1.RxBuff, WIFI_FLASH_ADDR, MESSAGE_MAX);	//连接成功后更新flash中的信息
			printf("Platform connection successful\r\n");
			Esp12_RevampState(NULL, __IDLE, FINISH, __IDLE, AT_OK, WAIT_1000, WAIT_100);
			LED1 = 1;
			break;
		case AT_CIPCLOSE:
			Esp12_RevampState("AT+CIPCLOSE\r\n", WAIT, AT_CIPCLOSE, AT_CIPSTART, AT_OK|AT_ERR, WAIT_1000, WAIT_100);	
			break;
		case AT_CIPSTART:
			memset(usart1.RxBuff, 0, sizeof(usart1.RxBuff));
			switch(network_flag.r.request_number){
				case CURR_WEATHER:
					sprintf((char *)usart1.RxBuff, "AT+CIPSTART=\"%s\",\"%s\",%s\r\n", API_TYPE, Weather_API_IP, API_PORT);
				break;
				case FUTURE_WEATHER:
					sprintf((char *)usart1.RxBuff, "AT+CIPSTART=\"%s\",\"%s\",%s\r\n", API_TYPE, Weather_API_IP, API_PORT);
				break;
				case EPIDEMIC:
					sprintf((char *)usart1.RxBuff, "AT+CIPSTART=\"%s\",\"%s\",%s\r\n", API_TYPE, Epidemic_API_IP, API_PORT);
				break;
				case VACCINE:
					sprintf((char *)usart1.RxBuff, "AT+CIPSTART=\"%s\",\"%s\",%s\r\n", API_TYPE, Vaccine_API_IP, API_PORT);
				break;
			}
			Esp12_RevampState((char *)usart1.RxBuff, WAIT, AT_CIPSTART, AT_CIPMODE, AT_OK, WAIT_10000, WAIT_100);	
			break;
		case AT_CIPMODE:
			Esp12_RevampState("AT+CIPMODE=1\r\n", WAIT, AT_CIPMODE, AT_CIPSEND, AT_OK, WAIT_1000, WAIT_100);
			break;
		case AT_CIPSEND:
			Esp12_RevampState("AT+CIPSEND\r\n", WAIT, AT_CIPSEND, HTTP_GET, AT_OK, WAIT_1000, WAIT_100);
			break;
		case HTTP_GET:
			usart2.cacheRxBuff = mymalloc(USART2_NETWORK_BUFF_MAX); 	//请求网络时，服务器返回的数据量非常庞大
			memset(usart2.cacheRxBuff, 0, USART2_NETWORK_BUFF_MAX);
			usart2.cacheLen = 0;
			usart2.RecLen = 0;
			network_flag.r.networkIsRequest = true;			//标记有请求计划
		
			switch(network_flag.r.request_number){
				case CURR_WEATHER:
					Usart2_SendString(Weather_Current_API_ADDR);
					esp12_state.parse = ParseCurrentWeather_Data;
					break;
				case FUTURE_WEATHER:
					Usart2_SendString(Weather_Future_API_ADDR);
					esp12_state.parse = ParseFutureWeather_Data;
					break;
//				case EPIDEMIC:
//					Usart2_SendString(Epidemic_API_ADDR);
//					esp12_state.parse = ParseEpidemic_Data;
//					break;
//				case VACCINE:
//					Usart2_SendString(Vaccine_API_ADDR);
//					esp12_state.parse = ParseVaccine_Data;
//					break;
			}		
			Esp12_RevampState(NULL, WAIT, HTTP_GET, HTTP_RESPONSE, AT_OK|AT_ERR, WAIT_1000, WAIT_2000);
			break;
		case HTTP_RESPONSE:
			if(usart2.RecFlag == true){
				usart2.RecFlag = false;
				
				if(esp12_state.parse() != false){	//如果解析失败，重新执行这个计划
					Esp12_SetRequestPlan(network_flag.r.request_number);printf("解析失败");
				}
				
				usart2.RecFlag = false;
				usart2.RecLen = 0;
				usart2.cacheLen = 0;
				memset(usart2.RxBuff, 0, sizeof(usart2.RxBuff));

				network_flag.r.networkIsRequest = false;	//标记未请求网络
				myfree(usart2.cacheRxBuff);					//释放内存
				usart2.cacheRxBuff = NULL;
				
				network_flag.r.requestIsFinish = true;		//标记本次请求完成
				
				Usart2_SendString("+++");
				
				Esp12_RevampState(NULL, WAIT, HTTP_RESPONSE, __IDLE, AT_OK|AT_ERR, WAIT_1000, WAIT_1000);
				printf("enter idle\r\n");
			}
			if(--esp12_state.timeout == 0){
				printf("request timeout\r\n");
				Esp12_RevampState(NULL, WAIT, HTTP_RESPONSE, __IDLE, AT_OK|AT_ERR, WAIT_1000, WAIT_100);
			}
			break;
	}
	
	Esp12_WaitResponse();	//等待应答
}
NETWORK_STA Esp12_GetNetWorkState(void)
{
	return (NETWORK_STA)network_flag.r.is_connect;
}
void Esp12_SetNetWorkState(NETWORK_STA sta)
{
	network_flag.r.is_connect = sta;
}


/*********************************************************************************************************
* 函 数 名 : Esp12_DownDataHandle
* 功能说明 : 处理下发数据
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 检测到断开连接会触发重连机制，检测到用户发来账号密码会重新配置sta
*********************************************************************************************************/ 
static unsigned char Esp12_DownDataHandle(void)
{
	unsigned char temp = 0;
	
	if(WifiUsart.RecFlag==true && (network_flag.r.is_connect==N_CONNECTED || network_flag.r.is_connect==N_CONFIGURED)){
		WifiUsart.RecFlag = false;
		WifiUsart.RecLen = 0;
		
		if(strstr((char *)WifiUsart.RxBuff, "DISCONNECT")){
			network_flag.r.is_connect = N_DISCONNECT;
			temp = DISCONNECT;
		}else if(strstr((char *)WifiUsart.RxBuff, "ssid")){
			if(Esp12_CheckFlashOrBuffer(ssid, pwsd, false) == true){
				printf("收到的信息包含ssid\r\n");
				temp = WAIT_CONF;
			}
		}

		memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	}
	
	return temp;
}
void Esp12_WaitResponse(void)
{
	if(esp12_state.state == WAIT){
		esp12_state.response = Esp12_MeasuringResponse(esp12_state.wish_ack);
		switch(esp12_state.response){
			case AT_OK:
				if(--esp12_state.interval == 0){	//间隔一定的时间后跳转到下一个状态
#if WIFI_DEGBUG
					printf("@@@%d->%d@@@\r\n", esp12_state.previous, esp12_state.next);
#endif
					esp12_state.err_count = TRY_COUNT;					//重设重发次数
					Esp12_JumpState(esp12_state.previous, esp12_state.next);	//跳转到下一个状态
				}	
				break;
			case AT_ERR:
				if(--esp12_state.interval == 0){	//间隔一定的时间后跳转到上一个状态	
#if WIFI_DEGBUG
					printf("###%d->%d###\r\n", esp12_state.previous, esp12_state.previous);
#endif
					Esp12_JumpState(esp12_state.previous, esp12_state.previous);	//跳转到上一个状态			
					if(--esp12_state.err_count == 0){								//error计数，超过一定次数会触发复位状态
						if(esp12_state.state>=AT_CIPCLOSE && esp12_state.state<=AT_CIPSEND){	
							//如果因为请求导致的复位，那么认定本次请求计划失败，将重新执行该计划
							Esp12_SetRequestPlan(network_flag.r.request_number);
							network_flag.r.requestIsFinish = true;		//标记本次请求完成
						}
						Esp12_RevampState("AT+RST\r\n", AT_RST, AT_ERR, AT_RST, AT_OK, WAIT_4000, WAIT_100);
						esp12_state.err_count = TRY_COUNT;	//重设重发次数
					}
				}
				break;
			case AT_IPOK:
#if WIFI_DEGBUG
				printf("已经获取到ip\r\n");
#endif
				Esp12_CheckFlashOrBuffer(ssid, pwsd, true);
				//拿到ip就可以进入下一个状态
				Esp12_RevampState(NULL, AT_CIPSNTPCFG, AT_IPOK, __IDLE, AT_OK, WAIT_1000, WAIT_100);
				break;
			case AT_IPERR:
#if WIFI_DEGBUG
				printf("未获取到ip\r\n");
#endif
				if((Esp12_CheckFlashOrBuffer(ssid, pwsd, true)==true) || (ssid[0]!=0 && pwsd[0]!=0)){	//如果已经存有账号密码，先尝试连接
					printf("--%s %s--\r\n", ssid, pwsd);
					Esp12_RevampState(NULL, AT_CWJAP, AT_CWJAP, AT_CWJAP, AT_OK, WAIT_1000, WAIT_100);
				}else{											//否则进入sta模式，等待用户输入
					printf("等待用户配置wifi信息\r\n");
					network_flag.r.is_connect = N_CONFIGURED;	//设置配网模式
					Esp12_RevampState(NULL, AT_CWMODE2, AT_IPERR, AT_CWMODE2, AT_OK, WAIT_1000, WAIT_100);
				}
				break;
			case AT_SNTPOK:
				Esp12_GetCurrentSNTP();
				Esp12_JumpState(esp12_state.previous, esp12_state.next);
				break;
			default:
				if(--esp12_state.timeout == 0){			//如果超过规定时间没有得到希望的应答会跳转到上一个状态
#if WIFI_DEGBUG
					printf("***timeout->%d***\r\n", esp12_state.previous);
#endif
					Esp12_JumpState(esp12_state.previous, esp12_state.previous);	//跳转到上一个状态
					if(--esp12_state.err_count == 0){		//超时计数，超过一定次数会触发复位状态
						if(esp12_state.state>=AT_CIPCLOSE && esp12_state.state<=AT_CIPSEND){	
							//如果因为请求导致的复位，那么认定本次请求计划失败，将重新执行该计划
							Esp12_SetRequestPlan(network_flag.r.request_number);
							network_flag.r.requestIsFinish = true;		//标记本次请求完成
						}
						Esp12_RevampState("AT+RST\r\n", AT_RST, AT_IPERR, AT_IPERR, AT_OK, WAIT_4000, WAIT_100);
						esp12_state.err_count = TRY_COUNT;	//重设重发次数
					}
				}
				break;
		}
	}
}
/*********************************************************************************************************
* 函 数 名 : CheckFlashOrBuffer
* 功能说明 : 检查内部flash或者串口接收缓冲区中是否存在账号密码
* 形    参 : ssid账号存储区，pwsd密码存储区，mode：true检测内部flash，false检查串口接收缓冲区
* 返 回 值 : true存在，false不存在
* 备    注 : 无
*********************************************************************************************************/ 
static bool Esp12_CheckFlashOrBuffer(unsigned char *ssid, unsigned char *pwsd, char mode)
{
	unsigned char cnt = 0;
	unsigned char *sp = NULL;
	
	if(poweron_link ==0)
	{
		poweron_link =1;
		return true;
	}
	memset(usart1.RxBuff, 0, sizeof(usart1.RxBuff));
	if(mode == true){
		W25Qxx_ReadDatas(usart1.RxBuff, WIFI_FLASH_ADDR, MESSAGE_MAX);
	}else{
		strcpy((char *)usart1.RxBuff, (char *)WifiUsart.RxBuff);
	}
	
	if(strstr((char *)usart1.RxBuff, "ssid") != NULL){
		sp = (unsigned char *)(strstr((char *)usart1.RxBuff, ":") + 1);
		if(mode == false){
			sp = (unsigned char *)(strstr((char *)sp, ":") + 1);
		}
		
		for(cnt=0; cnt<MESSAGE_MAX; cnt++){
			if(*sp == ' ')	break;
			*ssid++ = *sp++;
		}
		
		sp = (unsigned char *)(strstr((char *)sp, ":") + 1);
		for(cnt=0; cnt<MESSAGE_MAX; cnt++){
			if(*sp == 0xaa)	break;
			*pwsd++ = *sp++;
		}
	}else{
		return false;
	}
	
	return true;
}
/*********************************************************************************************************
* 函 数 名 : Esp12_JumpState
* 功能说明 : 状态跳转
* 形    参 : sta：当前状态，offset：跳转到什么状态
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
static void Esp12_JumpState(unsigned char sta, unsigned char offset)
{
	if(sta == AT_MQTTSUB)	esp12_state.state = FINISH;
	else					esp12_state.state = offset;
}
/*********************************************************************************************************
* 函 数 名 : Esp12_MeasuringResponse
* 功能说明 : 检测应答
* 形    参 : ack：希望的应答类型
* 返 回 值 : 非__NULL为有应答
* 备    注 : 无
*********************************************************************************************************/ 
static unsigned char Esp12_MeasuringResponse(unsigned char ack)
{
	unsigned char ret = __NULL;
	
	switch(ack){
		case AT_OK:
			if(strstr((char *)WifiUsart.RxBuff, "OK")){
				ret = AT_OK;
			}else if(strstr((char *)WifiUsart.RxBuff, "ERROR")){
				ret = AT_ERR;	
			}
			break;
		case AT_OK|AT_ERR:
			ret = AT_OK;
			break;
		case AT_IP:
			if(strstr((char *)WifiUsart.RxBuff, "0.0.0.0")){
				ret = AT_IPERR;
			}else if(strstr((char *)WifiUsart.RxBuff, "CIFSR:STAIP")){
				ret = AT_IPOK;
			}
			break;
		case AT_SNTPOK:
			if(strstr((char *)WifiUsart.RxBuff, "+CIPSNTPTIME")){
				if(!strstr((char *)WifiUsart.RxBuff, "1970")){
					ret = AT_SNTPOK;
				}
			}
			break;
	}
	
	return ret;
}
/*********************************************************************************************************
* 函 数 名 : Esp12_RevampState
* 功能说明 : 发送命令，并修改状态
* 形    参 : cmd：该状态要发送的命令，sta：发完这个命令后跳转到什么状态，pre：什么状态发起的命令,
			 nex：等待完成后下一个状态，to：超时时间，inte：和下一个状态的间隔时间
* 返 回 值 : 无
* 备    注 : 如果cmd为NULL，那么表示只跳转状态，不发指令
*********************************************************************************************************/ 
void Esp12_RevampState(char *cmd, unsigned char sta, unsigned char pre, unsigned char nex, unsigned char ack, unsigned short to, unsigned short inte)
{
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	WifiUsart.RecLen = 0;
	if(cmd != NULL){
		Usart2_SendString(cmd);	
	}
	esp12_state.state 		= sta;		//发完命令进入等待应答状态
	esp12_state.previous 	= pre;		//记录上一个状态
	esp12_state.next 		= nex;		//下一个要执行的状态
	esp12_state.wish_ack	= ack;		//希望的应答
	esp12_state.timeout 	= to;		//超时时间
	esp12_state.interval 	= inte;		//和下一个状态的间隔时间
	esp12_state.response 	= UNKNOWN;	//清除上一次收到的应答
}

/*********************************************************************************************************
* 函 数 名 : Esp12_Init
* 功能说明 : 初始化网络请求标志以及请求队列
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Esp12_Init(void)
{
#if NON_BLOCKING == 1	
	network_flag.r.is_request = false;			//标记无请求
	network_flag.r.networkIsRequest = false;	//标记未发起请求
	network_flag.r.requestIsFinish = true;		//标记已完成上一次的请求
	network_flag.r.is_connect = N_DISCONNECT;	//标记未联网
	network_flag.r.request_number = REQ_NULL;	//未获得请求编号
	QMsg.init(&ehead);							//初始化请求队列
#endif
	Usart2_Init(115200);
}
/*********************************************************************************************************
* 函 数 名 : Esp12_SetRequestPlan
* 功能说明 : 将请求计划加入队列中
* 形    参 : value：计划值
* 返 回 值 : false：队列已满；true：设置成功
* 备    注 : 返回值设定是为了和获取函数一致，成功应该返回false
*********************************************************************************************************/ 
unsigned char Esp12_SetRequestPlan(unsigned char value)
{
	if(QMsg.push(&ehead, value) == true){
		printf("The queue is full!\r\n");
		return false;
	}
	
	network_flag.r.is_request = true;
	
	return true;
}
/*********************************************************************************************************
* 函 数 名 : Esp12_GetRequestPlan
* 功能说明 : 从队列中获取网络请求计划
* 形    参 : 无
* 返 回 值 : false：队列为空，其他：计划值
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char Esp12_GetRequestPlan(void)
{
	unsigned char value;
	
	if(QMsg.pop(&ehead, &value) == true){
		printf("The queue is null!\r\n");
		return false;
	}
	
	return value;
}
#if NON_BLOCKING == 0
/*********************************************阻塞式请求*************************************************/
//注意：以下内容为阻塞编程，此写法并不稳定，请后来者自行完善，作者只提供大概流程方法
/*********************************************************************************************************
* 函 数 名 : Esp12_ConnectWifi
* 功能说明 : ESP12模块连接WiFi
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
void Esp12_ConnectWifi(void)
{
	unsigned char TryConnectTime = 0;
	
Restoration:
	printf("\r\nStart connecting to the server!\r\n");
	
	Usart2_SendString((u8 *)"+++"); Delay_Ms(50);					//退出透传	
	Usart2_SendString((u8 *)"+++"); Delay_Ms(50);
	
	while(!Esp12SendCmd("AT\r\n", "OK", 500));						//检测器件是否存在
	Delay_Ms(200);
	
	while(!Esp12SendCmd("AT+CWMODE=1\r\n", "OK", 500)); 	//设置为站点模式
	Delay_Ms(200);
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	
	/*调好后可以把指令回显关掉*/
	while(!Esp12SendCmd("ATE1\r\n", "OK", 500)); 			//开启指令回显
	Delay_Ms(200);
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	TryConnectTime = 0;
	if(Esp12SendCmd("AT+CIFSR\r\n", "0.0.0.0", 500))		//如果已经连接网络，将不再重新连接
	{
		Delay_Ms(200);
		memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
		memset(Usart1.RxBuff, 0, sizeof(Usart1.RxBuff));
		sprintf((char *)Usart1.RxBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PWSD);
		Usart2_SendString((unsigned char *)Usart1.RxBuff);						//连接目标AP				
		while(strstr((char *)WifiUsart.RxBuff, "OK") == NULL)
		{
			Delay_Ms(100);
			if(++TryConnectTime >= 100)
			{
				printf("\r\nWarning: Failed to connect to WIFI. WiFi module will be reset!\r\n");
				Esp12SendCmd("AT+RST\r\n", "OK", 100);
				for(TryConnectTime=0; TryConnectTime<10; TryConnectTime++)
					Delay_Ms(500);
				goto Restoration;
			}
		}
	}
	Delay_Ms(200);
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	
	while(!Esp12SendCmd("AT+CIPSNTPCFG=1,8\r\n", "OK", 500));	//设置SNT服务器
	Delay_Ms(200);
	memset(Usart1.RxBuff, 0, sizeof(Usart1.RxBuff));
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	Esp12_GetCurrentSNTP();
}
/*********************************************************************************************************
* 函 数 名 : BuildSSL_Connect
* 功能说明 : 构建SSL连接
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 433端口只能用SSL连接，当然本函数也可以构建80端口的连接
*********************************************************************************************************/ 
void BuildSSL_Connect(char *type, char *ip, char *port)
{
	unsigned char TryConnectTime = 0;

	Esp12_SendCmd("AT+CIPCLOSE\r\n", "OK", 200);			//关闭当前TCP、UDP连接
	Delay_Ms(500);
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	
	Esp12_SendCmd("AT+CIPCLOSE\r\n", "OK", 200);		//关闭当前TCP、UDP连接
	Delay_Ms(200);
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));

	memset(Usart1.RxBuff, 0, sizeof(Usart1.RxBuff));
	sprintf((char *)Usart1.RxBuff, "AT+CIPSTART=\"%s\",\"%s\",%s\r\n", type, ip, port); 
	while(!Esp12_SendCmd((char *)Usart1.RxBuff, "OK", 5000)){
		printf("API connection fail, Try to reconnect to %d!\r\n", TryConnectTime++);
        if (TryConnectTime >= 5){
			printf("Unable to connect to API, please check the network!!!\r\n");
            return;
        }
		Delay_Ms(500);
		memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	}
	Delay_Ms(200);	
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	
	Esp12_SendCmd("AT+CIPMODE=1\r\n", "OK", 500);				//进入透传模式
	Delay_Ms(200);
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	
	Esp12_SendCmd("AT+CIPSEND\r\n", "OK", 500);					//开始透传
	Delay_Ms(200);
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
}
/*********************************************************************************************************
* 函 数 名 : GetNcov_Api
* 功能说明 : 发起GET请求ncov api
* 形    参 : api_addr：api地址，parse_fun：处理回调函数
* 返 回 值 : 无
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char GetNcov_Api(char *api_addr, unsigned char (*parse_fun)(void))
{
	unsigned char TryConnectTime = 0;
	
	Usart2.cacheRxBuff = mymalloc(USART2_NETWORK_BUFF_MAX); 	//请求网络时，服务器返回的数据量非常庞大
	memset(Usart2.cacheRxBuff, 0, USART2_NETWORK_BUFF_MAX);
	network_flag.r.networkIsRequest = true;		//标记正在请求网络
	
	Usart2.cacheLen = 0;
	Usart2.RecLen = 0;
	 

 
    while(Usart2.RecFlag != true){				//等待收完数据
		Delay_Ms(100);
		if(++TryConnectTime >= 100){			//超过5秒还没有收到数据认为错误
			printf("request timeout\r\n");
			return 1;
		}
	}
   
    parse_fun();								//处理数据
    Usart2.RecFlag = false;
	Usart2.RecLen = 0;
	Usart2.cacheLen = 0;
	memset(Usart2.RxBuff, 0, sizeof(Usart2.RxBuff));

	network_flag.r.networkIsRequest = false;	//标记未请求网络
	myfree(Usart2.cacheRxBuff);					//释放内存
	Usart2.cacheRxBuff = NULL;
	
	Usart2_SendString("+++");
	Delay_Ms(100);
	Usart2_SendString("+++");
	Delay_Ms(100);
	
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	while(!Esp12_SendCmd("AT\r\n", "OK", 200)){
		Usart2_SendString("+++");
		Delay_Ms(100);
		Usart2_SendString("+++");
		Delay_Ms(100);
		Esp12_SendCmd("AT\r\n", "OK", 200);
	}
	
	memset(WifiUsart.RxBuff, 0, sizeof(WifiUsart.RxBuff));
	
    return 0;
}
/*********************************************************************************************************
* 函 数 名 : Esp12SendCmd
* 功能说明 : 向esp12模组发送指令并等待希望的应答
* 形    参 : cmd：发送的命令，ack：希望的应答，waittime：等待应答的时间（ms）
* 返 回 值 : 0：得到了期望的应答，1：没有得到期望的应答
* 备    注 : 阻塞式编程方法
*********************************************************************************************************/ 
unsigned char Esp12_SendCmd(char *cmd, char *ack, unsigned short waittime)
{
	WifiUsart.RecLen = 0;						//标示位、长度清零
	WifiUsart.RecFlag = false;
	waittime /= 10;                   			//超时时间
	Usart2_SendString(cmd);	//发送命令
    while(--waittime){							//等待串口接收完毕或超时退出
		if(WifiUsart.RecFlag == true){
			WifiUsart.RecLen = 0;
			WifiUsart.RecFlag = false;
			printf("%s\r\n", WifiUsart.RxBuff);
			if(strstr((char *)WifiUsart.RxBuff, (char *)ack)){
				return 1;
			}
		}
		Delay_Ms(10);
    }
	                  
   return 0; 
}
/**********************************************END*******************************************************/
#endif

/*********************************************************************************************************
* 函 数 名 : Crc16_Compute
* 功能说明 : CRC16校验
* 形    参 : ptr：待校验数据，len：校验长度
* 返 回 值 : 校验值
* 备    注 : x16+x15+x2+1
*********************************************************************************************************/  
static unsigned short Crc16_Compute(unsigned char *ptr, unsigned short len) 
{
	const static unsigned short crctalbeabs[16] = { 
		0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401, 
		0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400 
	};
	unsigned short crc = 0xffff; 
 
	while(len--){
		crc = crctalbeabs[(*ptr ^ crc) & 0xf] ^ (crc >> 4);	
		crc = crctalbeabs[((*ptr >> 4) ^ crc) & 0xf] ^ (crc >> 4);
		ptr++;
	} 
	
	return crc;
}
