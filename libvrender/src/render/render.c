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

#include "render.h"
#include "main_shader.h"
#include "shader_program_file.h"
#include "camera.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "math/noise.h"
#include "texture.h"
#include "input.h"
#include "marching_cubes.h"
#include "parser.h"
#include "thread.h"

static int init = 0;

static unsigned num_threads = 1;

// основная камера
static camera_t camera;

static double last_time = 0.0f;

static parser_t parser;
static float *volume = NULL;
static char *str_function = NULL;

// размер скалярного поля и размер сетки
static vector3ui volume_size, grid_size;
// шаг обработки сетки и скалярного поля
static vector3f grid_step, volume_step;

// текстура с данными скалярного поля
static texture_t volume_texture;

// ширина и высота окна
static unsigned int window_width = 640, window_height = 480;

// цвета и коэффициенты
static float coef_ambient = 0.05f, coef_diffuse = 1.0f,
			 coef_specular = 1.0f, coef_gamma = 2.2f,
			 material_shininess = 30.0f;
static vector3f material_color, light_color, light_spec_color;

// файл шейдера
shader_program_file_t pfile;
shader_program_t program;

// юниформы для шейдера
GLint uniform_modelmat, uniform_viewmat, uniform_projectionmat, 
	  uniform_model_inv, uniform_light_pos, uniform_viewer_pos,
	  uniform_volume_texture, uniform_volume_step, uniform_material_color,
	  uniform_light_color, uniform_light_spec_color, uniform_material_shininess,
	  uniform_coef_ambient, uniform_coef_diffuse, uniform_coef_specular,
	  uniform_coef_gamma;

// буферы с данными
GLuint vbo[4], vao;

// позиции источника света
static vector3f light_position, new_light_position;

// параметры камеры
float camera_step = 0.2f, camera_move_speed = 15.0f, 
	  camera_rotate_speed = 15.0f, camera_fov = 60.0f;

static matrix4f rotmat, modelmat, viewmat, projectionmat;
static int disable_mouse = 0;
static float rot_angle = 0.0f, light_rot_angle = 0.0f, light_rot_step = 0.01f;
static vector3f rot_axis;
static int light_animate = 0, isolevel_animate = 0;
static float isolevel = 30.0f, isolevel_begin = 0.0f, isolevel_end = 30.0f, isolevel_step = 0.01f;

static void update_object_orientation(void);
static void update_camera(double t);
static int init_shader(void);
static int init_buffers(void);

int init_shader(void)
{
	// открываем файл шейдера
	//if(!shader_program_file_create(&pfile, "shader.sf"))
	//	return 0;
	if(!shader_program_file_create_from_buffer(&pfile, main_shader_source))
		return 0;
	
	// получаем шейдерную программу
	shader_program_file_get_program(&pfile, &program);
	
	// линкуем её
	if(!shader_program_link(&program))
		return 0;
	
	shader_program_bind(&program);
	
	// получаем юниформы
	uniform_modelmat = shader_program_get_uniform_loc(&program, "model");
	uniform_viewmat = shader_program_get_uniform_loc(&program, "view");
	uniform_projectionmat = shader_program_get_uniform_loc(&program, "projection");
	uniform_model_inv = shader_program_get_uniform_loc(&program, "model_inv");
	uniform_light_pos = shader_program_get_uniform_loc(&program, "light_position");
	uniform_viewer_pos = shader_program_get_uniform_loc(&program, "viewer_position");
	uniform_volume_texture = shader_program_get_uniform_loc(&program, "volume_texture");
	uniform_volume_step = shader_program_get_uniform_loc(&program, "volume_step");

	uniform_material_color = shader_program_get_uniform_loc(&program, "material_color");
	uniform_light_color = shader_program_get_uniform_loc(&program, "light_color");
	uniform_light_spec_color = shader_program_get_uniform_loc(&program, "light_spec_color");
	uniform_material_shininess = shader_program_get_uniform_loc(&program, "material_shininess");
	uniform_coef_ambient = shader_program_get_uniform_loc(&program, "coef_ambient");
	uniform_coef_diffuse = shader_program_get_uniform_loc(&program, "coef_diffuse");
	uniform_coef_specular = shader_program_get_uniform_loc(&program, "coef_specular");
	uniform_coef_gamma = shader_program_get_uniform_loc(&program, "coef_gamma");
	
	shader_program_unbind(&program);
	
	return 1;
}

