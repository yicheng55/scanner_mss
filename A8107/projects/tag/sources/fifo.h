/******************************************
 * fifo.h
 ******************************************/
#ifndef _FIFO_H_
#define _FIFO_H_


typedef struct {
     int head;
     int tail;
     int size;
     char * buf;	
} fifo_t;


void fifo_init(void * buf, int size);
int fifo_read(void * buf, int nbytes);
int fifo_write(void * buf, int nbytes);
void Show_Fifo_Data(void);

	
#endif		//_FIFO_H_
