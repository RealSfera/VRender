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
	
	return (corners0 + corners1 + corners2) + (sides0 + sides1 + sides2) + (center0 + center1 + center2);
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
		total += interpolated_noise2d(v.x * (2 * i), v.y * (2 * i)) * (p * i);
	}
	
	return total;
}

float value_noise_3d(vector3f v)
{
	float total = 0.0f;
	float p = PERSISTENCE;
	int n = NUMBER_OCTAVES - 1;
	
	for(int i = 1; i < n; i++) {
		total += interpolated_noise3d(v.x * (2 * i), v.y * (2 * i), v.z * (2 * i)) * (p * i);
	}
	
	return total;
}


static float *snoise3d_data = NULL;
static vector3ui snoise_size;

void snoise3d_init_file(const char *filename, vector3ui snoise3d_size)
{
	IF_FAILED(filename);
	
	FILE *file = NULL;
	
	snoise_size = snoise3d_size;
	
	// существует файл с шумом ?...
	if((file = fopen(filename, "rb")) == NULL) {
		// ...файла нет, значит нужно создать
		ERROR_MSG("cannot open file %s with noise; creating it\n", filename);
		
		snoise3d_data = (float*) malloc(sizeof(float) * snoise_size.x*snoise_size.y*snoise_size.z);
		
		file = fopen(filename, "w");
		
		for(unsigned k = 0; k < snoise_size.z; k++) {
			for(unsigned j = 0; j < snoise_size.y; j++) {
				for(unsigned i = 0; i < snoise_size.x; i++) {
					snoise3d_data[i + j*snoise_size.x + k*snoise_size.x*snoise_size.y] = 
							simplex_noise_3d(vec3f(i, j ,k));
				}
			}
		}
		fwrite((void*) snoise3d_data, sizeof(float), snoise_size.x*snoise_size.y*snoise_size.z, file);
		fclose(file);
	} else {
		unsigned size = snoise3d_size.x*snoise3d_size.y*snoise3d_size.z;
		// ...файл есть, читаем данные
		snoise3d_data = (float*) malloc(sizeof(float) * size);
		unsigned read_bytes = fread((void*) snoise3d_data, sizeof(float), size, file);
		if(read_bytes*sizeof(float) != size*sizeof(float)) {
			ERROR_MSG("cannot read 3d noise data; file = %s\n", filename);
		}
		fclose(file);
	}
}

// трилинейная косинусная интерполяция значений
INLINE float tricerp(float v000, float v100, float v101, float v001,  
			  float v010, float v110, float v111, float v011, vector3f t)
{
	float result = 0.0f;
	
	float c1x = math_cerp(v000, v100, t.x);
	float c2x = math_cerp(v010, v110, t.x);
	float c3x = math_cerp(v001, v101, t.x);
	float c4x = math_cerp(v011, v111, t.x);
	
	float c1y = math_cerp(c1x, c2x, t.y);
	float c2y = math_cerp(c3x, c4x, t.y);
	
	result = math_cerp(c1y, c2y, t.z);
	
	return result;
}

float snoise3d_tricerp_file(vector3f p)
{
	#define _data(vx, vy, vz) snoise3d_data[((vx) % snoise_size.x) + ((vy) % snoise_size.y)*snoise_size.x + ((vz) % snoise_size.z)*snoise_size.x*snoise_size.y]
	
	int int_px = (int) p.x;
	int int_py = (int) p.y;
	int int_pz = (int) p.z;
	
	float v000 = _data( int_px,     int_py,     int_pz );
	float v100 = _data( int_px + 1, int_py,     int_pz );
	float v101 = _data( int_px + 1, int_py,     int_pz + 1 );
	float v001 = _data( int_px,     int_py,     int_pz + 1 );
	
	float v010 = _data( int_px,     int_py + 1, int_pz );
	float v110 = _data( int_px + 1, int_py + 1, int_pz );
	float v111 = _data( int_px + 1, int_py + 1, int_pz + 1 );
	float v011 = _data( int_px,     int_py + 1, int_pz + 1 );
	
	vector3f t = vec3f( p.x-int_px, p.y-int_py, p.z-int_pz);
	
	return tricerp(v000, v100, v101, v001, v010, v110, v111, v011, t);
}

static vector3f grad3[] = 
	{
		{1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0}, 
        {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1}, 
        {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}
	};

static int perm[512];
static int p[] = 
	{
		151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 
		142, 8, 99, 37, 240, 21, 10, 23, 190,  6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 
		203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 
		74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 
		105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54,  65, 25, 63, 161,  1, 216, 80, 73, 209, 76, 132, 
		187, 208,  89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186,  3, 
		64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 
		47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152,  2, 44, 154, 163,  70, 221, 
		153, 101, 155, 167,  43, 172, 9, 129, 22, 39, 253,  19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 
		112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241,  81, 51, 145, 
		235, 249, 14, 239, 107, 49, 192, 214,  31, 181, 199, 106, 157, 184,  84, 204, 176, 115, 121, 50, 45, 
		127,  4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
   };

