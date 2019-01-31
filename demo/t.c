#include <stdio.h>
#include "fifo.h"
int main(int argc, char *argv[])
{
    int i;  
    FIFO_Type   fifo;  
    FIFO_Type   *pfifo;  
    int         index;  
    float       fArray[10];  
    float       fValue;  
  
    char        cArray[10];  
    char        cValue;  
  
    pfifo = &fifo;  
  
    printf("测试FIFO元素为float型的数据\r\n");
    printf("初始化FIFO值。\r\n");  
    FIFO_Init(pfifo, fArray, sizeof(float), 10);  
    for (i = 0; i < 10; i++)  
    {  
        fValue = (100.0f+i*i);  
        FIFO_AddOne(pfifo, &fValue);  
    }  
    printf("当前元数个数：%d\r\n", pfifo->Counter);  
    index = 0;  
    while(FIFO_GetOne(pfifo, &fValue))  
    {  
        index ++;  
        printf("第%d个元素fValue = %0.3f\r\n",index, fValue);  
        if (index == 5)  
        {  
            printf("插入3个值。\r\n");  
            fValue = 1.23f;  
            FIFO_AddOne(pfifo, &fValue);  
  
            fValue = 2.34f;  
            FIFO_AddOne(pfifo, &fValue);  
  
            fValue = 3.45f;  
            FIFO_AddOne(pfifo, &fValue);  
        }  
    }  
  
    printf("\r\n\r\n");  
    printf("测试FIFO元素为char型的数据\r\n");  
    FIFO_Init(pfifo, cArray, sizeof(char), 10);  
    printf("初始化FIFO值。\r\n");  
	char tmp[] = "ABCDEFGHIJ";
    FIFO_Add(pfifo, (void *)tmp, 10);  
    printf("当前元数个数：%d\r\n", pfifo->Counter);  
    index = 0;  
    while(FIFO_GetOne(pfifo, &cValue))  
    {  
        index ++;  
        printf("第%d个元素cValue = %c\r\n",index, cValue);  
        if (index == 5)  
        {  
            printf("插入3个值。\r\n");  
            cValue = 'X';  
            FIFO_AddOne(pfifo, &cValue);  
  
            cValue = 'Y';  
            FIFO_AddOne(pfifo, &cValue);  
  
            cValue = 'Z';  
            FIFO_AddOne(pfifo, &cValue);  
        }  
    }  
	return 0;
  
}  
