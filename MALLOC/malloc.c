#include "malloc.h"

//�ڴ��(4�ֽڶ���)
__align(4) unsigned char membase[MEM_MAX_SIZE];            //SRAM�ڴ��
//�ڴ�����
unsigned short memmapbase[MEM_ALLOC_TABLE_SIZE];           //SRAM�ڴ��MAP
//�ڴ�������
const unsigned int memtblsize = MEM_ALLOC_TABLE_SIZE;    //�ڴ���С
const unsigned int memblksize = MEM_BLOCK_SIZE;          //�ڴ�ֿ��С
const unsigned int memsize = MEM_MAX_SIZE;               //�ڴ��ܴ�С


//�ڴ���������
struct _m_mallco_dev mallco_dev =
{
    Mem_init,           //�ڴ��ʼ��
    mem_perused,        //�ڴ�ʹ����
    membase,            //�ڴ��
    memmapbase,         //�ڴ����״̬��
    0,                  //�ڴ����δ����
};
/*********************************************************************************************************
* �� �� �� : mymemcpy
* ����˵�� : �����ڴ�
* ��    �� : des��Ŀ���ַ��src��Դ��ַ��n�����Ƶ��ڴ泤��
* �� �� ֵ : ��
* ��    ע : ��
*********************************************************************************************************/ 
void mymemcpy(void *des, void *src, unsigned int n)
{
    unsigned char *xdes = des;
    unsigned char *xsrc = src;
    while (n--)*xdes++ = *xsrc++;
}
//�����ڴ�
//*s:�ڴ��׵�ַ
//c :Ҫ���õ�ֵ
//count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
void mymemset(void *s, unsigned char c, unsigned int count)
{
    unsigned char *xs = s;
    while (count--)*xs++ = c;
}
//�ڴ�����ʼ��
void Mem_init(void)
{
    mymemset(mallco_dev.memmap, 0, memtblsize * 2); //�ڴ�״̬����������
    mymemset(mallco_dev.membase, 0, memsize);   //�ڴ��������������
    mallco_dev.memrdy = 1;                      //�ڴ�����ʼ��OK
}
//��ȡ�ڴ�ʹ����
//����ֵ:ʹ����(0~100)
unsigned char mem_perused(void)
{
    unsigned int used = 0;
    unsigned int i;
    for (i = 0; i < memtblsize; i++)
    {
        if (mallco_dev.memmap[i])used++;
    }
    return (used * 100) / (memtblsize);
}
//�ڴ����(�ڲ�����)
//memx:�����ڴ��
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ
unsigned int mem_malloc(unsigned int size)
{
    signed long offset = 0;
    unsigned short nmemb;  //��Ҫ���ڴ����
    unsigned short cmemb = 0; //�������ڴ����
    unsigned int i;
    if (!mallco_dev.memrdy)mallco_dev.init();   //δ��ʼ��,��ִ�г�ʼ��
    if (size == 0)return 0XFFFFFFFF;            //����Ҫ����
    nmemb = size / memblksize;                  //��ȡ��Ҫ����������ڴ����
    if (size % memblksize)nmemb++;
    for (offset = memtblsize - 1; offset >= 0; offset--) //���������ڴ������
    {
        if (!mallco_dev.memmap[offset])cmemb++; //�������ڴ��������
        else cmemb = 0;                         //�����ڴ������
        if (cmemb == nmemb)                     //�ҵ�������nmemb�����ڴ��
        {
            for (i = 0; i < nmemb; i++)         //��ע�ڴ��ǿ�
            {
                mallco_dev.memmap[offset + i] = nmemb;
            }
            return (offset * memblksize);       //����ƫ�Ƶ�ַ
        }
    }
    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��
}
//�ͷ��ڴ�(�ڲ�����)
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;
unsigned char mem_free(unsigned int offset)
{
    int i;
    if (!mallco_dev.memrdy) //δ��ʼ��,��ִ�г�ʼ��
    {
        mallco_dev.init();
        return 1;//δ��ʼ��
    }
    if (offset < memsize) //ƫ�����ڴ����.
    {
        int index = offset / memblksize;    //ƫ�������ڴ�����
        int nmemb = mallco_dev.memmap[index]; //�ڴ������
        for (i = 0; i < nmemb; i++)         //�ڴ������
        {
            mallco_dev.memmap[index + i] = 0;
        }
        return 0;
    }
    else return 2; //ƫ�Ƴ�����.
}
//�ͷ��ڴ�(�ⲿ����)
//ptr:�ڴ��׵�ַ
void myfree(void *ptr)
{
    unsigned int offset;
    if (ptr == NULL)return; //��ַΪ0.
    offset = (unsigned int)ptr - (unsigned int)mallco_dev.membase;
    mem_free(offset);   //�ͷ��ڴ�
}
//�����ڴ�(�ⲿ����)
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ.
void *mymalloc(unsigned int size)
{
    unsigned int offset;
    offset = mem_malloc(size);
    if (offset == 0XFFFFFFFF)return NULL;
    else return (void *)((unsigned int)mallco_dev.membase + offset);
}
//���·����ڴ�(�ⲿ����)
//*ptr:���ڴ��׵�ַ
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:�·��䵽���ڴ��׵�ַ.
void *myrealloc(void *ptr, unsigned int size)
{
    unsigned int offset;
    offset = mem_malloc(size);
    if (offset == 0XFFFFFFFF)return NULL;
    else
    {
        mymemcpy((void *)((unsigned int)mallco_dev.membase + offset), ptr, size); //�������ڴ����ݵ����ڴ�
        myfree(ptr);                                                //�ͷž��ڴ�
        return (void *)((unsigned int)mallco_dev.membase + offset);          //�������ڴ��׵�ַ
    }
}












