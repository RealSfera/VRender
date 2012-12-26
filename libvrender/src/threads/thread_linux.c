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

#ifdef __linux

#include "common.h"
#include "thread.h"
#include <pthread.h>

/* Обёртка над pthread */

int thread_create(thread_t *thread, void (*thread_callback)(void *arg), void *func_arg)
{
	int err = 0;

	TRACE_FUNC();	
	
	err = pthread_create((pthread_t*) thread, NULL, (void *(*)(void *)) thread_callback, func_arg);
	
	return err;
}

int thread_join(thread_t *thread)
{
	int err = 0;
	
	TRACE_FUNC();
	
	err = pthread_join((pthread_t) *thread, NULL);

	return err;
}

int thread_kill(thread_t *thread)
{
	int err = 0;
	
	TRACE_FUNC();
	
	err = pthread_cancel((pthread_t) *thread);

	return err;
}

thread_t thread_self(void)
{
	pthread_t result;
	
	TRACE_FUNC();
	
	result = pthread_self();

	return (thread_t) result;
}

int thread_create_mutex(thread_mutex_t *mutex)
{
	int err = 0;
	
	TRACE_FUNC();
	
	err = pthread_mutex_init( (pthread_mutex_t*) mutex, NULL);

	return err;
}

int thread_mutex_lock(thread_mutex_t *mutex)
{
	int err = 0;

	err = pthread_mutex_lock((pthread_mutex_t*) mutex);

	return err;
}

int thread_mutex_unlock(thread_mutex_t *mutex)
{
	int err = 0;

	err = pthread_mutex_unlock((pthread_mutex_t*) mutex);

	return err;
}

#endif /* __linux */
