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

#ifndef PARSER_H
#define PARSER_H

#define MAX_NUM_USER_VARS 20
#define MAX_NUM_TOKENS 256
#define MAX_NUM_FUNC_ARGS 7

typedef struct {
	int type;
	char *data;
	int data_size;
} token_t;

typedef struct {
	int type;
	char *name;
	int global_id;
} identifier_t;

typedef struct {
	int global_id;
	float value;
} float_var_value_t;

typedef struct {
		
	float_var_value_t *float_builtin_vars;
	
	identifier_t user_identifiers[MAX_NUM_USER_VARS];
	float_var_value_t float_user_vars[MAX_NUM_USER_VARS];
	unsigned num_user_ids;
	unsigned num_user_vars;
	int last_var_global_id;
	
	float_var_value_t *float_extra_vars;
	token_t tokens[MAX_NUM_TOKENS];
	unsigned num_tokens;
	unsigned index;
	int error;
	int init;
	int skip_expr;
	
} parser_t;

#ifdef __cplusplus
extern "C" {
#endif

// создать парсер (установить параметры по-умолчанию)
int parser_create(parser_t *parser);

// парсить строку text использую таблицу переменных var_table
int parser_parse_text(parser_t *parser, const char *text, float_var_value_t *var_table);

// очистить ресурсы парсера
void parser_clean(parser_t *parser);

#ifdef __cplusplus
}
#endif

#endif // PARSER_H
