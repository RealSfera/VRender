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

#include "string.h"
#include "shader_program_file.h"
#include "utils.h"

#define SHADER_VERTEX_BIT 1
#define SHADER_FRAGMENT_BIT 2
#define SHADER_GEOMETRY_BIT 4

static int parse_tokens(char* buffer_ptr, char **vertex_shader_ptr, char **fragment_shader_ptr, char **geometry_shader_ptr);
static int load_shader(const char *shader_buffer_ptr, shader_t *shader, unsigned int shader_type, unsigned int *bit_set, unsigned int bit);
static int parse_and_load_shader(char *buffer_ptr, shader_program_file_t *pfile);

// парсим и загружаем данные из buffer_ptr в pfile
int parse_and_load_shader(char *buffer_ptr, shader_program_file_t *pfile)
{
	char *vertex_shader_ptr = NULL;
	char *fragment_shader_ptr = NULL;
	char *geometry_shader_ptr = NULL;
	
	IF_FAILED0(buffer_ptr && pfile);
	
	TRACE_MSG("parse program file; program_id = %i \n", pfile->program.program_id);
	
	if(!parse_tokens(buffer_ptr, &vertex_shader_ptr, &fragment_shader_ptr, &geometry_shader_ptr)) {
		ERROR_MSG("failed to parse shader program file\n");
		return 0;
	}
	
	pfile->shaders_bit = 0;
	
	if(vertex_shader_ptr) {
		if(!load_shader(vertex_shader_ptr, &pfile->vertex_shader, SHADER_VERTEX, &pfile->shaders_bit, SHADER_VERTEX_BIT)) {
			ERROR_MSG("failed to load vs shader\n");
			return 0;
		}
	}	
	
	if(fragment_shader_ptr) {
		if(!load_shader(fragment_shader_ptr, &pfile->fragment_shader, SHADER_FRAGMENT, &pfile->shaders_bit, SHADER_FRAGMENT_BIT)) {
			ERROR_MSG("failed to load fs shader\n");
			return 0;
		}
	}
	
	if(geometry_shader_ptr) {
		if(!load_shader(geometry_shader_ptr, &pfile->geometry_shader, SHADER_GEOMETRY, &pfile->shaders_bit, SHADER_GEOMETRY_BIT)) {
			ERROR_MSG("failed to load gs shader\n");
			return 0;
		}
	}
	
	return 1;
}

// создаём shader, загружаем исходник в opengl и устанавливаем, соотвествующий типу шейдера, бит
int load_shader(const char *shader_buffer_ptr, shader_t *shader, unsigned int shader_type, unsigned int *bit_set, unsigned int bit)
{
	IF_FAILED0(shader_buffer_ptr && shader);
	
	shader_create(shader, shader_type);
	
	if(bit_set)
		*bit_set |= bit; // устанавливаем бит шейдера
	if(!shader_load_source_buffer(shader, shader_buffer_ptr)) {
		ERROR_MSG("failed to load shader buffer in shader program file\n");
		return 0;
	}
	
	return 1;
}


// проверить, есть ли токен token в последующих в буфере in_buffer.
// проверка до первого пробела или \n
int check_tok_str(const char *in_buffer, const char *token)
{
	int found = 0;
	
	while(*in_buffer != ' ' && *in_buffer != '\n') {
		if( *in_buffer != *token ) {
			found = 1;
			break;
		}
		in_buffer++;
		token++;
	}
	
	if(found)
		return 0;
	
	return 1;
}

// выводит текущий токен (до первого пробела или \n)
void next_tok_print(const char *in_buffer, char *output)
{
	unsigned int i = 0;
	
	while(*in_buffer != ' ' && *in_buffer != '\n') {
		*(output+i) = *in_buffer;
		in_buffer++;
		i++;
	}
}

// зануляет строку до сивола перехода (пробелами) и смещает указатель на конец строки
void delete_line(char *inout_buffer)
{
	while(*inout_buffer != '\n') {
		*inout_buffer = ' ';
		inout_buffer++;
	}
}

