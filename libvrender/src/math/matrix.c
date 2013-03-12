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

#include "math/vector.h"
#include "math/matrix.h"
#include "math/dmath.h"
#include <string.h>

void mat4(matrix4f out, float main_d)
{
	int i = 0, j = 0;
	
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			if(i == j)
				out[i][j] = main_d;
			else
				out[i][j] = 0.0f;
		}
	}

}

void mat3(matrix3f out, float main_d)
{
	int i = 0, j = 0;
	
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			if(i == j)
				out[i][j] = main_d;
			else
				out[i][j] = 0.0f;
		}
	}
}

void mat2(matrix2f out, float main_d)
{
	int i = 0, j = 0;
	
	for(i = 0; i < 2; i++) {
		for(j = 0; j < 2; j++) {
			if(i == j)
				out[i][j] = main_d;
			else
				out[i][j] = 0.0f;
		}
	}
}

void mat4_copy(matrix4f from, matrix4f to)
{
	memcpy((void*) to, (void*) from, 4*4*sizeof(float));
}

void mat3_copy(matrix3f from, matrix3f to)
{
	memcpy((void*) to, (void*) from, 3*3*sizeof(float));
}

void mat2_copy(matrix2f from, matrix2f to)
{
	memcpy((void*) to, (void*) from, 2*2*sizeof(float));
}

/*
void mat4_mult(matrix4 out, matrix4 m1, matrix4 m2)
{
	int i = 0, j = 0;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			out[i][j] = m1[i][0]*m2[0][j] + m1[i][1]*m2[1][j] + 
				m1[i][2]*m2[2][j] + m1[i][3]*m2[3][j];
		}
	}
}
*/

void mat4_mult2(matrix4f out, matrix4f m1, matrix4f m2)
{
	int i = 0, j = 0;
	matrix4f result;
	
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			result[i][j] = m1[i][0]*m2[0][j] + m1[i][1]*m2[1][j] + 
				m1[i][2]*m2[2][j] + m1[i][3]*m2[3][j];
		}
	}
	
	mat4_copy(result, out);
}

/*void mat4_mult2(matrix4 out, matrix4 m1, matrix4 m2)
{
	mat4_mult(out, m1, m2);
}*/

void mat4_mult3(matrix4f out, matrix4f m1, matrix4f m2, matrix4f m3)
{
	mat4_mult2(out, m1, m2);
	mat4_mult2(out, out, m3);
}

void mat3_mult3(matrix3f out, matrix3f m1, matrix3f m2, matrix3f m3)
{
	mat3_mult2(out, m1, m2);
	mat3_mult2(out, out, m3);
}

void mat4_mult4(matrix4f out, matrix4f m1, matrix4f m2, matrix4f m3, matrix4f m4)
{
	mat4_mult3(out, m1, m2, m3);
	mat4_mult2(out, out, m4);
}

/*
void mat4_mult5(matrix4 out, matrix4 m1, matrix4 m2, matrix4 m3, matrix4 m4, matrix4 m5)
{
	mat4_mult4(out, m1, m2, m3, m4);
	mat4_mult(out, out, m5);
}

void mat4_mult6(matrix4 out, matrix4 m1, matrix4 m2, matrix4 m3, matrix4 m4, matrix4 m5, matrix4 m6)
{
	mat4_mult5(out, m1, m2, m3, m4, m5);
	mat4_mult(out, out, m6);
}
*/

void mat4_mult_m(matrix4f m, matrix4f m2)
{
	mat4_mult2(m, m, m2);
}

void mat3_mult2(matrix3f out, matrix3f m1, matrix3f m2)
{
	int i = 0, j = 0;
	matrix3f result;

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			result[i][j] = m1[i][0]*m2[0][j] + m1[i][1]*m2[1][j] + 
				m1[i][2]*m2[2][j];
		}
	}
	
	mat3_copy(result, out);
}

void mat4_mult_c(matrix4f out, matrix4f m, float c)
{
	int i = 0, j = 0;
	
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			out[i][j] = m[i][j] * c;
		}
	}
}

void mat3_mult_c(matrix3f out, matrix3f m, float c)
{
	int i = 0, j = 0;
	
	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			out[i][j] = m[i][j] * c;
		}
	}
}

void mat4_mult_c_m(matrix4f m, float c)
{
	int i = 0, j = 0;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			m[i][j] = m[i][j] * c;
		}
	}
}

void mat3_mult_c_m(matrix3f m, float c)
{
	int i = 0, j = 0;

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			m[i][j] = m[i][j] * c;
		}
	}
}

