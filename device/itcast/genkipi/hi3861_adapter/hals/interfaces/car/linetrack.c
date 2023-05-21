#include "genki/car/linetrack.h"
#include "xl9555.h"

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "genki/car/config.h"


static unsigned char is_init = 0;
static unsigned char is_enable_state = 2; // 0:disable , 1:enable, 2:do not check
static osMutexId_t mutex_id = NULL;


static void lock_i2c(void) {
    osMutexAcquire(mutex_id, osWaitForever);
}

static void unlock_i2c(void) {
    osMutexRelease(mutex_id);
}


static void init(void) {
    if (is_init) return;

    // get mutex lock id
    mutex_id = CarConfig_getI2cMutexId();

    if (!CarLinetrack_isEnable()) return;

    XL9555_config_all_dir(0xffff);

    is_init = 1;
}


unsigned char CarLinetrack_isEnable(void) {
    if (is_enable_state != 2) {
        return is_enable_state;
    }

    lock_i2c();
    if (XL9555_is_enable() == XL9555_SUCCESS) {
        is_enable_state = 1;
        unlock_i2c();
        return 1;
    }
    unlock_i2c();
    is_enable_state = 0;
    return 0;
}

unsigned char CarLinetrack_getState(unsigned short *state) {
    init();

    lock_i2c();
    unsigned short val;
    unsigned int ret = XL9555_get_all_input(&val);
    unlock_i2c();

    if (ret == XL9555_SUCCESS) {
        *state = val;
        return 1;
    }
    return 0;
}