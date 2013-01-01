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

#include "common.h"
#include "parser.h"

#include "math/dmath.h"
#include "math/noise.h"
#include <ctype.h>
#include <string.h>
#include <locale.h>

/////////////// -------

static float call_sinf(float x)
{
	return math_sinf(x);
}

static float call_cosf(float x)
{
	return math_cosf(x);
}

static float call_asinf(float x)
{
	return math_asinf(x);
}

static float call_acosf(float x)
{
	return math_acosf(x);
}

static float call_fabs(float x)
{
	return math_fabs(x);
}

static float call_tanf(float x)
{
	return math_tanf(x);
}

static float call_sqrtf(float x)
{
	return math_sqrtf(x);
}

static float call_cerpf(float a, float b, float t)
{
	return math_cerp(a, b, t);
}

static float call_clampf(float x, float a, float b)
{
	return math_clamp(x, a, b);
}

static float call_lerpf(float a, float b, float t)
{
	return math_lerp(a, b, t);
}

static float call_maxf(float a, float b)
{
	return math_max(a, b);
}

static float call_minf(float a, float b)
{
	return math_min(a, b);
}

static float call_radian(float x)
{
	return RAD_TO_DEG(x);
}

static float call_ceilf(float x)
{
	return ceil((float) x);
}

static float call_coshf(float x)
{
	return cosh((float) x);
}

static float call_sinhf(float x)
{
	return sinh((float) x);
}

static float call_tanhf(float x)
{
	return tanh((float) x);
}

static float call_expf(float x)
{
	return exp((float) x);
}

static float call_floorf(float x)
{
	return floor((float) x);
}

static float call_fmod(float x, float y)
{
	return fmod((float) x, (float) y);
}

static float call_logf(float x)
{
	return log((float) x);
}

static float call_log10f(float x)
{
	return log10((float) x);
}

static float call_atanf(float x)
{
	return atan((float) x);
}

static float call_cbrtf(float x)
{
	return cbrt((float) x);
}

static float call_distance3(float x1, float y1, float z1,  
						   float x2, float y2, float z2)
{
	return vec3f_distance(vec3f(x1, y1, z1), vec3f(x2, y2, z2));
}

static float call_dot3(float x1, float y1, float z1,  
					   float x2, float y2, float z2)
{
	return vec3f_dot(vec3f(x1, y1, z1), vec3f(x2, y2, z2));
}

static float call_length3(float x, float y, float z)
{
	return vec3f_length(vec3f(x, y, z));
}

static float call_distance2(float x1, float y1,  
						   float x2, float y2)
{
	return vec2f_distance(vec2f(x1, y1), vec2f(x2, y2));
}

static float call_length2(float x, float y)
{
	return vec2f_length(vec2f(x, y));
}

static float call_dot2(float x1, float y1,  
					   float x2, float y2)
{
	return vec2f_dot(vec2f(x1, y1), vec2f(x2, y2));
}

static float call_degree(float x)
{
	return RAD_TO_DEG(x);
}

static float call_vnoise1(float x)
{
	return value_noise_1d(x);
}

static float call_vnoise2(float x, float y)
{
	return value_noise_2d(vec2f(x, y));
}

static float call_vnoise3(float x, float y, float z)
{
	//return value_noise_3d(vec3f(x, y, z));
	return vnoise3d_trilerp_file(vec3f(x, y, z));
}

static float call_pnoise3(float x, float y, float z)
{
	return perlin_noise_3d(vec3f(x, y, z));
}

/////////////// -----

// основные типы токенов
enum {
	UNKNOWN = 1,
	IDENTIFIER, FLOAT_NUMBER, INT_NUMBER, 
	EQUAL, PLUS_EQL, MINUS_EQL, MULT_EQL, DIV_EQL,
	EQL_EQL, GRT_EQL, LESS_EQL, NOT_EQL,
	GREATER, LESS, NOT,
	MULT_MULT, QUEST_MARK, DOT_DOT,
	PLUS, MINUS, MULT, DIV, 
	COMMA, DOT, SEMICOLON, COLON,
	PARENTH_LEFT, PARENTH_RIGHT
};

// типы идентификаторов
enum {
	VARIABLE = 1, FUNCTION
};

// типы указателей на функции с разным кол-вом параметров
typedef float (*func1_t)(float);
typedef float (*func2_t)(float, float);
typedef float (*func3_t)(float, float, float);
typedef float (*func4_t)(float, float, float, float);
typedef float (*func5_t)(float, float, float, float, float);
typedef float (*func6_t)(float, float, float, float, float, float);

typedef struct {
	int global_id;
	func1_t func;
} float_function1_t;

typedef struct {
	int global_id;
	func2_t func;
} float_function2_t;

typedef struct {
	int global_id;
	func3_t func;
} float_function3_t;

typedef struct {
	int global_id;
	func4_t func;
} float_function4_t;

typedef struct {
	int global_id;
	func5_t func;
} float_function5_t;

typedef struct {
	int global_id;
	func6_t func;
} float_function6_t;

typedef struct {
	int global_id;
	int n_args;
} function_param_t;


// встроенные функции с разным кол-вом параметров

