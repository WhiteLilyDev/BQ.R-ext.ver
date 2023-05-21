#include "at_io.h"

#include <hi_stdlib.h>
#include <hi_at.h>
#include <at.h>


hi_u32 at_hi_test(void) {
    printf("at cmd test!\r\n");
    return HI_ERR_SUCCESS;
}

static at_cmd_func g_at_test_func_tb[] = {
        {"+MYAT", 5, HI_NULL, HI_NULL, HI_NULL, (at_call_back_func) at_hi_test},
};

#define AT_TEST_FUNC_NUM (sizeof(g_at_test_func_tb) / sizeof(g_at_test_func_tb[0]))

void hi_at_test_cmd_register(void) {
    hi_at_register_cmd(g_at_test_func_tb, AT_TEST_FUNC_NUM);
}