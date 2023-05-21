
#ifndef GENKI_WEB_H
#define GENKI_WEB_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GET,
    POST
} WEB_METHOD;

typedef struct {
    const char *key;
    void *val;
} genki_web_map_entry_t;

typedef struct {
    size_t capacity;
    size_t length;
    genki_web_map_entry_t *items;
} genki_web_map_t;

typedef struct {
    const char* key;
    void* value;

    genki_web_map_t* _map;
    size_t _index;
} genki_web_map_iterator_t;

struct genki_web_request_t;
struct genki_web_response_t;
struct genki_web_service_t;

typedef int (*genki_web_do_request_func)(struct genki_web_request_t *request, struct genki_web_response_t *response);

typedef int (*genki_web_service_filter_add)(struct genki_web_service_t *service, const char *url,
                                            genki_web_do_request_func func);

typedef genki_web_do_request_func (*genki_web_service_filter_get)(struct genki_web_service_t *service, const char *url);

typedef char *(*genki_web_request_get_header)(struct genki_web_request_t *request, char *key);

typedef genki_web_map_iterator_t (*genki_web_request_header_iterator)(struct genki_web_request_t *request);
typedef bool (*genki_web_request_header_next)(genki_web_map_iterator_t* iterator);

typedef int (*genki_web_request_read)(struct genki_web_request_t *request, char *buf, int size);

typedef genki_web_map_iterator_t (*genki_web_response_header_iterator)(struct genki_web_response_t *response);
typedef bool (*genki_web_response_header_next)(genki_web_map_iterator_t* iterator);

typedef void (*genki_web_response_set_header)(struct genki_web_response_t *response, char *key, char *val);

typedef int (*genki_web_response_write)(struct genki_web_response_t *response, char *buf, int size);

typedef void (*genki_web_response_set_status)(struct genki_web_response_t *response, int status, const char *statusStr);

typedef struct genki_web_service_t {
    const char *name;

    genki_web_map_t *_filters;
    genki_web_service_filter_add addFilter;
    genki_web_service_filter_get getFilter;

    const char *link_name;
    const char *link_url;
} genki_web_service_t;

typedef struct genki_web_request_t {
    const char *url;
    WEB_METHOD method;
    const char *queryString;
    genki_web_map_t *_headers;

    int _fd;
    genki_web_request_get_header getHeader;
    genki_web_request_header_iterator headerIterator;
    genki_web_request_header_next headerNext;
    genki_web_request_read read;
} genki_web_request_t;

typedef struct genki_web_response_t {
    int _fd;
    int _init;
    int _status;
    const char *_statusStr;
    genki_web_map_t *_headers;

    genki_web_response_set_status setStatus;
    genki_web_response_set_header setHeader;
    genki_web_response_header_iterator  headerIterator;
    genki_web_response_header_next headerNext;
    genki_web_response_write write;
} genki_web_response_t;


void genki_web_start(void);

int genki_web_register(genki_web_service_t *service);

genki_web_service_t *genki_web_newService(const char *name);



#endif //GENKI_WEB_H
