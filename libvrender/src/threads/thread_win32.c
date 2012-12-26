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

#ifdef __WIN32

#include "thread.h"
#include <windows.h>

/* Обёртка для WinAPI над функциями для работы с потоками */

int thread_create(thread_t *thread, void (*thread_callback)(void *arg), void *func_arg)
{
	thread = (thread_t) CreateThread(0, 0, (LPTHREAD_START_ROUTINE) thread_callback, 
			(LPVOID) func_arg, 0, 0);

	return (thread != NULL) ? 1 : 0;
}

int thread_join(thread_t *thread)
{
	DWORD err = 0;

	err = WaitForSingleObject( (HANDLE) thread, INFINITE);

	return (err == WAIT_FAILED) ? 0 : 1;
}

int thread_kill(thread_t *thread)
{
	BOOL err = 0;
	
	err = TerminateThread((HANDLE) thread, 0);

	return err;
}

int thread_create_mutex(thread_mutex_t *mutex)
{
	InitializeCriticalSection((LPCRITICAL_SECTION) mutex);

	return 1;
}

int thread_mutex_lock(thread_mutex_t *mutex)
{
	EnterCriticalSection( (LPCRITICAL_SECTION) mutex);

	return 1;
}

int thread_mutex_unlock(thread_mutex_t *mutex)
{
	EnterCriticalSection( (LPCRITICAL_SECTION) mutex);
	
	return 1;
}

thread_t thread_self(void)
{
	return (thread_t) GetCurrentThread();
}

#endif /* __WIN32 */