vector4f mat4_mult_vec4(matrix4f m, vector4f v)
{
	return vec4f(v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0] + v.w*m[3][0], 
				v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1] + v.w*m[3][1],
				v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2] + v.w*m[3][2],
				v.x*m[0][3] + v.y*m[1][3] + v.z*m[2][3] + v.w*m[3][3]);
}

vector3f mat4_mult_vec3(matrix4f m, vector3f v)
{
	return vec3f_div_c( vec3f(v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0] + m[3][0], 
							v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1] + m[3][1],
							v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2] + m[3][2]),
				 (v.x*m[0][3] + v.y*m[1][3] + v.z*m[2][3] + m[3][3]) );
}

vector3f mat3_mult_vec3(matrix3f m, vector3f v)
{
	return vec3f(v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0], 
				 v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1],
				 v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2]);
}

void mat4_add(matrix4f out, matrix4f m1, matrix4f m2)
{
	int i = 0, j = 0;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			out[i][j] = m1[i][j] + m2[i][j];	
		}
	}
}

void mat3_add(matrix3f out, matrix3f m1, matrix3f m2)
{
	int i = 0, j = 0;

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			out[i][j] = m1[i][j] + m2[i][j];	
		}
	}
}

void mat4_sub(matrix4f out, matrix4f m1, matrix4f m2)
{
	int i = 0, j = 0;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			out[i][j] = m1[i][j] - m2[i][j];	
		}
	}
}

void mat3_sub(matrix3f out, matrix3f m1, matrix3f m2)
{
	int i = 0, j = 0;

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			out[i][j] = m1[i][j] - m2[i][j];	
		}
	}
}

void mat4_transponse(matrix4f m)
{
	int i = 0, j = 0;
	matrix4f result;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			result[i][j] = m[j][i];	
		}
	}

	mat4_copy(result, m);
}

void mat3_transponse(matrix3f m)
{
	int i = 0, j = 0;
	matrix3f result;

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			result[i][j] = m[j][i];	
		}
	}

	mat3_copy(result, m);
}

void mat4_submat(matrix3f out, int i, int j, matrix4f m)
{
	int in = 0, jn = 0, ifound = 0, jfound = 0;
	matrix3f result;

	for(in = 0; in < 3; in++) {
		if(in == i)
			ifound = 1;

		for(jn = 0; jn < 3; jn++) {
			if(jn == j)
				jfound = 1;

			result[in][jn] = m[in+ifound][jn+jfound];	
		}

		jfound = 0;
	}

	mat3_copy(result, out);
}

void mat3_submat(matrix2f out, int i, int j, matrix3f m)
{
	int in = 0, jn = 0, ifound = 0, jfound = 0;
	matrix2f result;

	for(in = 0; in < 2; in++) {
		if(in == i)
			ifound = 1;

		for(jn = 0; jn < 2; jn++) {
			if(jn == j)
				jfound = 1;

			result[in][jn] = m[in+ifound][jn+jfound];	
		}

		jfound = 0;
	}

	mat2_copy(result, out);
}

float mat2_det(matrix2f m)
{
	return (m[0][0]*m[1][1] - m[0][1]*m[1][0]);
}

/*
float mat3_det(matrix3 m)
{
	int i = 0, sign = 0;
	float det = 0.0f, temp_det = 0.0f;
	matrix2 temp_m1;

	for(i = 0; i < 3; i++) {
		mat3_submat(temp_m1, 0, i, m);
		temp_det = mat2_det(temp_m1);
		sign = ( ( (i+2) % 2 == 0 ) ? 1 : -1 );
		
		det += sign*m[0][i]*temp_det;
	}

	return det;
}
*/

float mat3_det(matrix3f m)
{
	return ( m[0][0]*m[1][1]*m[2][2] + m[0][1]*m[1][2]*m[2][0] +
			 m[1][0]*m[2][1]*m[0][2] - m[0][2]*m[1][1]*m[2][0] -
			 m[0][0]*m[1][2]*m[2][1] - m[1][0]*m[0][1]*m[2][2] );
}

float mat4_det(matrix4f m)
{
	int i = 0, sign = 0;
	float det = 0.0f, temp_det = 0.0f;
	matrix3f temp_m1;

	for(i = 0; i < 4; i++) {
		mat4_submat(temp_m1, 0, i, m);
		temp_det = mat3_det(temp_m1);
		sign = ( ( (i+2) % 2 == 0 ) ? 1 : -1 );
		
		det += sign*m[0][i]*temp_det;
	}

	return det;
}

