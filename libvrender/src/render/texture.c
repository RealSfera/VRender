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

#include "texture.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct image_spec_field {
	uint16_t x_origin;
	uint16_t y_origin;
	uint16_t image_width;
	uint16_t image_height;
	uint8_t pixel_depth;
	uint8_t image_descriptor;
};

struct tga_header {
	uint8_t id_length;
	uint8_t colormap_type;
	uint8_t image_type;
	uint8_t colormap_spec[5];
	struct image_spec_field image_spec;
};

static GLint last_tex1d_id, last_tex2d_id, last_tex3d_id;

void bind_safe(texture_t *texture)
{
	IF_FAILED(texture && texture->id);
	
	if(texture->dimension == 1)
		glGetIntegerv(GL_TEXTURE_BINDING_1D, &last_tex1d_id);
	else if(texture->dimension == 2)
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_tex2d_id);
	else if(texture->dimension == 2)
		glGetIntegerv(GL_TEXTURE_BINDING_3D, &last_tex3d_id);
	
	glBindTexture(texture->gl_textype, texture->id);
}

void unbind_safe(texture_t *texture)
{
	if(texture->dimension == 1)
		glBindTexture(texture->gl_textype, last_tex1d_id);
	else if(texture->dimension == 2)
		glBindTexture(texture->gl_textype, last_tex2d_id);
	else if(texture->dimension == 2)
		glBindTexture(texture->gl_textype, last_tex3d_id);
}

void texture_create1d_from_data(texture_t *texture, 
						  int internal_format, GLenum format, unsigned int size,
						  int minfilter, int magfilter,
						  GLenum data_type, void *data_buffer)
{
	IF_FAILED(texture && !texture->id && data_buffer);
	
	TRACE_MSG("create texture with data\n");
	
	glGenTextures(1, &texture->id);
	
	// настраиваем параметры текстуры
	texture->gl_minfilter = minfilter;
	texture->gl_magfilter = magfilter;
	texture->dimension = 1;
	texture->gl_mipmap_level = 0;
	texture->gl_textype = GL_TEXTURE_1D;
	texture->is_empty = 0;
	
	bind_safe(texture);
	
	// устанавливаем параметры
	TRACE_MSG("set texture parameters\n");
	glTexParameteri(texture->gl_textype, GL_TEXTURE_MIN_FILTER, texture->gl_minfilter);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_MAG_FILTER, texture->gl_magfilter);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	
	// загружаем данные
	TRACE_MSG("load texture data\n");
	glTexImage1D(texture->gl_textype, 0, internal_format, size, 0, format, data_type, (const GLvoid*) data_buffer);
	
	unbind_safe(texture);
}

void texture_create2d_from_data(texture_t *texture, 
						  int internal_format, GLenum format, int mipmap, unsigned int width, 
						  unsigned int height, int minfilter, int magfilter, 
						  GLenum data_type, void *data_buffer)
{
	IF_FAILED(texture && !texture->id && data_buffer);
	
	TRACE_MSG("create texture with data\n");
	
	glGenTextures(1, &texture->id);
	
	// настраиваем параметры текстуры
	texture->gl_minfilter = minfilter;
	texture->gl_magfilter = magfilter;
	texture->dimension = 2;
	texture->gl_mipmap_level = 0;
	texture->gl_textype = GL_TEXTURE_2D;
	texture->is_empty = 0;
	
	bind_safe(texture);
	
	// устанавливаем параметры
	TRACE_MSG("set texture parameters\n");
	glTexParameteri(texture->gl_textype, GL_TEXTURE_MIN_FILTER, texture->gl_minfilter);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_MAG_FILTER, texture->gl_magfilter);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if(mipmap)
		glTexParameteri(texture->gl_textype, GL_TEXTURE_BASE_LEVEL, texture->gl_mipmap_level);
	
	// загружаем данные
	TRACE_MSG("load texture data\n");
	glTexImage2D(texture->gl_textype, 0, internal_format, width, height, 0, format, data_type, (const GLvoid*) data_buffer);
	
	// генерируем мипмапы
	if(mipmap) {
		// ATI Bug
		glEnable(texture->gl_textype);
		
		glGenerateMipmap(texture->gl_textype);
		
		glDisable(texture->gl_textype);
	}
	
	unbind_safe(texture);
}

void texture_create3d_from_data(texture_t *texture, 
						  int internal_format, GLenum format, int mipmap, unsigned int width, 
						  unsigned int height, unsigned int depth, GLenum data_type, void *data_buffer)
{
	IF_FAILED(texture && !texture->id && data_buffer);
	
	TRACE_MSG("create 3D texture with data\n");
	
	glGenTextures(1, &texture->id);
	
	// настраиваем параметры текстуры
	if(mipmap)
		texture->gl_minfilter = GL_LINEAR_MIPMAP_LINEAR;
	else
		texture->gl_minfilter = GL_LINEAR;
	texture->gl_magfilter = GL_LINEAR;
	texture->dimension = 3;
	texture->gl_mipmap_level = 0;
	texture->gl_textype = GL_TEXTURE_3D;
	texture->is_empty = 0;
	
	bind_safe(texture);
	
	// устанавливаем параметры
	TRACE_MSG("set texture parameters\n");
	glTexParameteri(texture->gl_textype, GL_TEXTURE_MIN_FILTER, texture->gl_minfilter);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_MAG_FILTER, texture->gl_magfilter);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	if(mipmap)
		glTexParameteri(texture->gl_textype, GL_TEXTURE_BASE_LEVEL, texture->gl_mipmap_level);
	
	// загружаем данные
	TRACE_MSG("load texture data\n");
	glTexImage3D(texture->gl_textype, 0, internal_format, width, height, depth, 0, format, data_type, (const GLvoid*) data_buffer);
	
	// генерируем мипмапы
	if(mipmap) {
		// ATI Bug
		glEnable(texture->gl_textype);
		
		glGenerateMipmap(texture->gl_textype);
		
		glDisable(texture->gl_textype);
	}
	
	unbind_safe(texture);
}


