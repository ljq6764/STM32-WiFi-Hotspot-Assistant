#include "at24c02.h"
#include "systick.h"
#include "iic.h"
/*********************************************************************************************************
* �� �� �� : At24C02_Init
* ����˵�� : At24C02�˿ڳ�ʼ��
* ��    �� : ��
* �� �� ֵ : ��
* ��    ע : ��׼��©������������㿪�ľͺ�
*********************************************************************************************************/ 
void At24C02_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��PB����ʱ��
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;		//��ʼ������
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;			//��©���ģʽ
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;			//2MHZ
	GPIO_Init(GPIOC, &GPIO_InitStruct);						//����GPIO_InitStruct��ʼ��
	
	GPIO_SetBits(GPIOC, GPIO_Pin_4 | GPIO_Pin_5);			//PC10����ߵ�ƽ������
}
/*********************************************************************************************************
* �� �� �� : At24CXX_WriteOneByte
* ����˵�� : ��At24C02д��һ���ֽ�
* ��    �� : writeAddr��д���ַ��dataToWrite����Ҫд�������
* �� �� ֵ : ��
* ��    ע : writeAddr��д���Ǹ���ַ��dataToWrite��дʲô����
*********************************************************************************************************/ 
void At24C02_WriteOneByte(unsigned char writeAddr,unsigned char dataToWrite)
{
	IIC_Start();						//��ֹ�ź�
	IIC_WriteByte(0xa0);				//д��������ַ+д��0xa0
	if(IIC_Wait_ACK() == ACK){			//�ȴ��Է�Ӧ��
		IIC_WriteByte(writeAddr);		//д�����ݵ�ַ��WriteAddr
		if(IIC_Wait_ACK() == ACK){		//�ȴ��Է�Ӧ��
			IIC_WriteByte(dataToWrite); //д�����ݣ�DataToWrite
			if(IIC_Wait_ACK() == ACK){	//�ȴ��Է�Ӧ��
				IIC_Stop();				//�����ź�
			}else goto error;
		}else goto error;
	}else goto error;

error:
	IIC_Stop();	
}
/*********************************************************************************************************
* �� �� �� : At24C02_ReadOneByte
* ����˵�� : ��At24C02��ȡһ���ֽ�
* ��    �� : readAddr�����ĸ���ַ��
* �� �� ֵ : ��ȡ�����ֽ�
* ��    ע : readAddr���������ȡ
*********************************************************************************************************/ 
unsigned char At24C02_ReadOneByte(unsigned char readAddr)
{
	unsigned char temp;
	
	IIC_Start();							//��ֹ�ź�
	IIC_WriteByte(0xa0);					//д��������ַ+д��0xa0
	if(IIC_Wait_ACK() == ACK){				//�ȴ��Է�Ӧ��
		IIC_WriteByte(readAddr);			//д�����ݵ�ַ��ReadAddr
		if(IIC_Wait_ACK() == ACK){			//�ȴ��Է�Ӧ��
			IIC_Start();					//��ֹ�ź�
			IIC_WriteByte(0xa1);			//д��������ַ+����0xa1
			if(IIC_Wait_ACK() == ACK){		//�ȴ��Է�Ӧ��
				temp = IIC_ReadByte(NACK);	//��ȡ���ݣ��Է��������ݣ�������Ӧ��Ӧ��
				IIC_Stop();					//�����ź�
				return temp;
			}else goto error;
		}else goto error;
	}else goto error;
	
error:
	IIC_Stop();
	return temp;
}
/*********************************************************************************************************
* �� �� �� : At24C02_WritePageByte
* ����˵�� : At24C02ҳд
* ��    �� : dataToWrite��д������ݵ�ָ�룻pageAddr������ҳ��ʼд��0~31����num��д����ֽ���
* �� �� ֵ : ��
* ��    ע : дʲô������ҳ��ʼд��д���ٸ�
*********************************************************************************************************/ 
void At24C02_WritePageByte(unsigned char *dataToWrite, unsigned char pageAddr, unsigned short num)
{
	unsigned short cnt, i;
					
	for(i=0; i<(num>>3); i++){	//Ҫд����ҳ����д��,����һҳ
		IIC_Start();				//��ֹ�ź�
		IIC_WriteByte(0xa0);		//д������ַ
		if(IIC_Wait_ACK() == ACK){					//�ȴ��Է�Ӧ��
			IIC_WriteByte(pageAddr);				//Ҫд�����ݵĵ�ַ
			if(IIC_Wait_ACK() == ACK){				//�ȴ��Է�Ӧ��	
				for(cnt=0; cnt<((num>>3)?8:(num&0x7)); cnt++){
					IIC_WriteByte(*dataToWrite++);	//Ҫд�������
					if(IIC_Wait_ACK() == NACK){		//�ȴ��Է�Ӧ��
						goto error;
					}
				}
				IIC_Stop();			//�����ź�
				num -= 8;			//ÿ��д��8���ֽ�
				pageAddr += 8;		//��ַ��ҳƫ��
				Delay_Ms(10);		//�ȴ�д����
			}else goto error;
		}else goto error;
	}
error:
	IIC_Stop();
}
/*********************************************************************************************************
* �� �� �� : At24C02_ReadPageByte
* ����˵�� : At24C02ҳ��
* ��    �� : pBuffer�����ݻ�������readAddr����ȡ�ĵ�ַ��num����ȡ���ֽ���
* �� �� ֵ : ��
* ��    ע : �浽����ȥ�������￪ʼ���������ٸ�
*********************************************************************************************************/ 
void At24C02_ReadPageByte(unsigned char *pBuffer, unsigned char readAddr, unsigned short num)
{
	unsigned short cnt;
	
	IIC_Start();							//��ֹ�ź�
	IIC_WriteByte(0xa0);					//д������ַ
	if(IIC_Wait_ACK() == ACK){				//�ȴ��Է�Ӧ��
		IIC_WriteByte(readAddr);			//Ҫ��ȡ�����ݵĵ�ַ
		if(IIC_Wait_ACK() == ACK){			//�ȴ��Է�Ӧ��
			IIC_Start();					//��ֹ�ź�
			IIC_WriteByte(0xa1);			//��������ַ
			if(IIC_Wait_ACK() == ACK){				//�ȴ��Է�Ӧ��
				for(cnt=0; cnt<num-1; cnt++){
					*pBuffer = IIC_ReadByte(ACK);	//��ȡһ���ֽڣ������ӻ�Ӧ��
					pBuffer++;	
				}
				*pBuffer = IIC_ReadByte(NACK);		//��ȡһ���ֽڣ�����Ӧ��
				IIC_Stop();							//�����ź�
			}else goto error;
		}else goto error;
	}else goto error;
error:
	IIC_Stop();	
}
