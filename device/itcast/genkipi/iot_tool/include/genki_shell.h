

#ifndef GENKI_SHELL_H
#define GENKI_SHELL_H


typedef int (*genki_shell_pipe_readline_func)(char* buf, int size);
typedef int (*genki_shell_pipe_read_func)(char* buf, int size);


typedef struct genki_shell_pipe_t {
    char** args;
    int len;

    genki_shell_pipe_readline_func readLine;
    genki_shell_pipe_read_func read;
} genki_shell_pipe_t;

typedef int (*genki_shell_pipe_func)(genki_shell_pipe_t* pipe);


typedef struct genki_shell_service_t {
    const char *cmd;

    genki_shell_pipe_func doPipeline;
} genki_shell_service_t;


int genki_shell_register(genki_shell_service_t *service);


genki_shell_service_t *genki_shell_newService(const char *cmd);


#endif //GENKI_SHELL_H
