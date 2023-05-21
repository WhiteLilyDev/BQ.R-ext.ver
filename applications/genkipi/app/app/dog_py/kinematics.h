
#ifndef KINEMATICS_H
#define KINEMATICS_H


typedef struct Point_t {
    double x;
    double y;
} Point_t;

typedef struct Vec_t {
    double x;
    double y;
} Vec_t;

typedef struct Points_t {
    int len;
    struct Point_t* points;
} Points_t;


typedef int (*kinematic_inverse_cb)(double alpha, double beta);


double* inverse(struct Point_t temp_point_e);

void trajectory_circle(struct Point_t center, double radius, kinematic_inverse_cb cb);

#endif //KINEMATICS_H