int init_buffers(void)
{
	GLint attr_position;
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(4, vbo);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	
	shader_program_bind(&program);
	
	// получаем аттрибут для вершин
	attr_position = shader_program_get_attrib_loc(&program, "position");
	if(attr_position != -1) {
		glVertexAttribPointer(attr_position, 3, GL_FLOAT, GL_FALSE, sizeof(vector3f), (const GLvoid*) 0);
		glEnableVertexAttribArray(attr_position);
	}
	
	glUniform3f(uniform_volume_step, volume_step.x, volume_step.y, volume_step.z);
	
	glActiveTexture(GL_TEXTURE0);
	texture_bind(&volume_texture);
	glUniform1i(uniform_volume_texture, 0);
	
	shader_program_unbind(&program);
	
	return 1;
}

void render_update_volume_tex(void)
{		
	static int init = 0;
	
	if(init) {
		// если текстура уже инициализирована, то сначало удаляем её
		texture_release(&volume_texture);
		init = 0;
	}
	
	// создаем текстуру с данными скалярного поля.
	// используем только red компоненту
	texture_create3d_from_data(&volume_texture, GL_R32F, GL_RED, 0, 
							   volume_size.x, volume_size.y, volume_size.z, 
							   GL_FLOAT, (void*) volume);
	
	glUniform3f(uniform_volume_step, volume_step.x, volume_step.y, volume_step.z);
	
	init = 1;
}

static void update_volume_vbos(void)
{
	// полигонизируем скалярное поле
	if(!marching_cubes_create_vbos(volume, 
								   volume_size, 
								   grid_size, 
								   isolevel, vbo[0], vbo[1])) {
		
		ERROR_MSG("Marching Cubes: nothing to generate");
	}
}

int render_init(void)
{
	int gl_major = 0, gl_minor = 0;
	
	//IF_FAILED0(!init);
	
	//parser_t parser;
	//parser_create(&parser);
	//parser_parse_text(&parser, "", NULL);
	//parser_clean(&parser);
	//exit(1);
	
#ifdef __WIN32
	gl_funcs_init();
#endif
	
	TRACE_MSG("init base render system\n");
	
	TRACE_MSG("init 3D noise...\n");
	vnoise3d_init_file("noise3d_128.n", vec3ui(128, 128, 128));
	TRACE_MSG("noise initiliazed\n");
	
	glViewport(0, 0, window_width, window_height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0f);
	
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_CLAMP);
	
	glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
	glGetIntegerv(GL_MINOR_VERSION, &gl_minor);
	DEBUG_MSG("OpenGL version: %i.%i\n", gl_major, gl_minor);
	DEBUG_MSG("GLSL version: %s\n", (char*) glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	if( (gl_major < 3) || ( gl_major == 3 && gl_minor < 0 ) ) {
		ERROR_MSG("the system must support OpenGL 3.0+\n");
		return 0;
	}
	
	// настраиваем основную камеру
	TRACE_MSG("init main camera\n");
	camera_init(&camera, vec3f(0.0f, 0.0f, 0.0f), vec3f(0.0f, 1.0f, 0.0f), vec3f(0.0f, 0.0f, -1.0f));
	camera_set_limit(&camera, -95.0f, 95.0f, -360.0f, 360.0f);
	camera_set_fov(&camera, camera_fov);
	camera_set_move_velocity(&camera, camera_move_speed);
	camera_set_rotate_velocity(&camera, camera_rotate_speed);
	camera_set_aspect(&camera, (float) window_width / (float) window_height);
	camera_set_zplanes(&camera, 0.001f, 1000.0f);
	camera_update(&camera, 0.0);
	
	isolevel = 30.0f; 
	isolevel_animate = 0;
	
	// устанавливаем функцию по-умолчанию
	parser_create(&parser);
	str_function = "d = y;";
	
	// настраиваем и создаем скалярное поле
	render_set_volume_parameters(vec3ui(128, 128, 128), vec3ui(32, 32, 32));
	render_update_volume_tex();
	
	CHECK_GL_ERRORS();
	
	// инициализируем шейдер
	if(!init_shader())
		return 0;
	
	// инициализируем буферы с данными
	init_buffers();
	
	// устанавливаем параметры по-умолчанию
	new_light_position = light_position = vec3f(1.0f, 1.0f, 0.0f);
	rot_axis = vec3f(0.0f, 1.0f, 0.0f);
	rot_angle = 0.0f;
	light_animate = 0;
	light_rot_angle = 0.0f;
	mat4(rotmat, 1.0f);
	mat4(modelmat, 1.0f);
	material_color = vec3f(0.5f, 0.5f, 0.5f);
	light_color = vec3f(1.0f, 1.0f, 1.0f);
	light_spec_color = vec3f(1.0f, 1.0f, 1.0f);
	
	init = 1;
	
	// полигонизируем ск. п.
	update_volume_vbos();
	
	// обновляем рендер
	render_update(0.0f);
	
	return 1;
}

