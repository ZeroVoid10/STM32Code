#ifndef __cmd_func_H
#define __cmd_func_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f4xx_hal.h"
#include "main.h"
#include "usart.h"
#include "cmd.h"
#include "stdlib.h"
#include "math.h"
     
   
void cmd_hello_func(int argc,char *argv[]);  

#ifdef __cplusplus
}
#endif
#endif /*__ cmd_func_H */