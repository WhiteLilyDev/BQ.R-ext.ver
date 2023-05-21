#include "genki_web.h"

#include <stdio.h>
#include "lwip/sockets.h"

#define INITIAL_CAPACITY 16

#define PORT 80
#define SERVER_STRING "Genki: httpd/0.0.1\r\n"


static unsigned char is_init = 0;
static genki_web_map_t *services;
static genki_web_map_t *serviceMapper;
static genki_web_map_t *links;

static void accept_request(int client);

static int get_line(int sock, char *buf, int size);

static void response_status(int client, int status);

static genki_web_map_t *mapNew() {
    genki_web_map_t *map;
    map = malloc(sizeof(genki_web_map_t));
    if (map == NULL) return NULL;
    map->length = 0;
    map->capacity = INITIAL_CAPACITY;

    map->items = calloc(map->capacity, sizeof(genki_web_map_entry_t));
    if (map->items == NULL) {
        free(map);
        return NULL;
    }
    return map;
}

static void mapFree(genki_web_map_t *map) {
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
_map_set_item(genki_web_map_entry_t *items,
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

static bool _map_expand(genki_web_map_t *map) {
    size_t newCapacity = map->capacity * 2;
    if (newCapacity < map->capacity) {
        return false;
    }
    genki_web_map_entry_t *newItems = calloc(newCapacity, sizeof(genki_web_map_entry_t));
    if (newItems == NULL) {
        return false;
    }

    for (int i = 0; i < map->capacity; ++i) {
        genki_web_map_entry_t item = map->items[i];
        if (item.key != NULL) {
            _map_set_item(newItems, newCapacity, item.key, item.val, NULL);
        }
    }

    free(map->items);
    map->items = newItems;
    map->capacity = newCapacity;
    return true;
}

static const char *mapAdd(genki_web_map_t *map, const char *key, void *val) {
//    assert(val != NULL);
    if (val == NULL) return NULL;

    if (map->length >= map->capacity / 2) {
        if (!_map_expand(map)) {
            return NULL;
        }
    }
    return _map_set_item(map->items, map->capacity, key, val, &map->length);
}

static void *mapGet(genki_web_map_t *map, const char *key) {
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

static genki_web_map_iterator_t mapIterator(genki_web_map_t *map) {
    genki_web_map_iterator_t it;
    it._map = map;
    it._index = 0;
    return it;
}

static bool mapNext(genki_web_map_iterator_t *it) {
    // Loop till we've hit end of entries array.
    genki_web_map_t *map = it->_map;
    while (it->_index < map->capacity) {
        size_t i = it->_index;
        it->_index++;
        if (map->items[i].key != NULL) {
            // Found next non-empty item, update iterator key and value.
            genki_web_map_entry_t entry = map->items[i];
            it->key = entry.key;
            it->value = entry.val;
            return true;
        }
    }
    return false;
}


static void response_status(int client, int status) {
    char buf[128] = {0};

    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, SERVER_STRING);
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/plain\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "%d", status);
    send(client, buf, strlen(buf), 0);

    closesocket(client);
}

static int get_line(int sock, char *buf, int size) {
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n')) {
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0) {
            if (c == '\r') {
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n')) {
                    n = recv(sock, &c, 1, 0);
                } else {
                    c = '\n';
                }
            }
            buf[i] = c;
            i++;
        } else {
            c = '\n';
        }
    }
    buf[i] = '\0';

    return (i);
}

static void setRequestUrl(genki_web_request_t *request, const char *url) {
    if (!request) return;
    if (!url) return;
    request->url = url;
}

static void setRequestQueryString(genki_web_request_t *request, const char *queryString) {
    if (!request) return;
    if (!queryString) return;

    request->queryString = queryString;
}

static int requestRead(genki_web_request_t *request, char *buf, int size) {
    if (request && request->_fd > 0) {
        return recv(request->_fd, buf, size, 0);
    }
    return -1;
}

static char *requestGetHeader(genki_web_request_t *request, char *key) {
    return mapGet(request->_headers, key);
}

static genki_web_map_iterator_t requestHeaderIterator(genki_web_request_t *request) {
    return mapIterator(request->_headers);
}

static bool requestHeaderNext(genki_web_map_iterator_t *iterator) {
    return mapNext(iterator);
}

static genki_web_request_t *newRequest() {
    genki_web_request_t *request;

    request = calloc(1, sizeof(genki_web_request_t));
    request->_headers = mapNew();
    request->url = NULL;
    request->queryString = NULL;
    request->_fd = -1;

    request->read = requestRead;
    request->getHeader = requestGetHeader;
    request->headerIterator = requestHeaderIterator;
    request->headerNext = requestHeaderNext;
    return request;
}


static void freeRequest(genki_web_request_t *request) {
    if (!request) return;
    mapFree(request->_headers);
    free(request);
}