INLINE static float fade(float t)
{
	return t * t * t * (t * (t * 6.0f - 15.0f) +  10.0f);
}

void noise_init(void)
{
	for(int i = 0; i < 512; i++) {
		perm[i] = p[i & 255];
	}
}

////////////////////////////////// Perlin Noise - Шум Перлина
// Основан на реализациях Кена Перлина (Ken Perlin) и Стефана Густавсона (Stefan Gustavson)

float perlin_noise_3d(vector3f pos)
{
	int x = math_fast_floorf(pos.x);
	int y = math_fast_floorf(pos.y);
	int z = math_fast_floorf(pos.z);
	
	pos.x -= x;
	pos.y -= y;
	pos.z -= z;
	
	x &= 255;
	y &= 255;
	z &= 255;
	
	float u = fade(pos.x), v = fade(pos.y), w = fade(pos.z);
	
	int gi000 = perm[ x +	  perm[ y +		perm[z] ] ]		% 12;
	int gi001 = perm[ x +	  perm[ y +		perm[z + 1] ] ] % 12;
	int gi010 = perm[ x +	  perm[ y + 1 + perm[z] ] ]		% 12;
	int gi011 = perm[ x +	  perm[ y + 1 + perm[z + 1] ] ] % 12;
	int gi100 = perm[ x + 1 + perm[ y +		perm[z] ] ]		% 12;
	int gi101 = perm[ x + 1 + perm[ y +		perm[z + 1] ] ] % 12;
	int gi110 = perm[ x + 1 + perm[ y + 1 + perm[z] ] ]		% 12;
	int gi111 = perm[ x + 1 + perm[ y + 1 + perm[z + 1] ] ] % 12;
	
	float n000 = vec3f_dot(grad3[gi000], vec3f(pos.x,		 pos.y,		   pos.z));
	float n100 = vec3f_dot(grad3[gi100], vec3f(pos.x - 1.0f, pos.y,		   pos.z));
	float n010 = vec3f_dot(grad3[gi010], vec3f(pos.x,		 pos.y - 1.0f, pos.z));
	float n110 = vec3f_dot(grad3[gi110], vec3f(pos.x - 1.0f, pos.y - 1.0f, pos.z));
	float n001 = vec3f_dot(grad3[gi001], vec3f(pos.x,		 pos.y,		   pos.z - 1.0f));
	float n101 = vec3f_dot(grad3[gi101], vec3f(pos.x - 1.0f, pos.y,		   pos.z - 1.0f));
	float n011 = vec3f_dot(grad3[gi011], vec3f(pos.x,		 pos.y - 1.0f, pos.z - 1.0f));
	float n111 = vec3f_dot(grad3[gi111], vec3f(pos.x - 1.0f, pos.y - 1.0f, pos.z - 1.0f));
	
	float nx00 = math_mix(n000, n100, u);
	float nx01 = math_mix(n001, n101, u);
	float nx10 = math_mix(n010, n110, u);
	float nx11 = math_mix(n011, n111, u);
	
	float nxy0 = math_mix(nx00, nx10, v);
	float nxy1 = math_mix(nx01, nx11, v);
	
	float nxyz = math_mix(nxy0, nxy1, w);
			
	return nxyz;
}

////////////////////////////////// Simplex Noise - Симплекс Шум
// Основан на реализации Стефана Густавсона (Stefan Gustavson)

float simplex_noise_2d(vector2f pos)
{
	const float F2 = 0.5f * (math_sqrtf(3.0f) - 1.0f);
	const float G2 = (3.0f - math_sqrtf(3.0f)) / 6.0f;
	
	float s = (pos.x + pos.y) * F2;
	
	int i = math_fast_floorf(pos.x + s);
	int j = math_fast_floorf(pos.y + s);
	
	float t = (i + j) * G2;
	float X0 = i - t;
	float Y0 = j - t;
	
	float x0 = pos.x - X0;
	float y0 = pos.y - Y0;
	
	int i1, j1;
	
	if(x0 > y0) {
		i1 = 1; j1 = 0;
	} else {
		i1 = 0; j1 = 1;
	}
	
	float x1 = x0 - i1 + G2;
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2;
	float y2 = y0 - 1.0f + 2.0f * G2;
	
	int ii = i & 255;
	int jj = j & 255;
	
	int gi0 = perm[ii +		 perm[jj	 ]] % 12;
	int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
	int gi2 = perm[ii + 1  + perm[jj + 1]] % 12;
	
	float n0 = 0.0f, n1 = 0.0f, n2 = 0.0f;
	
#define dot2_3(v1, v2) ( v1.x*v2.x + v1.y*v2.y )
	
	float t0 = 0.5f - x0*x0 - y0*y0;
	if(t0 < 0.0f) { 
		n0 = 0.0f;
	} else {
		t0 *= t0;
		n0 = t0 * t0 * dot2_3(grad3[gi0], vec2f(x0, y0));
	}
	
	float t1 = 0.5f - x1*x1 - y1*y1;
	if(t1 < 0.0f) { 
		n1 = 0.0f;
	} else {
		t1 *= t1;
		n1 = t1 * t1 * dot2_3(grad3[gi1], vec2f(x1, y1));
	}
	
	float t2 = 0.5f - x2*x2 - y2*y2;
	if(t2 < 0.0f) { 
		n2 = 0.0f;
	} else {
		t2 *= t2;
		n2 = t2 * t2 * dot2_3(grad3[gi2], vec2f(x2, y2));
	}
	
#undef dot2_3

	return 70.0f * (n0 + n1 + n2);
}

