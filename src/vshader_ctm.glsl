#version 120

attribute vec4 vPosition;
attribute vec4 vNormal;
attribute vec4 vColor;

varying vec4 color;
varying vec4 N;
varying vec4 L;
varying vec4 V;
varying float distance;

uniform mat4 ctm;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;

void main()
{
		gl_Position = projection * model_view * ctm * vPosition;
		N = projection * model_view * ctm * vNormal;
		L = light_position - (projection * model_view * ctm * vPosition);
		vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
		V = eye_point - (projection * model_view * ctm * vPosition);
		distance = length(L);
		color = vColor;
}
