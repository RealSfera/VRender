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

#include "common.h"

const char *main_shader_source = 
STRINGIFY(
			
\n!!vs \n
\n \
#version 120 \n
\n
attribute vec3 position; \n
\n
varying vec3 fragment_l; \n
varying vec3 fragment_v; \n
varying vec3 fragment_h; \n
varying vec3 fragment_position; \n
\n
uniform mat4 model; \n
uniform mat4 view; \n
uniform mat4 projection; \n
uniform vec3 light_position; \n
uniform vec3 viewer_position; \n
\n
void main(void)  \n
{ \n
   vec3 pos = position; \n
   \n
   fragment_position = position; \n
   \n
   pos = vec3(model * vec4(pos, 1.0)); \n
   \n
   fragment_l = normalize(light_position - pos); \n
   fragment_v = normalize(viewer_position - pos); \n
   \n
   vec3 l_v = fragment_l + fragment_v; \n
   fragment_h = normalize(l_v); \n
   \n
   gl_Position = projection * view * model *vec4(position, 1.0); \n
} \n
\n
\n!!fs \n
\n \
#version 120 \n
\n
varying vec3 fragment_l; \n
varying vec3 fragment_v; \n
varying vec3 fragment_h; \n
varying vec3 fragment_position; \n
\n
uniform sampler3D volume_texture; \n
uniform vec3 volume_step; \n
\n
uniform vec3 material_front_color; \n
uniform vec3 material_back_color; \n
uniform vec3 light_color; \n
uniform vec3 light_spec_color; \n
uniform float material_shininess; \n
uniform float coef_ambient; \n
uniform float coef_diffuse; \n
uniform float coef_specular; \n
uniform float coef_gamma; \n
uniform mat3 model_inv; \n
\n
void main(void) { \n
   \n
   vec4 color = vec4(0.0, 0.0, 0.0, 1.0); \n
   vec3 material_color = vec3(0.0, 0.0, 0.0); \n
   \n
   ////////////// Расчет освещения
   \n
   vec3 gradient = vec3( \n
				   (texture3D(volume_texture, fragment_position+vec3(volume_step.x, 0.0, 0.0)).r -  \n
				   texture3D(volume_texture, fragment_position-vec3(volume_step.x, 0.0, 0.0)).r) / 2.0 * volume_step.x,  \n
				   (texture3D(volume_texture, fragment_position+vec3(0.0, volume_step.y, 0.0)).r -  \n
				   texture3D(volume_texture, fragment_position-vec3(0.0, volume_step.y, 0.0)).r) / 2.0 * volume_step.y,  \n
				   (texture3D(volume_texture, fragment_position+vec3(0.0, 0.0, volume_step.z)).r -  \n
				   texture3D(volume_texture, fragment_position-vec3(0.0, 0.0, volume_step.z)).r) / 2.0 * volume_step.z ); \n
   \n
   vec3 n = normalize(gradient) * model_inv; \n
   \n
   if(!gl_FrontFacing) { \n
		n = -n; \n
		material_color = material_back_color; \n
   } else { \n
		material_color = material_front_color; \n
   } \n
   \n
   vec3 l = normalize(fragment_l); \n
   vec3 h = normalize(fragment_h); \n
   vec3 v = normalize(fragment_v); \n
   \n
   //////// Сферические гармоники
   \n
   //// Коэффициенты для с. г.
   const float c1 = 0.429043; \n
   const float c2 = 0.511664; \n
   const float c3 = 0.743125; \n
   const float c4 = 0.886227; \n
   const float c5 = 0.247708; \n
   const vec3 L00  = vec3( 0.871297,  0.875222,  0.864470); \n
   const vec3 L1m1 = vec3( 0.175058,  0.245335,  0.312891); \n
   const vec3 L10  = vec3( 0.034675,  0.036107,  0.037362); \n
   const vec3 L11  = vec3(-0.004629, -0.029448, -0.048028); \n
   const vec3 L2m2 = vec3(-0.120535, -0.121160, -0.117507); \n
   const vec3 L2m1 = vec3( 0.003242,  0.003624,  0.007511); \n
   const vec3 L20  = vec3(-0.028667, -0.024926, -0.020998); \n
   const vec3 L21  = vec3(-0.077539, -0.086325, -0.091591); \n
   const vec3 L22  = vec3(-0.161784, -0.191783, -0.219152); \n
   \n
   vec3 sh_light = c1 * L22 * (n.x*n.x - n.y*n.y) + \n
				   c3 * L20 * n.z*n.z + \n
				   c4 * L00 - c5 * L20 + \n
				   2.0 * c1 * (L2m2*n.x*n.y + L21*n.x*n.z + L2m1*n.y*n.z) + \n
				   2.0 * c2 * (L11*n.x + L1m1*n.y + L10*n.z); \n
   ////////
   \n
   // ambient
   vec4 ambient = vec4(sh_light, 1.0) * coef_ambient; \n
   \n
   // diffuse
   vec4 diffuse = vec4( max(0.0, dot(n, l)) ) * coef_diffuse; \n
   \n
   // specular
   vec4 specular = vec4( pow(max(0.0, dot(h, n)), material_shininess) ) * coef_specular; \n
   \n
   \n
   color += vec4(material_color, 1.0) * diffuse * vec4(light_color, 1.0); \n
   color += vec4(material_color, 1.0) * ambient; \n
   color += specular * vec4(light_spec_color, 1.0); \n
   \n\n
   ////////////// Коррекция гаммы
   \n
   vec4 gamma_corrected = vec4(pow(color.rgb, vec3(1.0 / coef_gamma)), color.a); \n
   \n
   color = gamma_corrected; \n
   \n
   //////////////
			
   gl_FragColor = color;
}

);
