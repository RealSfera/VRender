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

#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#ifndef __WIN32
    #define GLCOREARB_PROTOTYPES
    #include "GL/glcorearb.h"
#endif

#ifdef __WIN32
	#include <GL/gl.h>
    #include "GL/glext.h"
    #include "gl_funcs.h"
#endif

#define _DEBUG_
#define _TRACE_

#ifdef _DEBUG_

	#ifdef _TRACE_

		#define TRACE_MSG(msg, ...) printf("[TRACE] %s() " msg, __FUNCTION__, ##__VA_ARGS__)
		#define TRACE_CODE(code) code
		#define TRACE_FUNC() printf("[FUNC] %s\n", __FUNCTION__)
		//#define TRACE_FUNC()

	#else /* _TRACE_ */

		#define TARCE_FUNC()
		#define TRACE_MSG(msg, ...)
		#define TRACE_CODE(code)

	#endif /* _TRACE_ */

	#define DEBUG_CODE(code) code

	#define ERROR_MSG(msg, ...) fprintf(stderr, "[ERROR] %s() " msg, __FUNCTION__, ##__VA_ARGS__)
	#define DEBUG_MSG(msg, ...) printf("[DEBUG] " msg, ##__VA_ARGS__)

	#define ASSERT_MSG(msg, ...) printf("[ASSERT] " msg, ##__VA_ARGS__)
	#define ASSERT(expr) if( !(expr) ) { printf("[ASSERT] " #expr " == %i\n", ( !(expr):0:1 )); }

	// Бывает удобно...
	#define IF_FAILED_RET(expr, ret) if( !(expr) ) { \
										printf( "file: %s line: %i function: %s \n" #expr " == %i\n", \
												__FILE__, __LINE__, __FUNCTION__, ( !(expr)?0:1 ) ); \
										return ret; \
									  }
									  
	#define IF_FAILED(expr) if( !(expr) ) { \
								printf( "file: %s line: %i function: %s \n" #expr " == %i\n", \
												__FILE__, __LINE__, __FUNCTION__, ( !(expr)?0:1 ) ); \
								return; \
							}
						
	#define IF_FAILED0(expr) if( !(expr) ) { \
								printf( "file: %s line: %i function: %s \n" #expr " == %i\n", \
												__FILE__, __LINE__, __FUNCTION__, ( !(expr)?0:1 ) ); \
								return 0; \
							  }
							  
	#define IF_FAILED1(expr) if( !(expr) ) { \
								printf( "file: %s line: %i function: %s \n" #expr " == %i\n", \
												__FILE__, __LINE__, __FUNCTION__, ( !(expr)?0:1 ) ); \
								return 1; \
							 }

#else /* _DEBUG_ */

	#define ERROR_MSG(msg, ...) fprintf(stderr, "[ERROR] " msg, ##__VA_ARGS__)
	
	#define DEBUG_MSG(msg, ...)
	#define ASSERT_MSG(msg, ...)
	#define ASSERT(expr)
	#define IF_FAILED_RET(expr, ret)
	#define IF_FAILED(expr)
	#define IF_FAILED0(expr)
	#define IF_FAILED1(expr)
	#define DEBUG_CODE(code)
	
	#define TARCE_FUNC()
	#define TRACE_MSG(msg, ...)
	#define TRACE_CODE(code)

#endif /* _DEBUG_ */

#define INFO_MSG(msg, ...)  printf("[INFO] " msg, ##__VA_ARGS__)
#define FATAL_MSG(msg, ...) fprintf(stderr, "[FATAL] " msg, ##__VA_ARGS__);

#define STRINGIFY(str) #str

#ifdef __GNUC__
	#define INLINE __inline
	#define SINLINE static __inline
#elif _WIN32
	#define INLINE __inline
#else
	#define INLINE
#endif

#endif /* COMMON_H_INCLUDED */