typedef struct {
	parser_t parser;
	vector3ui begin, end;
	vector3ui volume_size;
} volume_gen_t;

static void generate_volume(void *arg)
{
	volume_gen_t *gen = (volume_gen_t*) arg;
	
	float_var_value_t float_vars[] = 
		{
			{1, 0.0f}, // d
			{2, 0.0f}, // x
			{3, 0.0f}, // y
			{4, 0.0f}, // z
			{0, 0.0f}
		};
	
	for(unsigned k = gen->begin.z; k < gen->end.z; k++) {
		for(unsigned j = gen->begin.y; j < gen->end.y; j++) {
			for(unsigned i = gen->begin.x; i < gen->end.x; i++) {
				
				float_vars[0].value = 0.0f; float_vars[1].value = i; 
				float_vars[2].value = j; float_vars[3].value = k;
				
				if(parser_parse_text(&gen->parser, str_function, float_vars) != 0)
					return;
				
				volume[i + j*gen->volume_size.x + k*gen->volume_size.x*gen->volume_size.y] = float_vars[0].value;
				
			}
		}
	}
}

void render_set_number_of_threads(unsigned num)
{
	num_threads = num;
}

static float volume_func(vector3f pos) 
{
	// инициализируем таблицу переменных с соотвествующей позицией
	float_var_value_t float_vars[] = 
		{
			{1, 0.0f}, // d
			{2, pos.x}, // x
			{3, pos.y}, // y
			{4, pos.z}, // z
			{0, 0.0f}
		};

	// парсим функцию
	if(parser_parse_text(&parser, str_function, float_vars) != 0) {
		return 0.0f;
	}
	
	// получаем вычисленное значение d
	return float_vars[0].value; // d
}

