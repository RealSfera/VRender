/*
 *  Copyright (C) 2012 Evgeny Panov
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

#ifndef QUATERNION_H_INCLUDED
#define QUATERNION_H_INCLUDED

#include "common.h"
#include "math/vector.h"
#include "math/matrix.h"

typedef struct {
	float x, y, z, w;
} quaternion;

typedef struct {
	quaternion q1, q2;
} quaternion_lerp_t;

typedef struct {
	quaternion q1, q2;
	float omega;
} quaternion_slerp_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Инициализировать кватернион значениями */
SINLINE quaternion quat_init(float x, float y, float z, float w)
{
	quaternion result;
	
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;

	return result;
}

/* Преобразовать кватернион в вектор (если vec3, то w отбрасывается) */
SINLINE quaternion quat_init_vec3(vector3f v, float w)
{
	return quat_init(v.x, v.y, v.z, w);
}

SINLINE quaternion quat_init_vec4(vector4f v)
{
	return quat_init(v.x, v.y, v.z, v.w);
}

/* Сложение кватернионов */
SINLINE quaternion quat_add(quaternion q1, quaternion q2)
{
	return quat_init(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
}

/* Вычесть из кватерниона q1 кватернион q2 */
SINLINE quaternion quat_sub(quaternion q1, quaternion q2)
{
	return quat_init(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
}

SINLINE vector4f quat_to_vec4(quaternion q)
{
	return vec4f(q.x, q.y, q.z, q.w);
}

SINLINE vector3f quat_to_vec3(quaternion q)
{
	return vec3f(q.x, q.y, q.z);
}

/* Сравнение двух кватернионов на равенство */
SINLINE int quat_compare_equal(quaternion q1, quaternion q2)
{
	if(q1.x == q2.x && q1.y == q2.y && q1.z == q2.z && q1.w == q2.w)
		return 1;
	
	return 0;
}

/* Умножение кватернионов */
quaternion quat_mult(quaternion q1, quaternion q2);

/* Умножение кватернионов */
vector3f quat_mult_vec3(quaternion q, vector3f v);

/* Умножение кватерниона (по-элементно) на число c */
SINLINE quaternion quat_mult_c(quaternion q, float c)
{
	return quat_init(q.x * c, q.y * c, q.z * c, q.w * c);
}

/* Норма кватерниона N(q) = xx + yy + xx + ww */
SINLINE float quat_norm(quaternion q)
{
	return q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
}

/* Модуль (длина) кватерниона */
SINLINE float quat_magnitude(quaternion q)
{
	return math_sqrtf( quat_norm(q) );
}

/* Нормализация кватерниона */
SINLINE quaternion quat_normalize(quaternion q)
{
	float magnitude = quat_magnitude(q);
	return quat_mult_c(q, 1 / magnitude);
}

/* Сопряжение кватерниона */
SINLINE quaternion quat_conjugate(quaternion q)
{
	// conjugate(q) = (-v, w)
	return quat_init(-q.x, -q.y, -q.z, q.w );
}

/* Инверсный кватернион */
SINLINE quaternion quat_inverse(quaternion q)
{
	return quat_mult_c( quat_conjugate(q), 1 / quat_norm(q) );
}

/* Скалярное произведение кватернионов */
SINLINE float quat_inner_product(quaternion q1, quaternion q2)
{
	return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
}

/* Конвертировать кватернион в матрицу вращения */
void quat_to_matrix4(matrix4f out, quaternion q);

/* Конвертировать матрицу вращения в кватернион */
quaternion quat_from_matrix4(matrix4f m);

/* Преобразовать кватернион в ось у угол */
void quat_to_rotate_axis(quaternion q, float *angle, vector3f *axis);

/* Создать кватернион из угла и оси */
quaternion quat_from_rotate_axis(float angle, vector3f axis);

SINLINE void quat_print(quaternion q)
{
	printf("v(%.2f,%.2f,%.2f,%.2f)\n", q.x, q.y, q.z, q.w);
}


/* Инициализация сферической интерполяции между двумя единичными кватернионами */
quaternion_slerp_t quat_slerp_init_from_unit(quaternion q1, quaternion q2);

/* Инициализация сферической интерполяции между двумя кватернионами */
SINLINE quaternion_slerp_t quat_slerp_init(quaternion q1, quaternion q2)
{
	q1 = quat_normalize(q1);
	q2 = quat_normalize(q2);
	
	return quat_slerp_init_from_unit(q1, q2);
}

/* Сферическая интерполяция между двумя кватернионами на t */
quaternion quat_slerp(quaternion_slerp_t *lerp, float t);

/* Инициализация линейной интерполяции между двумя единичными кватернионами */
quaternion_lerp_t quat_lerp_init_from_unit(quaternion q1, quaternion q2);

/* Инициализация линейной интерполяции между двумя кватернионами */
SINLINE quaternion_lerp_t quat_lerp_init(quaternion q1, quaternion q2)
{
	q1 = quat_normalize(q1);
	q2 = quat_normalize(q2);
	
	return quat_lerp_init_from_unit(q1, q2);
}

/* Линейная интерполяция между двумя кватернионами на t */
SINLINE quaternion quat_lerp(quaternion_lerp_t *lerp, float t)
{
	if(t <= 0.0f)
		return lerp->q1;
	else if(t >= 1.0f)
		return lerp->q2;
		
	return quat_add(lerp->q1, quat_mult_c(lerp->q2, t));
}

#ifdef __cplusplus
}
#endif

#endif /* QUATERNION_H_INCLUDED */
