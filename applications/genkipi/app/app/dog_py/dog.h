
#ifndef GENKI_DOG_DOG_H
#define GENKI_DOG_DOG_H


typedef struct {
    int angle[8];
    int duration;
} dog_cmd_t;

typedef struct {
    int exec_count;
    dog_cmd_t* cmds;
    int len;
} dog_action_t;


void dog_init(void);

void dog_setRealAngle(int joint, int angle);

void dog_setLeftFrontAngle(int alpha, int beta);
void dog_setLeftBackAngle(int alpha, int beta);
void dog_setRightFrontAngle(int alpha, int beta);
void dog_setRightBackAngle(int alpha, int beta);

void dog_execAction(dog_action_t* action);
void dog_execJson(char* str, int len);

void dog_install(void);
void dog_execfree(int LeftFrontAngle[],int LeftBackAngle[],int RightFrontAngle[],int RightBackAngle[]);

void dog_leg_move(int index, double x, double y);

void dog_leg_angle(int index, double alpha, double beta);

#endif //GENKI_DOG_DOG_H
