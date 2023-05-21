#include "dog.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "pca9685.h"

#include "hi_nvm.h"
#include <hi_nv.h>
#include <hi_flash.h>
#include <hi_partition_table.h>

#include "cJSON.h"
#include "kinematics.h"

// left front leg
#define LF_A    4
#define LF_B    5
// left back leg
#define LB_A    10
#define LB_B    11
// right front leg
#define RF_A    3
#define RF_B    2
// right back leg
#define RB_A    13
#define RB_B    12


#define OFFSET_LF_A     45
#define OFFSET_LF_B     -45
#define OFFSET_LB_A     45
#define OFFSET_LB_B     -45

#define OFFSET_RF_A     45
#define OFFSET_RF_B     -45
#define OFFSET_RB_A     45
#define OFFSET_RB_B     -45

static int port_lf_a = LF_A;
static int port_lf_b = LF_B;
static int port_lb_a = LB_A;
static int port_lb_b = LB_B;
static int port_rf_a = RF_A;
static int port_rf_b = RF_B;
static int port_rb_a = RB_A;
static int port_rb_b = RB_B;

static int offset_lf_a = OFFSET_LF_A;
static int offset_lf_b = OFFSET_LF_B;
static int offset_lb_a = OFFSET_LB_A;
static int offset_lb_b = OFFSET_LB_B;
static int offset_rf_a = OFFSET_RF_A;
static int offset_rf_b = OFFSET_RF_B;
static int offset_rb_a = OFFSET_RB_A;
static int offset_rb_b = OFFSET_RB_B;


#define BASE_DELAY 10
static int current_angle[8] = {0, 0, 0, 0, 0, 0, 0};

void dog_init() {
    pca9685_servo_reset();
    pca9685_servo_init();
}

void dog_setRealAngle(int joint, int angle) {
    if (joint < 0 || joint > 15) return;
    pca9685_servo_set_angle(joint, angle);
}

void dog_setLeftFrontAngle(int alpha, int beta) {
    dog_setRealAngle(port_lf_a, offset_lf_a + alpha);
    dog_setRealAngle(port_lf_b, offset_lf_b + beta);
    current_angle[0] = alpha;
    current_angle[1] = beta;
}

void dog_setLeftBackAngle(int alpha, int beta) {
    dog_setRealAngle(port_lb_a, offset_lb_a + alpha);
    dog_setRealAngle(port_lb_b, offset_lb_b + beta);
    current_angle[2] = alpha;
    current_angle[3] = beta;
}

void dog_setRightFrontAngle(int alpha, int beta) {
    dog_setRealAngle(port_rf_a, 180 - (offset_rf_a + alpha));
    dog_setRealAngle(port_rf_b, 180 - (offset_rf_b + beta));
    current_angle[4] = alpha;
    current_angle[5] = beta;
}

void dog_setRightBackAngle(int alpha, int beta) {
    dog_setRealAngle(port_rb_a, 180 - (offset_rb_a + alpha));
    dog_setRealAngle(port_rb_b, 180 - (offset_rb_b + beta));
    current_angle[6] = alpha;
    current_angle[7] = beta;
}


static int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void dog_install(void) {
    dog_setLeftFrontAngle(90, 180);
    dog_setLeftBackAngle(90, 180);
    dog_setRightFrontAngle(90, 180);
    dog_setRightBackAngle(90, 180);
}

//自由控制四只腿的运动
void dog_execfree(int LeftFrontAngle[],int LeftBackAngle[],int RightFrontAngle[],int RightBackAngle[]) {
    dog_setLeftFrontAngle(LeftFrontAngle[0],LeftFrontAngle[1]);
    dog_setLeftBackAngle(LeftBackAngle[0], LeftBackAngle[1]);
    dog_setRightFrontAngle(RightFrontAngle[0], RightFrontAngle[1]);
    dog_setRightBackAngle(RightBackAngle[0], RightBackAngle[1]);
}

