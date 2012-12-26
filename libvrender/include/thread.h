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

#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

typedef void* thread_t;
typedef void* thread_mutex_t;

/* Создать поток */
int thread_create(thread_t *thread, void (*thread_callback)(void *arg), void *func_arg);

/* Ожидать завершения потока */
int thread_join(thread_t *thread);

/* Убить поток */
int thread_kill(thread_t *thread);

/* Создать мютекс */
int thread_create_mutex(thread_mutex_t *mutex);

/* Залочить\разлочить мютекс */
int thread_mutex_lock(thread_mutex_t *mutex);
int thread_mutex_unlock(thread_mutex_t *mutex);

/* Получить копию thread_t текущего потока */
thread_t thread_self(void);

#endif /* THREAD_H_INCLUDED */
