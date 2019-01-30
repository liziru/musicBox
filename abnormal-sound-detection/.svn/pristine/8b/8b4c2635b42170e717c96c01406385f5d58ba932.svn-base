/** 
 * File: ring_buf.c 
 * 
 * Description: 
 *   Ring-buffer implemented according to the 'Mirroring' solution: 
 *   http://en.wikipedia.org/wiki/Circular_buffer#Mirroring 
 * 
 * Change Logs: 
 *   Date           Author       Notes 
 *   2013-12-16     Glen         first implementation 
 */  
  
#include "ring_buf.h"  
#include <stdlib.h>  
#include <string.h>  
#include <assert.h>  
  
/** 
 * @brief  Initialize the ring-buffer. Allocate necessary memory for the buffer. 
 * 
 * @param  rbuf : Pointer to the ring-buffer object 
 * @param  depth: Maximum element number of the buffer 
 * @param  width: sizeof each element 
 * 
 * @return true : Succeeded 
 *         false: Failed 
 */  
bool ring_buf_init(ring_buf_p rbuf, size_t depth, size_t width)  
{  
    assert(depth > 0 && depth < MSB_MASK && width > 0);  
  
    rbuf->depth  = depth;  
    rbuf->width  = width;  
    rbuf->rd_idx = 0;  
    rbuf->wr_idx = 0;  
    rbuf->buf    = (short *)calloc(depth, width);  
    return rbuf->buf != NULL;  
}  
  
/** 
 * @brief  Release the ring-buffer object. Deallocate the buffer memory. 
 * 
 * @param  rbuf: Pointer to the ring-buffer object 
 */  
void ring_buf_free(ring_buf_p rbuf)  
{  
    free(rbuf->buf);  
    rbuf->buf = NULL;  
}  
  
/** 
 * @brief  Clear the ring-buffer object. 
 * 
 * @param  rbuf: Pointer to the ring-buffer object 
 */  
void ring_buf_clear(ring_buf_p rbuf)  
{  
    rbuf->rd_idx = 0;  
    rbuf->wr_idx = 0;  
}  
  
/** 
 * @brief  Whether the ring-buffer is empty or not. 
 * 
 * @param  rbuf: Pointer to the ring-buffer 
 * 
 * @return true : Empty. There's no elements stored in the buffer. 
 *         false: Not empty 
 */  
bool ring_buf_is_empty(ring_buf_p rbuf)  
{  
    return rbuf->rd_idx == rbuf->wr_idx;  
}  
  
/** 
 * @brief  Whether the ring-buffer is full or not. 
 * 
 * @param  rbuf: Pointer to the ring-buffer 
 * 
 * @return true : Full 
 *         false: Not full 
 */  
bool ring_buf_is_full(ring_buf_p rbuf)  
{  
    return (rbuf->rd_idx & IDX_MASK) == (rbuf->wr_idx & IDX_MASK) &&  
           (rbuf->rd_idx & MSB_MASK) != (rbuf->wr_idx & MSB_MASK);  
}  
  
/** 
 * @brief  Increase the buffer index while writing or reading the ring-buffer. 
 *         This is implemented according to the 'mirroring' solution: 
 *         http://en.wikipedia.org/wiki/Circular_buffer#Mirroring 
 * 
 * @param  rbuf : Pointer to the ring-buffer 
 * @param  p_idx: Pointer to the buffer index to be increased 
 */  
inline void ring_buf_incr(ring_buf_p rbuf, short *p_idx)  
{  
    short idx = *p_idx & IDX_MASK;  
    short msb = *p_idx & MSB_MASK;  
  
    if (++idx == rbuf->depth) {  
        msb ^= MSB_MASK;  
        idx = 0;  
    }  
    *p_idx = msb | idx;  
}  
  
/** 
 * @brief  Write new element to the ring-buffer. 
 * 
 * @param  rbuf   : Pointer to the ring-buffer object 
 * @param  wr_data: Pointer of the new element to be written to the buffer 
 * 
 * @return true : Succeeded 
 *         false: Failed 
 */  
bool ring_buf_write(ring_buf_p rbuf, short *wr_data)  
{  
    if (ring_buf_is_full(rbuf))  
        return false;  
    else {  
        memcpy(rbuf->buf + rbuf->wr_idx * rbuf->width, wr_data, rbuf->width);  
        ring_buf_incr(rbuf, &rbuf->wr_idx);  
  
        return true;  
    }  
}  
  
/** 
 * @brief  Read from the ring-buffer 
 * 
 * @param  rbuf   : Pointer to the ring-buffer object 
 * @param  rd_data: Pointer to the read result 
 * 
 * @return true : Succeeded 
 *         false: Failed 
 */  
bool ring_buf_read(ring_buf_p rbuf, short *rd_data)  
{  
    if (ring_buf_is_empty(rbuf))  
        return false;  
    else {  
    memcpy(rd_data, rbuf->buf + rbuf->rd_idx * rbuf->width, rbuf->width);  
        ring_buf_incr(rbuf, &rbuf->rd_idx);  
  
        return true;  
    }  
}  
