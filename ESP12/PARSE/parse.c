#include "parse.h"
#include "control.h"
#include "rtc.h"
#include "usart2.h"
#include "cJSON.h"
#include <time.h>
#include "malloc.h"

__ncov_data dataChina = {0};
__ncov_data dataGlobal = {0};
__weather_data weather_data; 

char myStrstr(char const **dst, char const *str, char len)
{ 
	char i = 0;
	
	if(len%2 != 0){			//len��Ҫż��
		return (char)(-1);
	}
	
	for(i=0; i<len/2; i++){
		if(strstr(dst[i], str)){
			return i;
		}
		if(strstr(dst[len-1-i], str)){
			return  (len-1-i);
		}
	}
	
	return (char)(-2);		//û���ҵ� 
}

short myAtoi(char const *dts, char len)
{
	char i;
	short num = 0;
	
	for(i=0; i<len&&dts[i]; i++){
		if(dts[i]>='0' && dts[i]<='9'){
			num *= 10;
			num += dts[i] - 48;
		}
	}
	
	return num;
}
/*********************************************************************************************************
* �� �� �� : Esp12_GetCurrentSNTP
* ����˵�� : ͨ��ESP12ģ���ȡSNTP������ʱ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void Esp12_GetCurrentSNTP(void)
{
	char *pStr;
	char index = 0;
	_calendar_obj time = {0};

	if((pStr=strstr((char *)usart2.RxBuff, "TIME:")) != NULL){
		
		if((pStr = strstr(pStr, ":")) != NULL){
			pStr++;
			char *pSplit = strtok(pStr, " ");
			
			time.week = myStrstr(weekTable0, pSplit, 8);	//����
			
			while((pSplit=strtok(NULL, " ")) != NULL){
				switch(index++){
					case 0:		//�·� 
						time.w_month = myStrstr(monthTable, pSplit, 12);
						break;
					case 1:		//�� 
						time.w_date = myAtoi(pSplit, 2); 
						break;
					case 2:		//ʱ�� 
						time.hour = myAtoi(pSplit, 2);	//ʱ���ʽ��Ҫ��00:00:00 �� 
						time.min = myAtoi(pSplit+3, 2);
						time.sec = myAtoi(pSplit+6, 2);
						break;
					case 3:		//�� 
						time.w_year = myAtoi(pSplit, 4); 
						break;
					default:
						break;
				}
			}
		}
	}
	printf("%d-%d-%d-%d %d:%d:%d\r\n", time.w_year, time.w_month+1, time.w_date, time.week+1,
									time.hour, time.min, time.sec);
	if(time.w_year == 0){
		return;
	}
	
	RTC_SetCurrentDateTime(time.w_year, time.w_month+1, time.w_date, time.hour, time.min, time.sec);
}
/*********************************************************************************************************
* �� �� �� : ParseFutureWeather_Data
* ����˵�� : �������������������Ϣ
* ��    �� : ��
* �� �� ֵ : 0���ɹ����������������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char ParseFutureWeather_Data(void)
{
	//��jsonԼ4kb�ֽڣ�cjson������Ҫ���ڴ�ǳ���,������Ҫ�򵥵Ĳü���һЩ��Ҫ���ֶ�
	char *head, *p;
	cJSON *root, *today, *tomorrow;
	
	/*����ԭʼjson��ʽ*/
	if((p = strstr((char *)usart2.cacheRxBuff, "}")) != NULL){	//�����������������
		p++;					//�ߵ�'}'����
		head = p;				//��¼ͷ��λ��
		*p = '{';				//��ԭ����','���޸�Ϊ'{'����Ϊ��json����ֹ
	}else return 1;
	
	if((p = strstr(p, "}")) != NULL){	//�����������������
		p++;					//�ߵ�'}'���棬��������һ��
		p = strstr(p, "}");		//�����������������
		if(p != NULL){
			p++;				//�ߵ�'}'����
			*p = '}';			//��ԭ����','���޸�Ϊ'}'����Ϊ��json����ֹ
			p++;				//�ߵ�'}'����
			memset(p, 0, 10);	//�ض�����
		}else return 2;
	}else return 3;
	
	printf("-%s-\r\n", head);							//����������JSONԭʼ����
	
	root = cJSON_Parse(head);
    if (root != NULL){									//��֤�Ƿ����json��ʽ
		printf("JSON format ok, start parse!!!\r\n");
			
		today = cJSON_GetObjectItem(root, "1");
		ctrl_dev.cache = (char *)mymalloc(CACHE_SIZE);
		memset(&weather_data, 0, sizeof(weather_data));
		if(today->type == cJSON_Object){	
			strcat(weather_data.todayWeather, cJSON_GetObjectItem(today, "day_weather")->valuestring);
			if(strcmp(weather_data.todayWeather, cJSON_GetObjectItem(today, "night_weather")->valuestring) != 0){
				strcat(weather_data.todayWeather, "ת");
				strcat(weather_data.todayWeather, cJSON_GetObjectItem(today, "night_weather")->valuestring);
			}
		
			strcat(weather_data.todayDegree, cJSON_GetObjectItem(today, "max_degree")->valuestring);
			strcat(weather_data.todayDegree, "/");
			strcat(weather_data.todayDegree, cJSON_GetObjectItem(today, "min_degree")->valuestring);
			strcat(weather_data.todayDegree, "C");
			
			printf("day_weather: %s-\r\n", weather_data.todayWeather);
			printf("max_degree: %s-\r\n", weather_data.todayDegree);
			}else return 4;
		cJSON_Delete(today);
		
		tomorrow = cJSON_GetObjectItem(root, "2");
		if(today->type == cJSON_Object){	
			strcat(weather_data.tomorrowWeather, cJSON_GetObjectItem(tomorrow, "day_weather")->valuestring);
			if(strcmp(weather_data.tomorrowWeather, cJSON_GetObjectItem(tomorrow, "night_weather")->valuestring) != 0){
				strcat(weather_data.tomorrowWeather, "ת");
				strcat(weather_data.tomorrowWeather, cJSON_GetObjectItem(tomorrow, "night_weather")->valuestring);
			}

			strcat(weather_data.tomorrowDegree, cJSON_GetObjectItem(tomorrow, "max_degree")->valuestring);
			strcat(weather_data.tomorrowDegree, "/");
			strcat(weather_data.tomorrowDegree, cJSON_GetObjectItem(tomorrow, "min_degree")->valuestring);
			strcat(weather_data.tomorrowDegree, "C");
			
			printf("day_weather: %s-\r\n", weather_data.tomorrowWeather);
			printf("max_degree: %s-\r\n", weather_data.tomorrowDegree);	
		}else return 5;
		cJSON_Delete(tomorrow);
	}else{
        printf("JSON format error:%s\r\n", cJSON_GetErrorPtr()); //���json��ʽ������Ϣ
        return 6;
    }
	
	myfree(ctrl_dev.cache);
	cJSON_Delete(root);
	
	return 0;
}