void texture_create2d_empty(texture_t *texture, 
						  int internal_format, int format, int multisample,
						  unsigned int width, unsigned int height)
{
	IF_FAILED(texture && !texture->id);
	
	TRACE_MSG("create empty texture\n");
	
	glGenTextures(1, &texture->id);
	
	texture->gl_minfilter = GL_LINEAR;
	texture->gl_magfilter = GL_LINEAR;
	texture->dimension = 2;
	texture->gl_mipmap_level = 0;
	//if(!multisample)
		texture->gl_textype = GL_TEXTURE_2D;
	//else
	//	texture->gl_textype = GL_TEXTURE_2D_MULTISAMPLE;
	texture->is_empty = 1;
	
	bind_safe(texture);
	
	glTexParameteri(texture->gl_textype, GL_TEXTURE_MIN_FILTER, texture->gl_minfilter);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_MAG_FILTER, texture->gl_magfilter);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture->gl_textype, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	
	//if(!multisample)
		glTexImage2D(texture->gl_textype, 0, internal_format, width, height, 0, 
					 format, GL_UNSIGNED_BYTE, (const GLvoid*) 0);
	//else
	//	glTexImage2DMultisample(texture->gl_textype, 1, internal_format, width, height, 0);
	
	unbind_safe(texture);
}

int texture_create2d_from_tga(texture_t *texture, int use_mipmap, const char *filename)
{
	long file_size = 0L;
	struct tga_header *header;
	char *buffer = NULL;
	GLenum format;
	int internal_format;
	
	IF_FAILED0(texture && !texture->id && filename);
	
	TRACE_MSG("create texture from tga-file\n");
	
	// узнаем размер файла
	file_size = utils_file_get_size(filename);
	if(file_size < 0L) {
		ERROR_MSG("cannot get tga-file size %s\n", filename);
		return 0;
	}
	
	// если размер файла меньше заголовока, значит это точно не TGA
	if(sizeof(struct tga_header) >= file_size) {
		ERROR_MSG("file too small %s\n", filename);
		return 0;
	}
	
	// выделяем память под буфер данных
	// file_size+1, иначе крэшится в glTexImage2D, возможно баг видео драйверов
	buffer = (char*) malloc(sizeof(char) * (file_size+1));
	TRACE_MSG("allocated %li B for file buffer\n", ((file_size+1)*sizeof(char)) );
	
	if(buffer == NULL) {
		ERROR_MSG("cannot allocate memory for file %s\n; file_size = %li", filename, file_size);
		return 0;
	}
	
	TRACE_MSG("read texture data\n");
	if(!utils_read_file(filename, buffer)) {
		ERROR_MSG("cannot open tga-file %s\n", filename);
		free(buffer);
		return 0;
	}
	
	header = (struct tga_header*) buffer; 
	
	// проверяем формат tga изображения. должно быть без сжатия и глубина цвета не меньше 24 бита.
	if(header->image_type != 2 || 
			(header->image_spec.pixel_depth != 24 && header->image_spec.pixel_depth != 32) ) {
		
		ERROR_MSG("cannot read this type of tga-file %s\n", filename);
		free(buffer);
		return 0;
	}
	
	// устанавливаем форматы хранения изображения
	format = (header->image_spec.pixel_depth == 24) ? GL_BGR : GL_BGRA;
	internal_format = (header->image_spec.pixel_depth == 24) ? GL_RGB8 : GL_RGBA8;
	
	// создаем текстуру
	texture_create2d_from_data(texture, internal_format, format, 1,
							header->image_spec.image_width, 
						  	header->image_spec.image_height, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
						  	GL_UNSIGNED_BYTE, (void*) (buffer+sizeof(struct tga_header)+header->id_length));
	
	free(buffer);
	
	return 1;
}

void texture_bind(texture_t *texture)
{
	IF_FAILED(texture && texture->id);
	
	glBindTexture(texture->gl_textype, texture->id);
}

void texture_unbind(texture_t *texture)
{
	IF_FAILED(texture && texture->id);
	
	glBindTexture(texture->gl_textype, 0);
}

GLuint texrure_get_id(texture_t *texture)
{
	IF_FAILED0(texture);
	
	return (GLuint) texture->id;
}

GLuint texture_get_type(texture_t *texture)
{
	IF_FAILED0(texture && texture->id);
	
	return (GLuint) texture->gl_textype;
}

GLuint texture_get_minfilter(texture_t *texture)
{
	IF_FAILED0(texture && texture->id);
	
	return (GLuint) texture->gl_minfilter;
}

GLuint texture_get_magfilter(texture_t *texture)
{
	IF_FAILED0(texture && texture->id);
	
	return (GLuint) texture->gl_magfilter;
}

int texture_get_is_empty(texture_t *texture)
{
	IF_FAILED_RET(texture && texture->id, -1);
	
	return texture->is_empty;
}

void texture_release(texture_t *texture)
{
	IF_FAILED(texture && texture->id);
	
	glDeleteTextures(1, &texture->id);
	
	texture->id = 0;
	texture->gl_minfilter = 0;
	texture->gl_magfilter = 0;
	texture->dimension = 0;
	texture->gl_textype = 0;
	texture->is_empty = 1;
}