static float_function1_t builtin_functions1[] =
	{
		{ 101, (func1_t) &call_sinf },
		{ 102, (func1_t) &call_cosf },
		{ 103, (func1_t) &call_tanf },
		{ 104, (func1_t) &call_sqrtf },
		{ 109, (func1_t) &call_radian },
		{ 110, (func1_t) &call_degree },
		{ 112, (func1_t) &call_fabs },
		{ 113, (func1_t) &call_asinf },
		{ 114, (func1_t) &call_acosf },
	
		{ 115, (func1_t) &call_ceilf },
		{ 116, (func1_t) &call_coshf },
		{ 117, (func1_t) &call_sinhf },
		{ 118, (func1_t) &call_tanhf },
		{ 119, (func1_t) &call_expf },
		{ 120, (func1_t) &call_floorf },
		{ 122, (func1_t) &call_logf },
		{ 123, (func1_t) &call_log10f },
		{ 124, (func1_t) &call_atanf },
		{ 125, (func1_t) &call_cbrtf },
	
		{ 132, (func1_t) &call_vnoise1 },
		{0, 0}
	};

static float_function2_t builtin_functions2[] =
	{
		{ 105, (func2_t) &call_minf },
		{ 106, (func2_t) &call_maxf },
	
		{ 121, (func2_t) &call_fmod },
	
		{ 131, (func2_t) &call_length2 },
	
		{ 133, (func2_t) &call_vnoise2 },
		{0, 0}
	};

static float_function3_t builtin_functions3[] =
	{
		{ 107, (func3_t) &call_cerpf },
		{ 108, (func3_t) &call_lerpf },
		{ 111, (func3_t) &call_clampf },
	
		{ 129, (func3_t) &call_length3 },

		{ 134, (func3_t) &call_vnoise3 },
	    { 135, (func3_t) &call_pnoise3 },
		{0, 0}
	};

static float_function4_t builtin_functions4[] =
	{
		{ 126, (func4_t) &call_dot2 },
		{ 130, (func4_t) &call_distance2 },
		{0, 0}
	};

static float_function5_t builtin_functions5[] =
	{
		{0, 0}
	};

static float_function6_t builtin_functions6[] =
	{
		{ 127, (func6_t) &call_distance3 },
		{ 128, (func6_t) &call_dot3 },
		{0, 0}
	};

static function_param_t builtin_functions_params[] =
	{
		{ 101, 1 },
		{ 102, 1 },
		{ 103, 1 },
		{ 104, 1 },
		{ 105, 2 },
		{ 106, 2 },
		{ 107, 3 },
		{ 108, 3 },
		{ 109, 1 },
		{ 110, 1 },
		{ 111, 3 },
		{ 112, 1 },
		{ 113, 1 },
		{ 114, 1 },

		{ 115, 1 },
		{ 116, 1 },
		{ 117, 1 },
		{ 118, 1 },
		{ 119, 1 },
		{ 120, 1 },
		{ 121, 1 },
		{ 122, 1 },
		{ 123, 1 },
		{ 124, 1 },
		{ 125, 1 },
	
		{ 126, 4 },
		{ 127, 6 },
		{ 128, 6 },
		{ 129, 3 },
		{ 130, 4 },
		{ 131, 2 },

		{ 132, 1 },
		{ 133, 2 },
		{ 134, 3 },
		{ 135, 3 },
		{0, 0}
	};


// встроенные идентификаторы
static identifier_t builtin_identifiers[] = 
	{
		{VARIABLE, "d", 1}, // 1
		{VARIABLE, "x", 2},
		{VARIABLE, "y", 3},
		{VARIABLE, "z", 4},
		//{VARIABLE, "t", 5},
		{VARIABLE, "i", 6},
		
		{FUNCTION, "sin", 101},
		{FUNCTION, "cos", 102},
		{FUNCTION, "tan", 103},
		{FUNCTION, "sqrt", 104},
		{FUNCTION, "min", 105},
		{FUNCTION, "max", 106},
		{FUNCTION, "cerp", 107},
		{FUNCTION, "lerp", 108},
		{FUNCTION, "radian", 109},
		{FUNCTION, "degree", 110},
		{FUNCTION, "clamp", 111},
		{FUNCTION, "abs", 112},
		{FUNCTION, "asin", 113},
		{FUNCTION, "acos", 114},

		{FUNCTION, "ceil", 115},
		{FUNCTION, "cosh", 116},
		{FUNCTION, "sinh", 117},
		{FUNCTION, "tanh", 118},
		{FUNCTION, "exp", 119},
		{FUNCTION, "floor", 120},
		{FUNCTION, "fmod", 121},
		{FUNCTION, "ln", 122},
		{FUNCTION, "log10", 123},
		{FUNCTION, "atan", 124},
		{FUNCTION, "cbrt", 125},
	
		{FUNCTION, "dot2", 126},
		{FUNCTION, "distance3", 127},
		{FUNCTION, "dot3", 128},
		{FUNCTION, "length3", 129},
		{FUNCTION, "distance2", 130},
		{FUNCTION, "length2", 131},

		{FUNCTION, "vnoise1", 132},
		{FUNCTION, "vnoise2", 133},
		{FUNCTION, "vnoise3", 134},
		{FUNCTION, "pnoise3", 135},
		{0, "", 0}
	};

// последний глобальный индекс переменных
static int last_var_global_id = 7;
//static int last_func_global_id = 136;

static void eval_expr(parser_t *p,float *value);
static void eval_expr0(parser_t *p,float *value);
static void eval_expr1(parser_t *p,float *value);
static void eval_expr2(parser_t *p,float *value);
static void eval_expr3(parser_t *p,float *value);
static void eval_expr4(parser_t *p,float *value);
static void eval_expr5(parser_t *p,float *value);
static void eval_expr6(parser_t *p,float *value);
static void eval_expr7(parser_t *p,float *value);
static void eval_expr8(parser_t *p,float *value);
static void eval_expr9(parser_t *p,float *value);
static void eval_expr10(parser_t *p,float *value);
static void eval_expr11(parser_t *p,float *value);
static void atom(parser_t *p,float *value);

