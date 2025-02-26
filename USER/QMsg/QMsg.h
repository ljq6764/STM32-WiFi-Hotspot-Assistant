#ifndef __QMsg_H
#define __QMsg_H

#include "io_bit.h"

#define MAXSIZE 11

typedef struct {
    unsigned char elem[MAXSIZE];
    unsigned char front;        //�±��ȡ,ָ���ͷԪ��
    unsigned char rear;         //�±겻��ȡ,ָ��Ԫ��Ӧ�����λ��
}CQueue;
extern CQueue qhead;

typedef struct {
	bool (*init)(CQueue *pQueue);
    bool (*push)(CQueue *pQueue, unsigned char val);
    bool (*pop)(CQueue *pQueue, unsigned char *rtval);
    int  (*get_length)(CQueue *pQueue);
    void (*show)(CQueue *pQueue);
    bool (*is_empty)(CQueue *pQueue);
    bool (*is_full)(CQueue *pQueue); 
    bool (*get_front)(CQueue *pQueue, unsigned char *rtval);
}__CQueue;
extern __CQueue QMsg;

#endif
