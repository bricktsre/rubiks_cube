#version 120

attribute vec4 vPosition;
attribute vec4 vNormal;

varying vec4 color;
varying vec4 N;
varying vec4 L;
varying vec4 V;
varying float distance;

uniform mat4 ctm;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;
uniform int isShadow;
uniform vec4 sPosition;

void main()
{
	if(isShadow == 1){
		float x = (light_position[0]-light_position[1])*((light_position[0]-vPosition[0])/(light_position[1]-vPosition[1]));
		float y = 0.01;
		float z = (light_position[2]-light_position[1])*((light_position[2]-vPosition[2])/(light_position[1]-vPosition[1]));
		gl_Position = projection * model_view * ctm * vec4(x,y,z,1.0);
		N = projection * model_view * ctm * vNormal;
		L = projection * model_view * ctm * (light_position - vec4(x,y,z,1.0));
		vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
		V = eye_point - (projection * model_view * ctm * vec4(x,y,z,1.0));
		distance = length(L);
	}
	else{	gl_Position = projection * model_view * ctm * vPosition;
		N = projection * model_view * ctm * vNormal;
		L = projection * model_view * ctm * (light_position - vPosition);
		vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
		V = eye_point - (projection * model_view * ctm * vPosition);
		distance = length(L);
	}
}