/*********************************************************************************************************
* �� �� �� : ParseCurrentWeather_Data
* ����˵�� : ������ǰ��������Ϣ
* ��    �� : ��
* �� �� ֵ : 0���ɹ����������������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char ParseCurrentWeather_Data(void)
{
	char *p;
	cJSON *root, *data, *observe, *air, *tips;
	
	/*����ԭʼjson��ʽ*/
	usart2.cacheRxBuff[usart2.cacheLen-1] = '\0';	//ȥ��ĩβ�ġ�)��
	p = strstr((char *)usart2.cacheRxBuff, "(");	//����ǰ����������
	p++;
	printf("-%s-\r\n", p);							//����������JSONԭʼ����
	
	root = cJSON_Parse((const char*)p);
    if (root != NULL){								//��֤�Ƿ����json��ʽ
		printf("JSON format ok, start parse!!!\r\n");
		data = cJSON_GetObjectItem(root, "data");	
		ctrl_dev.cache = (char *)mymalloc(CACHE_SIZE);
        if(data->type == cJSON_Object){		
			air = cJSON_GetObjectItem(data, "air");
			if(air->type == cJSON_Object){
				weather_data.currentAir = cJSON_GetObjectItem(air, "aqi")->valueint;
				strcpy(weather_data.currentAirName, cJSON_GetObjectItem(air, "aqi_name")->valuestring);
				
				printf("air: %d\r\n", weather_data.currentAir);
				printf("aqi_name: %s\r\n", weather_data.currentAirName);
			}
			cJSON_Delete(air);
			
			tips = cJSON_GetObjectItem(data, "tips");
			if(tips->type == cJSON_Object){
				cJSON *observe = cJSON_GetObjectItem(tips, "observe");
				if(observe->type == cJSON_Object){
					strcpy(weather_data.observe1, cJSON_GetObjectItem(observe, "0")->valuestring);
					strcpy(weather_data.observe2, cJSON_GetObjectItem(observe, "1")->valuestring);
					printf("observe1: %s\r\n", weather_data.observe1);
					printf("observe2: %s\r\n", weather_data.observe2);
				}
				cJSON_Delete(observe);
			}
			cJSON_Delete(tips);
			
			observe = cJSON_GetObjectItem(data, "observe");
			if(observe->type == cJSON_Object){
				strcpy(weather_data.currentDegree, cJSON_GetObjectItem(observe, "degree")->valuestring);
				strcpy(weather_data.currentHumidity, cJSON_GetObjectItem(observe, "humidity")->valuestring);
				strcpy(weather_data.updateTime, cJSON_GetObjectItem(observe, "update_time")->valuestring);
				strcpy(weather_data.currentWeather, cJSON_GetObjectItem(observe, "weather")->valuestring);
				strcpy(weather_data.windDirection, cJSON_GetObjectItem(observe, "wind_direction")->valuestring);
				strcpy(weather_data.windPower, cJSON_GetObjectItem(observe, "wind_power")->valuestring);
				
				printf("degree: %s\r\n", weather_data.currentDegree);
				printf("humidity: %s\r\n", weather_data.currentHumidity);
				printf("update_time: %s\r\n", weather_data.updateTime);
				printf("weather: %s\r\n", weather_data.currentWeather);
				printf("wind_direction: %s\r\n", weather_data.windDirection);
				printf("wind_power: %s\r\n", weather_data.windPower);
			
				memset(ctrl_dev.cache, 0, CACHE_SIZE);
				char wind_dir = weather_data.windDirection[0]-48-1;
				char wind_power = weather_data.windPower[0]-48-1;
				if(wind_dir >= 8){
					wind_dir = 7;
				}
				if(wind_power >= 10){
					wind_power = 9;
				}
			}
			cJSON_Delete(observe);
		}
		cJSON_Delete(data);
	}else{
        printf("JSON format error:%s\r\n", cJSON_GetErrorPtr()); //���json��ʽ������Ϣ
        return 1;
    }
	
	myfree(ctrl_dev.cache);
	cJSON_Delete(root);
	
	return 0;
}
/*********************************************************************************************************
* �� �� �� : ParseTime_Data
* ����˵�� : ����ʱ������
* ��    �� : ��
* �� �� ֵ : 0���ɹ����������������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char ParseTime_Data(void)
{
	time_t t;
    struct tm *time;
	cJSON *root;
	char *p, *head;
	
	/*����ԭʼjson��ʽ*/
	p = strstr((char *)usart2.cacheRxBuff, "{");
	head = p;
	if(p == NULL){
		printf("strstr error2\r\n");
		return 1;
	}
	
	p = strstr((char *)p, "}");
	if(p == NULL){
		printf("strstr error1\r\n");
		return 2;
	}
	p += 1;
	*p = '\0';
	
	printf("-%s-\r\n", head);	//�������json����
	
	root = cJSON_Parse(head);
    if (root != NULL){
        printf("JSON format ok, start parse!!!\r\n");
		t = (time_t )cJSON_GetObjectItem(root, "t")->valueint;	//ms
		printf("%d\r\n", t);
		time = localtime(&t);
		printf("%d/%02d/%02d %02d:%02d:%02d", 
			time->tm_year+1900, time->tm_mon+1, time->tm_mday, time->tm_hour+8, time->tm_min, time->tm_sec);
		RTC_SetCurrentDateTime(time->tm_year+1900, time->tm_mon+1, time->tm_mday, time->tm_hour+8, time->tm_min, time->tm_sec);
	}else{
        printf("JSON format error:%s\r\n", cJSON_GetErrorPtr()); //���json��ʽ������Ϣ
        return 3;
    }
	cJSON_Delete(root);
	
	return 0;
}