static int responseWrite(genki_web_response_t *response, char *data, int size) {
    if (response && response->_fd > 0) {
        if (!response->_init) {
            response->_init = 1;

            char buf[128];
            sprintf(buf, "HTTP/1.0 %d %s\r\n", response->_status, response->_statusStr);
            send(response->_fd, buf, strlen(buf), 0);
            sprintf(buf, SERVER_STRING);
            send(response->_fd, buf, strlen(buf), 0);

            // header
            genki_web_map_iterator_t iterator = response->headerIterator(response);
            while (response->headerNext(&iterator)) {
                sprintf(buf, "%s: %s\r\n", iterator.key, iterator.value);
                send(response->_fd, buf, strlen(buf), 0);
            }

            sprintf(buf, "\r\n");
            send(response->_fd, buf, strlen(buf), 0);
        }
        if (size > 0) {
            return send(response->_fd, data, size, 0);
        }
    }
    return -1;
}

static void responseSetStatus(genki_web_response_t *response, int status, const char *statusStr) {
    response->_status = status;
    response->_statusStr = statusStr;
}

static void responseSetHeader(genki_web_response_t *response, char *key, char *val) {
    char *v;
    v = malloc(strlen(val) + 1);
    strcpy(v, val);

    mapAdd(response->_headers, key, v);
}

static genki_web_map_iterator_t responseHeaderIterator(genki_web_response_t *response) {
    return mapIterator(response->_headers);
}

static bool responseHeaderNext(genki_web_map_iterator_t *iterator) {
    return mapNext(iterator);
}

static genki_web_response_t *newResponse() {
    genki_web_response_t *response;

    response = malloc(sizeof(genki_web_response_t));
    response->_headers = mapNew();

    responseSetStatus(response, 200, "Ok");
    response->_init = 0;
    response->write = responseWrite;
    response->setHeader = responseSetHeader;
    response->setStatus = responseSetStatus;
    response->headerIterator = responseHeaderIterator;
    response->headerNext = responseHeaderNext;
    return response;
}

static void freeResponse(genki_web_response_t *response) {
    mapFree(response->_headers);
    free(response);
}

static int getKeyValue(char *line, char *key, char *value) {
    size_t len = strlen(line);

    int index = -1;
    for (int i = 0; i < len; ++i) {
        if (i < len - 1 && line[i] == ':' && line[i + 1] == ' ') {
            index = i;
            break;
        }
    }
    if (index != -1) {
        memset(key, 0, index + 1);
        memcpy(key, line, index);
        key[index] = '\0';

        size_t vlen = 0;
        if (len - 2 > 0) {
            if (line[len - 1] == '\n' && line[len - 2] == '\r') {
                vlen = len - 2 - index - 2;
            } else if (line[len - 1] == '\n') {
                vlen = len - 1 - index - 2;
            } else {
                vlen = len - index - 2;
            }
        } else {
            return 0;
        }
        memset(value, 0, vlen + 1);
        memcpy(value, &line[index + 2], vlen);
        value[vlen] = '\0';
        return 1;
    }
    return 0;
}

static void accept_request(int client) {
    char buf[1024] = {0};
    int num;

    char method[32] = {0};
    char url[255] = {0};
    char protocol[32] = {0};

    char *queryString = NULL;

    /////////////////////////// url part /////////////////////////
    num = get_line(client, buf, sizeof(buf));
    if (num > 0) {
        char *p = strtok(buf, " ");
        memcpy(method, p, strlen(p));
        if (p) {
            p = strtok(NULL, " ");
            memcpy(url, p, strlen(p));
            url[strlen(p)] = '\0';
        }
        if (p) {
            p = strtok(NULL, " ");
            memcpy(protocol, p, strlen(p));
            protocol[strlen(p)] = '\0';
        }
    } else {
        // error url
        response_status(client, -1);
        return;
    }

    if (strcasecmp(method, "GET") == 0) {
        // p
        char *p = strtok(url, "?");
        if (p) {
            queryString = strtok(NULL, "");
        }
        size_t len = strlen(p);
        memcpy(url, p, len);
        url[len] = '\0';
    }

    char *key = malloc((strlen(url) + 1) * sizeof(char));
    memcpy(key, url, strlen(url));
    key[strlen(url)] = '\0';

    WEB_METHOD m = GET;
    if (strcasecmp(method, "GET") == 0) {
        m = GET;
    } else if (strcasecmp(method, "POST") == 0) {
        m = POST;
    }
    genki_web_request_t *request = newRequest();
    request->_fd = client;
    request->method = m;
    setRequestUrl(request, url);
    setRequestQueryString(request, queryString);

    /////////////////////////// header part /////////////////////
    int newlineCount = 0;
    do {
        num = get_line(client, buf, sizeof(buf));

        char key[64];
        char value[128];
        if (getKeyValue(buf, key, value)) {

            size_t len = strlen(value);
            char *v = calloc(len + 1, sizeof(char));
            memcpy(v, value, len);
            v[len] = '\0';

            mapAdd(request->_headers, key, v);
        }

        if (strcmp("\n", buf) == 0) {
            newlineCount += 1;
        } else {
            newlineCount = 0;
        }
        if (newlineCount == 1) {
            break;
        }
    } while (num > 0);

    genki_web_response_t *response = newResponse();
    response->_fd = client;

    genki_web_service_t *service = mapGet(serviceMapper, key);
    if (service) {
        genki_web_do_request_func func = service->getFilter(service, key);
        func(request, response);
    } else {
        service = mapGet(serviceMapper, "*");
        if (service) {
            genki_web_do_request_func func = service->getFilter(service, "*");
            if (func) {
                func(request, response);
            }
        }
    }

    responseWrite(response, NULL, 0);
    closesocket(client);

    freeRequest(request);
    freeResponse(response);
    free(key);
}

