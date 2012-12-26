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

#ifndef MARCHING_CUBES_H
#define MARCHING_CUBES_H

#include "common.h"
#include "math/vector.h"

typedef struct {
	vector3f vertices_positions[8];
	float vertices_values[8];
} cell_t;

typedef struct {
	unsigned int indices[3];
} triangle_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Полигонизировать одну ячейку алгоритмом Marching Cubes
 */
int marching_cubes_polygonise(cell_t cell, float isolevel, vector3f *out_vertices, 
							  unsigned short *number_of_vertices, triangle_t *out_triangles, 
							  unsigned short *number_of_triangles);
/* 
 * Полигонизировать volume с размером volume_size.
 * grid_size - размер сетки
 * isolevel - изо-значение в скалярном поле volume на основе котрого проиходит полигонизация
 * out_vertices, out_triangles -  выходные вершины и треугольники (индексы)
 */
int marching_cubes_create(const float *volume, vector3ui volume_size, vector3ui grid_size, 
						  float isolevel, vector3f *out_vertices, unsigned int *number_of_vertices, 
						  triangle_t *out_triangles, unsigned int *number_of_triangles);
/* 
 * Полигонизировать volume с размером volume_size.
 * grid_size - размер сетки
 * isolevel - изо-значение в скалярном поле volume на основе котрого проиходит полигонизация
 * vertex_vbo, index_vbo - вершинные буферы, куда нужно загрузить данные
 */
int marching_cubes_create_vbos(const float *volume, vector3ui volume_size, 
							  vector3ui grid_size, float isolevel,
							  GLuint vertex_vbo, GLuint index_vbo);

#ifdef __cplusplus
}
#endif

#endif // MARCHING_CUBES_H
