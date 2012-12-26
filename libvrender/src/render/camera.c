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

#include "common.h"
#include "camera.h"
#include "math/matrix.h"
#include "math/dmath.h"

static void camera_update_orientation(camera_t *cam);
static void camera_update_target_rot(camera_t *cam);
static void camera_set_angles_from_target(camera_t *cam, vector3f target);

void camera_init(camera_t *cam, vector3f pos, vector3f up, vector3f target)
{
	IF_FAILED(cam);
	
	TRACE_MSG("init camera\n");	
	
	// настраиваем стандартные параметры камеры
	cam->position = pos;
	cam->fovy = 45.0f;
	cam->aspect = 1.0f;
	cam->target = vec3f_norm(target);
	cam->z_planes = vec2f(0.5f, 100.0f);
	cam->up = up;
	cam->projection_type = CAMERA_PERSPECTIVE_PROJ;
	
	// если ортогональная проекция, то настраиваем ещё 4 плоскости отсечения
	if(cam->projection_type == CAMERA_ORTHOGONAL_PROJ) {
		cam->x_planes = vec2f(0.0f, 100.0f);
		cam->y_planes = vec2f(0.0f, 100.0f);
	}
	
	cam->move_velocity = 10.0f;
	cam->rotate_velocity = 50.0f;
	
	cam->angle_horizontal = 0.0f;
	cam->angle_vertical = 0.0f;
	
	// убираем пределы вращения
	cam->limit_vert_from = -360.0f;
	cam->limit_vert_to = 360.0f;
	cam->limit_horiz_from = -360.0f;
	cam->limit_horiz_to = 360.0f;
	
	// получаем углы из target
	camera_set_angles_from_target(cam, cam->target);
	
	// получаем временные кватернионы вращения и настраиваем временную интерполяцию
	cam->horizontal_rot = quat_from_rotate_axis(-cam->angle_horizontal, CAMERA_DEFAULT_VERTICAL_AXIS);
	cam->vertical_rot = quat_from_rotate_axis(cam->angle_vertical, CAMERA_DEFAULT_HORIZONTAL_AXIS);
	cam->quat_rotation = cam->quat_target_rotation = quat_mult(cam->horizontal_rot, cam->vertical_rot);
	cam->quat_lerp_rotation = quat_lerp_init(cam->quat_rotation, cam->quat_target_rotation);
}

void camera_set_angles_from_target(camera_t *cam, vector3f target)
{
	vector3f target_mod;
	
	IF_FAILED(cam);
	
	// вычислеям из оси target вертикальный и горизонтальный углы вращения
	
	target_mod = vec3f_norm(vec3f(target.x, 0.0f, target.z));
	
	if(target_mod.z >= 0.0f) {
		if(target_mod.x >= 0.0f) {
			cam->angle_horizontal = -90.0f + 360.0f - RAD_TO_DEG(math_asinf(target_mod.z));
		} else {
			cam->angle_horizontal = -90.0f + 180.0f + RAD_TO_DEG(math_asinf(target_mod.z));
		}
	} else {
		if(target_mod.x >= 0.0f) {
			cam->angle_horizontal = -90.0f + RAD_TO_DEG(math_asinf(-target_mod.z));
		} else {
			cam->angle_horizontal = -90.0f + 90.0f + RAD_TO_DEG(math_asinf(-target_mod.z));
		}
	}
	
	cam->angle_vertical = -RAD_TO_DEG(math_asinf(target.y));
}

// обновление ориентации камеры
void camera_update_orientation(camera_t *cam)
{
	vector3f new_target, vertical_axis;
	
	IF_FAILED(cam);
	
	vertical_axis = CAMERA_DEFAULT_VERTICAL_AXIS;
	new_target = CAMERA_DEFAULT_TARGET;

	// получяем направление повернутое вокруг вертикальной оси
	cam->target = vec3f_norm(quat_mult_vec3(cam->quat_rotation, new_target));
	
	// расчитываем новый вектор up
	cam->up = vec3f_norm(quat_mult_vec3(cam->quat_rotation, vertical_axis));
}

