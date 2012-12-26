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

#include "math/noise.h"

#define PERSISTENCE 0.5f
#define NUMBER_OCTAVES 3

////////////////////////////////// Value Noise -  Численный Шум

// генераторы случайных чисел в итервале [-1, 1]
static float int_noise1d(int x)
{
	x = (x << 13) ^ x;
	
	return (1.0f - 
			( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 
			1073741824.0f );
}

static float int_noise2d(int x, int y)
{
	int n = x + y * 57;
	
	n = (x << 13) ^ n;
	
	return (1.0f - 
			( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 
			1073741824.0f );
}

static float int_noise3d(int x, int y, int z)
{
	int n = x + y * 47 + z * 59;
	
	n = (x << 13) ^ n;
	
	return (1.0f - 
			( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 
			1073741824.0f );
}

// сглаженный шум
static float smooth_noise1d(float x)
{
	return int_noise1d((int) x) / 2.0f + 
			int_noise1d((int) (x - 1)) / 4.0f +
			int_noise1d((int) (x + 1)) / 4.0f;
}

static float smooth_noise2d(float x, float y)
{
	float corners = ( int_noise2d((int)(x - 1), (int)(y - 1)) +
					  int_noise2d((int)(x + 1), (int)(y - 1)) +
					  int_noise2d((int)(x - 1), (int)(y + 1)) +
					  int_noise2d((int)(x + 1), (int)(y + 1)) ) / 16.0f;
	float sides = ( int_noise2d((int)(x - 1), (int) y) +
					int_noise2d((int)(x + 1), (int) y) +
				    int_noise2d((int) x, (int)(y - 1)) +
				    int_noise2d((int) x, (int)(y + 1)) ) / 8.0f;
	float center = int_noise2d((int) x, (int) y) / 4.0f;
	
	return corners + sides + center;
}

static float smooth_noise3d(float x, float y, float z)
{
	float corners0 = ( int_noise3d((int)(x - 1), (int)(y - 1), (int) (z - 1)) +
					  int_noise3d((int)(x + 1), (int)(y - 1), (int) (z - 1)) +
					  int_noise3d((int)(x - 1), (int)(y + 1), (int) (z - 1)) +
					  int_noise3d((int)(x + 1), (int)(y + 1), (int) (z - 1)) ) / 1024.0f;
	float sides0 = ( int_noise3d((int)(x - 1), (int) y, (int) (z - 1)) +
					int_noise3d((int)(x + 1), (int) y, (int) (z - 1)) +
				    int_noise3d((int) x, (int)(y - 1), (int) (z - 1)) +
				    int_noise3d((int) x, (int)(y + 1), (int) (z - 1)) ) / 512.0f;
	
	float center0 = int_noise3d((int) x, (int) y, (int) (z - 1)) / 256.0f;
	
	
	float corners1 = ( int_noise3d((int)(x - 1), (int)(y - 1), (int) z) +
					  int_noise3d((int)(x + 1), (int)(y - 1), (int) z) +
					  int_noise3d((int)(x - 1), (int)(y + 1), (int) z) +
					  int_noise3d((int)(x + 1), (int)(y + 1), (int) z) ) / 128.0f;
	float sides1 = ( int_noise3d((int)(x - 1), (int) y, (int) z) +
					int_noise3d((int)(x + 1), (int) y, (int) z) +
				    int_noise3d((int) x, (int)(y - 1), (int) z) +
				    int_noise3d((int) x, (int)(y + 1), (int) z) ) / 64.0f;
	
	float center1 = int_noise3d((int) x, (int) y, (int) z) / 32.0f;
	
	
	float corners2 = ( int_noise3d((int)(x - 1), (int)(y - 1), (int) (z + 1)) +
					  int_noise3d((int)(x + 1), (int)(y - 1), (int) (z + 1)) +
					  int_noise3d((int)(x - 1), (int)(y + 1), (int) (z + 1)) +
					  int_noise3d((int)(x + 1), (int)(y + 1), (int) (z + 1)) ) / 16.0f;
	float sides2 = ( int_noise3d((int)(x - 1), (int) y, (int) (z + 1)) +
					int_noise3d((int)(x + 1), (int) y, (int) (z + 1)) +
				    int_noise3d((int) x, (int)(y - 1), (int) (z + 1)) +
				    int_noise3d((int) x, (int)(y + 1), (int) (z + 1)) ) / 8.0f;
	
	float center2 = int_noise3d((int) x, (int) y, (int) (z + 1)) / 4.0f;
	
	return corners0 + corners1 + corners2 + sides0 + sides1 + sides2 + center0 + center1 + center2;
}


// интерполяция шума

static float interpolated_noise1d(float x)
{
	int int_x = (int) x;
	float frac_x = x - int_x;
	
	float v1 = smooth_noise1d(int_x);
	float v2 = smooth_noise1d(int_x + 1);
	
	return math_cerp(v1, v2, frac_x);
}

static float interpolated_noise2d(float x, float y)
{
	int int_x = (int) x;
	float frac_x = x - int_x;
	
	int int_y = (int) y;
	float frac_y = y - int_y;
	
	float v1 = smooth_noise2d(int_x, int_y);
	float v2 = smooth_noise2d(int_x + 1, int_y);
	float v3 = smooth_noise2d(int_x, int_y + 1);
	float v4 = smooth_noise2d(int_x + 1, int_y + 1);
	
	float i1 = math_cerp(v1, v2, frac_x);
	float i2 = math_cerp(v3, v4, frac_x);
	
	return math_cerp(i1, i2, frac_y);
}

static float interpolated_noise3d(float x, float y, float z)
{
	int int_x = (int) x;
	float frac_x = x - int_x;
	
	int int_y = (int) y;
	float frac_y = y - int_y;
	
	int int_z = (int) z;
	float frac_z = z - int_z;
	
	float v1 = smooth_noise3d(int_x, int_y, int_z);
	float v2 = smooth_noise3d(int_x + 1, int_y, int_z);
	float v3 = smooth_noise3d(int_x, int_y + 1, int_z);
	float v4 = smooth_noise3d(int_x + 1, int_y + 1, int_z);
	
	float v5 = smooth_noise3d(int_x, int_y, int_z + 1);
	float v6 = smooth_noise3d(int_x + 1, int_y, int_z + 1);
	float v7 = smooth_noise3d(int_x, int_y + 1, int_z + 1);
	float v8 = smooth_noise3d(int_x + 1, int_y + 1, int_z + 1);
	
	float i1 = math_cerp(v1, v2, frac_x);
	float i2 = math_cerp(v3, v4, frac_x);
	float i3 = math_cerp(v5, v6, frac_x);
	float i4 = math_cerp(v7, v8, frac_x);
	
	float ii1 = math_cerp(i1, i2, frac_y);
	float ii2 = math_cerp(i3, i4, frac_y);
	
	return math_cerp(ii1, ii2, frac_z);
}

// Численный Шум

float value_noise_1d(float x)
{
	float total = 0.0f;
	float p = PERSISTENCE;
	int n = NUMBER_OCTAVES - 1;
	
	for(int i = 1; i < n; i++) {
		total += interpolated_noise1d(x * (2 * i)) * (p * i);
	}
	
	return total;
}

float value_noise_2d(vector2f v)
{
	float total = 0.0f;
	float p = PERSISTENCE;
	int n = NUMBER_OCTAVES - 1;
	
	for(int i = 1; i < n; i++) {
		total += interpolated_noise2d(v.x * (2 * i), v.x * (2 * i)) * (p * i);
	}
	
	return total;
}

float value_noise_3d(vector3f v)
{
	float total = 0.0f;
	float p = PERSISTENCE;
	int n = NUMBER_OCTAVES - 1;
	
	for(int i = 1; i < n; i++) {
		total += interpolated_noise3d(v.x * (2 * i), v.x * (2 * i), v.z * (2 * i)) * (p * i);
	}
	
	return total;
}


////////////////////////////////// Perlin Noise - Шум Перлина

INLINE static float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) +  10);
}

INLINE static float grad(int hash, vector3f p)
{
	int h = hash & 15;
	float u = h < 8 ? p.x : p.y;
	float v = h < 4 ? p.y : h == 12 || h == 14 ? p.x : p.z;
	
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float perlin_noise_1d(float x)
{
	return 0.0f;
}

float perlin_noise_2d(vector2f p)
{
	return 0.0f;
}

float perlin_noise_3d(vector3f p)
{
	return 0.0f;
}