/*********************************************************************************************************
* �� �� �� : ParseEpidemic_Data
* ����˵�� : ������������
* ��    �� : ��
* �� �� ֵ : 0���ɹ����������������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char ParseEpidemic_Data(void)
{
    int ret = 0;
    cJSON *root, *result_arr;
    cJSON *result, *global;
    time_t updateTime;
    struct tm *time;

	printf("-%s-\r\n", usart2.cacheRxBuff);
    root = cJSON_Parse((const char*)usart2.cacheRxBuff);
    if (root != NULL){
        printf("JSON format ok, start parse!!!\r\n");
        result_arr = cJSON_GetObjectItem(root, "results");
        if(result_arr->type == cJSON_Array)
        {
            result = cJSON_GetArrayItem(result_arr, 0);
            if(result->type == cJSON_Object)
            {
				ctrl_dev.cache = (char *)mymalloc(CACHE_SIZE/2);
				memset(&dataChina, 0, sizeof(dataChina));
                /* china data parse */
                dataChina.currentConfirmedCount = cJSON_GetObjectItem(result, "currentConfirmedCount")->valueint;
                dataChina.currentConfirmedIncr = cJSON_GetObjectItem(result, "currentConfirmedIncr")->valueint;
                dataChina.confirmedCount = cJSON_GetObjectItem(result, "confirmedCount")->valueint;
                dataChina.confirmedIncr = cJSON_GetObjectItem(result, "confirmedIncr")->valueint;
				dataChina.suspectedCount = cJSON_GetObjectItem(result, "suspectedCount")->valueint;
				dataChina.suspectedIncr = cJSON_GetObjectItem(result, "suspectedIncr")->valueint;
                dataChina.curedCount = cJSON_GetObjectItem(result, "curedCount")->valueint;
                dataChina.curedIncr = cJSON_GetObjectItem(result, "curedIncr")->valueint;
                dataChina.deadCount = cJSON_GetObjectItem(result, "deadCount")->valueint;
                dataChina.deadIncr = cJSON_GetObjectItem(result, "deadIncr")->valueint;
				dataChina.seriousCount = cJSON_GetObjectItem(result, "seriousCount")->valueint;
				dataChina.seriousIncr = cJSON_GetObjectItem(result, "seriousIncr")->valueint;

                printf("------------��������-------------\r\n");
                printf("�ִ�ȷ��:   %5ld, ������:%3ld\r\n", dataChina.currentConfirmedCount, dataChina.currentConfirmedIncr);
                printf("�ۼ�ȷ��:   %5ld, ������:%3ld\r\n", dataChina.confirmedCount, dataChina.confirmedIncr);
				printf("��������:   %5ld, ������:%3ld\r\n", dataChina.suspectedCount, dataChina.suspectedIncr);
                printf("�ۼ�����:   %5ld, ������:%3ld\r\n", dataChina.curedCount, dataChina.curedIncr);
                printf("�ۼ�����:   %5ld, ������:%3ld\r\n", dataChina.deadCount, dataChina.deadIncr);
                printf("�ִ���֢״: %5ld, ������:%3ld\r\n\r\n", dataChina.seriousCount, dataChina.seriousIncr);

				memset(&dataGlobal, 0, sizeof(dataGlobal));
                global = cJSON_GetObjectItem(result, "globalStatistics");
                if(global->type == cJSON_Object)
                {
                    dataGlobal.currentConfirmedCount = cJSON_GetObjectItem(global, "currentConfirmedCount")->valueint;
                    dataGlobal.currentConfirmedIncr = cJSON_GetObjectItem(global, "currentConfirmedIncr")->valueint;
                    dataGlobal.confirmedCount = cJSON_GetObjectItem(global, "confirmedCount")->valueint;
                    dataGlobal.confirmedIncr = cJSON_GetObjectItem(global, "confirmedIncr")->valueint;
                    dataGlobal.curedCount = cJSON_GetObjectItem(global, "curedCount")->valueint;
                    dataGlobal.curedIncr = cJSON_GetObjectItem(global, "curedIncr")->valueint;
                    dataGlobal.deadCount = cJSON_GetObjectItem(global, "deadCount")->valueint;
                    dataGlobal.deadIncr = cJSON_GetObjectItem(global, "deadIncr")->valueint;
                    printf("\r\n**********global ncov data**********\r\n");
                    printf("------------��������-------------\r\n");
                    printf("�ִ�ȷ��: %8ld, ������:%5ld\r\n", dataGlobal.currentConfirmedCount, dataGlobal.currentConfirmedIncr);
                    printf("�ۼ�ȷ��: %8ld, ������:%5ld\r\n", dataGlobal.confirmedCount, dataGlobal.confirmedIncr);
                    printf("�ۼ�����: %8ld, ������:%5ld\r\n", dataGlobal.deadCount, dataGlobal.deadIncr);
                    printf("�ۼ�����: %8ld, ������:%5ld\r\n\r\n", dataGlobal.curedCount, dataGlobal.curedIncr);

                }else return 1;
                
                /* ���뼶ʱ���ת�ַ��� */
                updateTime = (time_t )(cJSON_GetObjectItem(result, "updateTime")->valuedouble / 1000);
                updateTime += 8 * 60 * 60; /* UTC8У�� */
                time = localtime(&updateTime);
                /* ��ʽ��ʱ�� */
                strftime(dataChina.updateTime, 20, "%m-%d %H:%M", time);
                printf("������:%s\r\n", dataChina.updateTime);/* 06-24 11:21 */
            }else return 1;
        }else return 1;
    }else{
        printf("JSON format error:%s\r\n", cJSON_GetErrorPtr()); //���json��ʽ������Ϣ
        return 1;
    }
	
	myfree(ctrl_dev.cache);
    cJSON_Delete(root);

    return ret;
}
/*********************************************************************************************************
* �� �� �� : ParseVaccine_Data
* ����˵�� : ������������
* ��    �� : ��
* �� �� ֵ : 0���ɹ����������������
* ��    ע : ��
*********************************************************************************************************/ 
unsigned char ParseVaccine_Data(void)
{
    int ret_value = 1;
    cJSON *root, *data_obj, *vaccine_obj;
    cJSON *china_obj, *global_obj;
    
    root = cJSON_Parse((const char*)usart2.cacheRxBuff);
    if (root != NULL)
    {
        printf("JSON format ok, start parse!!!\r\n");
        ret_value = cJSON_GetObjectItem(root, "ret")->valueint;
        if(ret_value == 0)
        {
            data_obj = cJSON_GetObjectItem(root, "data");
            if(data_obj->type == cJSON_Object)
            {
                vaccine_obj = cJSON_GetObjectItem(data_obj, "VaccineTopData");
                if(vaccine_obj->type == cJSON_Object)
                {
					ctrl_dev.cache = (char *)mymalloc(CACHE_SIZE/2);
                    //Keil must set "Encode in UTF-8 without signature"                    
                    china_obj = cJSON_GetObjectItem(vaccine_obj, "�й�");
                    if(china_obj->type == cJSON_Object)
                    {
                        dataChina.total_vaccinations = cJSON_GetObjectItem(china_obj, "total_vaccinations")->valuedouble;
                        dataChina.new_vaccinations = cJSON_GetObjectItem(china_obj, "new_vaccinations")->valuedouble;
                        dataChina.total_vaccinations_per_hundred = cJSON_GetObjectItem(china_obj, "total_vaccinations_per_hundred")->valuedouble;			
                        printf("total: %0.1f, new: %0.1f, per_hundred: %.2f\r\n", \
                        dataChina.total_vaccinations, dataChina.new_vaccinations, dataChina.total_vaccinations_per_hundred);
                    }else return 1;
					
                    global_obj = cJSON_GetObjectItem(vaccine_obj, "ȫ��");
                    if(global_obj->type == cJSON_Object)
                    {   
                        dataGlobal.total_vaccinations = cJSON_GetObjectItem(global_obj, "total_vaccinations")->valuedouble;
                        dataGlobal.new_vaccinations = cJSON_GetObjectItem(global_obj, "new_vaccinations")->valuedouble;
                        dataGlobal.total_vaccinations_per_hundred = cJSON_GetObjectItem(global_obj, "total_vaccinations_per_hundred")->valuedouble;                  
                        printf("total: %0.1f, new: %0.1f, per_hundred: %.2f\n", \
                        dataGlobal.total_vaccinations, dataGlobal.new_vaccinations, dataGlobal.total_vaccinations_per_hundred);
                    } else return 1;
                } else return 1;
            } else return 1;
        }else return 1;
    }else{
        printf("JSON format error: %s\r\n", cJSON_GetErrorPtr()); //���json��ʽ������Ϣ
        return 1;
    }
	
	myfree(ctrl_dev.cache);
    cJSON_Delete(root);

    return 0;
}
