#ifndef __FIFO_H_  
#define __FIFO_H_  
  
#pragma pack(4)  
typedef struct FIFO_Type_STRU  
{  
    unsigned int            Depth;          // Fifo深度  
    volatile unsigned int   Head;           // Head为起始元素  
    volatile unsigned int   Tail;           // Tail-1为最后一个元素  
    volatile unsigned int   Counter;        // 元素个数  
    unsigned int            ElementBytes;   // 每个元素的字节数element  
    void                    *Buff;          // 缓存区  
}FIFO_Type;  
#pragma pack()  
  
/********************************************************************//** 
 * @brief       FIFO初始化 
 * @param[in]   pFIFO: FIFO指针 
 * @param[in]   pBuff: FIFO中缓存 
 * @param[in]   elementBytes:FIFO每个元素的字节数 
 * @param[in]   depth: FIFO深度 
 * @return      None 
 *********************************************************************/  
void FIFO_Init(FIFO_Type *pFIFO, void *pBuff, unsigned int elementBytes, unsigned int depth);  
  
/********************************************************************//** 
 * @brief       向FIFO添加一个元素 
 * @param[in]   pFIFO: FIFO指针 
 * @param[in]   pValue: 要添加的元素 
 * @return      1-TRUE or 0-FALSE 
 *********************************************************************/  
unsigned char FIFO_AddOne(FIFO_Type *pFIFO, void *pValue);  
  
/********************************************************************//** 
 * @brief       向FIFO添加多个元素 
 * @param[in]   pFIFO: FIFO指针 
 * @param[in]   pValues: 要添加的元素指针 
 * @param[in]   bytesToAdd: 要添加元素的长度 
 * @return      实际添加的元素个数 
 *********************************************************************/  
unsigned int FIFO_Add(FIFO_Type *pFIFO, void *pValues, unsigned int bytesToAdd);  
  
/********************************************************************//** 
* @brief       向FIFO删除元素 
* @param[in]   pFIFO: FIFO指针 
* @return      实际删除的元素个数 
*********************************************************************/  
unsigned char FIFO_DelAddOne(FIFO_Type *pFIFO);

/********************************************************************//** 
* @brief       向FIFO删除多个元素 
* @param[in]   pFIFO: FIFO指针 
* @param[in]   bytesToDel: 要删除的个数
* @return      实际删除的元素个数 
*********************************************************************/   
unsigned char FIFO_DelAdd(FIFO_Type *pFIFO, unsigned int bytesToDel);

/********************************************************************//** 
 * @brief       从FIFO读取一个元素 
 * @param[in]   pFIFO: FIFO指针 
 * @param[in]   pValue: 存放要读取的元素指针 
 * @return      1-TRUE or 0-FALSE 
 *********************************************************************/  
unsigned char FIFO_GetOne(FIFO_Type *pFIFO, void *pValue);  
  
/********************************************************************//** 
 * @brief       从FIFO读取多个元素 
 * @param[in]   pFIFO: FIFO指针 
 * @param[out]  pValues: 存放要读取的元素指针 
 * @param[in]   bytesToRead: 要读取的元素长度 
 * @return      实际读取的元素个数 
 *********************************************************************/  
unsigned int FIFO_Get(FIFO_Type *pFIFO, void *pValues, unsigned int bytesToRead);  
  
  
/********************************************************************//** 
 * @brief       清空FIFO 
 * @param[in]   pFIFO: FIFO指针 
 * @return      None 
 *********************************************************************/  
void FIFO_Clear(FIFO_Type *pFIFO);  
  
  
#endif  
