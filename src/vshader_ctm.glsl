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
//uniform int isShadow;

void main()
{
	/*if(isShadow == 1){
		vec4 tempV = ctm * vPosition;
		float x = light_position.x-(light_position.y*(light_position.x-tempV.x)/(light_position.y-tempV.y));
		float y = 0.01;
		float z = light_position.z-(light_position.y*(light_position.z-tempV.z)/(light_position.y-tempV.y));
		gl_Position = projection * model_view * vec4(x,y,z,1.0);
		N = projection * model_view * ctm * vNormal;
		L = projection * model_view * (light_position - vec4(x,y,z,1.0));
		vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
		V = eye_point - (projection * model_view * vec4(x,y,z,1.0));
		/*gl_Position = model_view * vec4(x,y,z,1.0);
		N = projection * model_view * ctm * vNormal;
		L = model_view * (light_position - vec4(x,y,z,1.0));
		vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
		V = eye_point - (model_view * vec4(x,y,z,1.0));
		distance = length(L);
		color = vColor;
	}else{*/
		gl_Position = projection * model_view * ctm * vPosition;
		N = projection * model_view * ctm * vNormal;
		L = light_position - (projection * model_view * ctm * vPosition);
		vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
		V = eye_point - (projection * model_view * ctm * vPosition);
		distance = length(L);
		color = vColor;
	//}
}

