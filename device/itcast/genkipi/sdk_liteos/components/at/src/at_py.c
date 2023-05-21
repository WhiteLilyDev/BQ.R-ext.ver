#include "at_io.h"

#include <hi_stdlib.h>
#include <hi_at.h>
#include <at.h>

hi_u32 at_hi_pyrun(hi_s32 argc, const hi_char *argv[]) {
    hi_at_printf("argc: %d\r\n", argc);

    for (int i = 0; i < argc; ++i) {
        hi_at_printf("%d: %s\r\n", argc, argv[i]);
    }

    hi_at_printf("OK\r\n");

    return HI_ERR_SUCCESS;
}

static at_cmd_func g_at_py_func_tb[] = {
        {"+PYRUN", 6, HI_NULL, HI_NULL, (at_call_back_func) at_hi_pyrun, (at_call_back_func) at_hi_pyrun},
};

#define AT_PY_FUNC_NUM (sizeof(g_at_py_func_tb) / sizeof(g_at_py_func_tb[0]))

void hi_at_py_cmd_register(void) {
    hi_at_register_cmd(g_at_py_func_tb, AT_PY_FUNC_NUM);
}