// обновляем желаемую позицию цели
void camera_update_target_rot(camera_t *cam)
{
	vector3f vertical_axis, horizontal_axis;
	quaternion quat_rot_horizontal, quat_rot_vertical;
	
	vertical_axis = CAMERA_DEFAULT_VERTICAL_AXIS;
	horizontal_axis = CAMERA_DEFAULT_HORIZONTAL_AXIS_INV;
	
	// получяем кватернионы вращения вокруг верткальной и горизонтальной осей
	
	// меняем знак угла (соотвественно направление вращения) для vertical_axis, 
	//   т.к. кватернион вращает по часовой стрелки относительно 
	//   оси vertical_axis, но для нас это вращение оказывается против часовой стрелки,
	//   поэтому меняем знак угла
	quat_rot_horizontal = quat_from_rotate_axis(-cam->angle_horizontal, vertical_axis);
	quat_rot_vertical = quat_from_rotate_axis(cam->angle_vertical, horizontal_axis);
	
	// умножаем кватернионы и получаем итоговое вращение
	cam->quat_target_rotation = quat_mult(quat_rot_horizontal, quat_rot_vertical);
}

void camera_update(camera_t *cam, double dt)
{
	IF_FAILED(cam);
	
	// если текущее вращение неравно затребованному, то интерполируем по dt 
	if(!quat_compare_equal(cam->quat_rotation, cam->quat_target_rotation))
		cam->quat_rotation = quat_lerp(&cam->quat_lerp_rotation, dt * cam->rotate_velocity);
	
	// если текущая позиция неравна затребованному, то интерполируем её по dt 
	if(!vec3f_comapre(cam->position, cam->target_position))
		cam->position = vec3f_lerp(cam->position, cam->target_position, dt * cam->move_velocity);
	
	// обновляем ориентацию камеры
	camera_update_orientation(cam);
}

void camera_set_target(camera_t *cam, vector3f target)
{
	IF_FAILED(cam);
	
	cam->target = target;
	
	camera_set_angles_from_target(cam, cam->target);
}

void camera_set_move_velocity(camera_t *cam, float velocity)
{
	IF_FAILED(cam);
	
	cam->move_velocity = velocity;
}

void camera_set_rotate_velocity(camera_t *cam, float velocity)
{
	IF_FAILED(cam);
	
	cam->rotate_velocity = velocity;
}

void camera_set_limit(camera_t *cam, float vert_from, float vert_to,
					  float horiz_from, float horiz_to)
{
	IF_FAILED(cam);
	
	cam->limit_vert_from = vert_from; 
	cam->limit_vert_to = vert_to;
	cam->limit_horiz_from = horiz_from;
	cam->limit_horiz_to = horiz_to;
}

void camera_set_position(camera_t *cam, vector3f pos)
{
	IF_FAILED(cam);

	cam->position = pos;
}

void camera_set_fov(camera_t *cam, float fovy)
{
	IF_FAILED(cam);
	
	cam->fovy = fovy;
}

void camera_set_up(camera_t *cam, vector3f up)
{
	IF_FAILED(cam);
	
	cam->up = up;
}

void camera_set_aspect(camera_t *cam, float aspect)
{
	IF_FAILED(cam);
	
	cam->aspect = aspect;
}

void camera_set_zplanes(camera_t *cam, float z_near, float z_far)
{
	IF_FAILED(cam);
	
	cam->z_planes = vec2f(z_near, z_far);
}

void camera_set_xplanes(camera_t *cam, float left, float right)
{
	IF_FAILED(cam);
	
	cam->x_planes = vec2f(left, right);
}

void camera_set_yplanes(camera_t *cam, float bottom, float top)
{
	IF_FAILED(cam);
	
	cam->y_planes = vec2f(bottom, top);
}

void camera_move(camera_t *cam, vector3f vec, float amount)
{
	IF_FAILED(cam);
	
	cam->target_position = vec3f_add(cam->position, vec3f_mult_c(vec, amount));
}

void camera_move_forward(camera_t *cam, float amount)
{
	IF_FAILED(cam);
	
	camera_move(cam, cam->target, amount);
}

void camera_move_backward(camera_t *cam, float amount)
{
	IF_FAILED(cam);
	
	camera_move(cam, cam->target, -amount);
}

