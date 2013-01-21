/*
 *  Copyright (C) 2012-2013 Evgeny Panov
 *  This file is part of libvrender.
 *
 *  libvrender is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libvrender is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libvrender.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include "common.h"
#include "math/vector.h"

typedef float matrixf_t;

typedef matrixf_t matrix4f[4][4];
typedef matrixf_t matrix3f[3][3];
typedef matrixf_t matrix2f[2][2];

#ifdef __cplusplus
extern "C" {
#endif

/* Инициализировать матрицу и установить значения главной диагонали (main_d) */
void mat4(matrix4f out, float main_d);
void mat3(matrix3f out, float main_d);
void mat2(matrix2f out, float main_d);

/* Копирование из матрицы from в матрицу to */
void mat4_copy(matrix4f from, matrix4f to);
void mat3_copy(matrix3f from, matrix3f to);
void mat2_copy(matrix2f from, matrix2f to);

/* Умножение матриц */
void mat4_mult2(matrix4f out, matrix4f m1, matrix4f m2);
void mat3_mult2(matrix3f out, matrix3f m1, matrix3f m2);

/* Множественное умножение матриц */
void mat4_mult3(matrix4f out, matrix4f m1, matrix4f m2, matrix4f m3);
void mat3_mult3(matrix3f out, matrix3f m1, matrix3f m2, matrix3f m3);
void mat4_mult4(matrix4f out, matrix4f m1, matrix4f m2, matrix4f m3, matrix4f m4);

/* Умножить матрицу m на матрицу m2. Результат будет в матрице m */
void mat4_mult_m(matrix4f m, matrix4f m2);

/* Умножение матрицы на число c */
void mat4_mult_c(matrix4f out, matrix4f m, float c);
void mat3_mult_c(matrix3f out, matrix3f m, float c);

/* Умножить матрицу m на число c и поместить результат в m */
void mat4_mult_c_m(matrix4f m, float c);
void mat3_mult_c_m(matrix3f m, float c);

/* Умножить матрицу на вектор */
vector4f mat4_mult_vec4(matrix4f m, vector4f v);
vector3f mat4_mult_vec3(matrix4f m, vector3f v);
vector3f mat3_mult_vec3(matrix3f m, vector3f v);

/* Сложение матрицы m1 и m2 */
void mat4_add(matrix4f out, matrix4f m1, matrix4f m2);
void mat3_add(matrix3f out, matrix3f m1, matrix3f m2);

/* Вычитание из матрицы m1 матрицы m2 */
void mat4_sub(matrix4f out, matrix4f m1, matrix4f m2);
void mat3_sub(matrix3f out, matrix3f m1, matrix3f m2);

/* Транспонировать матрицу m */
void mat4_transponse(matrix4f m);
void mat3_transponse(matrix3f m);

/** 
 * Подмат матрицы m (подмат(i, j)M - матрица собранная 
 * из всех строк и колонок матрицы m, но без  
 * строки i и колонки j
 */
void mat4_submat(matrix3f out, int i, int j, matrix4f m);
void mat3_submat(matrix2f out, int i, int j, matrix3f m);

/* Определитель матрицы */
float mat4_det(matrix4f m);
float mat3_det(matrix3f m);
float mat2_det(matrix2f m);

/* Создать обратную матрицу */
int mat4_inverse(matrix4f out, matrix4f m);
int mat3_inverse(matrix3f out, matrix3f m);

/* Создать матрицу переноса */
void mat4_translate(matrix4f out, float x, float y, float z);

/* Создать матрицу масштабирования */
void mat4_scale(matrix4f out, float x, float y, float z);

/* Создать матрицу вращения на угол angle вокруг оси*/
void mat4_rotate_x(matrix4f out, float angle);
void mat4_rotate_y(matrix4f out, float angle);
void mat4_rotate_z(matrix4f out, float angle);

/* Создать матрицу вращения на угол angle вокруг произвольной оси axis */
void mat4_rotate_axis(matrix4f out, float angle, vector3f axis);

/* Создать матрицу объёма отсечения */
void mat4_frustum(matrix4f out, float left, 
				  float right, float bottom, 
				  float top,   float z_near, 
				  float z_far);

/* Создать матрицу ортогональной проекции */
void mat4_ortho(matrix4f out, float left, 
				  float right, float bottom, 
				  float top,   float z_near, 
				  float z_far);

/**
 * Создать lookat-матрицу
 * target_point - точка, на которую нужно направить вид
 */
void mat4_lookat(matrix4f out, vector3f eye_position, 
				 vector3f target_point, vector3f up_dir);

/**
 * Создать матрицу вида
 * target_dir - направление взгляда
 */			 
void mat4_view(matrix4f out, vector3f eye_position, 
				 vector3f target_dir, vector3f up_dir);

/* Создать матрицу перспективной проекции */
void mat4_perspective(matrix4f out, float fov, float aspect, float z_near, float z_far);

/** 
 * Работают также как и их предшественники, 
 * но при этом m умножают на получившуюся матрицу.
 * Результат оказывается в матрице m
 */
void mat4_rotate_axis_mult(matrix4f m, float angle, vector3f axis);
void mat4_frustum_mult(matrix4f m, float left, 
				  float right, float bottom, 
				  float top,   float z_near, 
				  float z_far);
void mat4_ortho_mult(matrix4f m, float left, 
				  float right, float bottom, 
				  float top,   float z_near, 
				  float z_far);
void mat4_lookat_mult(matrix4f m, vector3f eye, 
				 vector3f center, vector3f up);
void mat4_perspective_mult(matrix4f m, float fov, float aspect, float z_near, float z_far);

/* Печатать на консоль элементы матрицы */
void mat4_print(matrix4f m);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_H_INCLUDED */
