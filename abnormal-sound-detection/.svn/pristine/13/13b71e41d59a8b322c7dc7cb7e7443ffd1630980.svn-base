/** 
 * File: ring_buf.h 
 * 
 * Description: 
 *   Header file for ring-buffer. 
 *   Implemented according to the 'Mirroring' solution: 
 *   http://en.wikipedia.org/wiki/Circular_buffer#Mirroring 
 * 
 * Change Logs: 
 *   Date           Author       Notes 
 *   2013-12-16     Glen         first implementation 
 */  
  
#ifndef _RING_BUF_H_  
#define _RING_BUF_H_  
  
#include <stdint.h>  
#include <stdbool.h>  
#include <stddef.h>  
  
#define IDX_MASK (SIZE_MAX>>1)  
#define MSB_MASK (~IDX_MASK)   /* also the maximum value of the buffer depth */  
  
/* ring buffer structure */  
struct ring_buf  
{  
    size_t depth ;             /* maximum element number */  
    size_t width ;             /* sizeof each element */  
    short rd_idx;             /* MSB is used for the 'mirror' flag */  
    short wr_idx;             /* MSB is used for the 'mirror' flag */  
    short   *buf  ;  
};  
  
typedef struct ring_buf  ring_buf_t;  
typedef struct ring_buf* ring_buf_p;  
  
bool ring_buf_init (ring_buf_p rbuf, size_t depth, size_t width);  
void ring_buf_free (ring_buf_p rbuf);  
void ring_buf_clear(ring_buf_p rbuf);  
  
bool ring_buf_full (ring_buf_p rbuf);  
bool ring_buf_empty(ring_buf_p rbuf);  
  
bool ring_buf_write(ring_buf_p rbuf, short *wr_data);  
bool ring_buf_read (ring_buf_p rbuf, short *rd_data);  
  
#endif  
