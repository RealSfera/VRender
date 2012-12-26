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

#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "math/dmath.h"
#include "math/quaternion.h"

#define CAMERA_PERSPECTIVE_PROJ 1
#define CAMERA_ORTHOGONAL_PROJ 2

typedef struct {
	vector3f position; // текущая позиция
	vector3f target_position; // желаемая позиция (до неё интерполируемся)
	float position_lerp_time; // [0..1]
	vector3f target; // вектор цели
	vector3f up; // вектор вверх
	float fovy; // вертикальный угол обзора
	float aspect; // соотношение сторон окна
	vector2f z_planes; // плоскости отсечения по z (x - ближняя, y - дальняя)
	vector2f x_planes; // плоскости отсечения по x для ортогональной проекции (x - левая, y - правая)
	vector2f y_planes; // плоскости отсечения по y для ортогональной проекции (x - нижняя, y - верхняя)
	
	// скорость движения и вращения
	float move_velocity, rotate_velocity;
	
	// задать тип проекции
	int projection_type; // CAMERA_PERSPECTIVE_PROJ, CAMERA_ORTHOGONAL_PROJ
	
	// кватернионы вращения (горизонтальный, вертикальный)
	quaternion horizontal_rot, vertical_rot;
	
	// желаемые кватернионы вращения
	quaternion horizontal_target_rot, vertical_target_rot;
	
	// настройки интерполяции кватернионов
	quaternion_lerp_t rotation_horizontal_lerp, rotation_vertical_lerp;
	
	
	//////
	
	quaternion quat_rotation, quat_target_rotation;
	quaternion_lerp_t quat_lerp_rotation;
	
	//////
	
	
	// углы поворота камеры (горизонтальный, вертикальный)
	float angle_horizontal, angle_vertical;
	
	// пределы врвщения камеры по вертикали и горизонтали (от и до)
	float limit_vert_from, limit_vert_to;
	float limit_horiz_from, limit_horiz_to;
	
} camera_t;

#define CAMERA_DEFAULT_TARGET vec3f(0.0f, 0.0f, -1.0f)
#define CAMERA_DEFAULT_TARGET_INV vec3f(0.0f, 0.0f, 1.0f)
#define CAMERA_DEFAULT_VERTICAL_AXIS vec3f(0.0f, 1.0f, 0.0f)
#define CAMERA_DEFAULT_VERTICAL_AXIS_INV vec3f(0.0f, -1.0f, 0.0f)
#define CAMERA_DEFAULT_HORIZONTAL_AXIS vec3f(1.0f, 0.0f, 0.0f)
#define CAMERA_DEFAULT_HORIZONTAL_AXIS_INV vec3f(-1.0f, 0.0f, 0.0f)

	
#ifdef __cplusplus
extern "C" {
#endif

/* Инициализация камеры */
void camera_init(camera_t *cam, vector3f pos, vector3f up, vector3f target);

/** 
 * Обновление камеры. 
 * Пересчитывает параметры камеры  
 */
void camera_update(camera_t *cam, double dt);
//void camera_update(camera_t *cam);

/* 
 * Установить ограничения обзора
 * Стандартный обзор от -360 до +360 (т.е. без ограничения) 
 *    по вертикали и горизонтали
 */
void camera_set_limit(camera_t *cam, float vert_from, float vert_to,
					  float horiz_from, float horiz_to);

/* Изменить цель камеры (точка в которую направлена камера) */
void camera_set_target(camera_t *cam, vector3f target);

/* Изменить угол обзора по вертикали (fovy) */
void camera_set_fov(camera_t *cam, float fovy);

/* Изменить соотношения экрана (ширина на высоту) */
void camera_set_aspect(camera_t *cam, float aspect);

/* Изменить Z плокости (ближняя и дальняя) */
void camera_set_zplanes(camera_t *cam, float z_near, float z_far);

/* Изменить скорость перемещения камеры */
void camera_set_move_velocity(camera_t *cam, float velocity);

/* Изменить скорость вращения камеры */
void camera_set_rotate_velocity(camera_t *cam, float velocity);

/* Перемещать камеру на amount по вектору vec */
void camera_move(camera_t *cam, vector3f vec, float amount);

/* Перемещать камеру */
void camera_move_forward(camera_t *cam, float amount);
void camera_move_backward(camera_t *cam, float amount);
void camera_move_right(camera_t *cam, float amount);
void camera_move_left(camera_t *cam, float amount);
void camera_move_up(camera_t *cam, float amount);
void camera_move_down(camera_t *cam, float amount);

/* Изменить позицию камеры */
void camera_set_position(camera_t *cam, vector3f pos);

/* Получить различные параметры камеры */
vector3f camera_get_position(camera_t *cam);
vector3f camera_get_target(camera_t *cam);
float camera_get_horizontal_angle(camera_t *cam);
float camera_get_vertical_angle(camera_t *cam);

/* Повернуть камеру (вертикально или горизонтально) на угол angle (в градусах)*/
void camera_rotate_vertical(camera_t *cam, float angle);
void camera_rotate_horizontal(camera_t *cam, float angle);

/* Получить различные расчитанные матрицы */
void camera_get_projection_matrix(camera_t *cam, matrix4f out);
void camera_get_view_matrix(camera_t *cam, matrix4f out);
void camera_get_viewprojection_matrix(camera_t *cam, matrix4f out);

	
#ifdef __cplusplus
}
#endif

#endif /* CAMERA_H_INCLUDED */
