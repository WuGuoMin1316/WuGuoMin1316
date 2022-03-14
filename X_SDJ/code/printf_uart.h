#ifndef __PRINTF_UART_H__
#define __PRINTF_UART_H__

#include "stdio.h"

#define _debug 1
#if _debug
#define my_printf  printf
#else
#define my_printf  /##/
#endif

extern uchar uart0_read_data(uchar *s, uchar j);
extern uchar uart0_read_fifo_num(void);

#endif
