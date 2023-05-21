
#include "genki_web_ohosdog.h"

#include <stdio.h>
#include "utils_file.h"
#include <stdint.h>
#include <stdlib.h>
#include "dog.h"

static void save_html(const char *page, const char *html) {
    int fd = UtilsFileOpen(page, O_CREAT_FS | O_TRUNC_FS | O_WRONLY_FS, 0);

    if (fd > 0) {
        UtilsFileWrite(fd, html, strlen(html));
        UtilsFileClose(fd);
    }
}

static int doHtml(genki_web_request_t *request, genki_web_response_t *response) {
    if (request->method != GET) {
        return -1;
    }
    const char *page = "plot.html";
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
        const char *html = "<!DOCTYPE html> <html lang=\"en\"><head><meta charset=\"UTF-8\"><title>奥特曼的狗</title><script src=\"https://cdn.staticfile.org/vue/2.2.2/vue.min.js\"></script><style> .bounce-enter-active { animation: bounce-in .5s; } .bounce-leave-active { animation: bounce-in .5s reverse;   }   @keyframes bounce-in {    0% {     transform: scale(0);    }    50% {     transform: scale(1.5); } 100% { transform: scale(1); }   }    body{    user-select: none;    text-align: center;    background: #05386b;  /* fallback for old browsers */    background: -webkit-linear-gradient(to right, #5cdb95, #05386b);  /* Chrome 10-25, Safari 5.1-6 */    background: linear-gradient(to bottom right, #5cdb95, #05386b); /* W3C, IE 10+/ Edge, Firefox 16+, Chrome 26+, Opera 12+, Safari 7+ */    background-size:100% 200%;    background-repeat:no-repeat;   }   h1   {    text-shadow: 2px 2px 3px grey;    width:300px;    height:50px;    background:#7ec6bc;    transition:width 2s;    -webkit-transition:width 2s; /* Safari */   }   h1:hover   {    width:800px;    animation:myfirst 2s;    -webkit-animation:myfirst 5s; /* Safari and Chrome */    box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24),0 17px 50px 0 rgba(0,0,0,0.19);   }   @-webkit-keyframes myfirst /* Safari 与 Chrome */   {    from {background: #7ec6bc}    to {background: #4284db}   }    table{    margin: 0 auto   } td   div{    width: 150px;    height: 150px;    border: 1px solid darkgrey;    display: flex;    align-items: center;    justify-content: center;    color: black;   }   #s{    background-color: red;    color: white;   } td   div:active{    background: deepskyblue;    color: white;   }   button{    width: 9rem;    border-radius: 1rem;    border: 2px solid #7ec6bc;    color: #152331;    padding: 16px 32px;    text-align: center;    text-decoration: none;    display: inline-block;    font-size: 16px;    margin: 4px 2px;    -webkit-transition-duration: 0.4s; /* Safari */    transition-duration: 0.4s;    cursor: pointer;   }   button:hover{    background-color: #7ec6bc;    color: aliceblue;    box-shadow: 0 12px 16px 0 rgba(0,0,0,0.24),0 17px 50px 0 rgba(0,0,0,0.19);   }   button:active {    border: 2px solid #3e8e41;    border-radius: 0rem;    background-color: #3e8e41;    box-shadow: 0 5px #666;    transform: translateY(4px);   }  </style> <script>  function myFunction(){  alert(\"全民制作人们大家好，我是练习时长两年半的个人练习生蔡徐坤，喜欢唱，跳，rap，篮球。music！！！\"); } </script>  </head>  <body>    <h1>鸿蒙狗控制器</h1>    <br><button id=\"a\">安装模式</button><br>  <br><button id=\"c\">蹲下</button> <button id=\"h\">起立</button><br>  <br><button id=\"i\">唱</button> <button id=\"j\">跳</button>   <button id=\"k\">Rap</button><br>    <br><button id=\"l\">篮球</button><br> <br><button id=\"d\">前进</button><br> <br><button id=\"f\">左转</button><button id=\"b\">原地踏步</button><button id=\"g\">右转</button><br><br><button id=\"e\">后退</button><br><br><button type=\"button\" onclick=\"myFunction()\" >这是什么 </button> <script> function u(p) {return `${p}`;}function g(i) {return document.getElementById(i);}function fe(e, f) {e.addEventListener('click', function (e) {/* do something*/f();});}function fg(u, f) {let x = new XMLHttpRequest();x.onreadystatechange = function () {if (this.readyState == 4 && this.status == 200) {f(x);}};x.open(\"GET\", u, true);x.send();}function fp(u, d, f, p) {let x = new XMLHttpRequest();if (p) {x.upload.addEventListener('progress', function (e) {p(e);});}x.onreadystatechange = function () {if (this.readyState == 4 && this.status == 200) {f(x);}};x.open('POST', u, true);x.timeout = 45000;x.send(d);}fe(g('a'), () => {fg(u('/dog/init'), () => {});});"
                           "fe(g('b'), () => {let data = {\"type\": 3, \"count\": 30, \"list\": [[31, 96, 41, 134, 50, 130, 19, 100, 50], [34, 108, 28, 126, 38, 123, 23, 112, 50], [49, 129, 18, 103, 29, 98, 39, 133, 50], [47, 130, 20, 102, 31, 98, 38, 133, 50], [30, 109, 31, 125, 41, 121, 19, 113, 50]]};fp(u('/dog/cmds'), JSON.stringify(data), () => {});});fe(g('c'), () => {let data = {\"type\": 3, \"count\": 30, \"list\": [[74, 111, 24, 151, 24, 151, 74, 111, 100], [24, 151, 74, 111, 74, 111, 24, 151, 100]]};fp(u('/dog/cmds'), JSON.stringify(data), () => {});});fe(g('d'), () => {let data = {\"type\": 3, \"count\": 30, \"list\": [[31, 96, 41, 134, 50, 130, 19, 100, 50], [34, 108, 14, 128, 24, 126, 23, 112, 50], [49, 129, 12, 104, 23, 101, 39, 133, 50], [42, 132, 20, 102, 31, 98, 33, 135, 50], [15, 114, 31, 125, 41, 121, 4, 115, 50]]};fp(u('/dog/cmds'), JSON.stringify(data), () => {});});fe(g('e'), () => {let data = {\"type\": 3, \"count\": 30, \"list\": [[63, 139, 23, 113, 34, 109, 54, 144, 50], [51, 131, 12, 126, 22, 125, 42, 135, 50], [35, 111, 46, 145, 54, 142, 24, 114, 50], [27, 113, 52, 143, 61, 138, 17, 116, 50], [35, 137, 30, 123, 40, 119, 26, 139, 50]]};fp(u('/dog/cmds'), JSON.stringify(data), () => {});});fe(g('f'), () => {let data = {\"type\": 3, \"count\": 30, \"list\": [[63, 139, 23, 113, 50, 130, 19, 100, 50], [51, 131, 12, 126, 24, 126, 23, 112, 50], [35, 111, 46, 145, 23, 101, 39, 133, 50], [42, 132, 20, 102, 31, 98, 33, 135, 50], [15, 114, 31, 125, 41, 121, 4, 115, 50]]};fp(u('/dog/cmds'), JSON.stringify(data), () => {});});fe(g('g'), () => {let data = {\"type\": 3, \"count\": 30, \"list\": [[31, 96, 41, 134, 34, 109, 54, 144, 50], [34, 108, 14, 128, 22, 125, 42, 135, 50], [49, 129, 12, 104, 54, 142, 24, 114, 50], [27, 113, 52, 143, 61, 138, 17, 116, 50], [35, 137, 30, 123, 40, 119, 26, 139, 50]]};fp(u('/dog/cmds'), JSON.stringify(data), () => {});});</script></body></html>";
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

static int doInit(genki_web_request_t *request, genki_web_response_t *response) {
    dog_install();

    return 1;
}

static int doCmds(genki_web_request_t *request, genki_web_response_t *response) {
    if (request->method == GET) return -1;

    int contentLength = 0;
    char *cl = request->getHeader(request, "Content-Length");
    if (cl == NULL) {
        return -1;
    }
    contentLength = atoi(cl);
    char buf[2048];
    int num;
    num = request->read(request, buf, 2048);

    dog_execJson(buf, num);

    return 1;
}


genki_web_service_t *newDogService(void) {
    genki_web_service_t *service = genki_web_newService("DOG");
    service->addFilter(service, "/dog", doHtml);
    service->addFilter(service, "/dog/init", doInit);
    service->addFilter(service, "/dog/cmds", doCmds);

    service->link_name = "Dog控制";
    service->link_url = "/dog";
    return service;
}