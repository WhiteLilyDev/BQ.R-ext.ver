#include "genki_web_base.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "genki_info.h"
#include "genki_wifi_ap.h"
#include "genki_wifi_sta.h"

#include "hi_upg_api.h"
#include "utils_file.h"
#include "hi_reset.h"

static char *ip4_str(unsigned int src) {
    char p[16] = {0};
    sprintf(p, "%d.%d.%d.%d", (src & 0xff), (src >> 8) & 0xff, (src >> 16) & 0xff, (src >> 24) & 0xff);

    int len = strlen(p);
    char *dst = (char *) calloc(len + 1, sizeof(char));
    memcpy(dst, p, len);
    dst[len] = '\0';

    return dst;
}

static int doWifiInfo(genki_web_request_t *request, genki_web_response_t *response) {
//    printf("do wifi info\r\n");
//    printf("url: %s\r\n", request->url);

    unsigned int a_ip, a_netmask, a_gateway;
    unsigned int ret = wifi_ap_get_addr(&a_ip, &a_netmask, &a_gateway);

    char buf[512] = {0};

    if (ret != 0) {
        sprintf(buf, "-1");
        response->write(response, buf, strlen(buf));
        return -1;
    }

    response->setHeader(response, "Content-Type", "application/json");

    char *str_aip = ip4_str(a_ip);
    char *str_anetmask = ip4_str(a_netmask);
    char *str_agateway = ip4_str(a_gateway);
    char aname[32] = {0};
    wifi_ap_get_ssid(aname);

    if (wifi_sta_is_connected()) {
        unsigned int s_ip, s_netmask, s_gateway;
        unsigned int ret = wifi_sta_get_addr(&s_ip, &s_netmask, &s_gateway);
        if (ret == 0) {

            char *str_sip = ip4_str(s_ip);
            char *str_snetmask = ip4_str(s_netmask);
            char *str_sgateway = ip4_str(s_gateway);
            char sname[32] = {0};
            wifi_sta_get_ssid(sname);
            sprintf(buf,
                    "{\"ap\": {\"name\": \"%s\", \"ip\": \"%s\", \"netmask\": \"%s\", \"gateway\": \"%s\"}, \"sta\": {\"name\": \"%s\", \"ip\": \"%s\", \"netmask\": \"%s\", \"gateway\": \"%s\"}}",
                    aname, str_aip, str_anetmask, str_agateway, sname, str_sip, str_snetmask, str_sgateway);

            free(str_sip);
            free(str_snetmask);
            free(str_sgateway);
        } else {
            sprintf(buf, "{\"error\": %x}", ret);
        }
    } else {
        sprintf(buf,
                "{\"ap\": {\"name\": \"%s\", \"ip\": \"%s\", \"netmask\": \"%s\", \"gateway\": \"%s\"}}",
                aname, str_aip, str_anetmask, str_agateway);
    }

    response->write(response, buf, strlen(buf));

    free(str_aip);
    free(str_anetmask);
    free(str_agateway);

    return 1;
}

static int doInfo(genki_web_request_t *request, genki_web_response_t *response) {
//    printf("do info\r\n");
//    printf("url: %s\r\n", request->url);

    response->setHeader(response, "Content-Type", "application/json");

    unsigned int code = GenkiVersionCode();
    char *str = GenkiVersionStr();

    char buf[128];
    sprintf(buf, "{\"code\": %d, \"str\": \"%s\"}", code, str);
    response->write(response, buf, strlen(buf));

    return 1;
}

static int doIcon(genki_web_request_t *request, genki_web_response_t *response) {
    char *icon = "A";

    response->setHeader(response, "Content-Type", "image/x-icon");

    response->write(response, icon, strlen(icon));
}

static void save_html(char *page, char *html) {
    int fd = UtilsFileOpen(page, O_CREAT_FS | O_TRUNC_FS | O_WRONLY_FS, 0);

    if (fd > 0) {
        UtilsFileWrite(fd, html, strlen(html));
        UtilsFileClose(fd);
    }
}

