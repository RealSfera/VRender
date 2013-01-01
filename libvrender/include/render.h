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

#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include "common.h"
#include "math/vector.h"

#define CHECK_GL_ERRORS() \
	int __render_gl_error = 0; \
	if((__render_gl_error = glGetError()) != GL_NO_ERROR) \
		ERROR_MSG("OpenGL error #%i in file %s on line %i \n", __render_gl_error, __FILE__, __LINE__)
	
#ifdef __cplusplus
extern "C" {
#endif

/* Инициализировать визуализатор (рендер) */
int render_init(void);

/* Освободить ресурсы рендера */
void render_destroy(void);

/* Установить различные параметры */
void render_set_isolevel(float level);
void render_set_isolevel_animation(int animate);
void render_set_isolevel_begin_anim(float level);
void render_set_isolevel_end_anim(float level);
void render_set_isolevel_step_anim(float step);
void render_set_light_color(vector3f color);
void render_set_light_spec_color(vector3f color);
void render_set_light_angle(float angle_pos);
void render_set_light_animation(int animate);
void render_set_light_rot_step(float step);
void render_set_volume_size(vector3ui volume_size);
void render_set_grid_size(vector3ui grid_size);
void render_set_material_color(vector3f front_color, vector3f back_color);
void render_set_material_shininess(float shininess);
void render_set_ambient_factor(float ambient);
void render_set_diffuse_factor(float diffuse);
void render_set_specular_factor(float specular);
void render_set_gamma_factor(float gamma);
void render_set_camera_step(float step);
void render_set_camera_move_speed(float speed);
void render_set_camera_fov(float fov);
void render_set_number_of_threads(unsigned num);
int render_set_function_text(const char *function_text);

/** 
 * Экспортирует текущий объект (с текущим изо-уровнем) в buffer в формате wavefront (.obj)
 * Выделяет память под данные и указатель на них присваивает buffer, поэтому требуется освобождение
 * выделенный памяти (free) в вызывающей функции
 */
int render_export_obj(char **buffer);

/* Обновить скалярное поле */
void render_update_volume_tex(void);

/* Изменение окна вывода */
void render_change_window(unsigned int width, unsigned int height);

/* Рисование кадра */
void render_draw(void);

/* Обновление рендера (камеры и т.п.) */
void render_update(double last_frame_time);


#ifdef __cplusplus
}
#endif

#endif /* RENDER_H_INCLUDED */