void camera_move_right(camera_t *cam, float amount)
{
	vector3f right;
	
	IF_FAILED(cam);
	
	right = vec3f_norm(vec3f_cross(cam->target, cam->up));
	
	camera_move(cam, right, amount);
}

void camera_move_left(camera_t *cam, float amount)
{
	vector3f left;
	
	IF_FAILED(cam);
	
	left = vec3f_norm(vec3f_cross(cam->up, cam->target));
	
	camera_move(cam, left, amount);
}


void camera_move_up(camera_t *cam, float amount)
{
	IF_FAILED(cam);
		
	camera_move(cam, vec3f(0.0f, 1.0f, 0.0f), amount);
}


void camera_move_down(camera_t *cam, float amount)
{
	IF_FAILED(cam);
		
	camera_move(cam, vec3f(0.0f, 1.0f, 0.0f), -amount);
}

vector3f camera_get_position(camera_t *cam)
{
	IF_FAILED_RET(cam, vec3f(0.0f, 0.0f, 0.0f));
	
	return cam->position;
}

vector3f camera_get_target(camera_t *cam)
{
	IF_FAILED_RET(cam, vec3f(0.0f, 0.0f, 0.0f));
	
	return cam->target;
}

float camera_get_horizontal_angle(camera_t *cam)
{
	IF_FAILED_RET(cam, -1.0f);
	
	return cam->angle_horizontal;
}

float camera_get_vertical_angle(camera_t *cam)
{
	IF_FAILED_RET(cam, -1.0f);
	
	return cam->angle_vertical;
}

void camera_rotate_vertical(camera_t *cam, float angle)
{
	IF_FAILED(cam);
	
	cam->angle_vertical += angle;
	
	if(cam->angle_vertical > 360.0f)
		cam->angle_vertical = 0.0f;
	else if(cam->angle_vertical < -360.0f)
		cam->angle_vertical = 0.0f;
	
	// обновляем желаемое вращение и интерполяцию
	camera_update_target_rot(cam);
	cam->quat_lerp_rotation = quat_lerp_init(cam->quat_rotation, cam->quat_target_rotation);

	if( cam->angle_vertical < cam->limit_vert_from ||
		cam->angle_vertical > cam->limit_vert_to ) {
		
		cam->angle_vertical -= angle;
	}
}

void camera_rotate_horizontal(camera_t *cam, float angle)
{
	IF_FAILED(cam);
	
	cam->angle_horizontal += angle;
	
	if(cam->angle_horizontal > 360.0f)
		cam->angle_horizontal = 0.0f;
	else if(cam->angle_horizontal < -360.0f)
		cam->angle_horizontal = 0.0f;
	
	// обновляем желаемое вращение и интерполяцию
	camera_update_target_rot(cam);
	cam->quat_lerp_rotation = quat_lerp_init(cam->quat_rotation, cam->quat_target_rotation);
	
	if( cam->angle_horizontal < cam->limit_horiz_from ||
		cam->angle_horizontal > cam->limit_horiz_to ) {
		
		cam->angle_horizontal -= angle;
	}
}

void camera_get_viewprojection_matrix(camera_t *cam, matrix4f out)
{
	matrix4f view, projection;
	
	IF_FAILED(cam);

	camera_get_view_matrix(cam, view);
	camera_get_projection_matrix(cam, projection);
	
	mat4_mult2(out, projection, view);
}

void camera_get_view_matrix(camera_t *cam, matrix4f out)
{
	IF_FAILED(cam);
	
	mat4_view(out, cam->position, cam->target, cam->up);
}


void camera_get_projection_matrix(camera_t *cam, matrix4f out)
{
	IF_FAILED(cam);
	
	if(cam->projection_type == CAMERA_PERSPECTIVE_PROJ) {
		mat4_perspective(out, cam->fovy, cam->aspect, cam->z_planes.x, cam->z_planes.y);
	} else if (cam->projection_type == CAMERA_ORTHOGONAL_PROJ) {
		mat4_ortho(out, cam->x_planes.x, cam->x_planes.y, cam->y_planes.x, cam->y_planes.y, cam->z_planes.x, cam->z_planes.y);
	} else {
		ERROR_MSG("camera projection type doesnt supported; type = %i\n", cam->projection_type);
		mat4(out, 1.0f);
	}
}