INLINE static int find_identifier(parser_t *p,const char *name, int type);
char* get_var_name(parser_t *p,int global_id);
INLINE static void set_error(parser_t *p,const char *error_text, int error_num);
static int create_user_var(parser_t *p,const char *name);
static float call_func(parser_t *p,int func_id);
static void assign_variable(parser_t *p,int var_id, float value);
static float get_var_value(parser_t *p,int global_id);

static int lexer_parser(parser_t *p, const char *text);
static void syntax_parser(parser_t *p);

#define find_var(p, name) (find_identifier(p, name, VARIABLE))
#define find_func(p, name) (find_identifier(p, name, FUNCTION))

//#define DMSG(text, ...) TRACE_MSG(text, ##__VA_ARGS__)
#define DMSG(text, ...)

// найти идентификатор в таблицах
INLINE static int find_identifier(parser_t *p, const char *name, int type)
{
	IF_FAILED_RET(name, -1);
	
	int i = 0;	
	while(builtin_identifiers[i].type != 0) {
		if(!strcmp(builtin_identifiers[i].name, name)) {
			if(builtin_identifiers[i].type == type)
				return builtin_identifiers[i].global_id;
		}
		i++;
	}

	for(i = 0; i < p->num_user_ids; i++) {
		if(!strcmp(p->user_identifiers[i].name, name)) {
			if(p->user_identifiers[i].type == type)
				return p->user_identifiers[i].global_id;
		}
	}
	
	return -1;
}

// получить имя переменной по глобальному номеру
char* get_var_name(parser_t *p, int global_id)
{
	int i = 0;
	
	while(builtin_identifiers[i].global_id != 0) {
		if(builtin_identifiers[i].global_id == global_id && builtin_identifiers[i].type == VARIABLE) {
			return builtin_identifiers[i].name;
		}
		i++;
	}

	for(i = 0; i < p->num_user_ids; i++) {
		if(p->user_identifiers[i].global_id == global_id && p->user_identifiers[i].type == VARIABLE) {
			return p->user_identifiers[i].name;
		}
	}

	return NULL;
}

// установить ошибку
INLINE static void set_error(parser_t *p, const char *error_text, int error_num)
{
	DMSG("syntax error %i: \"%s\" in token \"%s\"\n", error_num, error_text, p->tokens[p->index].data);
	
	if(p->error == 0)
		p->error = error_num;
}

// создать пользовательскую переменную
static int create_user_var(parser_t *p, const char *name)
{
	DMSG("create user var %s\n", name);
	
	// проверить на превышение кол-ва пользовательских переменных
	if(p->num_user_ids >= MAX_NUM_USER_VARS-1) {
		set_error(p, "max count user vars encountered", 12);
		return -1;
	}

	int id = p->num_user_ids++;
	DMSG("--- user_identifiers index = %i\n", id);
	
	int len = strlen(name);
	
	p->user_identifiers[id].name = (char*) malloc(sizeof(char)*(len+1));
	memcpy(p->user_identifiers[id].name, name, len+1);
	
	p->user_identifiers[id].global_id = p->last_var_global_id++;
	p->user_identifiers[id].type = VARIABLE;

	DMSG("--- var name %s, global_id = %i\n", p->user_identifiers[id].name, p->user_identifiers[id].global_id);

	int var_i = p->num_user_vars++;
	DMSG("--- float_user_vars index = %i\n", var_i);
	
	p->float_user_vars[var_i].global_id = p->user_identifiers[id].global_id;
	p->float_user_vars[var_i].value = 0.0f;
	
	return p->user_identifiers[id].global_id;
}

// присвоить значение переменной
static void assign_variable(parser_t *p, int global_id, float value)
{
	int i = 0;
	
	DMSG("assign %s(%i) = %f\n", get_var_name(p, global_id), global_id, value);

	for(i = 0; i < p->num_user_ids; i++) {
		if(p->float_user_vars[i].global_id == global_id) {
			p->float_user_vars[i].value = value;
			return;
		}
	}
	
	i = 0;
	while(p->float_extra_vars[i].global_id != 0) {
		if(p->float_extra_vars[i].global_id == global_id) {
			p->float_extra_vars[i].value = value;
			return;
		}
		i++;
	}
	
	i = 0;
	while(p->float_builtin_vars[i].global_id != 0) {
		if(p->float_builtin_vars[i].global_id == global_id) {
			p->float_builtin_vars[i].value = value;
			return;
		}
		i++;
	}
	
	set_error(p, "unknown variable", 6);
}

