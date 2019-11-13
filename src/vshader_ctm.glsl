#version 120

attribute vec4 vPosition;
attribute vec2 vNormal;

varying vec4 N;
varying vec4 L;
varying vec4 V;
varying float distance;

uniform mat4 ctm;
uniform mat4 model_view_matrix;
uniform mat4 projection_matrix;
uniform mat4 light_position;

void main()
{
	gl_Position = projection_matrix * model_view_matrix * ctm * vPosition / vPosition.w;
	N= projection_matrix * model_view_matrix * vNormal;
	L = light_position - (projection_matrix * model_view * vPosition);
	vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
	V = eye_point - (projection * model_view * vPosition);
	distance = length(l);
}

