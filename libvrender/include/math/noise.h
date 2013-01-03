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

#ifndef NOISE_H
#define NOISE_H

#include "math/vector.h"
#include "math/dmath.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Инициализация шумов */
void noise_init(void);

/* Классический Шум Перлина */
float perlin_noise_3d(vector3f pos);

/* Simplex Noise */
float simplex_noise_2d(vector2f pos);
float simplex_noise_3d(vector3f pos);

/* Численный шум */
float value_noise_1d(float x);
float value_noise_2d(vector2f p);
float value_noise_3d(vector3f p);


/**
 * Инициализировать симплекс шум из файла, если файл не существует, 
 * то он будет создан и проинициализирован шумом 
 */
void snoise3d_init_file(const char *filename, vector3ui snoise_size);

/* Косинусная 3D интерполяция значений симплекс шума из файла */
float snoise3d_tricerp_file(vector3f p);

#ifdef __cplusplus
}
#endif

#endif // NOISE_H