// вызвать функцию и вернуть результат её вызова
static float call_func(parser_t *p, int func_id)
{
	float args[MAX_NUM_FUNC_ARGS];
	int n_args = 0;
	
	func1_t func1_ptr = NULL;
	func2_t func2_ptr = NULL;
	func3_t func3_ptr = NULL;
	func4_t func4_ptr = NULL;
	func5_t func5_ptr = NULL;
	func6_t func6_ptr = NULL;

	if(p->tokens[p->index].type == IDENTIFIER) {
		p->index++;
	} else {
		set_error(p, "expected function name", 11);
		return 0.0f;
	}
	
	if(p->tokens[p->index].type == PARENTH_LEFT) {

		p->index++;

		int i = 0, flag_found = 0;
		while(builtin_functions_params[i].global_id != 0) {
			if(builtin_functions_params[i].global_id == func_id) {
				n_args = builtin_functions_params[i].n_args;
				break;
			}
			i++;
		}
		
		if(n_args == 0) {
			set_error(p, "wrong function", 10);
			return 0.0f;
		}
		
#define func_ptrs(n) \
	case n: {\
		while(builtin_functions##n[i].global_id != 0) {\
			if(builtin_functions##n[i].global_id == func_id) {\
				func##n##_ptr = builtin_functions##n[i].func;\
				flag_found = 1;\
				break;\
			}\
			i++;\
		}\
		break; }
		
		i = 0;
		switch(n_args) {
			func_ptrs(1);
			func_ptrs(2);
			func_ptrs(3);
			func_ptrs(4);
			func_ptrs(5);
			func_ptrs(6);
		};
#undef func_ptrs
		
		if(!flag_found) {
			set_error(p, "wrong number of arguments", 8);
			return 0.0f;
		}
		
		DMSG("call func id = %i, n_args = %i\n", func_id, n_args);

		if(!func1_ptr && !func2_ptr && !func3_ptr 
		   && !func4_ptr && !func5_ptr && !func6_ptr) {
			
			set_error(p, "null pointer to function", 7);
			return 0.0f;
		}

		int flag_found_pr = 0, r = 0;
		for(r = 0; r < n_args; r++) {
			eval_expr1(p, &args[r]);
			DMSG("args[%i] = %f\n", r, args[r]);
			if(p->tokens[p->index].type == COMMA) {
				p->index++;
				//continue;
			} else if(p->tokens[p->index].type == PARENTH_RIGHT) {
				flag_found_pr = 1;
				p->index++;
				break;
			}
		}

		if(!flag_found_pr) {
			set_error(p, "expected )", 5);
			return 0.0f;
		}
		
		if(r+1 != n_args) {
			set_error(p, "wrong number of arguments", 8);
			return 0.0f;
		}

	} else {
		set_error(p, "expected (", 9);
		return 0.0f;
	}
	
	if(p->skip_expr)
		return 0.0f;
	
	float result = 0.0f;

	switch(n_args) {
		case 1:
			result = (*func1_ptr)(args[0]);
			break;
		case 2:
			result = (*func2_ptr)(args[0], args[1]);
			break;
		case 3:
			result = (*func3_ptr)(args[0], args[1], args[2]);
			break;
		case 4:
			result = (*func4_ptr)(args[0], args[1], args[2], args[3]);
			break;
		case 5:
			result = (*func5_ptr)(args[0], args[1], args[2], args[3], args[4]);
			break;
		case 6:
			result = (*func6_ptr)(args[0], args[1], args[2], args[3], args[4], args[5]);
			break;
		default:
			set_error(p, "wrong number of arguments", 8);
	}

	return result;
}

// получить значение переменной
static float get_var_value(parser_t *p, int global_id)
{
	int i = 0;
	
	while(p->float_builtin_vars[i].global_id != 0) {
		if(p->float_builtin_vars[i].global_id == global_id) {
			return p->float_builtin_vars[i].value;
		}
		i++;
	}
	
	i = 0;
	while(p->float_extra_vars[i].global_id != 0) {
		if(p->float_extra_vars[i].global_id == global_id) {
			return p->float_extra_vars[i].value;
		}
		i++;
	}

	for(i = 0; i < p->num_user_ids; i++) {
		if(p->float_user_vars[i].global_id == global_id) {
			return p->float_user_vars[i].value;
		}
	}
	
	set_error(p, "unknown variable", 6);
	
	return 0.0f;
}

// вычисление переменных, констант и функций
static void atom(parser_t *p, float *value)
{
	int id = 0, last = 0;
	
	switch(p->tokens[p->index].type) {
		case IDENTIFIER:
			
			if((id = find_func(p, p->tokens[p->index].data)) != -1) {
				last = p->index;
				*value = call_func(p, id);
				
				if(!p->skip_expr)
					DMSG("call func: %s = %f\n", p->tokens[last].data, *value);
				
			} else if((id = find_var(p, p->tokens[p->index].data)) != -1) {
				
				if(p->skip_expr) {
					p->index++;
					return;
				}
				
				last = p->index;
				*value = get_var_value(p, id);
				DMSG("get var: %s = %f\n", p->tokens[last].data, *value);
				p->index++;
			} else {
				if(p->tokens[p->index+1].type == PARENTH_LEFT)
					set_error(p, "unknown function", 14);
				else
					set_error(p, "unknown variable", 6);
				//index++;
			}
			
			break;
		case FLOAT_NUMBER:
			
			if(p->skip_expr) {
				p->index++;
				return;
			}
			
			*value = (float) atof((const char*) p->tokens[p->index].data);
			DMSG("found float num: %f\n", *value);
			p->index++;
			break;
		case INT_NUMBER:
			
			if(p->skip_expr) {
				p->index++;
				return;
			}
			
			*value = (float) atoi((const char*) p->tokens[p->index].data);
			DMSG("found int num: %f\n", *value);
			p->index++;
			break;
		case PARENTH_RIGHT:
			return;
		default:
			set_error(p, "unknown error", -1);
	};
}

// вычисление скобок
static void eval_expr11(parser_t *p, float *value)
{
	if(p->tokens[p->index].type == PARENTH_LEFT) {
		p->index++;
		
		// вычисляем выражение после открывающей скобки
		eval_expr(p, value);
		
		// псоле вычисления должна идти закрывающая скобка...
		if(p->tokens[p->index].type != PARENTH_RIGHT) {
			//error = -8;
			set_error(p, "expected )", 5);
		}
		p->index++;
	} else {
		
		// вычисляем идентификатор или число
		atom(p, value);
	}
}