void render_set_volume_parameters(vector3ui volume_size_v, vector3ui grid_size_v)
{
	volume_size = volume_size_v;
	volume_step = vec3f_div(vec3f(1.0f, 1.0f, 1.0f), vec3ui_to_vec3f(volume_size));
	grid_size = grid_size_v;
	grid_step = vec3f_div(vec3f(1.0f, 1.0f, 1.0f), vec3ui_to_vec3f(grid_size));
	
	if(volume) {
		free(volume);
		volume = NULL;
	}
	
	volume = (float*) malloc(sizeof(float) * volume_size.x*volume_size.y*volume_size.z);

	if(num_threads >= 2) {
		thread_t *threads = (thread_t*) malloc(sizeof(thread_t) * num_threads);
		volume_gen_t *func_args = (volume_gen_t*) malloc(sizeof(volume_gen_t) * num_threads);
		
		float_var_value_t float_vars[] = 
			{
				{1, 0.0f}, // d
				{2, 0.0f}, // x
				{3, 0.0f}, // y
				{4, 0.0f}, // z
				{0, 0.0f}
			};
		
		for(int i = 0; i < num_threads; i++) {
			
			parser_create(&func_args[i].parser);
			parser_clean(&func_args[i].parser);
			
			if(parser_parse_text(&func_args[i].parser, str_function, float_vars) != 0)
				break;
			
			func_args[i].begin = vec3ui(0, 0, volume_size.z * ((float) (i) / (float) num_threads));
			func_args[i].end = vec3ui(volume_size.x, volume_size.y, volume_size.z * ((float) (i+1) / (float) num_threads));
			func_args[i].volume_size = volume_size;
			
			thread_create(&threads[i], generate_volume, (void*) &func_args[i]);
		}
		
		for(int i = 0; i < num_threads; i++) {
			thread_join(&threads[i]);
			parser_clean(&func_args[i].parser);
		}

		free(threads);
		free(func_args);
	} else {
		// проходимся по всему массиву и устанавливаем соотвествующее функции значение
		for(unsigned k = 0; k < volume_size.z; k++) {
			for(unsigned j = 0; j < volume_size.y; j++) {
				for(unsigned i = 0; i < volume_size.x; i++) {
					volume[i + j*volume_size.x + k*volume_size.x*volume_size.y] = volume_func(vec3f(i, j, k));
				}
			}
		}
	}
	
	update_volume_vbos();
}

void render_update(double last_frame_time)
{
	static int isolevel_offset = 1;
	matrix4f model_inv_mat4;
	matrix3f model_inv_mat3;
	vector3f camera_pos;
	
	IF_FAILED(init);
	
	mat4(modelmat, 1.0f);
	
	last_time = last_frame_time;
	
	// обновляем камеру (позицию и т.п)
	update_camera(last_frame_time);
	
	mat4_mult2(modelmat, modelmat, rotmat);
	
	// обновляем вращение объекта (arcball)
	update_object_orientation();
	
	// смещаем объект
	matrix4f translatemat;
	mat4_translate(translatemat, -0.5f, -0.5f, -1.0f);
	
	mat4_mult2(modelmat, modelmat, translatemat);
	
	// вращаем источник света
	if(light_animate) {
		light_rot_angle += light_rot_step;
		if(light_rot_angle > 360.0f) light_rot_angle = 0.0f;
	}
	
	matrix4f light_mat;
	mat4_rotate_axis(light_mat, light_rot_angle, vec3f(0.0f, 1.0f, 0.0f));
	new_light_position = mat4_mult_vec3(light_mat, light_position);
	
	
	// изменяем изо-уровень
	if(isolevel_animate) {
		
		isolevel += isolevel_step * isolevel_offset;
		
		if(isolevel_begin <= isolevel_end) {
			if(isolevel >= isolevel_end)
				isolevel_offset = -1;
			else if(isolevel < isolevel_begin)
				isolevel_offset = 1;
		} else {
			if(isolevel < isolevel_end)
				isolevel_offset = 1;
			else if(isolevel >= isolevel_begin)
				isolevel_offset = -1;
		}
		
	}
	
	// поулчаем параметры камеры
	camera_pos = camera_get_position(&camera);
	camera_get_view_matrix(&camera, viewmat);
	camera_get_projection_matrix(&camera, projectionmat);
	
	mat4_inverse(model_inv_mat4, modelmat);
	mat4_submat(model_inv_mat3, 3, 3, model_inv_mat4);
	
	// обновляем вершинные буферы
	if(isolevel_animate)
		update_volume_vbos();
	
	shader_program_bind(&program);
	
	// устанавливаем юниформы
	glUniformMatrix4fv(uniform_modelmat, 1, GL_TRUE, (const GLfloat *) modelmat);
	glUniformMatrix4fv(uniform_viewmat, 1, GL_TRUE, (const GLfloat *) viewmat);
	glUniformMatrix4fv(uniform_projectionmat, 1, GL_TRUE, (const GLfloat *) projectionmat);
	glUniformMatrix3fv(uniform_model_inv, 1, GL_TRUE, (const GLfloat *) model_inv_mat3);
	glUniform3f(uniform_light_pos, new_light_position.x, new_light_position.y, new_light_position.z);
	glUniform3f(uniform_viewer_pos, camera_pos.x, camera_pos.y, camera_pos.z);

	glUniform1f(uniform_coef_ambient, coef_ambient);
	glUniform1f(uniform_coef_diffuse, coef_diffuse);
	glUniform1f(uniform_coef_specular, coef_specular);
	glUniform1f(uniform_material_shininess, material_shininess);
	glUniform1f(uniform_coef_gamma, coef_gamma);
	glUniform3f(uniform_material_color, material_color.x, material_color.y, material_color.z);
	glUniform3f(uniform_light_color, light_color.x, light_color.y, light_color.z);
	glUniform3f(uniform_light_spec_color, light_spec_color.x, light_spec_color.y, light_spec_color.z);
	
	shader_program_unbind(&program);

}

