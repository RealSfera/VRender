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

#ifndef SHADER_PROGRAM_H_INCLUDED
#define SHADER_PROGRAM_H_INCLUDED

#include "shader.h"

typedef struct {
	unsigned int program_id;
	shader_t vertex_shader;
	shader_t fragment_shader;
	shader_t geometry_shader;
	unsigned int is_configured; // шейдерная программа готова для запуска (имеются необходимые шейдеры)
	unsigned int is_linked;
} shader_program_t;

	
#ifdef __cplusplus
extern "C" {
#endif

/* Создать шейдерную программу (а после установить необходимые шейдеры)*/
void shader_program_create(shader_program_t *program);

/* Линковать шейдерную программу */
int shader_program_link(shader_program_t *program);

/* Установить шейдер в шейдерной программе */
int shader_program_set_shader(shader_program_t *program, shader_t *shader);

/* Удалить установку шейдера */
void shader_program_unset_shader(shader_program_t *program, shader_t *shader);

/* Забиндить\отбиндить шейдерную программу */
void shader_program_bind(shader_program_t *program);
void shader_program_unbind(shader_program_t *program);

/* Поулчить id шейдерной программы */
GLuint shader_program_get_id(shader_program_t *program);

/* Получить местоположение аттрибута\юниформа */
int shader_program_get_uniform_loc(shader_program_t *program, const char* name);
int shader_program_get_attrib_loc(shader_program_t *program, const char* name);

/* Освободить ресурсы шейдерной программы */
void shader_program_release(shader_program_t *program);

	
#ifdef __cplusplus
}
#endif

#endif /* SHADER_PROGRAM_H_INCLUDED */