// парсим файл на предмет меток (токенов) типа !!vs, !!fs и !!gs
// в указатели помещаем указатели на место, где находится каждый вид шейдера
int parse_tokens(char* buffer_ptr, char **vertex_shader_ptr, char **fragment_shader_ptr, char **geometry_shader_ptr)
{
	char *tok = NULL;
	int tok_found = 0;
	
	IF_FAILED0(buffer_ptr);
	
	*vertex_shader_ptr = NULL;
	*fragment_shader_ptr = NULL;
	*geometry_shader_ptr = NULL;
	
	while(*buffer_ptr != '\0') {
		if((tok = strstr(buffer_ptr, "\n!!")) != NULL) {
			
			// пропускаем \n
			tok++;
			
			tok_found = 0;
			// ищем токены
			if( check_tok_str(tok+2, "vs") ) {
				*vertex_shader_ptr = tok+4; tok_found = 1;
			} else if( check_tok_str(tok+2, "fs") ) {
				*fragment_shader_ptr = tok+4; tok_found = 1;
			} else if( check_tok_str(tok+2, "gs") ) {
				*geometry_shader_ptr = tok+4; tok_found = 1;
			} else if( check_tok_str(tok+2, "include") ) {
				
				ERROR_MSG("!!include not supported\n");
				
				delete_line(tok);
			} else {
				char token[32];
				
				next_tok_print(tok+2, token);
				ERROR_MSG("unknown token \"%s\"\n", token);
				
				delete_line(tok);
			}
			
			// если нашли до этого токен !!, то этот токен будет для предыдущего нулевым байтом
			if(tok_found) {
				*tok = '\0'; 
				*(tok+1) = *(tok+2) = *(tok+3) = ' ';
			}
			
			// символ следующий после токена будет началом шейдера
			buffer_ptr = tok+4;
		} else {
			break;
		}
	}
	
	return 1;
}

int shader_program_file_create(shader_program_file_t *pfile, const char *filename)
{
	char *buffer = NULL;
	long file_size = 0L;
	
	IF_FAILED0(pfile && filename);
	
	TRACE_MSG("create shader program file; filename = %s \n", filename);
	
	file_size = utils_file_get_size(filename);
	// file_size + 1 - чтобы хватило места для \0
	buffer = (char*) malloc(sizeof(char) * (file_size+1));
	
	if(!utils_read_file(filename, buffer)) {
		ERROR_MSG("cannot open shader program file\n");
		return 0;
	}
	
	if(!parse_and_load_shader(buffer, pfile))
		return 0;
	
	free(buffer);
	
	return 1;
}

int shader_program_file_create_from_buffer(shader_program_file_t *pfile, const char *buffer)
{
	IF_FAILED0(pfile && buffer);
	
	TRACE_MSG("create shader program from buffer\n");
	
	unsigned long length = strlen(buffer);
	char *copy_buffer = (char*) malloc(sizeof(char) * length+1);
	memcpy(copy_buffer, buffer, length+1);
	
	if(!parse_and_load_shader(copy_buffer, pfile)) {
		free(copy_buffer);
		return 0;
	}
	
	free(copy_buffer);
	
	return 1;
}

int shader_program_file_get_program(shader_program_file_t *pfile, shader_program_t *program)
{
	IF_FAILED0(pfile && program);
	
	if(!pfile->program.program_id)
		shader_program_create(&pfile->program);
	
	// устанавливаем шейдер в шейдерную программу, если его его номер > 0 и если установлен его бит
	if(pfile->vertex_shader.id && (pfile->shaders_bit & SHADER_VERTEX_BIT))
		shader_program_set_shader(&pfile->program, &pfile->vertex_shader);
	if(pfile->fragment_shader.id && (pfile->shaders_bit & SHADER_FRAGMENT_BIT))
		shader_program_set_shader(&pfile->program, &pfile->fragment_shader);
	if(pfile->geometry_shader.id && (pfile->shaders_bit & SHADER_GEOMETRY_BIT))
		shader_program_set_shader(&pfile->program, &pfile->geometry_shader);
	
	*program = pfile->program;
	
	return 1;
}

void shader_program_file_destroy(shader_program_file_t *pfile)
{
	IF_FAILED(pfile);
	
	TRACE_MSG("release shader program file; program_id = %i \n", pfile->program.program_id);
	
	if(pfile->shaders_bit & SHADER_VERTEX_BIT) {
		shader_delete(&pfile->vertex_shader);
	}
	
	if(pfile->shaders_bit & SHADER_FRAGMENT_BIT) {
		shader_delete(&pfile->fragment_shader);
	}
	
	if(pfile->shaders_bit & SHADER_GEOMETRY_BIT) {
		shader_delete(&pfile->geometry_shader);
	}
	
	if(pfile->program.program_id) {
		shader_program_release(&pfile->program);
	}
	
	pfile->shaders_bit = 0;
}
