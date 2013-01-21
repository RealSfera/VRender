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

#include "math/quaternion.h"
#include "math/vector.h"
#include "math/matrix.h"
#include "math/dmath.h"

quaternion_slerp_t quat_slerp_init_from_unit(quaternion q1, quaternion q2)
{
	quaternion_slerp_t result;
	float cos_omega = 0.0f, inv_sin_omega = 0.0f;
	
	cos_omega = quat_inner_product(q1, q2);
	
	if(cos_omega < 0.0f) {
		cos_omega = -cos_omega;
		q2 = quat_init(-q2.x, -q2.y, -q2.z, -q2.w);
	}
	
	if(cos_omega > 0.9999f)
		cos_omega = 0.9999f;
	
	result.omega = math_acosf(cos_omega);
	
	inv_sin_omega = 1.0f / math_sinf(result.omega);
	
	result.q1 = quat_mult_c(q1, inv_sin_omega);
	result.q2 = quat_mult_c(q2, inv_sin_omega);
	
	return result;
}

quaternion quat_slerp(quaternion_slerp_t *lerp, float t)
{
	if(t <= 0.0f)
		return quat_mult_c(lerp->q1, math_sinf(lerp->omega));
	else if(t >= 1.0f)
		return quat_mult_c(lerp->q2, math_sinf(lerp->omega));
		
	return quat_add( quat_mult_c(lerp->q1, math_sinf( (1.0f - t)*lerp->omega )), 
					 quat_mult_c(lerp->q2, math_sinf( t*lerp->omega )) );
}

quaternion_lerp_t quat_lerp_init_from_unit(quaternion q1, quaternion q2)
{
	float inner = 0.0f;
	quaternion_lerp_t result;
	
	inner = quat_inner_product(q2, q1);
	
	if(inner < 0.0f)
		q2 = quat_init(-q2.x, -q2.y, -q2.z, -q2.w);
		
	q2 = quat_sub(q2, q1);
	
	result.q1 = q1;
	result.q2 = q2;
	
	return result;
}

// qq' = (vv' + wv' + w'v, ww' - v.v')
quaternion quat_mult(quaternion q1, quaternion q2)
{
	vector3f v_cross, v_q1, v_q2;
	float v_dot = 0.0f;
	quaternion result;
	
	v_q1 = quat_to_vec3(q1); // v 
	v_q2 = quat_to_vec3(q2); // v'
	v_cross = vec3f_cross(v_q1, v_q2); // vv'
	v_dot 	= vec3f_dot(v_q1, v_q2); // v.v'

	result =  quat_init_vec3( 
				vec3f_add(
					vec3f_add(v_cross, vec3f_mult_c(v_q2, q1.w)), // vv' + wv'
					vec3f_mult_c(v_q1, q2.w) // w'v
				), (q1.w*q2.w - v_dot) // ww' - v.v'
			);
	return quat_normalize(result);
}

vector3f quat_mult_vec3(quaternion q, vector3f v)
{
	quaternion vec_q, res_q;
	v = vec3f_norm(v);
	
	vec_q = quat_init(v.x, v.y, v.z, 0.0f);
	
	res_q = quat_mult(vec_q, quat_conjugate(q));
	res_q = quat_mult(q, res_q);
	
	return quat_to_vec3(res_q);
}

void quat_to_matrix4(matrix4f out, quaternion q)
{
	float x, y, z, w, xx, zz, yy, xy, xz, xw, yz, zw, yw;
	
	x = q.x; y = q.y; z = q.z; w = q.w;

	xx = x * x; zz = z * z; yy = y * y;
	xy = x * y; xz = x * z; yz = y * z;
	xw = x * w; zw = z * w; yw = y * w;

	mat4(out, 1.0f);

	out[0][0] = 1.0f - 2.0f * (yy + zz);
	out[0][1] = 2.0f * (xy - zw);
	out[0][2] = 2.0f * (xz + yw);
	
	out[1][0] = 2.0f * (xy + zw);
	out[1][1] = 1.0f - 2.0f * (xx + zz);
	out[1][2] = 2.0f * (yz - xw);

	out[2][0] = 2.0f * (xz - yw);
	out[2][1] = 2.0f * (yz + xw);
	out[2][2] = 1.0f - 2.0f * (xx + yy);
}

quaternion quat_from_matrix4(matrix4f m)
{
	float T = 0.0f, s, x, y, z, w;

	T = m[0][0] + m[1][1] + m[2][2] + 1;

	if(T > 0.0f) {
		s = 0.5f / math_sqrtf(T);
		x = (m[2][1] - m[1][2]) * s;
		y = (m[0][2] - m[2][0]) * s;
		z = (m[1][0] - m[0][1]) * s;
		w = 0.25f / s;
	} else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2])) {
		s = math_sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]) * 2;
		x = 0.5f / s;
		y = (m[0][1] + m[1][0]) / s;
		z = (m[0][2] + m[2][0]) / s;
		w = (m[1][2] + m[2][1]) / s;
	} else if (m[1][1] > m[2][2]) {
		s = math_sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]) * 2;
		x = (m[0][1] + m[1][0]) / s;
		y = 0.5f / s;
		z = (m[1][2] + m[2][1]) / s;
		w = (m[0][2] + m[2][0]) / s;
	} else {
		s = math_sqrtf(1.0f + m[2][2] - m[0][0] - m[1][1]) * 2;
		x = (m[0][2] + m[2][0]) / s;
		y = (m[1][2] + m[2][1]) / s;
		z = 0.5f / s;
		w = (m[0][1] + m[1][0]) / s;
	}
	
	return quat_init(x, y, z, w);
}

quaternion quat_from_rotate_axis(float angle, vector3f axis)
{
	float rad = DEG_TO_RAD(angle);
	float sin_a = math_sinf(rad / 2.0f), cos_a = math_cosf(rad / 2.0f);
	quaternion result;
	
	result.x = axis.x * sin_a;
	result.y = axis.y * sin_a;
	result.z = axis.z * sin_a;
	result.w = cos_a;
	
	return quat_normalize(result);
}

void quat_to_rotate_axis(quaternion q, float *angle, vector3f *axis)
{
	quaternion q_n = quat_normalize(q);
	float cos_a = q_n.w;
	float sin_a = math_sqrtf(1.0f - cos_a * cos_a);
	*angle = ((math_acosf(cos_a) * 2) * 180.0f ) / MATH_PI;
	
	if(math_fabs(sin_a) < 0.0005f)
		sin_a = 1.0f;
	
	axis->x = q_n.x / sin_a;
	axis->y = q_n.y / sin_a;
	axis->z = q_n.z / sin_a;
}
