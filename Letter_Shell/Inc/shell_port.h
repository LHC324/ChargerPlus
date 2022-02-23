#ifndef _SHELL__PORT_H_
#define _SHELL__PORT_H_

#include "shell.h"

/*定义shell缓冲区尺寸*/
#define SHELL_BUFFER_SIZE 128U

/*声明shell对象*/
extern Shell shell;

/*初始化shell*/
void User_Shell_Init(void);

#endif /* _SHELL_PORT_H_ */