int mat4_inverse(matrix4f out, matrix4f m)
{
	int i = 0, j = 0, sign = 0;
	matrix3f submat;
	float temp_det = 0.0f, m_det = 0.0f;
	
	mat4(out, 1.0f);
	m_det = mat4_det(m);
	
	if(math_fabs(m_det) < 0.0005f)
		return 0;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			mat4_submat(submat, j, i, m);
			temp_det = mat3_det(submat);
			sign = ( (i+j) % 2 == 0 ? 1 : -1 );

			out[i][j] = (sign * temp_det) / m_det;
		}
	}
	
	return 1;
}

int mat3_inverse(matrix3f out, matrix3f m)
{
	int i = 0, j = 0, sign = 0;
	matrix2f submat;
	float temp_det = 0.0f, m_det = 0.0f;
	
	m_det = mat3_det(m);

	if(math_fabs(m_det) < 0.0005f)
		return 0;

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 3; j++) {
			mat3_submat(submat, j, i, m);
			temp_det = mat2_det(submat);
			sign = ( (i+j) % 2 == 0 ? 1 : -1 );

			out[i][j] = (sign * temp_det) / m_det;
		}
	}
	
	return 1;
}

void mat4_translate(matrix4f out, float x, float y, float z)
{
	mat4(out, 1.0f);

	out[0][3] = x;
	out[1][3] = y;
	out[2][3] = z;
}

void mat4_scale(matrix4f out, float x, float y, float z)
{
	mat4(out, 1.0f);

	out[0][0] = x;
	out[1][1] = y;
	out[2][2] = z;
}

void mat4_rotate_x(matrix4f out, float angle)
{
	float rad = DEG_TO_RAD(angle);
	float sinA = math_sinf(rad), cosA = math_cosf(rad);
	mat4(out, 1.0f);

	out[1][1] = cosA;
	out[1][2] = -sinA;
	out[2][1] = sinA;
	out[2][2] = cosA;
}

void mat4_rotate_y(matrix4f out, float angle)
{
	float rad = DEG_TO_RAD(angle);
	float sinA = math_sinf(rad), cosA = math_cosf(rad);
	mat4(out, 1.0f);

	out[0][0] = cosA;
	out[0][2] = -sinA;
	out[2][0] = sinA;
	out[2][2] = cosA;
}

void mat4_rotate_z(matrix4f out, float angle)
{
	float rad = DEG_TO_RAD(angle);
	float sinA = math_sinf(rad), cosA = math_cosf(rad);
	mat4(out, 1.0f);

	out[0][0] = cosA;
	out[0][1] = -sinA;
	out[1][0] = sinA;
	out[1][1] = cosA;
}

void mat4_rotate_axis(matrix4f out, float angle, vector3f axis)
{
	float rad = DEG_TO_RAD(angle);
	float c = math_cosf(rad), s = math_sinf(rad);
	vector3f v = vec3f_norm(axis);
	mat4(out, 1.0f);
	
	out[0][0] = c + v.x*v.x*(1 - c);
	out[0][1] = v.x*v.y*(1 - c) - v.z*s;
	out[0][2] = v.x*v.z*(1 - c) + v.y*s;

	out[1][0] = v.y*v.x*(1 - c) + v.z*s;
	out[1][1] = c + v.y*v.y*(1 - c);
	out[1][2] = v.y*v.z*(1 - c) - v.x*s;

	out[2][0] = v.z*v.x*(1 - c) - v.y*s;
	out[2][1] = v.z*v.y*(1 - c) + v.x*s;
	out[2][2] = c + v.z*v.z*(1 - c);
}

void mat4_ortho(matrix4f out, float left, 
				  float right, float bottom, 
				  float top,   float z_near, 
				  float z_far)
{
	mat4(out, 1.0f);

	if(left == right || top == bottom || z_near == z_far)
		return;

	out[0][0] = 2.0f / (right - left);
	out[0][3] = (right + left) / (right - left);
	
	out[1][1] = 2.0f / (top - bottom);
	out[1][3] = (top + bottom) / (top - bottom);

	out[2][2] = -2.0f / (z_far - z_near);
	out[2][3] = (z_far + z_near) / (z_far - z_near);

	out[3][3] = 1.0f;
}

