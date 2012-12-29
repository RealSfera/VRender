/*
 *  Copyright (C) 2012 Evgeny Panov
 *  This file is part of vrender-gui.
 *
 *  vrender-gui is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  vrender-gui is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with vrender-gui.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "glwindow.h"
#include "input.h"
#include <QtGui>

GLWindow::GLWindow(QWidget *parent) :
	QGLWidget(parent)
{
	
	// настраиваем формат Qt OpenGL
	gl_format.setProfile(QGLFormat::CoreProfile);
	gl_format.setDirectRendering(true);
	gl_format.setDoubleBuffer(true);
	gl_format.setSwapInterval(0);
	gl_format.setSampleBuffers(true);
	
	// ...и устанавливаем
	setFormat(gl_format);
	
	// устанавливаем параметры по-умолчанию
    is_multithreading = false;
    isolevel = 30.0f; isolevel_begin = 0.0f; isolevel_end = 30.0f;
    isolevel_step = 0.01f; isolevel_is_animate = 0;
    camera_step = 0.2f; camera_move_speed = 15.0f; camera_fov = 16.0f;

    material_color = vec3f(0.5f, 0.5f, 0.5f);
    light_color = vec3f(1.0f, 1.0f, 1.0f);
    light_spec_color = vec3f(1.0f, 1.0f, 1.0f);
    material_shininess = 30.0f; coef_gamma = 2.2f; coef_ambient = 0.5f;
    coef_diffuse = 1.0f; coef_specular = 1.0f;

    volume_size = vec3ui(128, 128, 128); grid_size = vec3ui(32, 32, 32);

    light_angle = 0.0f; light_rot_step = 0.01f;
    light_is_animate = 0;
	
	// устанавливаем фокус для окна OpenGL
	setFocusPolicy(Qt::StrongFocus);
	
	// запкскаем основной таймер
	timer_id = startTimer(0);
}

GLWindow::~GLWindow()
{
	killTimer(timer_id);
	render_destroy();
}

void GLWindow::initializeGL()
{
	// инициализируем рендер из libvrender
	if(!render_init()) {
		qDebug() << "ERROR: failed to init render";
		exit(-1);
	}
	
	// инициализация ввода
	input_init();
}

// получить текущее время в милисекундах
INLINE static unsigned long get_current_time_ms()
{
	QTime time = QTime::currentTime();
	
	return (time.hour()*60*60 + time.minute()*60 + time.second()) * 1000 + time.msec();
}

void GLWindow::update_options()
{
	// установка опций освещения, материала и камеры
	render_set_gamma_factor(coef_gamma);
	render_set_ambient_factor(coef_ambient);
	render_set_diffuse_factor(coef_diffuse);
	render_set_specular_factor(coef_specular);
	render_set_material_shininess(material_shininess);
	render_set_material_color(material_color);
	render_set_light_color(light_color);
	render_set_light_spec_color(light_spec_color);
	render_set_isolevel_begin_anim(isolevel_begin);
	render_set_isolevel_end_anim(isolevel_end);
	render_set_isolevel_step_anim(isolevel_step);
	render_set_light_rot_step(light_rot_step);
	render_set_camera_step(camera_step);
	render_set_camera_move_speed(camera_move_speed);
	render_set_camera_fov(camera_fov);
	render_set_number_of_threads( (is_multithreading) ? 2 : 1 );
}

void GLWindow::paintGL()
{
	static unsigned int fps_time_ms = 0, fps_ticks = 0;
	static unsigned long last_time = 0L;
	unsigned long current_time = 0;
	
	current_time = get_current_time_ms();
	frame_time_ms = current_time - last_time;
	
	update_options();
	render_update(frame_time_ms / 1000.0);
	render_draw();
	fps_ticks++;
	
	last_time = current_time;
	
	fps_time_ms += frame_time_ms;
	
	if(fps_time_ms >= 1000) {
		current_fps = fps_ticks;
		fps_ticks = 0;
		fps_time_ms = 0;
	}
}

unsigned int GLWindow::get_fps()
{
	return current_fps;
}

void GLWindow::set_antialiasing(bool enable)
{
	gl_format.setSampleBuffers(enable);
	
	setFormat(gl_format);
}

void GLWindow::resizeGL(int w, int h)
{	
	render_change_window(w, h);
	render_update(frame_time_ms / 1000.0);
}

// преобразует код клавиши из формата X11 в нужный
static unsigned int qkey_to_kbkey(int key)
{
	// проверяем в каком дипазоне находится клавиша и преобразуем её
	if(key >= Qt::Key_A && key <= Qt::Key_Z) {
		// большие буквы латинского алфавита
		return KB_A + (key - Qt::Key_A);
	} else if (key >= Qt::Key_0 && key <= Qt::Key_9) {
		// числа
		return KB_0 + (key - Qt::Key_0);
	} else if (key >= Qt::Key_F1 && key <= Qt::Key_F12) {
		// F1...F12
		return KB_F1 + (key - Qt::Key_F1);
	} else {
		// спец. символы
		switch(key) {
			case Qt::Key_Escape: return KB_esc;
			case Qt::Key_Left: return KB_cursor_left;
			case Qt::Key_Up: return KB_cursor_up;
			case Qt::Key_Right: return KB_cursor_right;
			case Qt::Key_Down: return KB_cursor_down;
			case Qt::Key_Space: return KB_space;
			case Qt::Key_Shift: return KB_shift;
			case Qt::Key_Control: return KB_ctrl;
			case Qt::Key_Alt:return KB_alt;
			case Qt::Key_Tab: return KB_tab;
			case Qt::Key_Backspace: return KB_backspace;
			case Qt::Key_Return: return KB_enter;
			default:
				return KB_NULL;
		};
	}
}

// преобразует код Qt кнопки мыши во внутренний формат
static unsigned int qbtn_to_mousebtn(int button)
{
	switch(button) {
		case Qt::LeftButton: return MOUSE_LEFTBTN;
		case Qt::RightButton: return MOUSE_RIGHTBTN;
		case Qt::MiddleButton: return MOUSE_MIDDLEBTN;
	};
	
	return MOUSE_NULL;
}

static QString get_error_string(int error)
{
	QString result;
	
	switch(error) {
		case -100:
			result = QString::fromUtf8("Передача неверной строки с выражением");
			break;
		case -20:
			result = QString::fromUtf8("Ошибка лексического анализа выражения");
			break;
		case -1:
			result = QString::fromUtf8("Неизвестная ошибка");
			break;
		case 0:
			result = QString::fromUtf8("Нет выражения");
			break;
		case 1:
			result = QString::fromUtf8("Передача неверной строки с функцией");
			break;
		case 2:
			result = QString::fromUtf8("Переменная должна быть инициализированна с помощью \'=\'");
			break;
		case 3:
			result = QString::fromUtf8("null");
			break;
		case 4:
			result = QString::fromUtf8("Ожидание \':\'");
			break;
		case 5:
			result = QString::fromUtf8("Ожидание \')\'");
			break;
		case 6:
			result = QString::fromUtf8("Неизвестная переменная");
			break;
		case 7:
			result = QString::fromUtf8("Обнаружен нулевой указатель на функцию");
			break;
		case 8:
			result = QString::fromUtf8("Неверное число аргументов функции");
			break;
		case 9:
			result = QString::fromUtf8("Ожидание \'(\'");
			break;
		case 10:
			result = QString::fromUtf8("Неверная функция");
			break;
		case 11:
			result = QString::fromUtf8("Ожидание имени функции");
			break;
		case 12:
			result = QString::fromUtf8("Превышено максимальное количество пользовательских переменных");
			break;
		case 13:
			result = QString::fromUtf8("Неизвестный идентификатор");
			break;
		case 14:
			result = QString::fromUtf8("Неизвестная функция");
			break;
		case 15:
			result = QString::fromUtf8("Деление на ноль");
			break;
		
	};
	
	return result;
}

void GLWindow::keyPressEvent(QKeyEvent *event)
{
	input_event_on_keypress(qkey_to_kbkey(event->key()));
}

void GLWindow::keyReleaseEvent(QKeyEvent *event)
{
	input_event_on_keyreleased(qkey_to_kbkey(event->key()));
}

void GLWindow::mousePressEvent(QMouseEvent *event)
{
	input_event_on_btnpress(qbtn_to_mousebtn(event->button()), event->x(), event->y());
}

void GLWindow::mouseReleaseEvent(QMouseEvent *event)
{
	input_event_on_btnreleased(qbtn_to_mousebtn(event->button()), event->x(), event->y());
}

void GLWindow::mouseMoveEvent(QMouseEvent *event)
{
	input_event_on_mouse_motion(event->x(), event->y());
}

bool GLWindow::set_function_text(const char* text)
{
	int error = 0;
	if((error = render_set_function_text(text)) != 0) {
		QString error_str = get_error_string(error);
		QMessageBox::critical(this, QString::fromUtf8("Ошибка в выражении"), 
							  QString::fromUtf8("При вычислении выражения произошла ошибка: "+error_str.toUtf8()));
		
		return false;
	}
	
	return true;
}

////

void GLWindow::set_light_rot_step(float step)
{
	light_rot_step = step;
}

void GLWindow::set_isolevel_step(float step)
{
	isolevel_step = step;
}

void GLWindow::set_isolevel(float level)
{
	isolevel = level;
	render_set_isolevel(isolevel);
}

void GLWindow::set_isolevel_animation(int animate)
{
	isolevel_is_animate = animate;
	render_set_isolevel_animation(isolevel_is_animate);
}

void GLWindow::set_isolevel_begin(float level)
{
	isolevel_begin = level;
}

void GLWindow::set_isolevel_end(float level)
{
	isolevel_end = level;
}

void GLWindow::set_volume_parameters(vector3ui volume_size, vector3ui grid_size)
{
	this->volume_size = volume_size;
	this->grid_size = grid_size;
}

void GLWindow::set_material_color(vector3f color)
{
	material_color = color;
}

void GLWindow::set_light_color(vector3f color)
{
	light_color = color;
}

void GLWindow::set_light_spec_color(vector3f color)
{
	light_spec_color = color;
}

void GLWindow::set_material_shininess(float shininess)
{
	material_shininess = shininess;
}

void GLWindow::set_ambient_factor(float ambient)
{
	coef_ambient = ambient;
}

void GLWindow::set_diffuse_factor(float diffuse)
{
	coef_diffuse = diffuse;
}

void GLWindow::set_specular_factor(float specular)
{
	coef_specular = specular;
}

void GLWindow::set_gamma_factor(float gamma)
{
	coef_gamma = gamma;
}

void GLWindow::set_light_angle(float angle)
{
	light_angle = angle;
	render_set_light_angle(angle);
}

void GLWindow::set_light_animation(int animate)
{
	light_is_animate = animate;
	render_set_light_animation(light_is_animate);
}

void GLWindow::set_camera_step(float step)
{
	camera_step = step;
}

void GLWindow::set_camera_move_speed(float speed)
{
	camera_move_speed = speed;
}

void GLWindow::set_camera_fov(float fov)
{
	camera_fov = fov;
}

////

void GLWindow::set_number_of_threads(unsigned num)
{
	if(num > 1)
		is_multithreading = true;
	else
		is_multithreading = false;
}

void GLWindow::begin_generation()
{
	render_set_volume_parameters(volume_size, grid_size);
	render_update_volume_tex();
}