// вычисление унарного ! (факториал)
static void eval_expr10(parser_t *p, float *value)
{	
	eval_expr11(p, value);
	
	if(p->skip_expr)
		return;
	
	if(p->tokens[p->index].type == NOT) {
		DMSG("found unary: %s (i = %i)\n", p->tokens[p->index].data, p->index);
		
		if(*value == 0.0f) {
			*value = 1.0f;
			p->index++;
			return;
		}
		
		// пропускаем отрицательный факториал
		if(*value < 0.0f) {
			DMSG("skip factorial; value < 0\n");
			p->index++;
			return;
		}
		
		float s = 1.0f;
		for(unsigned i = 1; i <= (unsigned) *value; i += 1) {
			s *= i;
		}
		*value = s;
		
		p->index++;
	}
}

// вычисление унарного + и -
static void eval_expr9(parser_t *p, float *value)
{
	int last = -1;
	
	if(p->tokens[p->index].type == PLUS || p->tokens[p->index].type == MINUS) {
		// если токен + или -, то смещаемся к след. токену..
		last = p->index++;
	}
	// ...и вычисляем выражение (после + или -) 
	eval_expr10(p, value);
	
	if(p->skip_expr)
		return;
	
	// если был обнаружен унарный -, то меняем знак
	if(last > -1)
		if(p->tokens[last].type == MINUS) {
			DMSG("found unary: %s (i = %i)\n", p->tokens[last].data, last);
			*value = -(*value);
			//DMSG("value = %f\n", *value);
		}
}

// вычисление возведения в степень
static void eval_expr8(parser_t *p, float *value)
{
	eval_expr9(p, value);

	if(p->tokens[p->index].type == MULT_MULT) {
		
		// степень
		float part = 0.0f;
		
		DMSG("found power: %s (i = %i)\n", p->tokens[p->index].data, p->index);
		p->index++;
		eval_expr8(p, &part);
		
		if(p->skip_expr)
			return;
		
		
		// оптимизации возведения
		
		if(part == 0.0f) {
			*value = 1.0f;
			return;
		}
		
		if(*value == 0.0f) {
			*value = 0.0f;
			return;
		}
		
		if(part == 1.0f) {
			return;
		}
		
		if(part == 2.0f) {
			*value = *value * *value;
			return;
		}
		
		if(part == 3.0f) {
			*value = *value * *value* *value;
			return;
		}
		

		//DMSG("part = %f ** value = %f (i = %i)\n", part, *value, index);
		*value = powf(*value, part);
	}
}

// вычисление * и /
static void eval_expr7(parser_t *p, float *value)
{
	eval_expr8(p, value);
	
	while(p->tokens[p->index].type == MULT || p->tokens[p->index].type == DIV) {
		float part = 0.0f;
		int last = 0;
		DMSG("found arithmetic: %s (i = %i)\n", p->tokens[p->index].data, p->index);
		last = p->index++;
		
		// вычисляем выражение после арифм. операции
		eval_expr8(p, &part);
		
		if(p->skip_expr)
			return;
		
		switch(p->tokens[last].type) {
			case MULT:
				*value *= part;
				break;
			case DIV:
				if(part == 0.0f) {
					set_error(p, "dividing by zero", 15);
					*value = 0.0f;
					return;
				}
				*value /= part;
				break;
		};
		DMSG("value = %f (i = %i)\n", *value, p->index);
	}
}

// вычисление + и -
static void eval_expr6(parser_t *p, float *value)
{
	eval_expr7(p, value);
	
	while(p->tokens[p->index].type == PLUS || p->tokens[p->index].type == MINUS) {
		float part = 0.0f;
		int last = 0;
		DMSG("found arithmetic: %s (i = %i)\n", p->tokens[p->index].data, p->index);
		last = p->index++;
		
		// вычисляем выражение после арифм. операции
		eval_expr7(p, &part);
		
		if(p->skip_expr)
			return;
		
		switch(p->tokens[last].type) {
			case PLUS:
				*value += part;
				break;
			case MINUS:
				*value -= part;
				break;
		};
	}
}

// вычисление операций сравнения > < >= <=
static void eval_expr5(parser_t *p, float *value)
{
	eval_expr6(p, value);
	
	if(p->tokens[p->index].type == GREATER || p->tokens[p->index].type == LESS ||
	   p->tokens[p->index].type == GRT_EQL || p->tokens[p->index].type == LESS_EQL) {
		
		float part = 0.0f;
		int last = 0;
		
		DMSG("found relational-operator: %s (i = %i)\n", p->tokens[p->index].data, p->index);
		
		last = p->index++;
		eval_expr6(p, &part);
		
		if(p->skip_expr)
			return;
		
		switch(p->tokens[last].type) {
			case GREATER:
				*value = *value > part;
				break;
			case LESS:
				*value = *value < part;
				break;
			case GRT_EQL:
				*value = *value >= part;
				break;
			case LESS_EQL:
				*value = *value <= part;
				break;
		};
	}
}

// вычисление операций сравнения != ==
static void eval_expr4(parser_t *p, float *value)
{
	eval_expr5(p, value);
	int last = 0;
	
	if(p->tokens[p->index].type == EQL_EQL || p->tokens[p->index].type == NOT_EQL) {
		
		float part = 0.0f;
		
		DMSG("found equal-operator: %s (i = %i)\n", p->tokens[p->index].data, p->index);
		
		last = p->index++;
		eval_expr5(p, &part);
		
		if(p->skip_expr)
			return;
		
		switch(p->tokens[last].type) {
			case EQL_EQL:
				*value = *value == part;
				break;
			case NOT_EQL:
				*value = *value != part;
				break;
		};
	}
}

