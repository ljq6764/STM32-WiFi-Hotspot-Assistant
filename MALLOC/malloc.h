#ifndef __MALLOC_H
#define __MALLOC_H

#include "stm32f10x.h"

#ifndef NULL
#define NULL 0
#endif

//�ڴ�����趨.̫С������������ݽ���ʧ��
#define MEM_BLOCK_SIZE          32                              //�ڴ���СΪ32�ֽ�
#define MEM_MAX_SIZE            10*1024                         //�������ڴ� 50K
#define MEM_ALLOC_TABLE_SIZE    MEM_MAX_SIZE/MEM_BLOCK_SIZE     //�ڴ���С


//�ڴ���������
struct _m_mallco_dev
{
    void (*init)(void);             	//��ʼ��
    unsigned char(*perused)(void);      //�ڴ�ʹ����
    unsigned char  *membase;            //�ڴ��
    unsigned short *memmap;             //�ڴ����״̬��
    unsigned char  memrdy;              //�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev; //��mallco.c���涨��

void mymemset(void *s, unsigned char c, unsigned int count);//�����ڴ�
void mymemcpy(void *des, void *src, unsigned int n); 		//�����ڴ�
void Mem_init(void);                     					//�ڴ�����ʼ������(��/�ڲ�����)
unsigned int mem_malloc(unsigned int size);               	//�ڴ����(�ڲ�����)
unsigned char mem_free(unsigned int offset);                //�ڴ��ͷ�(�ڲ�����)
unsigned char mem_perused(void);                   			//���ڴ�ʹ����(��/�ڲ�����)
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(void *ptr);                 		 //�ڴ��ͷ�(�ⲿ����)
void *mymalloc(unsigned int size);               //�ڴ����(�ⲿ����)
void *myrealloc(void *ptr, unsigned int size);   //���·����ڴ�(�ⲿ����)

#endif













