#ifndef __SHELL_H__
#define __SHELL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ADD_CMD(cmd_name, cmd_usage, cmd_func) \
    { \
        cmd_name, \
        cmd_usage, \
        cmd_func \
    } \

#define OPEN_ECHO 

// TODO: change RX_BUFFER_SIZE to MAX_CMD_LENGTH
#define MAX_CMD_ARG_LENGTH 16
#define RX_BUFFER_SIZE 128
#define MAX_CMD_INFO_LENGTH 128
#define MAX_ARGC 8
#define OPEN_ECHO

extern char buffer_rx_temp;
extern char UART_RX_Buffer[RX_BUFFER_SIZE];
extern int UART_RX_Count;
extern const char *delim;

extern char *cmd_argv[MAX_ARGC];

struct cmd_struct {
    char cmd_name[MAX_CMD_ARG_LENGTH];
    char cmd_usage[MAX_CMD_INFO_LENGTH];
    void (*cmd_func)(int argc, char *argv[]);
};

void echoInput(void);
void startShell(void);
void prompt(void);

void shell_init(void);
void cmd_parse(char *cmd_line, int *argc, char *argv[]);
int cmd_exec(int argc, char *argv[]);
void cmd_help(int argc, char *argv[]);
//void readCmd(char *cmd);

#ifdef __cplusplus
}
#endif

#endif /* __SHEll_H__ */