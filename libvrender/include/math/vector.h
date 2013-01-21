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

#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "common.h"
#include "math/dmath.h"

// float
typedef struct {
	float x, y, z, w;
} vector4f;

typedef struct {
	float x, y, z;
} vector3f;

typedef struct {
	float x, y;
} vector2f;


// unsigned int
typedef struct {
	unsigned int x, y, z, w;
} vector4ui;

typedef struct {
	unsigned int x, y, z;
} vector3ui;

typedef struct {
	unsigned int x, y;
} vector2ui;

#ifdef __cplusplus
extern "C" {
#endif

/****** Инициализировать вектор значениями ******/
SINLINE vector4f vec4f(float x, float y, float z, float w)
{
	vector4f result;
	
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	
	return result;
}

SINLINE vector3f vec3f(float x, float y, float z)
{
	vector3f result;
	
	result.x = x;
	result.y = y;
	result.z = z;
	
	return result;
}

SINLINE vector3ui vec3ui(unsigned int x, unsigned int y, unsigned int z)
{
	vector3ui result;
	
	result.x = x;
	result.y = y;
	result.z = z;
	
	return result;
}

SINLINE vector2f vec2f(float x, float y)
{
	vector2f result;
	
	result.x = x;
	result.y = y;
	
	return result;
}


/****** Получить массив компонентов вектора ******/
SINLINE void vec4f_get_array(vector4f v, float *arr)
{
	arr[0] = v.x;
	arr[1] = v.y;
	arr[2] = v.z;
	arr[3] = v.w;
}

SINLINE void vec3f_get_array(vector3f v, float *arr)
{
	arr[0] = v.x;
	arr[1] = v.y;
	arr[2] = v.z;
}

SINLINE void vec3ui_get_array(vector3ui v, unsigned int *arr)
{
	arr[0] = v.x;
	arr[1] = v.y;
	arr[2] = v.z;
}

SINLINE void vec2f_get_array(vector2f v, float *arr)
{
	arr[0] = v.x;
	arr[1] = v.y;
}

/****** Преобразовать вектор одной размерности в другую ******/
SINLINE vector4f vec3f_to_vec4f(vector3f v)
{
	return vec4f(v.x, v.y, v.z, 1.0f);
}

SINLINE vector3f vec4f_to_vec3f(vector4f v)
{
	return vec3f(v.x / v.w, v.y / v.w, v.z / v.w);
}


/****** Преобразовать векторы с разными типами данных ******/
SINLINE vector3f vec3ui_to_vec3f(vector3ui v)
{
	return vec3f((float) v.x, (float) v.y, (float) v.z);
}

SINLINE vector3ui vec3f_to_vec3ui(vector3f v)
{
	return vec3ui((unsigned int) v.x, (unsigned int) v.y, (unsigned int) v.z);
}

/****** Сложить вектор v1 и v2 ******/
SINLINE vector4f vec4f_add(vector4f v1, vector4f v2)
{
	return vec4f(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

SINLINE vector3f vec3f_add(vector3f v1, vector3f v2)
{
	return vec3f(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

SINLINE vector3ui vec3ui_add(vector3ui v1, vector3ui v2)
{
	return vec3ui(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

SINLINE vector2f vec2f_add(vector2f v1, vector2f v2)
{
	return vec2f(v1.x + v2.x, v1.y + v2.y);
}

/****** Вычесть из вектора v1 вектор v2 ******/
SINLINE vector4f vec4f_sub(vector4f v1, vector4f v2)
{
	return vec4f(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

SINLINE vector3f vec3f_sub(vector3f v1, vector3f v2)
{
	return vec3f(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

SINLINE vector3ui vec3ui_sub(vector3ui v1, vector3ui v2)
{
	return vec3ui(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

SINLINE vector2f vec2f_sub(vector2f v1, vector2f v2)
{
	return vec2f(v1.x - v2.x, v1.y - v2.y);
}

/****** Прибавить к компонентам вектора число c ******/
SINLINE vector4f vec4f_add_c(vector4f v, float c)
{
	return vec4f(v.x + c, v.y + c, v.z + c, v.w + c);
}

SINLINE vector3f vec3f_add_c(vector3f v, float c)
{
	return vec3f(v.x + c, v.y + c, v.z + c);
}

SINLINE vector3ui vec3ui_add_c(vector3ui v, unsigned int c)
{
	return vec3ui(v.x + c, v.y + c, v.z + c);
}

/****** Вычесть из компонентов вектора число c ******/
SINLINE vector4f vec4f_sub_c(vector4f v, float c)
{
	return vec4f(v.x - c, v.y - c, v.z - c, v.w - c);
}

SINLINE vector3f vec3f_sub_c(vector3f v, float c)
{
	return vec3f(v.x - c, v.y - c, v.z - c);
}

SINLINE vector3ui vec3ui_sub_c(vector3ui v, unsigned int c)
{
	return vec3ui(v.x - c, v.y - c, v.z - c);
}

/****** Умножить компоненты вектора v1 на компоненты вектора v2 ******/
SINLINE vector4f vec4f_mult(vector4f v1, vector4f v2)
{
	return vec4f(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

SINLINE vector3f vec3f_mult(vector3f v1, vector3f v2)
{
	return vec3f(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

SINLINE vector3ui vec3ui_mult(vector3ui v1, vector3ui v2)
{
	return vec3ui(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

/****** Умножить компоненты вектора на число c ******/
SINLINE vector4f vec4f_mult_c(vector4f v, float c)
{
	return vec4f(v.x * c, v.y * c, v.z * c, v.w * c);
}

SINLINE vector3f vec3f_mult_c(vector3f v, float c)
{
	return vec3f(v.x * c, v.y * c, v.z * c);
}

SINLINE vector3ui vec3ui_mult_c(vector3ui v, unsigned int c)
{
	return vec3ui(v.x * c, v.y * c, v.z * c);
}

/****** Разделить компоненты вектора v1 на компоненты вектора v2 ******/
SINLINE vector4f vec4f_div(vector4f v1, vector4f v2)
{
	return vec4f(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

SINLINE vector3f vec3f_div(vector3f v1, vector3f v2)
{
	return vec3f(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

SINLINE vector3ui vec3ui_div(vector3ui v1, vector3ui v2)
{
	return vec3ui(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

/****** Разделить компоненты вектора на число c ******/
SINLINE vector4f vec4f_div_c(vector4f v, float c)
{
	return vec4f(v.x / c, v.y / c, v.z / c, v.w / c);
}

SINLINE vector3f vec3f_div_c(vector3f v, float c)
{
	return vec3f(v.x / c, v.y / c, v.z / c);
}

SINLINE vector3ui vec3ui_div_c(vector3ui v, unsigned int c)
{
	return vec3ui(v.x / c, v.y / c, v.z / c);
}

/****** Проверить на равество вектора v1 и v2 ******/
SINLINE int vec4f_comapre(vector4f v1, vector4f v2)
{
	return ( (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w) ? 1 : 0 );
}

#define EPSILON 0.001
SINLINE int vec3f_comapre(vector3f v1, vector3f v2)
{
	return ( ( math_fabs(v1.x - v2.x) < EPSILON && math_fabs(v1.y - v2.y) < EPSILON && math_fabs(v1.z - v2.z) < EPSILON) ? 1 : 0 );
}

/****** Интерполяция векторов ******/
SINLINE vector4f vec4f_lerp(vector4f v1, vector4f v2, float t)
{
	return vec4f( math_lerp(v1.x, v2.x, t), 
				 math_lerp(v1.y, v2.y, t),
				 math_lerp(v1.z, v2.z, t),
				 math_lerp(v1.w, v2.w, t) );
}

SINLINE vector3f vec3f_lerp(vector3f v1, vector3f v2, float t)
{
	return vec3f( math_lerp(v1.x, v2.x, t), 
				 math_lerp(v1.y, v2.y, t),
				 math_lerp(v1.z, v2.z, t) );
}

SINLINE vector2f vec2f_lerp(vector2f v1, vector2f v2, float t)
{
	return vec2f( math_lerp(v1.x, v2.x, t), 
				 math_lerp(v1.y, v2.y, t) );
}

/****** Получить длину вектора (модуль вектора) ******/
SINLINE float vec4f_length(vector4f v)
{
	return math_sqrtf(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

SINLINE float vec3f_length(vector3f v)
{
	return math_sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

SINLINE float vec2f_length(vector2f v)
{
	return math_sqrtf(v.x*v.x + v.y*v.y);
}

/****** Расстояние между двумя точками ******/
SINLINE float vec2f_distance(vector2f p1, vector2f p2)
{
	return math_sqrtf(math_powf((p2.x - p1.x), 2.0f) + math_powf((p2.y - p1.y), 2.0f));
}

SINLINE float vec3f_distance(vector3f p1, vector3f p2)
{
	return math_sqrtf(math_powf((p2.x - p1.x), 2.0f) + math_powf((p2.y - p1.y), 2.0f) + math_powf((p2.z - p1.z), 2.0f));
}

/****** Получть скалярное произведение векторов ******/
SINLINE float vec4f_dot(vector4f v1, vector4f v2)
{
	return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w);
}

SINLINE float vec3f_dot(vector3f v1, vector3f v2)
{
	return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}

SINLINE float vec2f_dot(vector2f v1, vector2f v2)
{
	return (v1.x*v2.x + v1.y*v2.y);
}

/****** Получить векторное произведение ******/
SINLINE vector3f vec3f_cross(vector3f v1, vector3f v2)
{
	return vec3f(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

/****** Нормализировать вектор v ******/
SINLINE vector4f vec4f_norm(vector4f v)
{
	float v_length = vec4f_length(v);

	return vec4f(v.x / v_length, v.y / v_length, v.z / v_length, v.w / v_length);
}

SINLINE vector3f vec3f_norm(vector3f v)
{
	float v_length = vec3f_length(v);

	return vec3f(v.x / v_length, v.y / v_length, v.z / v_length);
}

/****** Вывести на консоль значения вектора v ******/
SINLINE void vec4f_print(vector4f v)
{
	printf("v(%.2f;%.2f;%.2f;%.2f)\n", v.x, v.y, v.z, v.w);
}

SINLINE void vec3f_print(vector3f v)
{
	printf("v(%.2f;%.2f;%.2f)\n", v.x, v.y, v.z);
}

SINLINE void vec3ui_print(vector3ui v)
{
	printf("v(%i;%i;%i)\n", v.x, v.y, v.z);
}

#ifdef __cplusplus
}
#endif

#endif /* VECTOR_H_INCLUDED */
