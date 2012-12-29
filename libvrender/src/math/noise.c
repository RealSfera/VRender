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


static float *noise3d_data = NULL;
static vector3ui noise_size;

void vnoise3d_init_file(const char *filename, vector3ui noise3d_size)
{
	IF_FAILED(filename);
	
	FILE *file = NULL;
	
	noise_size = noise3d_size;
	
	// существует файл с шумом ?...
	if((file = fopen(filename, "rb")) == NULL) {
		// ...файла нет, значит нужно создать
		ERROR_MSG("cannot open file %s with noise; creating it\n", filename);
		
		noise3d_data = (float*) malloc(sizeof(float) * noise_size.x*noise_size.y*noise_size.z);
		
		file = fopen(filename, "w");
		
		for(unsigned k = 0; k < noise_size.z; k++) {
			for(unsigned j = 0; j < noise_size.y; j++) {
				for(unsigned i = 0; i < noise_size.x; i++) {
					noise3d_data[i + j*noise_size.x + k*noise_size.x*noise_size.y] = 
							value_noise_3d(vec3f(i, j ,k));
				}
			}
		}
		fwrite((void*) noise3d_data, sizeof(float), noise_size.x*noise_size.y*noise_size.z, file);
		fclose(file);
	} else {
		unsigned size = noise3d_size.x*noise3d_size.y*noise3d_size.z;
		// ...файл есть, читаем данные
		noise3d_data = (float*) malloc(sizeof(float) * size);
		unsigned read_bytes = fread((void*) noise3d_data, sizeof(float), size, file);
		if(read_bytes*sizeof(float) != size) {
			ERROR_MSG("cannot read 3d noise data; file = %s", filename);
		}
		fclose(file);
	}
}

// трилинейная интерполяция значений
INLINE float trilerp(float v000, float v100, float v101, float v001,  
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

float vnoise3d_trilerp_file(vector3f p)
{
	#define _data(vx, vy, vz) noise3d_data[((vx) % noise_size.x) + ((vy) % noise_size.y)*noise_size.x + ((vz) % noise_size.z)*noise_size.x*noise_size.y]
	
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
	
	return trilerp(v000, v100, v101, v001, v010, v110, v111, v011, t);
}


////////////////////////////////// Perlin Noise - Шум Перлина

static int p[512];
static int permutation[] = 
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

INLINE static float grad(int hash, vector3f p)
{
	int h = hash & 15;
	float u = h < 8 ? p.x : p.y;
	float v = h < 4 ? p.y : h == 12 || h == 14 ? p.x : p.z;
	
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

void perlin_noise_init(void)
{
	for(int i = 0; i < 256; i++) {
		p[256 + i] = p[i] = permutation[i];
	}
}

float perlin_noise_1d(float x)
{
	return 0.0f;
}

float perlin_noise_2d(vector2f p)
{
	return 0.0f;
}

float perlin_noise_3d(vector3f pos)
{
	int x = (int) math_floorf(pos.x) & 255;
	int y = (int) math_floorf(pos.y) & 255;
	int z = (int) math_floorf(pos.z) & 255;
	
	pos.x -= math_floorf(pos.x);
	pos.y -= math_floorf(pos.y);
	pos.z -= math_floorf(pos.z);
	
	float u = fade(x), v = fade(y), w = fade(z);
	
	int A = p[x] + y, AA = p[A] + z, AB = p[A+1] + z;
	int B = p[x+1] + y, BA = p[B] + z, BB = p[B+1] + z;
	
	float result = math_lerp(w, math_lerp(v, math_lerp(u, grad(p[AA], vec3f(x, y, z)), 
										   grad(p[BA], vec3f(x-1.0f, y, z))),
						 math_lerp(u, grad(p[AB], vec3f(x, y-1.0f, z)),
										   grad(p[BB], vec3f(x-1.0f, y-1.0f, z)))),
						 math_lerp(v, math_lerp(u, grad(p[AA+1], vec3f(x, y, z-1.0f)), 
										   grad(p[BA+1], vec3f(x-1.0f, y, z-1.0f))),
						 math_lerp(u, grad(p[AB+1], vec3f(x, y-1.0f, z-1.0f)),
									       grad(p[BB+1], vec3f(x-1.0f, y-1.0f, z-1.0f)))));
			
	return result;
}
