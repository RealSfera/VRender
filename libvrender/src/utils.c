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

#include "utils.h"

long utils_file_get_size(const char *filename)
{
	FILE *file = NULL;
	long result = 0L;
	
	IF_FAILED_RET(filename, -1L);
	
	if((file = fopen(filename, "rb")) == NULL) {
		ERROR_MSG("cannot open file %s for read\n", filename);
		return -1L;
	}
	
	// перемещаемся в конец файла
	fseek(file, 0, SEEK_END);
	
	result = ftell(file);
	
	fclose(file);
	
	// и узнаем на каком байте находится указатель
	return result;
}

int utils_read_file(const char *filename, char *buffer)
{
	FILE *file = NULL;
	int ch = 0;

	IF_FAILED0(filename && buffer);
	
	if((file = fopen(filename, "rb")) == NULL) {
		ERROR_MSG("cannot open file %s for read\n", filename);
		return 0;
	}
	
	// препологаем, что буфер достаточного размера
	while((ch = getc(file)) != EOF) {
		*buffer++ = (char) ch;
	}
	*buffer = '\0';
	
	fclose(file);
	
	return 1;
}