static int doIndexHtml(genki_web_request_t *request, genki_web_response_t *response) {
//    printf("do index html\r\n");
//    printf("url: %s\r\n", request->url);
//    genki_web_map_iterator_t iterator = request->headerIterator(request);
//    while (request->headerNext(&iterator)) {
//        printf("%s: %s\r\n", iterator.key, iterator.value);
//    }
    if (request->method != GET) {
        return -1;
    }
    char* page = "index.html";
    unsigned int size;
    char buf[128];
    if (UtilsFileStat(page, &size) >= 0) {
        sprintf(buf, "%d", size);
        response->setHeader(response, "Content-Length", buf);

        int fd = UtilsFileOpen(page, O_RDONLY_FS, 0);
        if (fd > 0) {
            unsigned char num;
            while ((num = UtilsFileRead(fd, buf, 128)) > 0) {
                response->write(response, buf, num);
            }
            UtilsFileClose(fd);
        } else {
            sprintf(buf, "<html><body><h2 id='a'>ERROR</h2></body></html>");
            response->write(response, buf, strlen(buf));
        }
    } else {
        char *html = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><title>传智教育元气派</title><style> body, .cb {text-align: center;}  .a {width: 720px;border: 1px solid grey;border-radius: 4px;margin: 10px auto;}  p {font-weight: bold;text-align: left;margin: 5px;}  table {width: 100%;}  table, tr, td {border: 1px solid darkgray;}  td {width: 50%;}  .n td {width: auto;}  .cf {margin: 16px auto;}  button {margin: 10px;padding: 5px 36px;}</style></head><body><h2>传智教育元气派</h2><div class=\"a\"><p>版本信息:</p><table><tr><td>版本号:</td><td id=\"c\"></td></tr><tr><td>版本标识:</td><td id=\"s\"></td></tr></table></div><div class=\"a\"><p>网络信息:</p><table class=\"n\"><thead><tr><td>WIFI名称</td><td>模式</td><td>IP信息</td><td>操作</td></tr></thead><tbody id=\"tn\"></tbody></table></div><div class=\"a\"><p>连接WIFI:</p><hr><div>WIFI名称: <input id=\"n\" type=\"text\"></div><div>WIFI密码: <input id=\"d\" type=\"password\"></div><button id=\"w\">连接WIFI</button></div><div class=\"a\"><p>OTA更新:</p><hr><div class=\"cf\"><input type=\"file\" id=\"f\"></div><div class=\"cb\"><div id=\"p\"></div></div><button id=\"b\">OTA更新</button></div><div class=\"a\"><p>应用功能:</p><hr><div id=\"ex\"></div></div><script type=\"application/javascript\"> function uu(p) {return p;}let ua = uu(\"/ota/upg\");let ub = uu(\"/info\");let uc = uu(\"/wifi/info\");let ud = uu(\"/wifi\");let ue = uu(\"/sta/clear\");let uf = uu(\"/ap\");let ug = uu(\"/link\");function g(i) {return document.getElementById(i);}let fo = g('f');let bo = g('b');let po = g('p');let bw = g('w');let wn = g('n');let wp = g('d');bo.style.display = 'none';let t = 15;function fg(u, f) {let x = new XMLHttpRequest();x.onreadystatechange = function () {if (this.readyState == 4 && this.status == 200) {f(x);}};x.open(\"GET\", u, true);x.send();}function fp(u, d, f, p) {let x = new XMLHttpRequest();if (p) {x.upload.addEventListener('progress', function (e) {p(e);});}x.onreadystatechange = function () {if (this.readyState == 4 && this.status == 200) {f(x);}};x.open('POST', u, true);x.timeout = 45000;x.send(d);}function fb(b, f) {b.addEventListener('click', f);}function fub(f) {if (f) {bo.style.display = 'inline';bo.addEventListener('click', fcu);} else {bo.style.display = 'none';bo.removeEventListener('click', fcu);}}function fup(p) {po.innerText = \"更新进度\" + p + \"%\";}function fur() {po.innerText = \"重启倒计时,剩余\" + t + \"秒\";}function upg() {fp(ua, fo.files[0], (x) => {if (parseInt(x.response) == -101) {/* tx error*/} else if (parseInt(x.response) == -102) {/* commit error, upg error*/} else if (parseInt(x.response) == 1) {/* upg success*/window.setInterval(() => {t -= 1;fur();if (t < 0) {location.reload();}}, 1000);}}, (e) => {let size = fo.files[0].size;if (e.loaded <= size) {let p = Math.round(e.loaded / size * 100);fup(p);}if (e.loaded == e.total) {fup(100);}});}function fcu(e) {upg();fub(false);}function fv() {fg(ub, (x) => {let j = JSON.parse(x.response);g('c').innerText = j['code'];g('s').innerText = j['str'];});}function fl() {let ex = g('ex');ex.parentElement.style.visibility = 'hidden';fg(ug, (x) => {let j = JSON.parse(x.response);if (j.length > 0) {ex.parentElement.style.visibility = 'visible';for (let i = 0; i < j.length; i++) {let v = j[i];let a = fe('a', v['name']);a.target = '_blank';a.href = v['link'];fa(ex, a);}}});}function fe(e, t) {let a = document.createElement(e);if (t) a.innerHTML = t;return a;}function fa(a, b) {a.appendChild(b);}function fi(a, h) {a.innerHTML = h;}function fut(a, b) {let tr = fe('tr');let x = fe('td');fi(x, a['name']);let y = fe('td');fi(y, b);let z = fe('td');fi(z, `ip:${a['ip']}<br>子网掩码:${a['netmask']}<br>网关:${a['gateway']}`);let o = fe('td');if ('STA' === b) {let p = fe('button');fi(p, '忘记');fa(o, p);fb(p, () => {fg(ue, (x) => {let f = parseInt(x.response) == 1;alert(f ? '忘记wifi成功' : '忘记wifi失败');if (f) {location.reload();}});});} else {let an = fe('input');let ap = fe('input');let ab = fe('button');fi(ab, '修改AP');fa(o, fe('span', 'AP名称:'));fa(o, an);fa(o, fe('br'));fa(o, fe('span', 'AP密码:'));fa(o, ap);fa(o, fe('br'));fa(o, ab);fb(ab, () => {fp(uf, `ap=${an.value}&pwd=${ap.value}`, (x) => {let f = parseInt(x.response) == 1;alert(f ? 'AP修改成功' : 'AP修改失败,检查AP密码是否够8位');if (f) {location.reload();}});});}fa(tr, x);fa(tr, y);fa(tr, z);fa(tr, o);return tr;}function fn() {fg(uc, (x) => {let j = JSON.parse(x.response);let r = g('tn');let a = j['ap'];r.innerHTML = '';if (a) {fa(r, fut(a, 'AP'));}let s = j['sta'];if (s) {fa(r, fut(s, 'STA'));}});}function fw() {fp(ud, `ssid=${wn.value}&pwd=${wp.value}`, (x) => {let f = parseInt(x.response) == 1;alert(f ? '连接wifi成功' : '连接wifi失败,检查wifi账号和密码');if (f) {location.reload();}});}fb(bw, fw);fo.addEventListener('input', (e) => {let fl = fo.files[0].size;fub(fl > 0);});fv();fn();fl();</script></body></html>";
        size_t len = strlen(html);

        char buf[128];
        sprintf(buf, "%d", len);
        response->setHeader(response, "Content-Type", "text/html; charset=UTF-8");
        response->setHeader(response, "Content-Length", buf);
        response->write(response, html, len);
        save_html(page, html);
    }

    return 1;
}