// вычисление цикла k..n: expr
static void eval_expr3(parser_t *p, float *value)
{
	float begin = 0.0f, end = 0.0f;
	
	eval_expr4(p, value);
	
	if(p->tokens[p->index].type == DOT_DOT) {
		DMSG("found loop: %s (i = %i)\n", p->tokens[p->index].data, p->index);
		
		begin = *value;
		
		p->index++;
		eval_expr2(p, &end);
		
		if(p->tokens[p->index].type == COLON) {
			float part = 0.0f, sum = 0.0f;
			int last = 0;
			
			//DMSG("begin = %f end = %f\n", begin, end);
			
			last = ++p->index;
			
			// в зависимости от соотношения начала и конца цикла 
			//  запускаем цикл либо +1, либо -1
			if(begin <= end) {
				for(unsigned b = (unsigned) begin; b <= (unsigned) end; b++) {
					p->index = last;
					
					// присваиваем i номер текущей итерации
					assign_variable(p, find_var(p, "i"), (float) b);

					eval_expr(p, &part);
					
					
					// вычисляем сумму
					sum += part;
				}
			} else {
				for(unsigned b = (unsigned) begin; b >= (unsigned) end; b--) {
					p->index = last;
					assign_variable(p, find_var(p, "i"), (float) b);

					eval_expr(p, &part);

					sum += part;
				}
			}
			
			// обнуляем переменную i
			assign_variable(p, find_var(p, "i"), 0.0f);
			
			*value = sum;
			//index++;
		} else {
			set_error(p, "expected :", 4);
		}
	}
}

// вычисление тернарного : ?
static void eval_expr2(parser_t *p, float *value)
{
	eval_expr3(p, value);
	
	//if(p->skip_expr)
	//	return;

	if(p->tokens[p->index].type == QUEST_MARK) {
		DMSG("found ternary: %s (i = %i)\n", p->tokens[p->index].data, p->index);
		if(*value != 0.0f) {
			p->index++;
			eval_expr(p, value);

			p->index++;
			
			// парсим выражение после : но не вычисляем его
			float s = 0.0f;
			p->skip_expr = 1;
			eval_expr(p, &s);
			p->skip_expr = 0;
			
		} else {
			
			int flag_end = 0;
			// ищем :
			while(p->tokens[p->index].type != COLON) {
				if(p->tokens[p->index].type == SEMICOLON || p->index > (p->num_tokens - 1)) {
					flag_end = 1;
					break;
				}
				p->index++;
			}

			if(!flag_end) {
				// если нашли :, то вычисляем выражение после него
				p->index++;
				DMSG("found \":\" (i = %i)\n", p->index);
				eval_expr(p, value);
			} else {
				set_error(p, "expected :", 4);
			}
		}
	}
}

// высислений выражений в присваивании (и само присваивание)
static void eval_expr1(parser_t *p, float *value)
{
	
	if(p->tokens[p->index].type == IDENTIFIER) {
		
		int var_id = 0;
		
		// это переменная?
		if((var_id = find_var(p, p->tokens[p->index].data)) == -1) {
			if(p->tokens[p->index+1].type == PARENTH_LEFT) {
				// скорее всего это функция (т.к. есть открывающая скобка)
				eval_expr2(p, value);
				return;
			}
			
			if(p->tokens[p->index+1].type == EQUAL) {
				// если это неизвестная переменная и стоит знак присваивания, то создаем её
				var_id = create_user_var(p, p->tokens[p->index].data);
			} else {
				set_error(p, "unknown variable", 6);
			}
		}
		
		if(var_id <= 0)
			return;

		DMSG("found var: %s\n", p->tokens[p->index].data);
		
		// переходим к след. токену
		p->index++;
		
		// это присваивание?
		if(p->tokens[p->index].type == EQUAL || p->tokens[p->index].type == PLUS_EQL ||
		   p->tokens[p->index].type == MINUS_EQL || p->tokens[p->index].type == MULT_EQL ||
		   p->tokens[p->index].type == DIV_EQL) {
			
			int last = 0;
			DMSG("found assigment: %s\n", p->tokens[p->index].data);

			last = p->index++;
			
			// вычисляем значение переменной после =
			eval_expr(p, value);
			
			if(p->skip_expr)
				return;
			
			// получаем значение переменной к котрой будет присваиваться значение
			// (для случаем += -= *= /=)
			float var_value = get_var_value(p, var_id);
			
			switch(p->tokens[last].type) {
				case EQUAL:
					var_value =  *value;
					break;
				case PLUS_EQL:
					var_value +=  *value;
					break;
				case MINUS_EQL:
					var_value -=  *value;
					break;
				case MULT_EQL:
					var_value *=  *value;
					break;
				case DIV_EQL:
					var_value /=  *value;
					break;
			}
			
			// присваиваем значение
			assign_variable(p, var_id, var_value);
			*value = var_value;

			return;
		} else {
			p->index--;
		}
	}
	
	eval_expr2(p, value);
}

// вычисление ,
static void eval_expr0(parser_t *p, float *value)
{
	eval_expr1(p, value);
	
	if(p->tokens[p->index].type == COMMA) {
		DMSG("found comma: %s\n", p->tokens[p->index].data);
		
		p->index++;
		
		// вычисляем выражение после запятой
		eval_expr(p, value);
	}
}

