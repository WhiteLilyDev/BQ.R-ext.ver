#include "genki_shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include "hi_uart.h"

static const hi_uart_attribute g_uart_cfg = {115200, 8, 1, 0, 0};

#define IDX HI_UART_IDX_0

#define INITIAL_CAPACITY 16

typedef struct {
    const char *key;
    void *val;
} genki_shell_map_entry_t;

typedef struct {
    size_t capacity;
    size_t length;
    genki_shell_map_entry_t *items;
} genki_shell_map_t;

typedef struct {
    const char *key;
    void *value;

    genki_shell_map_t *_map;
    size_t _index;
} genki_shell_map_iterator_t;

static genki_shell_map_t *mapNew() {
    genki_shell_map_t *map;
    map = malloc(sizeof(genki_shell_map_t));
    if (map == NULL) return NULL;
    map->length = 0;
    map->capacity = INITIAL_CAPACITY;

    map->items = calloc(map->capacity, sizeof(genki_shell_map_entry_t));
    if (map->items == NULL) {
        free(map);
        return NULL;
    }
    return map;
}

static void mapFree(genki_shell_map_t *map) {
    for (int i = 0; i < map->capacity; ++i) {

//        if (map->items[i] != NULL) {
        if (map->items[i].key != NULL) {
            free((void *) map->items[i].key);
        }
        if (map->items[i].val != NULL) {
            free((void *) map->items[i].val);
        }
//            free(map->items[i]);
//        }
    }
    free(map->items);
    free(map);
}

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

static uint64_t _hash_key(const char *key) {
    uint64_t hash = FNV_OFFSET;
    for (const char *p = key; *p; p++) {
        hash ^= (uint64_t)(
        unsigned char) (*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

static const char *
_map_set_item(genki_shell_map_entry_t *items,
              size_t capacity, const char *key, void *value, size_t *plength) {
    uint64_t hash = _hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));
    while (items[index].key != NULL) {
        if (strcmp(key, items[index].key) == 0) {
            items[index].val = value;
            return items[index].key;
        }
        index++;
        if (index >= capacity) {
            index = 0;
        }
    }
    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    items[index].key = key;
    items[index].val = value;
    return key;
}

static bool _map_expand(genki_shell_map_t *map) {
    size_t newCapacity = map->capacity * 2;
    if (newCapacity < map->capacity) {
        return false;
    }
    genki_shell_map_entry_t *newItems = calloc(newCapacity, sizeof(genki_shell_map_entry_t));
    if (newItems == NULL) {
        return false;
    }

    for (int i = 0; i < map->capacity; ++i) {
        genki_shell_map_entry_t item = map->items[i];
        if (item.key != NULL) {
            _map_set_item(newItems, newCapacity, item.key, item.val, NULL);
        }
    }

    free(map->items);
    map->items = newItems;
    map->capacity = newCapacity;
    return true;
}

static const char *mapAdd(genki_shell_map_t *map, const char *key, void *val) {
//    assert(val != NULL);
    if (val == NULL) return NULL;

    if (map->length >= map->capacity / 2) {
        if (!_map_expand(map)) {
            return NULL;
        }
    }
    return _map_set_item(map->items, map->capacity, key, val, &map->length);
}

static void *mapGet(genki_shell_map_t *map, const char *key) {
    uint64_t hash = _hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(map->capacity - 1));

    while (map->items[index].key != NULL) {
        if (strcmp(key, map->items[index].key) == 0) {
            return map->items[index].val;
        }
        index++;
        if (index >= map->capacity) {
            index = 0;
        }
    }
    return NULL;
}

static genki_shell_map_iterator_t mapIterator(genki_shell_map_t *map) {
    genki_shell_map_iterator_t it;
    it._map = map;
    it._index = 0;
    return it;
}

static bool mapNext(genki_shell_map_iterator_t *it) {
    // Loop till we've hit end of entries array.
    genki_shell_map_t *map = it->_map;
    while (it->_index < map->capacity) {
        size_t i = it->_index;
        it->_index++;
        if (map->items[i].key != NULL) {
            // Found next non-empty item, update iterator key and value.
            genki_shell_map_entry_t entry = map->items[i];
            it->key = entry.key;
            it->value = entry.val;
            return true;
        }
    }
    return false;
}

static unsigned char is_init = 0;
static genki_shell_map_t *services;
static unsigned char is_shell_started = 0;

