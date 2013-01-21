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

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include "common.h"

typedef struct {
	unsigned int id;
	unsigned int dimension; // 1 - 1D, 2 - 2D, 3 - 3D
	unsigned int gl_textype; // GL_TEXTURE_*
	unsigned int gl_minfilter, gl_magfilter; // GL_NEAREST, GL_LINEAR и т.п
	
	unsigned int gl_mipmap_level, gl_tex3d_layer; // пока неиспользуются
	
	int is_empty;
} texture_t;

	
#ifdef __cplusplus
extern "C" {
#endif

/* Создать 1D текстуру с данными */
void texture_create1d_from_data(texture_t *texture, 
						  int internal_format, GLenum format, unsigned int size,
						  int minfilter, int magfilter,
						  GLenum data_type, void *data_buffer);

/* Создать 2D текстуру с данными */
void texture_create2d_from_data(texture_t *texture, 
						  int internal_format, GLenum format, int mipmap, unsigned int width, 
						  unsigned int height, int minfilter, int magfilter, 
						  GLenum data_type, void *data_buffer);

/* Создать 3D текстуру с данными */
void texture_create3d_from_data(texture_t *texture, 
						  int internal_format, GLenum format, int mipmap, unsigned int width, 
						  unsigned int height, unsigned int depth, GLenum data_type, void *data_buffer);

/* Создать пустую (без данных) текстуру */
void texture_create2d_empty(texture_t *texture, 
						  int internal_format, int format, int multisample,
						  unsigned int width, unsigned int height);

/* Создать 2d текстуру из TGA файла (файл должен быть без сжатия) */
int texture_create2d_from_tga(texture_t *texture, int use_mipmap, const char *filename);

/* Сделать текстуру активной */
void texture_bind(texture_t *texture);

/* Получить id текстуры */
GLuint texrure_get_id(texture_t *texture);

/* Получить различные параметры */
GLuint texture_get_type(texture_t *texture);
GLuint texture_get_minfilter(texture_t *texture);
GLuint texture_get_magfilter(texture_t *texture);

/* Возвращает 1, если текстура пустая */
int texture_get_is_empty(texture_t *texture);

/* Освободить ресурсы текстуры */
void texture_release(texture_t *texture);

	
#ifdef __cplusplus
}
#endif

#endif /* TEXTURE_H_INCLUDED */
