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

#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#define SHADER_VERTEX 1
#define SHADER_FRAGMENT 2
#define SHADER_GEOMETRY 3

#include "common.h"

typedef struct {
	unsigned int id;
	unsigned int type; // SHADER_*
	unsigned int is_compiled;
	unsigned int shader_program; // 0, если не присоединен к шейдерной программе
} shader_t;

	
#ifdef __cplusplus
extern "C" {
#endif

/* Создать шейдер (без данных) */
void shader_create(shader_t *shader, unsigned int type);

/* Загрузить исходник шейдера из файла */
int shader_load_source_file(shader_t *shader, const char* filename);

/* Загрузить исходник из буффера */
int shader_load_source_buffer(shader_t *shader, const char* buffer);

/* Получить id шейдера */
GLuint shader_get_id(shader_t *shader);

/* Получить тип шейдера */
GLenum shader_get_type(shader_t *shader);

/* Удалить шейдер */
void shader_delete(shader_t *shader);

	
#ifdef __cplusplus
}
#endif

#endif /* SHADER_H_INCLUDED */