void mat4_frustum(matrix4f out, float left, 
				  float right, float bottom, 
				  float top,   float z_near, 
				  float z_far)
{
	mat4(out, 1.0f);

	if(left == right || top == bottom || z_near == z_far)
		return;

	out[0][0] = 2.0f * z_near / (right - left);
	out[0][2] = -((right + left) / (right - left));
	
	out[1][1] = 2.0f * z_near / (top - bottom);
	out[1][2] = (top + bottom) / (top - bottom);

	out[2][2] = -(z_far + z_near) / (z_far - z_near);
	out[2][3] = -2.0f * z_far * z_near / (z_far - z_near);

	out[3][2] = -1.0f;
}

// Базовое преобразование вида. Без перемещения камеры
static void mat4_view_basic_transform(matrix4f out, vector3f forward_dir, vector3f up_dir)
{
	matrix4f m;
	
	mat4(out, 1.0f);
	mat4(m, 1.0f);

	vector3f f = vec3f_norm(forward_dir);
	vector3f l = vec3f_norm(vec3f_cross(up_dir, f));
	vector3f u = vec3f_norm(vec3f_cross(f, l));
	
	/*m[0][0] = l.x;
	m[1][0] = l.y;
	m[2][0] = l.z;

	m[0][1] = u.x;
	m[1][1] = u.y;
	m[2][1] = u.z;

	m[0][2] = f.x;
	m[1][2] = f.y;
	m[2][2] = f.z;*/
	
	
	
	m[0][0] = l.x;
	m[0][1] = l.y;
	m[0][2] = l.z;

	m[1][0] = u.x;
	m[1][1] = u.y;
	m[1][2] = u.z;

	m[2][0] = f.x;
	m[2][1] = f.y;
	m[2][2] = f.z;
	
	mat4_copy(m, out);
}


void mat4_lookat(matrix4f out, vector3f eye_position, 
				 vector3f target_point, vector3f up_dir)
{
	matrix4f m, translate;
	mat4(out, 1.0f);

	mat4_view_basic_transform(m, vec3f_sub(eye_position, target_point), up_dir);
	
	mat4_translate(translate, -eye_position.x, -eye_position.y, -eye_position.z);
	mat4_mult2(out, m, translate);
}

void mat4_view(matrix4f out, vector3f eye_position, 
				 vector3f target_dir, vector3f up_dir)
{
	matrix4f m, translate;
	mat4(out, 1.0f);

	mat4_view_basic_transform(m, vec3f_mult_c(target_dir, -1.0f), up_dir);
	
	mat4_translate(translate, -eye_position.x, -eye_position.y, -eye_position.z);
	mat4_mult2(out, m, translate);
}

void mat4_perspective(matrix4f out, float fov, float aspect, float z_near, float z_far)
{
	float left = 0.0f, right = 0.0f, top = 0.0f, bottom = 0.0f;

	top = math_tanf(fov * MATH_PI / 360.0f) * z_near;
	bottom = -top;
	left = aspect * bottom;
	right = aspect * top;

	mat4_frustum(out, left, right, bottom, top, z_near, z_far);
}

void mat4_rotate_axis_mult(matrix4f m, float angle, vector3f axis)
{
	matrix4f rotate;
	
	mat4_rotate_axis(rotate, angle, axis);
	mat4_mult_m(m, rotate);
}

void mat4_frustum_mult(matrix4f m, float left, 
				  float right, float bottom, 
				  float top,   float z_near, 
				  float z_far)
{
	matrix4f frustum;
	
	mat4_frustum(frustum, left, right, bottom, top, z_near, z_far);
	mat4_mult_m(m, frustum);
}

void mat4_ortho_mult(matrix4f m, float left, 
				  float right, float bottom, 
				  float top,   float z_near, 
				  float z_far)
{
	matrix4f ortho;
	
	mat4_ortho(ortho, left, right, bottom, top, z_near, z_far);
	mat4_mult_m(m, ortho);
}

void mat4_lookat_mult(matrix4f m, vector3f eye_position, 
				 vector3f target_point, vector3f up_dir)
{
	matrix4f lookat;
	
	mat4_lookat(lookat, eye_position, target_point, up_dir);
	mat4_mult_m(m, lookat);
}

void mat4_view_mult(matrix4f m, vector3f eye_position, 
				 vector3f target_dir, vector3f up_dir)
{
	matrix4f view;
	
	mat4_view(view, eye_position, target_dir, up_dir);
	mat4_mult_m(m, view);
}

void mat4_perspective_mult(matrix4f m, float fov, float aspect, float z_near, float z_far)
{
	matrix4f perspective;
	
	mat4_perspective_mult(perspective, fov, aspect, z_near, z_far);
	mat4_mult_m(m, perspective);
}

void mat4_print(matrix4f m)
{
	int i = 0, j = 0;

	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			printf("%.2f ", m[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