// проверка на последний индекс и точку-с-запятой
static void eval_expr(parser_t *p, float *value)
{
	//DMSG("eval0: %s, i = %i\n", p->tokens[p->current_index].data, p->current_index);
	
	// есть ли ещё токены...
	if(p->index > (p->num_tokens - 1)) {
		set_error(p, "no expression", 0);
		p->error = -1;
		return;
	}
	
	// если текущий токен ; то выходим
	if(p->tokens[p->index].type == SEMICOLON) {
		*value = 0;
		return;
	}
	
	// переходим к первому этапу парсинга
	eval_expr0(p, value);
	//p->current_index--;
}

// синтаксический анализ
void syntax_parser(parser_t *p)
{
	do {
		float value = 0.0f;
		if(p->tokens[p->index].type == IDENTIFIER) {
			// если первый токен идентификатор, то парсим всё сначало
			eval_expr(p, &value);
			if(p->tokens[p->index].type != SEMICOLON) {
				if(!p->error)
					set_error(p, "expected ;", 1);
				break;
			}
		} else {
			// если первый токен неидентификатор, то парсим исключая присваивание
			eval_expr2(p, &value);
			if(p->tokens[p->index].type != SEMICOLON) {
				if(!p->error)
					set_error(p, "expected ;", 1);
				break;
			}
		}
		p->index++;
	} while(p->index < (p->num_tokens - 1));
}

static void destroy_vars(parser_t *p)
{
	IF_FAILED(p && p->init == 1);
	
	// удалаем все созданные переменные
	for(int i = 0; i < p->num_user_ids; i++) {
		p->user_identifiers[i].type = 0;

		if(p->user_identifiers[i].name)
			free(p->user_identifiers[i].name);
		
		p->user_identifiers[i].name = NULL;

		p->user_identifiers[i].global_id = 0;

		p->float_user_vars[i].global_id = 0;
		p->float_user_vars[i].value = 0.0f;
	}
	
	p->num_user_ids = 0;
	p->num_user_vars = 0;
}

int parser_create(parser_t *parser)
{
	IF_FAILED0(parser);
	
	parser->float_builtin_vars = NULL;
	
	parser->num_user_ids = 0;
	parser->num_user_vars = 0;
	parser->last_var_global_id = last_var_global_id;
	
	parser->num_tokens = 0;
	parser->index = 0;
	parser->error = 0;
	parser->skip_expr = 0;
	
	parser->init = 0;
	
	// устанавливаем локаль, чтобы atof преобразовывало числа с точкой (иначе может только с запятой)
	setlocale(LC_NUMERIC, "C");
	
	return 1;
}

int parser_parse_text(parser_t *parser, const char *text, float_var_value_t *var_table)
{
	IF_FAILED0(parser && text && var_table);
	
#if 0
	char *text1 = "d = y; a = 0;";
	float_var_value_t test_vars[] =
		{
			{1, 0.0f}, // d
			{2, 0.0f}, // x
			{3, 0.0f}, // y
			{4, 0.0f}, // z
			{5, 0.0f}, // t
			{0, 0.0f}
		};
#endif
	
	if(!parser->init) {
		
		// если парсер неинициализированн, то устанавливаем основные параметры
		
		parser->index = 0;
		parser->error = 0;
		parser->num_tokens = 0;
		
		if(!lexer_parser(parser, text))
			return -20;
		
		// выделяем память под доп. переменные и устанавливаем значение переменной "i" в 0.0f
		parser->float_extra_vars = (float_var_value_t*) malloc(sizeof(float_var_value_t) * 2);
		parser->float_extra_vars[0].global_id = 6; parser->float_extra_vars[0].value = 0.0f;
		parser->float_extra_vars[0].global_id = 0; parser->float_extra_vars[0].value = 0.0f;
		
		parser->init = 1;
	}
	
	parser->float_builtin_vars = var_table;
	//parser->float_builtin_vars = test_vars;
	
	// уничтожаем (если есть) созданные пользовательские переменные
	destroy_vars(parser);
	
	// парсим строку
	syntax_parser(parser);
	
	parser->index = 0;
	
	if(parser->error > 0) {
		int ret = parser->error;
		parser->error = 0;
		
		return ret;
	}
	
	return 0;
}

void parser_clean(parser_t *parser) 
{
	IF_FAILED(parser);
	
	// проводим очистку только в инициализированном парсере
	if(parser->init == 1) {
		
		// очищаем данные токенов
		for(int i = 0; i < parser->num_tokens; i++) {
			if(parser->tokens[i].data)
				free(parser->tokens[i].data);
			parser->tokens[i].data = NULL;
			
			parser->tokens[i].type = 0;
		}
		
		// очищаем дополнительные встроенные переменные
		if(parser->float_extra_vars)
			free(parser->float_extra_vars);
		
		parser->float_extra_vars = NULL;
		
		
		// сбрасываем текущий индекс, ошибку и кол-во токенов
		parser->index = 0;
		parser->error = 0;
		parser->num_tokens = 0;
		
		// сбрасываем инициализацию
		parser->init = 0;
	}
}

#define skip_ws(p) while(*p == ' ') p++;
#define copy_text(text_ptr, to_ptr, n) for(int i = 0; i < n; i++) to_ptr[i] = text_ptr[i]; to_ptr[n] = '\0';

