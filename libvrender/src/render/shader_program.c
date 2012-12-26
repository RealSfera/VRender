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

#include "shader_program.h"

static int check_configuration(shader_program_t *program);
static void print_program_info(unsigned int program_id);

void shader_program_create(shader_program_t *program)
{
	IF_FAILED(program);
	
	TRACE_MSG("create shader program\n");
	
	program->program_id = glCreateProgram();
	TRACE_MSG("shader program id = %i\n", program->program_id);
	program->is_configured = 0;
	program->is_linked = 0;
}

int check_configuration(shader_program_t *program)
{
	IF_FAILED0(program);
	
	TRACE_MSG("check shader program configuration; id = %i\n", program->program_id);
	
	// шейдерная программа должна состоять как минимум из вершинного и фрагметного шейдеров
	// при их отсутствии последствия неопределены (взято из спецификации)
	if(program->vertex_shader.id > 0 && program->fragment_shader.id > 0)
		return 1;

	return 0;
}

// печатает последнюю информацию о шейдерной программе
void print_program_info(unsigned int program_id)
{
	char *log = NULL;
	int length = 0;
	
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
	log = (char*) malloc(sizeof(char) * length);
	
	glGetProgramInfoLog(program_id, length, NULL, log);
	DEBUG_CODE(printf("\n%s\n", log);)
	
	free(log);
}

int shader_program_link(shader_program_t *program)
{
	int program_link = 0, program_validate = 0;
	
	TRACE_MSG("link shader program; id = %i\n", program->program_id);
	
	IF_FAILED0(program && program->program_id);
	
	// проверяем конфигурацию шейдерной программы и линкуем её
	if(check_configuration(program)) {
		glLinkProgram(program->program_id);
		glGetProgramiv(program->program_id, GL_LINK_STATUS, &program_link);
		if(!program_link) {
			ERROR_MSG("shader program linking failed\n");
			print_program_info(program->program_id);
			return 0;
		}
		
		// проверим, всё ли нормально с шейдерной программой
		TRACE_MSG("validate shader program; id = %i\n", program->program_id);
		glValidateProgram(program->program_id);
		glGetProgramiv(program->program_id, GL_VALIDATE_STATUS, &program_validate);
		if(!program_validate) {
			ERROR_MSG("shader program is invalid\n");
			print_program_info(program->program_id);
			return 0;
		}
	} else {
		ERROR_MSG("shader program not configured\n");
		return 0;
	}
	
	program->is_linked = 1;
	
	return 1;
}

int shader_program_set_shader(shader_program_t *program, shader_t *shader)
{		
	IF_FAILED0(program && shader);
	IF_FAILED0(program->program_id && shader->id && shader->is_compiled);
	
	TRACE_MSG("set shader to program; program_id = %i shader_id = %i\n", program->program_id, shader->id);
	
	// в зависимости от типа шейдера, сначало отсоединяем предыдущий шейдер, затем присоединяем новый
	switch(shader->type) {
		case SHADER_VERTEX:
			if(program->vertex_shader.id)
				glDetachShader(program->program_id, program->vertex_shader.id);
			glAttachShader(program->program_id, shader->id);
			
			shader->shader_program = program->program_id;
			program->vertex_shader = *shader;
			break;
		case SHADER_FRAGMENT:
			if(program->fragment_shader.id)
				glDetachShader(program->program_id, program->fragment_shader.id);
			glAttachShader(program->program_id, shader->id);
			shader->shader_program = program->program_id;
			program->fragment_shader = *shader;
			break;
		case SHADER_GEOMETRY:
			if(program->geometry_shader.id)
				glDetachShader(program->program_id, program->geometry_shader.id);
			glAttachShader(program->program_id, shader->id);
			shader->shader_program = program->program_id;
			program->geometry_shader = *shader;
			break;
	};
	
	// произошли изменения в шейдерной программе => её нужно перелинковать
	program->is_linked = 0;
	
	return 1;
}

void shader_program_unset_shader(shader_program_t *program, shader_t *shader)
{
	IF_FAILED(program && shader);
	IF_FAILED(program->program_id && shader->id && shader->is_compiled);
	
	TRACE_MSG("unset shader to program; program_id = %i shader_id = %i\n", program->program_id, shader->id);
	
	switch(shader->type) {
		case SHADER_VERTEX:
			if(program->vertex_shader.id)
				glDetachShader(program->program_id, program->vertex_shader.id);
			program->vertex_shader.id = 0;
			break;
		case SHADER_FRAGMENT:
			if(program->fragment_shader.id)
				glDetachShader(program->program_id, program->fragment_shader.id);
			program->fragment_shader.id = 0;
			break;
		case SHADER_GEOMETRY:
			if(program->geometry_shader.id)
				glDetachShader(program->program_id, program->geometry_shader.id);
			program->geometry_shader.id = 0;
			break;
	};
	
	program->is_linked = 0;
}

void shader_program_bind(shader_program_t *program)
{	
	IF_FAILED(program && program->program_id);
	
	if(!program->is_linked) {
		ERROR_MSG("failed to bind shader program; program not linked\n");
		return;
	}
	
	glUseProgram(program->program_id);
}

void shader_program_unbind(shader_program_t *program)
{
	IF_FAILED(program && program->program_id);
		
	if(!program->is_linked) {
		ERROR_MSG("failed to unbind shader program; program not linked\n");
		return;
	}
		
	glUseProgram(0); // ???
}

int shader_program_get_uniform_loc(shader_program_t *program, const char* name)
{
	IF_FAILED0(program && program->program_id && name && program->is_linked);
		
	unsigned int attribute = glGetUniformLocation(program->program_id, (char*) name);
	
	if(attribute == -1) {
		ERROR_MSG("failed to get uniform location; name = %s\n", name);
		return -1;
	}

	return attribute;
}

int shader_program_get_attrib_loc(shader_program_t *program, const char* name)
{
	IF_FAILED0(program && program->program_id && name && program->is_linked);
		
	unsigned int attribute = glGetAttribLocation(program->program_id, (char*) name);
	
	if(attribute == -1) {
		ERROR_MSG("failed to get attrib location; name = %s\n", name);
		return -1;
	}

	return attribute;
}

GLuint shader_program_get_id(shader_program_t *program)
{
	IF_FAILED0(program);
	
	return (GLuint) program->program_id;
}

void shader_program_release(shader_program_t *program)
{
	IF_FAILED(program && program->program_id);
	
	TRACE_MSG("release shader program; program_id = %i \n", program->program_id);
	
	if(program->vertex_shader.id)
		glDetachShader(program->program_id, program->vertex_shader.id);
	if(program->fragment_shader.id)
		glDetachShader(program->program_id, program->fragment_shader.id);
	if(program->geometry_shader.id)
		glDetachShader(program->program_id, program->geometry_shader.id);
	
	glDeleteProgram(program->program_id);
	
	program->program_id = 0;
	program->is_configured = 0;
	program->is_linked = 0;
}
