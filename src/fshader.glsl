#version 120

varying vec4 N;
varying vec4 L;
varying vec4 V;
varying float distance;

uniform vec4 ambient_product, diffuse_product, specular_product;
uniform float shininess;
uniform float attenuation_constant, attenuation_linear, attenuation_quadratic;

vec4 ambient, diffuse, specular;

void main()
{
	vec4 NN = normalize(N);
	vec4 VV = normalize(V);
	vec4 LL = normalize(L);
	ambient = ambient_product;
	vec4 H = normalize(LL + VV);
	diffuse = max(dot(LL, NN), 0.0) * diffuse_product;
	specular = pow(max(dot(NN, H), 0.0), shininess) * specular_product;
	float attenuation = 1/(attenuation_constant + (attenuation_linear * distance) + (attenuation_quadratic * distance * distance));
	gl_FragColor = ambient + attenuation * (diffuse + specular);
}
