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

#include "shader.h"
#include "utils.h"

static void print_shader_info(unsigned int shader_id);
static int compile_shader(unsigned int shader_id, const char *buffer);

void shader_create(shader_t *shader, unsigned int type)
{
	IF_FAILED(shader && type);
	
	TRACE_MSG("create shader; type = %i\n", type);
	
	// создаем шейдер опрделенного типа
	switch(type) {
		case SHADER_VERTEX:
			shader->id = glCreateShader(GL_VERTEX_SHADER);
			break;
		case SHADER_FRAGMENT:
			shader->id = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		case SHADER_GEOMETRY:
			shader->id = glCreateShader(GL_GEOMETRY_SHADER);
			break;
		default:
			ERROR_MSG("unknown shader type\n");
			return;
	};
	
	shader->type = type;
	shader->is_compiled = 0;
	shader->shader_program = 0;
}

// печатает последнюю информацию о шейдере (например, ошибки)
void print_shader_info(unsigned int shader_id)
{
	char *log = NULL;
	int length = 0;
	
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
	log = (char*) malloc(sizeof(char) * length);
	
	glGetShaderInfoLog(shader_id, length, NULL, log);
	DEBUG_CODE(printf("\n%s\n", log);)
	
	free(log);
}

// загружаем исходник шейдера в opengl и компилим его
int compile_shader(unsigned int shader_id, const char *buffer)
{
	int shader_compile = 0;
	
	TRACE_MSG("compile shader; id = %i\n", shader_id);
	
	IF_FAILED0(shader_id && buffer);
	
	glShaderSource(shader_id, 1, &buffer, NULL);
	glCompileShader(shader_id);
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compile);
	
	if(!shader_compile) {
		ERROR_MSG("failed to compile shader; shader_id = %i\n", shader_id);
		print_shader_info(shader_id);
		return 0;
	}
	
	return 1;
}

int shader_load_source_file(shader_t *shader, const char* filename)
{
	char *buffer = NULL;
	long file_size = 0L;
	
	IF_FAILED0(shader && filename && shader->id);
	
	TRACE_MSG("load shader from file; filename = %s id = %i\n", filename, shader->id);
	
	file_size = utils_file_get_size(filename);
	buffer = (char*) malloc(sizeof(char) * file_size);
	
	if(!utils_read_file(filename, buffer)) {
		ERROR_MSG("cannot read shader file %s; shader_type = %i\n", filename, shader->type);
		
		free(buffer);
		
		return 0;
	}
	
	if(!compile_shader(shader->id, buffer)) {
		ERROR_MSG("shader compiling failed; shader_type = %i\n", shader->type);
		
		free(buffer);
		
		return 0;
	}
	
	shader->is_compiled = 1;
	
	free(buffer);
	
	return 1;
}

int shader_load_source_buffer(shader_t *shader, const char* buffer)
{
	IF_FAILED0(shader && buffer && shader->id);
	
	TRACE_MSG("load shader from buffer; id = %i\n", shader->id);
	
	if(!compile_shader(shader->id, buffer)) {
		ERROR_MSG("shader compiling failed; shader_type = %i\n", shader->type);
		return 0;
	}
	
	shader->is_compiled = 1;
	
	return 1;
}

GLuint shader_get_id(shader_t *shader)
{
	IF_FAILED0(shader);
	
	return (GLuint) shader->id;
}

GLenum shader_get_type(shader_t *shader)
{
	IF_FAILED0(shader);
	
	switch(shader->type) {
		case SHADER_VERTEX: return GL_VERTEX_SHADER;
		case SHADER_FRAGMENT: return GL_FRAGMENT_SHADER;
		case SHADER_GEOMETRY: return GL_GEOMETRY_SHADER;
	};
	
	return 0;
}

void shader_delete(shader_t *shader)
{
	IF_FAILED(shader);
	
	TRACE_MSG("delete shader; type = %i id = %i\n", shader->type, shader->id);
	
	if(shader->id != 0)
		glDeleteShader(shader->id);
	
	shader->id = 0;
	shader->type = 0;
	shader->is_compiled = 0;
	shader->shader_program = 0;
}
