
#ifndef GENKI_THREAD_H
#define GENKI_THREAD_H


#include "ohos_init.h"
#include "cmsis_os2.h"

int GenkiThread_run(osThreadFunc_t func, void *argument);

int GenkiThread_run2(osThreadFunc_t func, void *argument, unsigned int size);

#endif //GENKI_THREAD_H