void dog_execAction(dog_action_t *action) {

    for (int i = 0; i < action->exec_count; ++i) {
        for (int j = 0; j < action->len; ++j) {
            dog_cmd_t cmd = action->cmds[j];
            int duration = cmd.duration;

            int delay_counter = duration / BASE_DELAY;
            for (int k = 0; k < delay_counter; ++k) {
                int v[8] = {0};
                for (int l = 0; l < 8; ++l) {
                    int current = current_angle[l];
                    int dst = cmd.angle[l];

                    v[l] = current;
                    if (current > dst) {
                        int value = map(BASE_DELAY * k, 0, duration, 0, current - dst);

                        if (current - value >= dst) {
                            v[l] = current - value;
                        }
                    } else if (current < dst) {
                        int value = map(BASE_DELAY * k, 0, duration, 0, dst - current);

                        if (current + value <= dst) {
                            v[l] = current + value;
                        }
                    }
                }

                dog_setLeftFrontAngle(v[0], v[1]);
                dog_setLeftBackAngle(v[2], v[3]);
                dog_setRightFrontAngle(v[4], v[5]);
                dog_setRightBackAngle(v[6], v[7]);
                usleep(BASE_DELAY * 1000);
            }
        }
    }

}

void dog_execJson(char *buf, int len) {
    cJSON *recv_json;
    recv_json = cJSON_ParseWithLength(buf, len);

    int type = cJSON_GetObjectItem(recv_json, "type")->valueint;
    if (type == 1) {
        dog_install();
    } else if (type == 2) {
        int index = cJSON_GetObjectItem(recv_json, "index")->valueint;

        cJSON *data_json;
        data_json = cJSON_GetObjectItem(recv_json, "data");
        int alpha = cJSON_GetArrayItem(data_json, 0)->valueint;
        int beta = cJSON_GetArrayItem(data_json, 1)->valueint;

        if (index == 0) {
            dog_setLeftFrontAngle(alpha, beta);
        } else if (index == 1) {
            dog_setLeftBackAngle(alpha, beta);
        } else if (index == 2) {
            dog_setRightFrontAngle(alpha, beta);
        } else if (index == 3) {
            dog_setRightBackAngle(alpha, beta);
        }
    } else if (type == 3) {
        int execute_count = 1;
        if (cJSON_HasObjectItem(recv_json, "count")) {
            execute_count = cJSON_GetObjectItem(recv_json, "count")->valueint;
        }

        dog_action_t *action = malloc(sizeof(dog_action_t));
        action->exec_count = execute_count;

        // 发送多条
        cJSON *list_json;
        list_json = cJSON_GetObjectItem(recv_json, "list");
        int count = cJSON_GetArraySize(list_json);

        action->len = count;
        action->cmds = malloc(sizeof(dog_cmd_t) * count);

        for (int i = 0; i < count; i++) {
            cJSON *item_json;
            item_json = cJSON_GetArrayItem(list_json, i);

            dog_cmd_t cmd;
            cmd.angle[0] = cJSON_GetArrayItem(item_json, 0)->valueint;
            cmd.angle[1] = cJSON_GetArrayItem(item_json, 1)->valueint;
            cmd.angle[2] = cJSON_GetArrayItem(item_json, 2)->valueint;
            cmd.angle[3] = cJSON_GetArrayItem(item_json, 3)->valueint;
            cmd.angle[4] = cJSON_GetArrayItem(item_json, 4)->valueint;
            cmd.angle[5] = cJSON_GetArrayItem(item_json, 5)->valueint;
            cmd.angle[6] = cJSON_GetArrayItem(item_json, 6)->valueint;
            cmd.angle[7] = cJSON_GetArrayItem(item_json, 7)->valueint;
            cmd.duration = cJSON_GetArrayItem(item_json, 8)->valueint;

            action->cmds[i] = cmd;
        }
        dog_execAction(action);
    }
    cJSON_Delete(recv_json);
}

void dog_leg_move(int index, double x, double y) {
    struct Point_t point = {x, y};
    double *angle = inverse(point);

    if (angle) {
        double alpha = angle[0];
        double beta = angle[1];
        dog_leg_angle(index, alpha, beta);
    } else {
        printf("inverse failed!\n");
    }
    free(angle);
}

void dog_leg_angle(int index, double alpha, double beta) {
    if (index == 0) {
        dog_setLeftFrontAngle((int) alpha, (int) beta);
    } else if (index == 1) {
        dog_setLeftBackAngle((int) alpha, (int) beta);
    } else if (index == 2) {
        dog_setRightFrontAngle((int) alpha, (int) beta);
    } else if (index == 3) {
        dog_setRightBackAngle((int) alpha, (int) beta);
    }
}