static int doOtaUpg(genki_web_request_t *request, genki_web_response_t *response) {
//    printf("do ota upg\r\n");
//    printf("url: %s\r\n", request->url);
//    genki_web_map_iterator_t iterator = request->headerIterator(request);
//    while (request->headerNext(&iterator)) {
//        printf("%s: %s\r\n", iterator.key, iterator.value);
//    }
    if (request->method == GET) return -1;

    int contentLength = 0;
    char *cl = request->getHeader(request, "Content-Length");
    if (cl == NULL) {
        return -1;
    }
    contentLength = atoi(cl);
    char buf[1024];
    int num;
    unsigned int ret;
    int recv_len = 0;

    while (recv_len < contentLength) {
        num = request->read(request, buf, 1024);

        ret = hi_upg_transmit(recv_len, buf, num);
        if (ret != 0) {
            response->setHeader(response, "Content-Length", "4");
            sprintf(buf, "%d", -101);
            response->write(response, buf, strlen(buf));
            break;
        }

        recv_len += num;
    }

    if (ret == 0) {
        ret = hi_upg_transmit_finish();
        if (ret != 0) {
            response->setHeader(response, "Content-Length", "4");
            sprintf(buf, "%d", -102);
            response->write(response, buf, strlen(buf));
        } else {
            response->setHeader(response, "Content-Length", "1");
            sprintf(buf, "%d", 1);
            response->write(response, buf, strlen(buf));
            usleep(1000 * 1000);
            hi_upg_finish();
        }
    }

    return 1;
}