// лексический анализ
int lexer_parser(parser_t *p, const char *text)
{
	IF_FAILED0(text);
	
	char *ptr = (char*) text;
	
	while(*ptr != '\0') {
		
		// пропускаем пробелы
		skip_ws(ptr);
		
		// пропускаем переход на след. строку
		if(*ptr == '\n') {
			ptr++;
			continue;
		}
		
		if( isalpha(*ptr) ) {
			// найден идентификатор
			
			char *begin = ptr;
			
			// считываем все последующие символы и цифры
			while( (isalpha(*ptr) || isdigit(*ptr)) && ( (ptr - begin) < 63 ) ) ptr++;
			
			p->tokens[p->num_tokens].type = IDENTIFIER;
			p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * (ptr - begin ));
			copy_text(begin, p->tokens[p->num_tokens].data, (ptr - begin));
			
			//printf("found identifier: %s\n", p->tokens[p->number_tokens].data);
			
			p->num_tokens++;
		} else if( isdigit(*ptr) ) {
			// это число
			
			char *begin = ptr;
			
			while(isdigit(*ptr)) ptr++;
			
			// если есть точка, значит это вещественое число
			if( *(ptr) == '.' && isdigit(*(ptr+1)) ) {
				ptr++;
				while(isdigit( *ptr )) ptr++;
				p->tokens[p->num_tokens].type = FLOAT_NUMBER;
			} else {
				p->tokens[p->num_tokens].type = INT_NUMBER;
			}
			
			p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * (ptr - begin ));
			copy_text(begin, p->tokens[p->num_tokens].data, (ptr - begin));
			
			//printf("found number: %s\n", tokens[parser->number_tokens].data);
			
			p->num_tokens++;
		} else if( isprint(*ptr) ) {
			// нйаден спец. символ
			
			switch(*ptr) {
				case '=':
					if(*(ptr+1) == '=') {
						p->tokens[p->num_tokens].type = EQL_EQL;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 2);
						copy_text(ptr, p->tokens[p->num_tokens].data, 2);
						ptr++;
					} else {
						p->tokens[p->num_tokens].type = EQUAL;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
						copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					}
					break;
				case '+':
					if(*(ptr+1) == '=') {
						p->tokens[p->num_tokens].type = PLUS_EQL;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 2);
						copy_text(ptr, p->tokens[p->num_tokens].data, 2);
						ptr++;
					} else {
						p->tokens[p->num_tokens].type = PLUS;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
						copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					}
					break;
				case '-':
					if(*(ptr+1) == '=') {
						p->tokens[p->num_tokens].type = MINUS_EQL;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 2);
						copy_text(ptr, p->tokens[p->num_tokens].data, 2);
						ptr++;
					} else {
						p->tokens[p->num_tokens].type = MINUS;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
						copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					}
					break;
				case '*':
					if(*(ptr+1) == '=') {
						p->tokens[p->num_tokens].type = MULT_EQL;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 2);
						copy_text(ptr, p->tokens[p->num_tokens].data, 2);
						ptr++;
					} else if(*(ptr+1) == '*') {
						p->tokens[p->num_tokens].type = MULT_MULT;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 2);
						copy_text(ptr, p->tokens[p->num_tokens].data, 2);
						ptr++;
					} else {
						p->tokens[p->num_tokens].type = MULT;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
						copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					}
					break;
				case '/':
					if(*(ptr+1) == '=') {
						p->tokens[p->num_tokens].type = DIV_EQL;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 2);
						copy_text(ptr, p->tokens[p->num_tokens].data, 2);
						ptr++;
					} else {
						p->tokens[p->num_tokens].type = DIV;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
						copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					}
					break;
				case ';':
					p->tokens[p->num_tokens].type = SEMICOLON;
					p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
					copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					break;
				case ':':
					p->tokens[p->num_tokens].type = COLON;
					p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
					copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					break;
				case '?':
					p->tokens[p->num_tokens].type = QUEST_MARK;
					p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
					copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					break;
				case '(':
					p->tokens[p->num_tokens].type = PARENTH_LEFT;
					p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
					copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					break;
				case ')':
					p->tokens[p->num_tokens].type = PARENTH_RIGHT;
					p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
					copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					break;
				case '.':
					if(*(ptr+1) == '.') {
						p->tokens[p->num_tokens].type = DOT_DOT;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 2);
						copy_text(ptr, p->tokens[p->num_tokens].data, 2);
						ptr++;
					} else {
						p->tokens[p->num_tokens].type = DOT;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
						copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					}
					break;
				case ',':
					p->tokens[p->num_tokens].type = COMMA;
					p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
					copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					break;
				case '>':
					if(*(ptr+1) == '=') {
						p->tokens[p->num_tokens].type = GRT_EQL;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 2);
						copy_text(ptr, p->tokens[p->num_tokens].data, 2);
					} else {
						p->tokens[p->num_tokens].type = GREATER;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
						copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					}
					break;
				case '<':
					if(*(ptr+1) == '=') {
						p->tokens[p->num_tokens].type = LESS_EQL;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 2);
						copy_text(ptr, p->tokens[p->num_tokens].data, 2);
					} else {
						p->tokens[p->num_tokens].type = LESS;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
						copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					}
					break;
				case '!':
					if(*(ptr+1) == '!') {
						p->tokens[p->num_tokens].type = NOT_EQL;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 2);
						copy_text(ptr, p->tokens[p->num_tokens].data, 2);
					} else {
						p->tokens[p->num_tokens].type = NOT;
						p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
						copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					}
					break;
				default:
					p->tokens[p->num_tokens].type = UNKNOWN;
					p->tokens[p->num_tokens].data = (char*) malloc(sizeof(char) * 1);
					copy_text(ptr, p->tokens[p->num_tokens].data, 1);
					break;		
			};
			
			//printf("found (%i): %s\n", p->tokens[p->number_tokens].type, p->tokens[p->number_tokens].data);
			
			p->num_tokens++;
			
			// переходим к след. символу
			ptr++;
		} else {
			
			// неверный символ, выходим с ошибкой
			return 0;
		}
		
		
	}
	
	return 1;
}
