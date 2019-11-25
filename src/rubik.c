/*
 * triangle.c
 *
 *  Created on: Aug 28, 2017
 *      Author: Thumrongsak Kosiyatrakul
 */


#ifdef __APPLE__  // include Mac OS X verions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#endif  // __APPLE__

#include "initShader.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrixOperations.h"
#include "track.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

typedef GLfloat vec4[4];
typedef GLfloat mat4[16];
typedef GLfloat vec2[2];

typedef struct {
	vec4 reflect_ambient;
	vec4 reflect_diffuse;
	vec4 reflect_specular;
	GLfloat shininess;
} material;

material cube_materials[5] = {
	{{1.0, 0.0, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 25},
	{{0.0, 1.0, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 25},
	{{0.0, 0.0, 1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 25},
	{{1.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 25},
	{{1.0, 0.5, 0.0, 1.0}, {1.0, 0.5, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0}, 25}};

material other_materials[3] = {
	{{0,0.4,0.0,1.0},{0,0.4,0,1.0},{0,0.4,0,1.0},1},
	{{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},{1.0,1.0,1.0,1.0},1},
	{{0,0.35,0.0,1.0},{0.0,0,0.0,1.0},{0,0,0,1.0},100}
};

vec4 light_diffuse = {1.0,1.0,1.0,1.0};
vec4 light_specular = {1.0,1.0,1.0,1.0};
vec4 light_ambient = {0.3, 0.3, 0.3, 1.0};
GLfloat attenuation_constant = 0.5, attenuation_linear=0.1, attenuation_quadratic=0.1, shininess=1.0;
GLuint att_const_location, att_lin_location, att_quad_location, shininess_location, shadow_location;
GLuint amb_prod_location, diff_prod_location, spec_prod_location, light_location;

int num_vertices = 0, animate = 0;;
GLuint model_view_location, projection_location, ctm_location;
mat4 model_view, projection, ctm;
mat4 ball_ctms[5];
vec4 lrb, tnf;
vec4 light_position = {0,3,0,1.0};
GLfloat theta = M_PI/2, phi = M_PI/4, radius = 3.0;

vec4 cube_vertices[60] = {
	{0.1,0.9,0.0,1.0},{0.1,0.1,0.0,1.0},{0.9,0.1,0.0,1.0},
	{0.1,0.9,0.0,1.0},{0.9,0.1,0.0,1.0},{0.9,0.9,0.0,1.0},
	{1.0,0.9,-0.1,1.0},{1.0,0.1,-0.1,1.0},{1.0,0.1,-0.9,1.0},
	{1.0,0.9,-0.1,1.0},{1.0,0.1,-0.9,1.0},{1.0,0.9,-0.9,1.0},
	{0.9,0.9,-1.0,1.0},{0.9,0.1,-1.0,1.0},{0.1,0.1,-1.0,1.0},
	{0.9,0.9,-1.0,1.0},{0.1,0.1,-1.0,1.0},{0.1,0.9,-1.0,1.0},
	{0.0,0.9,-0.9,1.0},{0.0,0.1,-0.9,1.0},{0.0,0.1,-0.1,1.0},
	{0.0,0.9,-0.9,1.0},{0.0,0.1,-0.1,1.0},{0.0,0.9,-0.1,1.0},
	{0.1,1.0,-0.9,1.0},{0.1,1.0,-0.1,1.0},{0.9,1.0,-0.1,1.0},
	{0.1,1.0,-0.9,1.0},{0.9,1.0,-0.1,1.0},{0.9,1.0,-0.9,1.0},
	{0.1,0.0,-0.1,1.0},{0.1,0.0,-0.9,1.0},{0.9,0.0,-0.9,1.0},
	{0.1,0.0,-0.1,1.0},{0.9,0.0,-0.9,1.0},{0.9,0.0,-0.1,1.0},
	{0.0,0.1,-0.1,1.0},{0.1,0.0,-0.1,1.0},{0.1,0.1,0.0,1.0},
	{0.1,0.9,0.0,1.0},{0.1,1.0,-0.1,1.0},{0.0,0.9,-0.1,1.0},
	{0.9,0.1,0.0,1.0},{0.9,0.0,-0.1,1.0},{1.0,0.1,-0.1,1.0},
	{1.0,0.9,-0.1,1.0},{0.9,1.0,-0.1,1.0},{0.9,0.9,0.0,1.0},
	{0.1,0.1,-1.0,1.0},{0.1,0.0,-0.9,1.0},{0.0,0.1,-0.9,1.0},
	{0.0,0.9,-0.9,1.0},{0.1,1.0,-0.9,1.0},{0.1,0.9,-1.0,1.0},
	{1.0,0.1,-0.9,1.0},{0.9,0.0,-0.9,1.0},{0.9,0.1,-1.0,1.0},
	{0.9,0.9,-1.0,1.0},{0.9,1.0,-0.9,1.0},{1.0,0.9,-0.9,1.0}
};

void makeCube(vec4 *vertices, vec4 *normals, int *v_index, int *n_index){
	int i;
	mat4 trans, scale, copy;
	matrixTranslation(0,0,0,trans);
	matrixScale(1,1,1,scale);
	matrixMultiplication(trans,scale,copy);
	vec4 temp;
	for(i = 0; i<60; i+=3){
		matrixVectorMultiplication(copy,cube_vertices[i],temp);
		vectorCopy(temp, vertices[*v_index]);
		matrixVectorMultiplication(copy,cube_vertices[i+1],temp);
		vectorCopy(temp, vertices[(*v_index)+1]);
		matrixVectorMultiplication(copy,cube_vertices[i+2],temp);
		vectorCopy(temp, vertices[(*v_index)+2]);

		vec4 t1,t2,t3;
		vectorSubtraction(vertices[(*v_index)+1],vertices[*v_index],t1);
		vectorSubtraction(vertices[(*v_index)+2],vertices[*v_index],t2);
		vectorCrossProduct(t1,t2,t3);
		vectorCopy(t3, normals[*n_index]);
		vectorCopy(t3, normals[(*n_index)+1]);
		vectorCopy(t3, normals[(*n_index)+2]);
		(*v_index)+=3;
		(*n_index)+=3;
	}
}	

void fill(vec4 *vertices, vec4 *normals) {
	int v_index = 0, n_index = 0;
	makeCube(vertices,normals,&v_index,&n_index);
}

void init(void)
{
	num_vertices = 132;
	vec4 vertices[num_vertices];
	vec4 normals[num_vertices];
	fill(vertices,normals);

	/*int width = 800;
	  int height = 800;
	  GLubyte my_texels[width][height][3];

	  FILE *fp = fopen("p2texture04.raw", "r");
	  fread(my_texels, width * height * 3, 1, fp);
	  fclose(fp);
	  */
	GLuint program = initShader("vshader_ctm.glsl", "fshader.glsl");
	glUseProgram(program);

	/*GLuint mytex[1];
	  glGenTextures(1, mytex);
	  glBindTexture(GL_TEXTURE_2D, mytex[0]);
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, my_texels);
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

	  int param;
	  glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &param);
	  */
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(textures), textures); 
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));

	/*GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
	  glEnableVertexAttribArray(vTexCoord);
	  glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0 + (sizeof(vertices) + sizeof(normals)));

	  GLuint texture_location = glGetUniformLocation(program, "texture");
	  glUniform1i(texture_location, 0);
	  */
	identityMatrix(model_view);
	vec4 e = {radius*cos(theta)*sin(phi),radius*cos(phi),radius*sin(theta)*sin(phi),0.0};
	vec4 a = {0.0,0.0,0,0.0};
	vec4 vup = {0.0,1.0,0.0,0.0};
	lookAt(e,a,vup,model_view);

	identityMatrix(projection);
	makeVector(-1,1,-1,0,lrb);
	makeVector(1,-1,-14,0,tnf);
	frustum(lrb,tnf,projection);
	matrixTranslation(light_position[0],light_position[1],light_position[2],ctm);
	int i;
	for(i=0;i<5;i++){
		identityMatrix(ball_ctms[i]);
	}
	model_view_location = glGetUniformLocation(program, "model_view");
	projection_location = glGetUniformLocation(program, "projection");
	ctm_location = glGetUniformLocation(program, "ctm");
	light_location = glGetUniformLocation(program, "light_position");
	att_const_location = glGetUniformLocation(program, "attenuation_constant");
	att_lin_location = glGetUniformLocation(program, "attenuation_linear");
	att_quad_location = glGetUniformLocation(program, "attenuation_quadratic");
	shininess_location = glGetUniformLocation(program, "shininess");
	shadow_location = glGetUniformLocation(program, "isShadow");
	amb_prod_location = glGetUniformLocation(program, "ambient_product");
	diff_prod_location = glGetUniformLocation(program, "diffuse_product");
	spec_prod_location = glGetUniformLocation(program, "specular_product");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glDepthRange(1,0);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	glUniformMatrix4fv(model_view_location, 1, GL_FALSE, model_view);
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection);	
	glUniform4fv(light_location, 1, light_position);
	glUniform1fv(att_const_location, 1, (GLfloat *) &attenuation_constant);
	glUniform1fv(att_lin_location, 1, (GLfloat *) &attenuation_linear);
	glUniform1fv(att_quad_location, 1, (GLfloat *) &attenuation_quadratic);
	glUniform1i(shadow_location, 0);
	glUniform1fv(shininess_location, 1, (GLfloat *) &cube_materials[0].shininess);

	vec4 amb_product, diff_product, spec_product;
	vectorProduct(cube_materials[0].reflect_ambient, light_ambient, amb_product);
	vectorProduct(cube_materials[0].reflect_diffuse, light_diffuse, diff_product);
	vectorProduct(cube_materials[0].reflect_specular, light_specular, spec_product);
	glUniform4fv(amb_prod_location, 1, amb_product);
	glUniform4fv(diff_prod_location, 1, diff_product);
	glUniform4fv(spec_prod_location, 1, spec_product);

	mat4 temp;
	identityMatrix(temp);
	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, temp);
	glDrawArrays(GL_TRIANGLES, 0, 132);
	/*glUniform1i(shadow_location, 1);
	  for(i=0;i<5;i++){
	  glUniform4fv(light_location, 1, light_position);
	  glUniformMatrix4fv(ctm_location, 1, GL_FALSE, ball_ctms[i]);
	  glUniform1fv(shininess_location, 1, (GLfloat *) &other_materials[2].shininess);

	  vec4 amb_product, diff_product, spec_product;
	  vectorProduct(other_materials[2].reflect_ambient, light_ambient, amb_product);
	  vectorProduct(other_materials[2].reflect_diffuse, light_diffuse, diff_product);
	  vectorProduct(other_materials[2].reflect_specular, light_specular, spec_product);
	  glUniform4fv(amb_prod_location, 1, amb_product);
	  glUniform4fv(diff_prod_location, 1, diff_product);
	  glUniform4fv(spec_prod_location, 1, spec_product);
	  glDrawArrays(GL_TRIANGLES, (i*10800)+10836, 10800);
	  }*/
	glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
	if(key == 'q')
		exit(0);
	else if(key == 'j'){
		vec4 temp = {-0.1,0,0,0};
		vectorAddition(light_position, temp, light_position);
	}else if(key == 'l'){
		vec4 temp = {0.1,0,0,0};
		vectorAddition(light_position, temp, light_position);
	}
	else if(key == 'i'){
		vec4 temp = {0,0,-0.1,0};
		vectorAddition(light_position, temp, light_position);
	}
	else if(key == 'k'){
		vec4 temp = {0,0,0.1,0};
		vectorAddition(light_position, temp, light_position);
	}
	else if(key == 'u'){
		vec4 temp = {0,0.1,0,0};
		vectorAddition(light_position, temp, light_position);
	}
	else if(key == 'o'){
		vec4 temp = {0,-0.1,0,0};
		vectorAddition(light_position, temp, light_position);
	}else if(key == 'w'){
		if(phi <= 0) return;
		phi -= 0.05;
		vec4 e = {radius*cos(theta)*sin(phi),radius*cos(phi),radius*sin(theta)*sin(phi),0.0};
		vec4 a = {0.0,0.0,0,0.0};
		vec4 vup = {0.0,1.0,0.0,0.0};
		lookAt(e,a,vup,model_view);
	}else if(key == 's'){
		if(phi >= M_PI) return;
		phi += 0.05;
		vec4 e = {radius*cos(theta)*sin(phi),radius*cos(phi),radius*sin(theta)*sin(phi),0.0};
		vec4 a = {0.0,0.0,0,0.0};
		vec4 vup = {0.0,1.0,0.0,0.0};
		lookAt(e,a,vup,model_view);
	}else if(key == 'a'){
		theta += 0.05;
		vec4 e = {radius*cos(theta)*sin(phi),radius*cos(phi),radius*sin(theta)*sin(phi),0.0};
		vec4 a = {0.0,0.0,0,0.0};
		vec4 vup = {0.0,1.0,0.0,0.0};
		lookAt(e,a,vup,model_view);
	}else if(key == 'd'){
		theta -= 0.05;
		vec4 e = {radius*cos(theta)*sin(phi),radius*cos(phi),radius*sin(theta)*sin(phi),0.0};
		vec4 a = {0.0,0.0,0,0.0};
		vec4 vup = {0.0,1.0,0.0,0.0};
		lookAt(e,a,vup,model_view);
	}else if(key == 'g'){
		animate = 1;
	}
	mat4 trans;
	matrixTranslation(light_position[0],light_position[1],light_position[2],trans);
	matrixCopy(trans,ctm);
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if( button == 3){
		lrb[0] *= 1/1.02;
		lrb[1] *= 1/1.02;
		lrb[2] *= 1/1.02;
		tnf[0] *= 1/1.02;
		frustum(lrb,tnf,projection);
		glutPostRedisplay();
	}else if( button == 4){
		lrb[0] *= 1.02;
		lrb[1] *= 1.02;
		lrb[2] *= 1.02;
		tnf[0] *= 1.02;
		frustum(lrb,tnf,projection);
		glutPostRedisplay();
	}
}

void idle(void) {
	if(animate){mat4 spin, copy;
		matrixRotateY(0.0075,spin);
		matrixMultiplication(spin,ball_ctms[1],copy);
		matrixCopy(copy,ball_ctms[1]);
		matrixRotateY(0.01,spin);
		matrixMultiplication(spin,ball_ctms[2],copy);
		matrixCopy(copy,ball_ctms[2]);
		matrixRotateY(0.0115,spin);
		matrixMultiplication(spin,ball_ctms[3],copy);
		matrixCopy(copy,ball_ctms[3]);
		matrixRotateY(0.014,spin);
		matrixMultiplication(spin,ball_ctms[4],copy);
		matrixCopy(copy,ball_ctms[4]);
		glutPostRedisplay();
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512,512);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Pool");
	glewInit();
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