static vector3f compute_sphere_vector(unsigned int mouse_x, unsigned int mouse_y)
{
	float x = 0.0f, y = 0.0f, vec_length = 0.0f;
	vector3f result;
	
	// приводим координаты мыши к интервалу [-1,1]
	x = ((float) mouse_x / (float) window_width) * 2.0f - 1.0f;
	y = ((float) mouse_y / (float) window_height) * 2.0f - 1.0f;
	
	result = vec3f(x, -y, 0.0f);
	
	// v_len = x*x + y*y
	vec_length = x*x + y*y;
	
	// R*R = Z*Z + (X*X + Y*Y)  <=>  Z*Z = R*R - (X*X + Y*Y) => (R = 1, (X*X + Y*Y) = v_len) => Z = sqrt(1 - v_len)
	if(vec_length > 1.0f) {
		result = vec3f_norm(result);
	} else {
		result.z = math_sqrtf(1.0f - vec_length);
	}
	
	return result;
}

void update_object_orientation(void)
{
	static int mx = 0, my = 0, last_mouse_x = 0, last_mouse_y = 0;
	static int arcball = 0;
	
	IF_FAILED(init);
	
	if(!disable_mouse) {
		
		if(!arcball && input_get_mousebtn_state(MOUSE_LEFTBTN)) {
			arcball = 1;
			
			input_get_mouse_position(&mx, &my);
			
			last_mouse_x = mx;
			last_mouse_y = my;
		} else if(arcball && !input_get_mousebtn_state(MOUSE_LEFTBTN)) {
			arcball = 0;
		}
		
		if(arcball) {
			input_get_mouse_position(&mx, &my);
			
			if(mx < 0)
				mx = 0;
			if(my < 0)
				my = 0;
			
			if(last_mouse_x != mx || last_mouse_y != my) {
				vector3f v1 = compute_sphere_vector(last_mouse_x, last_mouse_y);
				vector3f v2 = compute_sphere_vector(mx, my);
				rot_angle = RAD_TO_DEG(math_acosf(math_min(1.0f, vec3f_dot(v1, v2))));
				
				matrix3f modelview3, modelview_inv3;
				matrix4f modelview4;
				mat4_mult2(modelview4, viewmat, modelmat);
				mat4_submat(modelview3, 3, 3, modelview4);
				mat3_inverse(modelview_inv3, modelview3);
				rot_axis = mat3_mult_vec3(modelview_inv3, vec3f_cross(v1, v2));
				
				mat4_rotate_axis_mult(rotmat, rot_angle, rot_axis);
				
				last_mouse_x = mx;
				last_mouse_y = my;
			}
		}
	}
}

void update_camera(double t)
{	
	IF_FAILED(init);
	
	//// обновляем позицию камеры
	if(input_get_kbkey_state(KB_space))
		camera_move_up(&camera, camera_step);
	if(input_get_kbkey_state(KB_shift))
		camera_move_down(&camera, camera_step);
	if(input_get_kbkey_state(KB_W) || input_get_kbkey_state(KB_w))
		camera_move_forward(&camera, camera_step);
	if(input_get_kbkey_state(KB_S) || input_get_kbkey_state(KB_s))
		camera_move_backward(&camera, camera_step);
	if(input_get_kbkey_state(KB_A) || input_get_kbkey_state(KB_a))
		camera_move_left(&camera, camera_step);
	if(input_get_kbkey_state(KB_D) || input_get_kbkey_state(KB_d))
		camera_move_right(&camera, camera_step);
	
	camera_update(&camera, t);
}

