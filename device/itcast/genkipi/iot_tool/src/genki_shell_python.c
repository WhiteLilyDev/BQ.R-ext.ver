#include "genki_shell_python.h"

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "dtpython.h"

#include "ohos_init.h"
#include "cmsis_os2.h"


static int doPipeline(genki_shell_pipe_t *pipe) {
    printf("MicroPython For GenkiPI. Shell Powered by Itcast.\r\n");
    printf("Type 'help()', 'exit()' for information.\r\n");

    DTPython_Init();
    DTPython_RunCode("print('Python Runtime Start!')");

    char buf[512];
    while (1) {
        int num = pipe->readLine(buf, 512);

        if (num <= 0) {
            continue;
        }

        char line[num];
        memcpy(line, buf, num);
        line[num] = '\0';

        if (strcasecmp(line, "help()") == 0) {
            printf(">>> help !!!\r\n");
        } else if (strcasecmp(line, "exit()") == 0) {
            printf(">>> exit MicroPython Env !!!\r\n");
            break;
        } else {
            DTPython_RunCode(line);
        }
    }

    DTPython_Deinit();
    return 0;
}

genki_shell_service_t *newPythonShell(void) {
    genki_shell_service_t *service = genki_shell_newService("python");
    service->doPipeline = doPipeline;
    return service;
}
