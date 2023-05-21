#include "kinematics.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


#define RATIO(x) x * 3.0
static struct Point_t point_o = {RATIO(0), RATIO(0)};
static struct Point_t point_b = {RATIO(13.0), RATIO(0)};
static struct Point_t point_e = {RATIO(5.0), RATIO(66.0)};

static double theta = 119.0;

static double link_ob = RATIO(13.0);
static double link_bc = RATIO(27.0);
static double link_oa = RATIO(27.0);
static double link_ad = RATIO(35.0);
static double link_cd = RATIO(27.0);
static double link_de = RATIO(30.0);

#define deg2rad(deg) (M_PI * deg / 180.0)
#define rad2deg(rad) (rad * 180.0 / M_PI)
#define link_ae sqrt(link_ad * link_ad + link_de * link_de - 2 * link_ad * link_de * cos(M_PI * theta / 180.0))


static double calc_radians_between_vector(struct Vec_t a, struct Vec_t b) {
    return atan2(a.x * b.y - a.y * b.x, a.x * b.x + a.y * b.y);
}


/**
 *
 * @param p0: 圆0的圆心
 * @param p1: 圆1的圆心
 * @param r0: 圆0的半径
 * @param r1: 圆1的半径
 * @return: 交点
 */
static struct Points_t* calculate_intersection(struct Point_t * p0, struct Point_t * p1, double r0, double r1) {
    struct Points_t* points = malloc(sizeof(struct Points_t));

    // 圆心距离
    double d = sqrt(pow(p0->x - p1->x, 2) + pow(p0->y - p1->y, 2));
    // 半径之和
    double r = r0 + r1;

    // 一个圆在另一个圆内，则没有交点
    if (d < fabs(r0 - r1)) {
        points->len = 0;
        return points;
    }

    // 如果两圆相离，则没有交点
    if (d > r) {
        points->len = 0;
        return points;
    }

    // 如果两圆相切，则有一个交点
    if (d == r) {
        points->len = 1;
        points->points = malloc(sizeof(struct Point_t));
        points->points->x = p0->x + (p1->x - p0->x) * (r0 / r);
        points->points->y =  p0->y + (p1->y - p0->y) * (r0 / r);
        return points;
    }
    // 如果两圆相交，则有两个交点
    double a = (r0 * r0 - r1 * r1 + d * d) / (2 * d);

    double h0 = sqrt(r0 * r0 - a * a);

    double p2x = p0->x + (p1->x - p0->x) * a / d;
    double p2y = p0->y + (p1->y - p0->y) * a / d;

    double x3 = p2x + h0 * (p1->y - p0->y) / d;
    double y3 = p2y - h0 * (p1->x - p0->x) / d;

    double x4 = p2x - h0 * (p1->y - p0->y) / d;
    double y4 = p2y + h0 * (p1->x - p0->x) / d;

    points->len = 2;
    points->points = malloc(2 * sizeof(struct Point_t));
    struct Point_t p3;
    p3.x = x3;
    p3.y = y3;
    struct Point_t p4;
    p4.x = x4;
    p4.y = y4;

    memcpy(points->points, &p3, sizeof(Point_t));
    memcpy(&(points->points)[1], &p4, sizeof(Point_t));

    return points;
}

double* inverse(struct Point_t temp_point_e)
{
    temp_point_e.x = RATIO(temp_point_e.x);
    temp_point_e.y = RATIO(temp_point_e.y);
    // 计算左交点
    struct Points_t *intersection_left_points = calculate_intersection(&point_o, &temp_point_e, link_oa, link_ae);
    if(intersection_left_points->len == 0) {
        printf("------------求左交点失败！\n");
        return NULL;
    }

    struct Point_t point_a;
    point_a = intersection_left_points->points[0];
    if (intersection_left_points->len == 2) {
        if (intersection_left_points->points[1].x < intersection_left_points->points[0].x) {
            point_a = intersection_left_points->points[1];
        }
    }
    free(intersection_left_points->points);
    free(intersection_left_points);

    // 计算点D位置
    struct Points_t *d_points = calculate_intersection(&point_a, &temp_point_e, link_ad, link_de);
    if (d_points->len == 0) {
        printf("------------求交点D失败！\n");
        return NULL;
    }
    // 向量AE要在向量AD的顺时针位(AD方向的右侧）
    struct Vec_t vec_ae;
    vec_ae.x = temp_point_e.x - point_a.x;
    vec_ae.y = temp_point_e.y - point_a.y;

    double dx = d_points->points[0].x;
    double dy = d_points->points[0].y;
    struct Vec_t vec_ad;
    vec_ad.x = dx - point_a.x;
    vec_ad.y = dy - point_a.y;

    double vmin = calc_radians_between_vector(vec_ae, vec_ad);
    struct Point_t point_d;
    point_d = d_points->points[0];

    for (int i = 1; i < d_points->len; ++i) {
        dx = d_points->points[i].x;
        dy = d_points->points[i].y;
        vec_ad.x = dx - point_a.x;
        vec_ad.y = dy - point_a.y;

        double v = calc_radians_between_vector(vec_ae, vec_ad);
        if (v < vmin) {
            vmin = v;

            point_d = d_points->points[i];
        }
    }
    free(d_points->points);
    free(d_points);

    // 计算右交点
    struct Points_t *intersection_right_points = calculate_intersection(&point_d, &point_b, link_cd, link_bc);
    if(intersection_right_points->len == 0) {
        printf("------------求右交点失败！\n");
        return NULL;
    }

    struct Point_t point_c;
    point_c = intersection_right_points->points[0];
    if (intersection_right_points->len == 2) {
        if (intersection_right_points->points[1].x > intersection_right_points->points[0].x) {
            point_c = intersection_right_points->points[1];
        }
    }
    free(intersection_right_points->points);
    free(intersection_right_points);

    struct Vec_t vec_oa;
    vec_oa.x = point_a.x - point_o.x;
    vec_oa.y = point_a.y - point_o.y;

    struct Vec_t vec_x_negative;
    vec_x_negative.x = -1;
    vec_x_negative.y = 0;

    struct Vec_t vec_bc;
    vec_bc.x = point_c.x - point_b.x;
    vec_bc.y = point_c.y - point_b.y;

    struct Vec_t vec_x_positive;
    vec_x_positive.x = 1;
    vec_x_positive.y = 0;

    double alpha = calc_radians_between_vector(vec_oa, vec_x_negative);
    double beta = calc_radians_between_vector(vec_bc, vec_x_negative);

    double * ret = malloc(2 * sizeof(double));
    ret[0] = rad2deg(alpha);
    ret[1] = rad2deg(beta);
    return ret;
}


void trajectory_circle(struct Point_t center, double radius, kinematic_inverse_cb cb) {

    int slice_count = (int)(radius * 10);
    double step = 2.0 * M_PI / (slice_count - 1);

    int sleep_time = (int)(1.0 * 1000 * 1000  / slice_count);

    double current = 0;
    for (int i = 0; i < slice_count; ++i) {
        double x = center.x + radius * cos(current);
        double y = center.y + radius * sin(current);

        struct Point_t end;
        end.x = x;
        end.y = y;
        double* value = inverse(end);
        if (value) {
            cb(value[0], value[1]);

            usleep(sleep_time);

            free(value);
        } else {
            printf("inverse failed\n");
        }

        current += step;
    }

}