static int doLink(genki_web_request_t *request, genki_web_response_t *response) {
    response->setHeader(response, "Content-Type", "application/json");

    char buf[128];

    sprintf(buf, "[");
    response->write(response, buf, strlen(buf));

    genki_web_map_iterator_t iterator = mapIterator(links);
    int index = 0;
    while (mapNext(&iterator)) {
        const char *key = iterator.key;
        const char *value = iterator.value;

        sprintf(buf, "{\"name\": \"%s\",\"link\": \"%s\"}", key, value);
        response->write(response, buf, strlen(buf));
        index++;

        if(index != links->length) {
            sprintf(buf, ",");
            response->write(response, buf, strlen(buf));
        }
    }

    sprintf(buf, "]");
    response->write(response, buf, strlen(buf));

    return 1;
}

static void init(void) {
    if (is_init) return;
    is_init = 1;
    services = mapNew();
    serviceMapper = mapNew();
    links = mapNew();

    genki_web_service_t *service = genki_web_newService("LINK");
    service->addFilter(service, "/link", doLink);
    genki_web_register(service);
}


void genki_web_start(void) {
    init();

    int sock_fd;
    int ret;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("[GenkiWeb]sock_fd create error\r\n");
        return;
    }
    // receive addr config
    struct sockaddr_in recvfrom_addr;
    int recvfrom_addr_len = sizeof(recvfrom_addr);

    memset((void *) &recvfrom_addr, 0, recvfrom_addr_len);
    recvfrom_addr.sin_family = AF_INET;
    recvfrom_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recvfrom_addr.sin_port = htons(PORT);
    // bind
    ret = bind(sock_fd, (struct sockaddr *) &recvfrom_addr, recvfrom_addr_len);
    if (ret == -1) {
        perror("[GenkiWeb]bind error\r\n");
        return;
    }

    // listen, just listen 2 connect
    ret = listen(sock_fd, 1);
    if (ret == -1) {
        printf("[GenkiWeb]listen error\r\n");
        return;
    }

    while (is_init) {
        int client_fd;
        struct sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);

        client_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (client_fd < 0) {
            printf("[GenkiWeb]accept client error %d %d\r\n", client_fd, sock_fd);
            closesocket(client_fd);
//            shutdown(client_fd);
            continue;
        }

        accept_request(client_fd);
    }
}

int genki_web_register(genki_web_service_t *service) {
    init();

    const char *name = service->name;
    genki_web_service_t *s = mapGet(services, name);
    if (s) {
        printf("service [%s] has register\r\n", name);
        return -1;
    }

    int flag = 1;
    genki_web_map_iterator_t iterator = mapIterator(service->_filters);
    while (mapNext(&iterator)) {
        void *f = mapGet(serviceMapper, iterator.key);
        if (f) {
            printf("service [%s] filter url [%s] has used\r\n", name, iterator.key);
            flag = 0;
            break;
        }
    }
    if (flag) {
        mapAdd(services, name, service);

        iterator = mapIterator(service->_filters);
        while (mapNext(&iterator)) {
            mapAdd(serviceMapper, iterator.key, service);
        }

        if (service->link_name && service->link_url) {
            mapAdd(links, service->link_name, service->link_url);
        }
    }
}

static int serviceAddFilter(struct genki_web_service_t *service, const char *url, genki_web_do_request_func func) {
    mapAdd(service->_filters, url, func);
    return 1;
}

static genki_web_do_request_func serviceGetFilter(struct genki_web_service_t *service, const char *url) {
    return mapGet(service->_filters, url);
}

genki_web_service_t *genki_web_newService(const char *name) {
    genki_web_service_t *service;
    service = calloc(1, sizeof(genki_web_service_t));

    // name
    service->name = name;

    // filters
    service->_filters = mapNew();

    // add filter
    service->addFilter = serviceAddFilter;
    // get filter
    service->getFilter = serviceGetFilter;

    return service;
}