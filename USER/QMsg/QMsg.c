#include "QMsg.h"

static bool InitCQueue(CQueue *pQueue);          			//��ʼ������
static bool Push(CQueue *pQueue, unsigned char val);		//��ӣ��Ӷ�β(rear)��
static bool Pop(CQueue *pQueue, unsigned char *rtval);		//���ӣ��Ӷ���(front)��
static int GetCQueueLen(CQueue *pQueue);         			//��ȡ���г���
static void ShowQueue(CQueue *pQueue);          		 	//�����������Ԫ��
static bool IsEmpty(CQueue *pQueue);             			//����Ϊ���򷵻�true
static bool IsFull(CQueue *pQueue);             			//�������򷵻�false
static bool GetFront(CQueue *pQueue, unsigned char *rtval);	//��ȡ����Ԫ��

//���п���������
__CQueue QMsg = {
	.init = InitCQueue,
	.push = Push,
	.pop = Pop,
	.get_length = GetCQueueLen,
	.show = ShowQueue,
	.is_empty = IsEmpty,
	.is_full = IsFull,
	.get_front = GetFront,
};

//��ʼ������
bool InitCQueue(CQueue *pQueue)             
{
    if(NULL == pQueue){
        return true;
	}
        
    pQueue->front = 0;
    pQueue->rear = 0;
    
    return false;
}

//��ӣ��Ӷ�β(rear)��
bool Push(CQueue *pQueue, unsigned char val)          
{
    //���֮ǰӦ�����ж϶����Ƿ��Ѿ�����
    if(IsFull(pQueue)){
        return true;
    }

    pQueue->elem[pQueue->rear] = val;               //��Ԫ�ط�����У�����Ҳ˵��front�ǿ�ȡ��
    pQueue->rear = (pQueue->rear+ 1) % MAXSIZE;    	//���¶�βλ��
    
    return false;
}

//���ӣ��Ӷ���(front)��
bool Pop(CQueue *pQueue, unsigned char *rtval)        
{
    //����֮ǰӦ�����ж϶����Ƿ��ǿյ�
    if(IsEmpty(pQueue)){
        return true;
    }

    *rtval = pQueue->elem[pQueue->front];           //ȡ����ͷԪ��
    pQueue->front = (pQueue->front+ 1) % MAXSIZE;  //���¶�ͷλ��

    return false;
}

//��ȡ���г���
int GetCQueueLen(CQueue *pQueue)            
{
    return ((pQueue->rear - pQueue->front + MAXSIZE) % MAXSIZE);
}

//�����������Ԫ��
void ShowQueue(CQueue *pQueue)              
{

    int front = pQueue->front;
    int rear = pQueue->rear;

    while (front != rear){
        printf("%2d", pQueue->elem[front++]);
    }

    printf("\r\n");
}

//����Ϊ���򷵻�true
bool IsEmpty(CQueue *pQueue)                
{
    return pQueue->rear == pQueue->front;
}

//�������򷵻�false
bool IsFull(CQueue *pQueue)                 
{
    return (pQueue->rear + 1)%MAXSIZE == pQueue->front;
}

//��ȡ����Ԫ��
bool GetFront(CQueue *pQueue, unsigned char *rtval)   
{
    if (!IsEmpty(pQueue)){
        *rtval = pQueue->elem[pQueue->front];   //����ͷԪ��ͨ��*rtval����
        return true;
    }

    return false;
}


