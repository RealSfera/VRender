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

#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "common.h"

// коды клавиш. таблица приближена к формату ASCII
enum {
	KB_NULL = 0,
	
	KB_backspace = 8, KB_tab, KB_enter = 13, KB_shift = 16, KB_ctrl, KB_alt,
	KB_esc = 27, KB_cursor_right = 28, KB_cursor_left, KB_cursor_up, KB_cursor_down, KB_space = 32,
	
	KB_0 = 48, KB_1, KB_2, KB_3, KB_4, KB_5, KB_6, KB_7, KB_8, KB_9,
	
	KB_A = 65, KB_B, KB_C, KB_D, KB_E, KB_F, KB_G, KB_H, KB_I, KB_J, KB_K, KB_L, KB_M, KB_N, 
		KB_O, KB_P, KB_Q, KB_R, KB_S, KB_T, KB_U, KB_V, KB_W, KB_X, KB_Y, KB_Z,
	
	KB_a = 97, KB_b, KB_c, KB_d, KB_e, KB_f, KB_g, KB_h, KB_i, KB_j, KB_k, KB_l, KB_m, KB_n, 
		KB_o, KB_p, KB_q, KB_r, KB_s, KB_t, KB_u, KB_v, KB_w, KB_x, KB_y, KB_z,
	
	KB_F1 = 112, KB_F2, KB_F3, KB_F4, KB_F5, KB_F6, KB_F7, KB_F8, KB_F9, KB_F10, KB_F11, KB_F12,
	
	MAX_KB_KEYS
};

// коды кнопок мыши
enum {
	MOUSE_NULL = 0,
	
	MOUSE_LEFTBTN = 1, MOUSE_MIDDLEBTN, MOUSE_RIGHTBTN,
	
	MAX_MOUSE_BUTTONS
};

#ifdef __cplusplus
extern "C" {
#endif

/* Инициализировать систему ввода */
void input_init(void);

/* Получить состояние клавиши клавиатуры. 0 - не нажата, 1 - нажата, -1 - ошибка */
int input_get_kbkey_state(unsigned int key);

/* Получить состояние кнопки мыши. 0 - не нажата, 1 - нажата, -1 - ошибка */
int input_get_mousebtn_state(unsigned int key);

/* Получить текущие координаты мыши */
void input_get_mouse_position(int *mouse_x, int *mouse_y);

/* Получить текущее вращение колёсика мыши */
float input_get_wheel_rot(void);

/* Установить предельные углы вращения колёсика */
void input_set_wheel_limits(unsigned from, unsigned to);

/* Различные события клавиатуры и мыши */
void input_event_on_keypress(unsigned key);
void input_event_on_keyreleased(unsigned key);
void input_event_on_btnpress(unsigned key, int mouse_x, int mouse_y);
void input_event_on_btnreleased(unsigned key, int mouse_x, int mouse_y);
void input_event_on_mouse_motion(int mouse_x, int mouse_y);
void input_event_on_mouse_wheel(float steps);

#ifdef __cplusplus
}
#endif

#endif /* INPUT_H_INCLUDED */
