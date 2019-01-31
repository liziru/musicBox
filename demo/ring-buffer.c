#include "stdio.h"
#include "ring_buf.h"
int main()  
{  
    ring_buf_t rbuf;  
    short tmp;
    ring_buf_init(&rbuf, sizeof(short), 4);  
  
    tmp=1;
    printf("ret = %d\n", ring_buf_write(&rbuf, &tmp));
    tmp=2;
    printf("ret = %d\n", ring_buf_write(&rbuf, &tmp));
    tmp=3;
    printf("ret = %d\n", ring_buf_write(&rbuf, &tmp));
    tmp=4;
    printf("ret = %d\n", ring_buf_write(&rbuf, &tmp));
    int k = 0;  
    
    for (k = 0; k < 8; k++){
        if (ring_buf_read(&rbuf, &tmp)){
            printf("%d\n", tmp);  
        }else{
            printf("read fail.\n");
        }
    }
  
    ring_buf_free(&rbuf);  
  
    return 0;  
} 
