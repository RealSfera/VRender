/*
 *  Copyright (C) 2012-2013 Evgeny Panov
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

#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QtOpenGL/QGLWidget>
#include "render.h"

class GLWindow : public QGLWidget
{
		Q_OBJECT
		
		// основной таймер
		int timer_id;
		
		// время кадра в милисекундах
		unsigned int frame_time_ms;
		
		// текущий fps
		unsigned int current_fps;
		
		// настройки QtOpenGL
		QGLFormat gl_format;

		// Опции рендеринга
		
		// Настройки изо-уровня
		float isolevel, isolevel_begin, isolevel_end, isolevel_step;
		bool isolevel_is_animate;
		
		// Настройки камеры
		float camera_step, camera_move_speed, camera_fov;
		
		// Настройки метериала и освещения
		vector3f material_front_color, material_back_color, light_color, light_spec_color;
		float material_shininess, coef_gamma, coef_ambient,
			  coef_diffuse, coef_specular;
		vector3ui volume_size, grid_size;
		
		// Настройки расположения источника света
		float light_angle, light_rot_step;
		bool light_is_animate;

		// Количество потоков
		unsigned threads_num;

		// Включить многопоточность
		bool is_multithreading;
		
	public:
		GLWindow(QWidget *parent = 0);
		~GLWindow();
		
		// Получить текущий fps
		unsigned int get_fps();
		
		bool set_function_text(const char* text);
		void set_isolevel(float level);
		void set_isolevel_animation(int animate);
		void set_isolevel_begin(float level);
		void set_isolevel_end(float level);
		void set_isolevel_step(float step);
		void set_volume_size(vector3ui volume_size);
		void set_grid_size(vector3ui grid_size);
		void set_material_color(vector3f front_color, vector3f back_color);
		void set_light_color(vector3f color);
		void set_light_spec_color(vector3f color);
		void set_material_shininess(float shininess);
		void set_ambient_factor(float ambient);
		void set_diffuse_factor(float diffuse);
		void set_specular_factor(float specular);
		void set_gamma_factor(float gamma);
		void set_light_angle(float angle);
		void set_light_animation(int animate);
		void set_light_rot_step(float step);
		void set_camera_step(float step);
		void set_camera_move_speed(float speed);
		void set_camera_fov(float fov);
		void set_number_of_threads(unsigned num);
		
		void begin_generation();
		
	protected:
		void paintGL();
		void initializeGL();
		void resizeGL(int w, int h);

		void update_options();
	
		void keyPressEvent(QKeyEvent *event);
		void keyReleaseEvent(QKeyEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void wheelEvent(QWheelEvent *event);
		void timerEvent(QTimerEvent *) {updateGL();}
		
};

#endif // GLWINDOW_H
