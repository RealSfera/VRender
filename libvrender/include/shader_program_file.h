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

#ifndef SHADER_PROGRAM_FILE_H_INCLUDED
#define SHADER_PROGRAM_FILE_H_INCLUDED

#include "shader.h"
#include "shader_program.h"

#define SHADER_MAX_BUFFER_SIZE 1024*1024*64

typedef struct {
	shader_program_t program;
	unsigned int shaders_bit; // SHADER_VERTEX_BIT и т.д.
	shader_t vertex_shader;
	shader_t fragment_shader;
	shader_t geometry_shader;
} shader_program_file_t;

	
#ifdef __cplusplus
extern "C" {
#endif

/* Инициализация (загрузить из файлы или буфера) */
int shader_program_file_create(shader_program_file_t *pfile, const char *filename);
int shader_program_file_create_from_buffer(shader_program_file_t *pfile, const char *buffer);

/* Получить шейдерную программу с загруженными шейдерами (без линковки) */
int shader_program_file_get_program(shader_program_file_t *pfile, shader_program_t *program);

/* Освободить ресурсы */
void shader_program_file_destroy(shader_program_file_t *pfile);

	
#ifdef __cplusplus
}
#endif

#endif /* SHADER_PROGRAM_FILE_H_INCLUDED */