float simplex_noise_3d(vector3f pos)
{
	const float F3 = 1.0f / 3.0f;
	const float G3 = 1.0f / 6.0f;
	
	float s = (pos.x + pos.y + pos.z) * F3;
	
	int i = math_fast_floorf(pos.x + s);
	int j = math_fast_floorf(pos.y + s);
	int k = math_fast_floorf(pos.z + s);
	
	
	float t = (i + j + k) * G3;
	float X0 = i - t;
	float Y0 = j - t;
	float Z0 = k - t;
	
	float x0 = pos.x - X0;
	float y0 = pos.y - Y0;
	float z0 = pos.z - Z0;
	
	int i1, i2,  j1, j2,  k1, k2;
	
	if(x0 >= y0) {
		if(y0 >= z0) {
			i1 = 1; j1 = 0; k1 = 0;
			i2 = 1; j2 = 1; k2 = 0;
		} else if(x0 >= z0) {
			i1 = 1; j1 = 0; k1 = 0;
			i2 = 1; j2 = 0; k2 = 1;
		} else {
			i1 = 0; j1 = 0; k1 = 1;
			i2 = 1; j2 = 0; k2 = 1;
		}
	} else {
		if(y0 < z0) {
			i1 = 0; j1 = 0; k1 = 1;
			i2 = 0; j2 = 1; k2 = 1;
		} else if(x0 < z0) {
			i1 = 0; j1 = 1; k1 = 0;
			i2 = 0; j2 = 1; k2 = 1;
		} else {
			i1 = 0; j1 = 1; k1 = 0;
			i2 = 1; j2 = 1; k2 = 0;
		}
	}
	
	float x1 = x0 - i1 + G3;
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;
	float x2 = x0 - i2 + 2.0f * G3;
	float y2 = y0 - j2 + 2.0f * G3;
	float z2 = z0 - k2 + 2.0f * G3;
	float x3 = x0 - 1.0f + 3.0f * G3;
	float y3 = y0 - 1.0f + 3.0f * G3;
	float z3 = z0 - 1.0f + 3.0f * G3;
	
	int ii = i & 255;
	int jj = j & 255;
	int kk = k & 255;
	
	int gi0 = perm[ii +		 perm[jj +		perm[kk		]]] % 12;
	int gi1 = perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]] % 12;
	int gi2 = perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]] % 12;
	int gi3 = perm[ii + 1 +  perm[jj + 1 +  perm[kk + 1 ]]] % 12;
	
	float n0 = 0.0f, n1 = 0.0f, n2 = 0.0f, n3 = 0.0f;
	
	float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
	if(t0 < 0.0f) { 
		n0 = 0.0f;
	} else {
		t0 *= t0;
		n0 = t0 * t0 * vec3f_dot(grad3[gi0], vec3f(x0, y0, z0));
	}
	
	float t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
	if(t1 < 0.0f) { 
		n1 = 0.0f;
	} else {
		t1 *= t1;
		n1 = t1 * t1 * vec3f_dot(grad3[gi1], vec3f(x1, y1, z1));
	}
	
	float t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
	if(t2 < 0.0f) { 
		n2 = 0.0f;
	} else {
		t2 *= t2;
		n2 = t2 * t2 * vec3f_dot(grad3[gi2], vec3f(x2, y2, z2));
	}
	
	float t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
	if(t3 < 0.0f) { 
		n3 = 0.0f;
	} else {
		t3 *= t3;
		n3 = t3 * t3 * vec3f_dot(grad3[gi3], vec3f(x3, y3, z3));
	}
	
	return 32.0f * (n0 + n1 + n2 + n3);
	//return 16.0f * (n0 + n1 + n2 + n3) + 1.0f;
}
