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

#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Возвращает размер файла в байтах или -1 при ошибке */
long utils_file_get_size(const char *filename);

/**
 * Открывает и считывает файл в buffer. 
 * Буфер должен быть достаточного размера. 
 * Возвращает -1, если произошла ошибка
 */
int utils_read_file(const char *filename, char *buffer);
	
#ifdef __cplusplus
}
#endif

#endif /* UTILS_H_INCLUDED */
