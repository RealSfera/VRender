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

#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#include "common.h"
#include <math.h>

#define MATH_PI 3.141592653589793238462643
#define DEG_TO_RAD(angle) ( ( MATH_PI * (angle) ) / 180.f )
#define RAD_TO_DEG(angle) ( ( (angle) * 180.0f ) / MATH_PI )

#define math_lerp(a, b, t) ( (a) + ((b) - (a)) * t )
#define math_min(a, b) ( (a) < (b) ? (a) : (b) )
#define math_max(a, b) ( (a) > (b) ? (a) : (b) )
#define math_clamp(x, a, b) (  (x) < (a) ? (a) : ( (x) > (b) ? (b) : (x) )  )
#define math_mix(a, b, t) ( (1 - t) * a + (t * b) )

#ifdef __cplusplus
extern "C" {
#endif

/****** Математические функции ******/
SINLINE float math_sqrtf(float c)
{
	return sqrtf(c);
}

SINLINE float math_sinf(float a)
{
	return sinf(a);
}

SINLINE float math_cosf(float a)
{
	return cosf(a);
}

SINLINE float math_tanf(float a)
{
	return tanf( a);
}

SINLINE float math_fabs(float a)
{
	return fabs(a);
}

SINLINE float math_acosf(float a)
{
	return acosf(a);
}

SINLINE float math_asinf(float a)
{
	return asinf(a);
}

SINLINE float math_powf(float c, float power)
{
	return (float) pow((float) c, (float) power);
}

/* Косинусная интерполяция */
SINLINE float math_cerp(float a, float b, float t)
{
	float ft = t * MATH_PI;
	float f = (1.0f - math_cosf(ft)) * 0.5f;
	
	return a * (1.0f - f) + b * f;
}

SINLINE float math_floorf(float a)
{
	return floorf(a);
}

SINLINE int math_fast_floorf(float a)
{
	return a > 0 ? (int) a : (int) (a - 1);
}
	
#ifdef __cplusplus
}
#endif

#endif /* MATH_H_INCLUDED */
