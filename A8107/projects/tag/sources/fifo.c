#include <stdio.h>
#include <string.h>
#include "tagdefine.h"
#include "fifo.h"

fifo_t  led_fifo;
fifo_t *f = &led_fifo;


//This initializes the FIFO structure with the given buffer and size
void fifo_init( void * buf, int size){
     f->head = 0;
     f->tail = 0;
     f->size = size;
     f->buf = buf;
}


//This reads nbytes bytes from the FIFO
//The number of bytes read is returned
int fifo_read( void * buf, int nbytes){
     int i;
     char * p;
     p = buf;
     for(i=0; i < nbytes; i++){
          if( f->tail != f->head ){ //see if any data is available
               *p++ = f->buf[f->tail];  //grab a byte from the buffer
               f->tail++;  //increment the tail
               if( f->tail == f->size ){  //check for wrap-around
                    f->tail = 0;
               }
          } else {
               return i; //number of bytes read
          }
     }
     return nbytes;
}

//This writes up to nbytes bytes to the FIFO
//If the head runs in to the tail, not all bytes are written
//The number of bytes written is returned
int fifo_write( void * buf, int nbytes){
     int i;
     const char * p;
     p = buf;
     for(i=0; i < nbytes; i++)
	   {
           //first check to see if there is space in the buffer
           if( (f->head + 1 == f->tail) ||( (f->head + 1 == f->size) && (f->tail == 0) ))
					 {
						 if(f->head + 1 == f->tail)
						 {
							 printf("f->head is %d f->size is %d)\r\n",f->head,f->tail);
							 printf("f->head + 1 == f->tail\r\n");
						 }
						 if((f->head + 1 == f->size))
						 {
							 printf("f->head is %d f->size is %d)\r\n",f->head,f->size);
							 printf("(f->head + 1 == f->size)\r\n");
						 }
						 if((f->tail == 0) )
						 {
							 printf("(f->tail == 0)\r\n");
						 }
                 return i; //no more room
           } 
					 else {
               f->buf[f->head] = *p++;
               f->head++;  //increment the head
               if( f->head == f->size ){  //check for wrap-around
                    f->head = 0;
               }
           }
     } 
		 
     return nbytes;
}

void Show_Fifo_Data()
{
	int i = 0;
	for(i = 0; i < 4; i++)
	{
		printf("buf[%d] = %d\r\n", i,f->buf[i]);
	}
}
