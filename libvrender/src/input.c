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

#include "input.h"

// массив состояний клавиш клавиатуры
static int input_kb_keys_state[MAX_KB_KEYS];

// массив состояний кнопок мыши
static int input_mouse_btns_state[MAX_MOUSE_BUTTONS];

static int current_mouse_x = 0, current_mouse_y = 0;
static float current_wheel_rot = 0.0f;

static unsigned wheel_limit_from = 0, wheel_limit_to = 0;

static int init = 0;

void input_init(void)
{
	int i = 0;
	
	IF_FAILED(!init);
	
	TRACE_MSG("init input system\n");
	
	for(i = 0; i < MAX_KB_KEYS; i++) {
		input_kb_keys_state[i] = 0;
	}
	
	for(i = 0; i < MAX_MOUSE_BUTTONS; i++) {
		input_mouse_btns_state[i] = 0;
	}
	
	init = 1;
}

int input_get_kbkey_state(unsigned int key)
{
	if(key > KB_NULL && key < MAX_KB_KEYS) {
		return input_kb_keys_state[key];
	}
	
	// неверный код клавиши
	return -1;
}

int input_get_mousebtn_state(unsigned int key)
{
	if(key > MOUSE_NULL && key < MAX_MOUSE_BUTTONS) {
		return input_mouse_btns_state[key];
	}
	
	// неверный код кнопки
	return -1;
}

void input_get_mouse_position(int *mouse_x, int *mouse_y)
{
	if(mouse_x)
		*mouse_x = current_mouse_x;
	if(mouse_y)
		*mouse_y = current_mouse_y;
}

float input_get_wheel_rot(void)
{
	return current_wheel_rot;
}

void input_set_wheel_limits(unsigned from, unsigned to)
{
	wheel_limit_from = from;
	wheel_limit_to = to;
	
	current_wheel_rot = (from+to) / 2.0f;
}

void input_event_on_keypress(unsigned key)
{
	IF_FAILED(init);
	
	// код клавиши должен быть валидный
	if(key > KB_NULL && key < MAX_KB_KEYS) {
		input_kb_keys_state[key] = 1;
	}
}

void input_event_on_keyreleased(unsigned key)
{
	IF_FAILED(init);

	if(key > KB_NULL && key < MAX_KB_KEYS) {
		input_kb_keys_state[key] = 0;
		
		// если мы отпустили букву в верхнем регистре (или нижнем), 
		//    то мы также отпустили и в нижнем (верхнем) регистре
		if(key >= KB_A && key <= KB_Z)
			input_kb_keys_state[(key - KB_A)+KB_a] = 0;
		else if(key >= KB_a && key <= KB_z)
			input_kb_keys_state[(key - KB_a)+KB_A] = 0;
	}
}

void input_event_on_btnpress(unsigned key, int mouse_x, int mouse_y)
{
	IF_FAILED(init);
	
	if(key > MOUSE_NULL && key < MAX_MOUSE_BUTTONS) {
		input_mouse_btns_state[key] = 1;
		
		current_mouse_x = mouse_x;
		current_mouse_y = mouse_y;
	}
}

void input_event_on_btnreleased(unsigned key, int mouse_x, int mouse_y)
{
	IF_FAILED(init);

	if(key > MOUSE_NULL && key < MAX_MOUSE_BUTTONS) {
		input_mouse_btns_state[key] = 0;
		
		current_mouse_x = mouse_x;
		current_mouse_y = mouse_y;
	}
}

void input_event_on_mouse_motion(int mouse_x, int mouse_y)
{
	IF_FAILED(init);
	
	current_mouse_x = mouse_x;
	current_mouse_y = mouse_y;
}

void input_event_on_mouse_wheel(float steps)
{
	IF_FAILED(init);
	
	current_wheel_rot += steps;
	
	if(current_wheel_rot > wheel_limit_to)
		current_wheel_rot = wheel_limit_to;
	else if(current_wheel_rot < wheel_limit_from)
		current_wheel_rot = wheel_limit_from;
}
