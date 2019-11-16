#version 120

attribute vec4 vPosition;
attribute vec4 vNormal;

varying vec4 color;
varying vec4 N;
varying vec4 L;
varying vec4 V;
varying float distance;

uniform vec4 ambient_product, diffuse_product, specular_product;
uniform float shininess;
uniform float attenuation_constant, attenuation_linear, attenuation_quadratic;

vec4 ambient, diffuse, specular;
uniform mat4 ctm;
uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 light_position;

void main()
{
	gl_Position = projection * model_view * ctm * vPosition;
	/*N = projection * model_view * vNormal;
	L = projection * model_view * (light_position - vPosition);
	vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
	V = eye_point - (projection * model_view * vPosition);
	distance = length(L);
	*/ambient = ambient_product;
	
	vec4 N = normalize(projection * model_view * vNormal);
	vec4 L_temp = projection * model_view * (light_position - vPosition);
	vec4 L = normalize(L_temp);
	diffuse = max(dot(L,N), 0.0) * diffuse_product;
	vec4 eye_point = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 V = normalize(eye_point - (projection * model_view * vPosition));
	vec4 H = normalize(L + V);
	specular = pow(max(dot(N, H), 0.0), shininess) * specular_product;
	float distance = length(L_temp);
	float attenuation = 1/(attenuation_constant + (attenuation_linear * distance) +
			(attenuation_quadratic * distance * distance));
	color = ambient + (attenuation * (diffuse + specular));
}