void render_draw(void)
{	
	GLint size = 0;
	
	IF_FAILED(init);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glActiveTexture(GL_TEXTURE0);
	//texture_bind(&volume_texture);

	shader_program_bind(&program);
	
	glBindVertexArray(vao);
	
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size / sizeof(unsigned int), GL_UNSIGNED_INT, NULL);
	
	shader_program_unbind(&program);
 	
 	CHECK_GL_ERRORS();
}

void render_change_window(unsigned int width, unsigned int height)
{
	IF_FAILED(init);
	
	TRACE_MSG("window change; width = %i height = %i\n", width, height);	
	
	window_width = width;
	window_height = height;
	
	glViewport(0, 0, window_width, window_height);
	camera_set_aspect(&camera, (float) window_width / (float) window_height);
}

void render_destroy(void)
{
	IF_FAILED(init);
	
	TRACE_MSG("release render resources\n");
	
	shader_program_file_destroy(&pfile);
	
	glDeleteBuffers(4, vbo);
	glDeleteVertexArrays(1, &vao);
	
	parser_clean(&parser);
	
	if(volume) {
		free(volume);
		volume = NULL;
	}
}

int render_set_function_text(const char *function_text)
{	
	IF_FAILED_RET(function_text, -100);
	
	float_var_value_t float_vars[] = 
		{
			{1, 0.0f}, // d
			{2, 0.0f}, // x
			{3, 0.0f}, // y
			{4, 0.0f}, // z
			{0, 0.0f}
		};
	
	int error = 0;
	
	parser_clean(&parser);
	
	// выполняем пре-парсинг функции, чтобы сразу определить синтаксические ошибки
	if((error = parser_parse_text(&parser, function_text, float_vars)) != 0) {
		ERROR_MSG("error %i in function statement\n", error);
		return error;
	}
	
	str_function = (char*) function_text;
	
	return 0;
}

////

void render_set_isolevel(float level)
{
	isolevel = level;
	update_volume_vbos();
}

void render_set_isolevel_animation(int animate)
{
	isolevel_animate = (animate ? 1 : 0);
}

void render_set_isolevel_begin_anim(float level)
{
	isolevel_begin = level;
}

void render_set_isolevel_end_anim(float level)
{
	isolevel_end = level;
}

void render_set_isolevel_step_anim(float step)
{
	isolevel_step = step;
}

void render_set_material_color(vector3f color)
{
	material_color = color;
}

void render_set_light_color(vector3f color)
{
	light_color = color;
}

void render_set_light_spec_color(vector3f color)
{
	light_spec_color = color;
}

void render_set_material_shininess(float shininess)
{
	material_shininess = shininess;
}

void render_set_ambient_factor(float ambient)
{
	coef_ambient = ambient;
}

void render_set_diffuse_factor(float diffuse)
{
	coef_diffuse = diffuse;
}

void render_set_specular_factor(float specular)
{
	coef_specular = specular;
}

void render_set_gamma_factor(float gamma)
{
	coef_gamma = gamma;
}

void render_set_light_angle(float angle_pos)
{
	if(light_rot_angle > 360.0f)
		light_rot_angle = 0.0f;
	else if(light_rot_angle < 0.0f)
		light_rot_angle = 360.0f;
	
	light_rot_angle = angle_pos;
}

void render_set_light_rot_step(float step)
{
	light_rot_step = step;
}

void render_set_light_animation(int animate)
{
	light_animate = (animate ? 1 : 0);
}

void render_set_camera_step(float step)
{
	camera_step = step;
}

void render_set_camera_move_speed(float speed)
{
	camera_move_speed = speed;
	
	camera_set_move_velocity(&camera, camera_move_speed);
}

void render_set_camera_fov(float fov)
{
	camera_fov = fov;
	
	camera_set_fov(&camera, camera_fov);
}

////