static int doWifi(genki_web_request_t *request, genki_web_response_t *response) {
    if (request->method == GET) return -1;

    int contentLength = 0;
    char *cl = request->getHeader(request, "Content-Length");
    if (cl == NULL) {
        return -1;
    }
    contentLength = atoi(cl);

    if (contentLength > 1024) {
        response->write(response, "-1", 2);
        return -1;
    }
    char buf[contentLength + 1];
    int num = request->read(request, buf, contentLength);
    if (num <= 0) {
        response->write(response, "-1", 2);
        return -1;
    }

    buf[contentLength] = '\0';
    printf("%s %d\r\n", buf, num);
    char *p1 = strtok(buf, "&");
    if (p1) {
        char *p2 = strtok(NULL, "");

        char *key1 = strtok(p1, "=");
        char *value1 = strtok(NULL, "");

        char *key2 = strtok(p2, "=");
        char *value2 = strtok(NULL, "");

        char ssid[32] = {0};
        char password[64] = {0};
        unsigned char flag = 1;
        if (strcasecmp(key1, "ssid") == 0 && strcasecmp(key2, "pwd") == 0) {
            memcpy(ssid, value1, strlen(value1));
            memcpy(password, value2, strlen(value2));
        } else if (strcasecmp(key2, "ssid") == 0 && strcasecmp(key1, "pwd") == 0) {
            memcpy(ssid, value2, strlen(value2));
            memcpy(password, value1, strlen(value1));
        } else {
            flag = 0;
        }

        if (flag) {
            printf("ssid: %s   pwd: %s\r\n", ssid, password);
            if (wifi_sta_connect(ssid, password, "GenkiPI") == 0) {
                // connect success
                wifi_sta_set_cache(ssid, password);

                response->write(response, "1", 1);
            } else {
                response->write(response, "-3", 2);
            }
        } else {
            response->write(response, "-2", 2);
        }
    } else {
        response->write(response, "-1", 2);
    }
    return 1;
}

static int doStaClear(genki_web_request_t *request, genki_web_response_t *response) {
    if (wifi_sta_is_connected()) {
        wifi_sta_disconnect();
        wifi_sta_clear_cache();
    }
    response->write(response, "1", 1);
    return 1;
}

static void reboot_task(void) {
    usleep(3000000);
    hi_soft_reboot(5);
}

static int doAp(genki_web_request_t *request, genki_web_response_t *response) {
    if (request->method == GET) return -1;

    int contentLength = 0;
    char *cl = request->getHeader(request, "Content-Length");
    if (cl == NULL) {
        return -1;
    }
    contentLength = atoi(cl);

    if (contentLength > 1024) {
        response->write(response, "-1", 2);
        return -1;
    }
    char buf[contentLength + 1];
    int num = request->read(request, buf, contentLength);
    if (num <= 0) {
        response->write(response, "-1", 2);
        return -1;
    }

    buf[contentLength] = '\0';
    printf("%s %d\r\n", buf, num);
    char *p1 = strtok(buf, "&");
    if (p1) {
        char *p2 = strtok(NULL, "");

        char *key1 = strtok(p1, "=");
        char *value1 = strtok(NULL, "");

        char *key2 = strtok(p2, "=");
        char *value2 = strtok(NULL, "");

        char ap[32] = {0};
        char password[64] = {0};
        unsigned char flag = 1;
        if (strcasecmp(key1, "ap") == 0 && strcasecmp(key2, "pwd") == 0) {
            memcpy(ap, value1, strlen(value1));
            memcpy(password, value2, strlen(value2));
        } else if (strcasecmp(key2, "ap") == 0 && strcasecmp(key1, "pwd") == 0) {
            memcpy(ap, value2, strlen(value2));
            memcpy(password, value1, strlen(value1));
        } else {
            flag = 0;
        }

        if (flag) {
            printf("ap: %s   pwd: %s\r\n", ap, password);
            if (strlen(password) < 8) {
                response->write(response, "-3", 2);
            } else {
                wifi_ap_set_cache(ap, password);

                response->write(response, "1", 1);

                osThreadAttr_t attr;
                attr.name = "reboot";
                attr.attr_bits = 0U;
                attr.cb_mem = NULL;
                attr.cb_size = 0U;
                attr.stack_mem = NULL;
                attr.stack_size = 1024;
                attr.priority = 25;

                if (osThreadNew((osThreadFunc_t) reboot_task, NULL, &attr) == NULL) {
                    printf("Create web task Failed!\r\n");
                }
            }
        } else {
            response->write(response, "-2", 2);
        }
    } else {
        response->write(response, "-1", 2);
    }
    return 1;
}

genki_web_service_t *newBaseService(void) {
    genki_web_service_t *service = genki_web_newService("BASE");
    service->addFilter(service, "*", doIndexHtml);
    service->addFilter(service, "/info", doInfo);
    service->addFilter(service, "/wifi/info", doWifiInfo);
    service->addFilter(service, "/favicon.ico", doIcon);
    service->addFilter(service, "/ota/upg", doOtaUpg);
    service->addFilter(service, "/wifi", doWifi);
    service->addFilter(service, "/sta/clear", doStaClear);
    service->addFilter(service, "/ap", doAp);

    return service;
}