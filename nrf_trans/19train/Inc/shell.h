#ifndef __SHELL_H__
#define __SHELL_H__

#ifdef __cplusplus
extern "C" {
#endif

extern char buffer_rx_temp;
extern char UART_RX_Buffer[256];
extern int UART_RX_Count;
extern char UART_RX_Alert[];

void echoInput(void);
void prompt(void);
void shellInit(void);
void startShell(void);
void readCmd(char *cmd);

#ifdef __cplusplus
}
#endif

#endif /* __SHEll_H__ */