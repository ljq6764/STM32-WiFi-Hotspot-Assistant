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
	
	if(len%2 != 0){			//len需要偶数
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
	
	return (char)(-2);		//没有找到 
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
* 函 数 名 : Esp12_GetCurrentSNTP
* 功能说明 : 通过ESP12模块获取SNTP服务器时间
* 形    参 : 无
* 返 回 值 : 无
* 备    注 : 无
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
			
			time.week = myStrstr(weekTable0, pSplit, 8);	//星期
			
			while((pSplit=strtok(NULL, " ")) != NULL){
				switch(index++){
					case 0:		//月份 
						time.w_month = myStrstr(monthTable, pSplit, 12);
						break;
					case 1:		//日 
						time.w_date = myAtoi(pSplit, 2); 
						break;
					case 2:		//时间 
						time.hour = myAtoi(pSplit, 2);	//时间格式需要“00:00:00 ” 
						time.min = myAtoi(pSplit+3, 2);
						time.sec = myAtoi(pSplit+6, 2);
						break;
					case 3:		//年 
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
* 函 数 名 : ParseFutureWeather_Data
* 功能说明 : 解析今明两天的天气信息
* 形    参 : 无
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char ParseFutureWeather_Data(void)
{
	//此json约4kb字节，cjson解析需要的内存非常大,所以需要简单的裁剪掉一些不要的字段
	char *head, *p;
	cJSON *root, *today, *tomorrow;
	
	/*纠正原始json格式*/
	if((p = strstr((char *)usart2.cacheRxBuff, "}")) != NULL){	//跳过昨天的天气数据
		p++;					//走到'}'后面
		head = p;				//记录头部位置
		*p = '{';				//把原来的','号修改为'{'，作为新json的起止
	}else return 1;
	
	if((p = strstr(p, "}")) != NULL){	//跳过今天的天气数据
		p++;					//走到'}'后面，继续找下一个
		p = strstr(p, "}");		//跳过明天的天气数据
		if(p != NULL){
			p++;				//走到'}'后面
			*p = '}';			//把原来的','号修改为'}'，作为新json的终止
			p++;				//走到'}'后面
			memset(p, 0, 10);	//截断数据
		}else return 2;
	}else return 3;
	
	printf("-%s-\r\n", head);							//经过处理后的JSON原始数据
	
	root = cJSON_Parse(head);
    if (root != NULL){									//验证是否符合json格式
		printf("JSON format ok, start parse!!!\r\n");
			
		today = cJSON_GetObjectItem(root, "1");
		ctrl_dev.cache = (char *)mymalloc(CACHE_SIZE);
		memset(&weather_data, 0, sizeof(weather_data));
		if(today->type == cJSON_Object){	
			strcat(weather_data.todayWeather, cJSON_GetObjectItem(today, "day_weather")->valuestring);
			if(strcmp(weather_data.todayWeather, cJSON_GetObjectItem(today, "night_weather")->valuestring) != 0){
				strcat(weather_data.todayWeather, "转");
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
				strcat(weather_data.tomorrowWeather, "转");
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
        printf("JSON format error:%s\r\n", cJSON_GetErrorPtr()); //输出json格式错误信息
        return 6;
    }
	
	myfree(ctrl_dev.cache);
	cJSON_Delete(root);
	
	return 0;
}

/*********************************************************************************************************
* 函 数 名 : ParseCurrentWeather_Data
* 功能说明 : 解析当前的天气信息
* 形    参 : 无
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char ParseCurrentWeather_Data(void)
{
	char *p;
	cJSON *root, *data, *observe, *air, *tips;
	
	/*纠正原始json格式*/
	usart2.cacheRxBuff[usart2.cacheLen-1] = '\0';	//去掉末尾的‘)’
	p = strstr((char *)usart2.cacheRxBuff, "(");	//跳过前面多余的内容
	p++;
	printf("-%s-\r\n", p);							//经过处理后的JSON原始数据
	
	root = cJSON_Parse((const char*)p);
    if (root != NULL){								//验证是否符合json格式
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
        printf("JSON format error:%s\r\n", cJSON_GetErrorPtr()); //输出json格式错误信息
        return 1;
    }
	
	myfree(ctrl_dev.cache);
	cJSON_Delete(root);
	
	return 0;
}
/*********************************************************************************************************
* 函 数 名 : ParseTime_Data
* 功能说明 : 解析时间数据
* 形    参 : 无
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
*********************************************************************************************************/ 
unsigned char ParseTime_Data(void)
{
	time_t t;
    struct tm *time;
	cJSON *root;
	char *p, *head;
	
	/*纠正原始json格式*/
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
	
	printf("-%s-\r\n", head);	//修正后的json数据
	
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
        printf("JSON format error:%s\r\n", cJSON_GetErrorPtr()); //输出json格式错误信息
        return 3;
    }
	cJSON_Delete(root);
	
	return 0;
}

/*********************************************************************************************************
* 函 数 名 : ParseEpidemic_Data
* 功能说明 : 解析疫情数据
* 形    参 : 无
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
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

                printf("------------国内疫情-------------\r\n");
                printf("现存确诊:   %5ld, 较昨日:%3ld\r\n", dataChina.currentConfirmedCount, dataChina.currentConfirmedIncr);
                printf("累计确诊:   %5ld, 较昨日:%3ld\r\n", dataChina.confirmedCount, dataChina.confirmedIncr);
				printf("境外输入:   %5ld, 较昨日:%3ld\r\n", dataChina.suspectedCount, dataChina.suspectedIncr);
                printf("累计治愈:   %5ld, 较昨日:%3ld\r\n", dataChina.curedCount, dataChina.curedIncr);
                printf("累计死亡:   %5ld, 较昨日:%3ld\r\n", dataChina.deadCount, dataChina.deadIncr);
                printf("现存无症状: %5ld, 较昨日:%3ld\r\n\r\n", dataChina.seriousCount, dataChina.seriousIncr);

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
                    printf("------------海外疫情-------------\r\n");
                    printf("现存确诊: %8ld, 较昨日:%5ld\r\n", dataGlobal.currentConfirmedCount, dataGlobal.currentConfirmedIncr);
                    printf("累计确诊: %8ld, 较昨日:%5ld\r\n", dataGlobal.confirmedCount, dataGlobal.confirmedIncr);
                    printf("累计死亡: %8ld, 较昨日:%5ld\r\n", dataGlobal.deadCount, dataGlobal.deadIncr);
                    printf("累计治愈: %8ld, 较昨日:%5ld\r\n\r\n", dataGlobal.curedCount, dataGlobal.curedIncr);

                }else return 1;
                
                /* 毫秒级时间戳转字符串 */
                updateTime = (time_t )(cJSON_GetObjectItem(result, "updateTime")->valuedouble / 1000);
                updateTime += 8 * 60 * 60; /* UTC8校正 */
                time = localtime(&updateTime);
                /* 格式化时间 */
                strftime(dataChina.updateTime, 20, "%m-%d %H:%M", time);
                printf("更新于:%s\r\n", dataChina.updateTime);/* 06-24 11:21 */
            }else return 1;
        }else return 1;
    }else{
        printf("JSON format error:%s\r\n", cJSON_GetErrorPtr()); //输出json格式错误信息
        return 1;
    }
	
	myfree(ctrl_dev.cache);
    cJSON_Delete(root);

    return ret;
}
/*********************************************************************************************************
* 函 数 名 : ParseVaccine_Data
* 功能说明 : 解析疫苗数据
* 形    参 : 无
* 返 回 值 : 0：成功；其他：错误代码
* 备    注 : 无
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
                    china_obj = cJSON_GetObjectItem(vaccine_obj, "中国");
                    if(china_obj->type == cJSON_Object)
                    {
                        dataChina.total_vaccinations = cJSON_GetObjectItem(china_obj, "total_vaccinations")->valuedouble;
                        dataChina.new_vaccinations = cJSON_GetObjectItem(china_obj, "new_vaccinations")->valuedouble;
                        dataChina.total_vaccinations_per_hundred = cJSON_GetObjectItem(china_obj, "total_vaccinations_per_hundred")->valuedouble;			
                        printf("total: %0.1f, new: %0.1f, per_hundred: %.2f\r\n", \
                        dataChina.total_vaccinations, dataChina.new_vaccinations, dataChina.total_vaccinations_per_hundred);
                    }else return 1;
					
                    global_obj = cJSON_GetObjectItem(vaccine_obj, "全球");
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
        printf("JSON format error: %s\r\n", cJSON_GetErrorPtr()); //输出json格式错误信息
        return 1;
    }
	
	myfree(ctrl_dev.cache);
    cJSON_Delete(root);

    return 0;
}