static void init(void) {
    if (is_init) return;
    is_init = 1;
    services = mapNew();

//    genki_cmd *cmd = malloc(sizeof(genki_cmd));
//    cmd->name = "cmd";
//    cmd->len = 3;
//    cmd->callback = (genki_cmd_callback_func) call_shell_env;
//    genki_shell_register(cmd);

    hi_uart_deinit(IDX);
    unsigned int ret = hi_uart_init(IDX, &g_uart_cfg, HI_NULL);
    if (ret != 0) {
        printf("############ Serial CMD Failed ############## \r\n");
        printf("%x\r\n", ret);
        printf("############################################# \r\n");
    } else {
        printf("############ Serial CMD Success ############## \r\n");
    }
}

static int readline(char *buf, int size) {
    int i = 0;
    char c = '\0';
    int n;

    while (i < size - 1) {
        if (c == '\n') {
            break;
        }
        n = hi_uart_read(IDX, &c, 1);
        if (n > 0) {
            if (c == '\r') {
                n = hi_uart_read(IDX, &c, 1);
                if ((n > 0) && (c == '\n')) {
                    buf[i] = c;
                } else {
                    buf[i] = '\r';
                    i++;
                    buf[i] = c;
                    i++;
                }
            } else if (c == '\n') {
                buf[i] = c;
            } else {
                buf[i] = c;
                i++;
            }
        } else {
            c = '\n';
//            i++;
        }
    }
    buf[i] = '\0';
    return (i);
}

static int read(char* buf, int size) {
    return hi_uart_read(IDX, buf, 1);
}

static char **get_cmd_args(char *line, size_t *count) {
    char **args = NULL;
    *count = 0;

    size_t len = strlen(line);

    int i = 0;
    int start = 0;
    while (i < len) {
        if (line[i] == ' ') {
            if (start == i) {
                start += 1;
            } else {
                size_t l = i - start;

                if (args) {
                    args = realloc(args, (*count + 1) * sizeof(char*));
                } else {
                    args = malloc(sizeof(char *));
                }

                args[*count] = malloc((l + 1) * sizeof(char));
                memcpy(args[*count], &line[start], l);
                args[*count][l] = '\0';

                *count += 1;
                start = i + 1;
            }
        }
        i++;
    }
    if (start <= len - 1) {
        size_t l = len - start;

        if (args) {
            args = realloc(args, (*count + 1) * sizeof(char*));
        } else {
            args = malloc(sizeof(char *));
        }
        args[*count] = malloc((l + 1) * sizeof(char));
        memcpy(args[*count], &line[start], l);
        args[*count][l] = '\0';


        *count += 1;
    }
    return args;
}

int genki_shell_register(genki_shell_service_t *service) {
    init();

    const char *cmd = service->cmd;
    genki_shell_service_t *s = mapGet(services, cmd);
    if (s) {
        printf("cmd service [%s] has register\r\n", cmd);
        return -1;
    }

    mapAdd(services, cmd, service);
    return 1;
}

static genki_shell_pipe_t* newPipe(char** args, int count) {
    genki_shell_pipe_t *pipe;

    pipe = malloc(sizeof(genki_shell_pipe_t));
    pipe->args = args;
    pipe->len = count;

    pipe->readLine = readline;
    pipe->read = read;

    return pipe;
}

static void freePipe(genki_shell_pipe_t* pipe) {
    free(pipe);
}

int genki_shell_start(void) {
    init();

    if (is_shell_started) return 1;
    is_shell_started = 1;

    char buf[128];
    while (is_shell_started) {
        int n = readline(buf, 128);
        if (n <= 0) {
            continue;
        }

        char line[n + 1];
        memcpy(line, buf, n);
        line[n] = '\0';

//        printf("line: %s %d\r\n", line, n);

        size_t count;
        char **args = get_cmd_args(line, &count);

        if (count > 0) {
            char *cmd = args[0];
            genki_shell_service_t* service = mapGet(services, cmd);

            if (service) {
                if(service->doPipeline) {
                    genki_shell_pipe_t* pipe = newPipe(args, count);

                    service->doPipeline(pipe);

                    freePipe(pipe);
                }
            } else {
                printf("unknown cmd: %s\r\n", line);
            }
        }
        for (int i = 0; i < count; ++i) {
            free(args[i]);
        }
        free(args);
    }

}

genki_shell_service_t *genki_shell_newService(const char *cmd) {
    genki_shell_service_t *service;

    service = calloc(1, sizeof(genki_shell_service_t));
    service->cmd = cmd;

    return service;
}