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

/* Классический Шум Перлина */
void perlin_noise_init(void);
//float perlin_noise_1d(float x);
//float perlin_noise_2d(vector2f p);
float perlin_noise_3d(vector3f p);

/* Численный шум */
float value_noise_1d(float x);
float value_noise_2d(vector2f p);
float value_noise_3d(vector3f p);


/**
 * Инициализировать численный шум из файла, если файл не существует, 
 * то он будет создан и проинициализирован шумом 
 */
void vnoise3d_init_file(const char *filename, vector3ui noise_size);

/* Трилинейная интерполяция значений численного шума из файла */
float vnoise3d_trilerp_file(vector3f p);

#ifdef __cplusplus
}
#endif

#endif // NOISE_H
