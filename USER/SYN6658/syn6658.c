#include "syn6658.h"
#include "usart3.h"
#include "systick.h"
#include "QMsg.h"
#include "rtc.h"
#include "control.h"
#include "parse.h"
const char syn6658_i[] = "[i1]";	//0 - ��ʶ����ƴ��,1 - ʶ����ƴ��
const char syn6658_m[] = "[m3]";	//3 - ���� (Ů��)
									//51 - ��С�� (����)
									//52 - ��Сǿ (����)
									//53 - ������ (Ů��)
									//54 - ����Ѽ (Ч����)
									//55 - С���� (Ůͯ��																		
const char syn6658_s[] = "[s5]";	//����ֵ��0��10��˵��������ֵԽС������Խ��
const char syn6658_t[] = "[t8]";	//���ֵ��0��10�����ֵԽС����ƵֵԽ��
const char syn6658_v[] = "[v1]";  	//����ֵ��0��10�������ĵ��ڷ�ΧΪ��������Ƶ�豸֧�ֵ����ֵ
const char syn6658_d[] = "[[d]";	//�������ã��������������⣩�ָ�ΪĬ��ֵ

__report_plan report_plan;
CQueue qhead;

void TaskReportMsg_Handle(void)
{
	if(report_plan.is_report_plan != true){		//���û�в����ƻ�
		return;
	}
	
	if(report_plan.is_syn_free != true){		//�����ʱģ�鲻�ǿ���
		return;
	}
	
	report_plan.current_plan = Syn6658_GetReportPlan();	//�Ӷ����ж�ȡ�ƻ�
	if(report_plan.current_plan == REPORT_IDLE_VAL){	//����Ϊ��
		report_plan.is_report_plan = false;				//ȡ�������ƻ�	
		return;	
	}

	report_plan.is_syn_free = false;	//���ģ�����ڷ�æ
	char temp[64] = {0};
	
	switch(report_plan.current_plan){	//����Ҫ������������Ϻã����͸�ģ�鼴��
		case 0:	//����ʱ��ļƻ�
			sprintf(temp, "����ʱ�䣬%02d:%02d:%02d��%d/%02d/%02d", calendar.hour, calendar.min, calendar.sec, calendar.w_year, calendar.w_month, calendar.w_date);
			Syn6658_SyntheticVoiceCmd(temp, SYN_MODULE_REPORT);
			break;
		case 1:	//���������ļƻ�
			sprintf(temp, "����������С�꣬���ڵ��¶ȱȽ����ʣ����żǵô�ɡ��\r\n");
			Syn6658_SyntheticVoiceCmd(temp, SYN_MODULE_REPORT);
			break;
	}
}
//���ƻ���ӵ������ƻ�������
unsigned char Syn6658_SetReportPlan(unsigned char value)
{
	if(QMsg.push(&qhead, value) == true){
		printf("The queue is full!\r\n");
		return REPORT_IDLE_VAL;
	}
	
	report_plan.is_report_plan = true;			//����в����ƻ�
	
	return 0;
}

unsigned char Syn6658_GetReportPlan(void)
{
	unsigned char value;
	
	if(QMsg.pop(&qhead, &value) == true){
		report_plan.is_report_plan = false;		//ȡ�������ƻ�	
		printf("The queue is null!\r\n");
		return REPORT_IDLE_VAL;
	}
	
	return value;
}
/*********************************************************************************************************
* �� �� �� : Syn6658_SyntheticVoiceCmd
* ����˵�� : ��syn6658��������
* ��    �� : text����Ҫ�������ı�,��ģ���޷�����Ӣ�ģ� isInit��trueΪ��ʼ�������Ҫ�ȴ�ģ����Ӧ�� falseΪ�����ı������ȴ���Ӧ��
			 �������Ĳ�������¾ʹ���false��������������true��
* �� �� ֵ : ��
* ��    ע : ������������Ҫ�ȴ���һ�������ƻ���ɺ󣬲���������һ�β��ţ�������һ�β����ƻ��ᱻ�жϡ�
*********************************************************************************************************/ 
void Syn6658_SyntheticVoiceCmd(const char *text, unsigned char isInit)
{
	unsigned char headOfFrame[5];
	unsigned char length = strlen(text);
	unsigned char ErrorCounter = 0;

	headOfFrame[0] = 0xFD; 		//����֡ͷ
	headOfFrame[1] = 0x00; 		//���������ȸ�λ
	headOfFrame[2] = length + 2;//���������ȵ�λ
	headOfFrame[3] = 0x01; 		//�����֣��ϳɲ�������
	headOfFrame[4] = 0x01; 		//��������������ʽΪ GBK
	Usart3_SendPackage(headOfFrame, 5);				//����֡ͷ
	Usart3_SendPackage((unsigned char *)text, length);//��������

	while(1){
		if(usart3.RecFlag == true){
			usart3.RecFlag = false;
			usart3.RecLen = 0;
			if(usart3.RxBuff[0] == SYN_MODULE_RECEIVE_CORRECT_CMD){	//�յ���ȷ������֡�ش�
				break;
			}else{	//ʧ�������·���
				ErrorCounter++;
				if(ErrorCounter&0x8){
					printf("error1: %x\r\n", usart3.RxBuff[0]);
					break;
				}
				Usart3_SendPackage(headOfFrame, 5);					//����֡ͷ
				Usart3_SendPackage((unsigned char *)text, length);	//��������
			}
		}
	}
	if(isInit == true){
		ErrorCounter = 0;
		memset(usart3.RxBuff, 0, sizeof(usart3.RxBuff[0]));
		while(1){
			if(usart3.RecFlag == true){
				Usart_StatusClear(USART3);
				if(usart3.RxBuff[0] == SYN_MODULE_FREE){				//ģ�����
					break;
				}else{													//ʧ�������·���
					ErrorCounter++;
					if(ErrorCounter&0x8){
						printf("error2: %x\r\n", usart3.RxBuff[0]);
						break;
					}
					Usart3_SendPackage(headOfFrame, 5);					//����֡ͷ
					Usart3_SendPackage((unsigned char *)text, length);	//��������
				}
			}
		}
	}
}

void Syn6658_Init(void)
{
	Usart3_Init(115200);
	Delay_Ms(500);
	Delay_Ms(500);
	/*ÿ���ϵ綼Ҫ���³�ʼ������Ȼ�ͻ��Ĭ��*/
	Syn6658_SyntheticVoiceCmd(syn6658_s, SYN_MODULE_INIT);			//��ʼ�����٣����ò���ʱ�����������ȴ���ɣ�Ҳ����SYN_MODULE_INIT
	Syn6658_SyntheticVoiceCmd(syn6658_v, SYN_MODULE_INIT);			//��ʼ������
	//Syn6658_SyntheticVoiceCmd("��ӭʹ�ã���ӯ���ȵ�����", SYN_MODULE_REPORT);  	//������������������
	//Syn6658_SyntheticVoiceCmd("��ӭʹ�ã���ӯ���ȵ�����", SYN_MODULE_INIT);  		//����������������
	
	//��������Ϊ���������ŵ�֧�֣��������Ҫ�����
	report_plan.is_report_plan = false;	//����޲����ƻ�
	report_plan.is_syn_free = true;		//���ģ�����ڿ���
	report_plan.click_cont_index = 0;
	report_plan.current_plan = REPORT_IDLE_VAL;
	QMsg.init(&qhead);					//��ʼ����Ϣ�������
}
