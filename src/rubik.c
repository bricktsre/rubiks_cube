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
#include "cube_info.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

typedef GLfloat vec4[4];
typedef GLfloat mat4[16];
typedef GLfloat vec2[2];

GLfloat attenuation_constant = 0.5, attenuation_linear=0.1, attenuation_quadratic=0.1, shininess=1.0;
GLuint att_const_location, att_lin_location, att_quad_location, shininess_location, shadow_location;
GLuint light_location;

int num_vertices = 0, animate = 0;;
GLuint model_view_location, projection_location, ctm_location;
mat4 model_view, projection, ctm;
vec4 lrb, tnf;
vec4 light_position = {0,0,3,1.0};

struct trackState temp = {0,{0.0,0.0,0.0,0.0},{1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0},{0.0,0.0,0.0,0.0},0.0,&ctm};
struct trackState *track = &temp;

void makeCube(vec4 *vertices, vec4 *normals, vec4 *colors, vec4 point, int *v_index, int *n_index, int *c_index, int *r_index){
	int i;
	mat4 trans, scale, copy;
	matrixTranslation(point[0],point[1],point[2],trans);
	matrixScale(1,1,1,scale);
	matrixMultiplication(trans,scale,copy);
	vec4 temp;
	for(i = 0; i<132; i+=3){
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
		if(i<36){
			vectorCopy(cube_colors[(*r_index)], colors[(*c_index)]);
			vectorCopy(cube_colors[(*r_index)+1], colors[(*c_index)+1]);
			vectorCopy(cube_colors[(*r_index)+2], colors[(*c_index)+2]);
			(*r_index)+=3;
		}else{
			vectorCopy(black_parts[i-36], colors[(*c_index)]);
			vectorCopy(black_parts[i-35], colors[(*c_index)+1]);
			vectorCopy(black_parts[i-34], colors[(*c_index)+2]);
		}(*v_index)+=3;
		(*n_index)+=3;
		(*c_index)+=3;
	}
}	

void fill(vec4 *vertices, vec4 *normals, vec4 *colors) {
	int v_index = 0, n_index = 0, c_index = 0, r_index=0;
	int i,j,k;
	for(i=0;i<3;i++){
		for(j=0;j<3;j++){
			for(k=0;k<3;k++){
				vec4 p = {-1.5+k,-1.5+i,1.5-j,1.0};
				makeCube(vertices,normals,colors,p,&v_index,&n_index,&c_index,&r_index);
			}
		}
	}
}

void init(void)
{
	num_vertices = 27*132;
	vec4 vertices[num_vertices];
	vec4 normals[num_vertices];
	vec4 colors[num_vertices];
	fill(vertices,normals,colors);

	GLuint program = initShader("vshader_ctm.glsl", "fshader.glsl");
	glUseProgram(program);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));

	identityMatrix(model_view);
	vec4 e = {0.0,0.0,4.0,0.0};
	vec4 a = {0.0,0.0,0,0.0};
	vec4 vup = {0.0,1.0,0.0,0.0};
	lookAt(e,a,vup,model_view);

	identityMatrix(projection);
	makeVector(-1,1,-1,0,lrb);
	makeVector(1,-1,-10,0,tnf);
	frustum(lrb,tnf,projection);

	identityMatrix(ctm);

	model_view_location = glGetUniformLocation(program, "model_view");
	projection_location = glGetUniformLocation(program, "projection");
	ctm_location = glGetUniformLocation(program, "ctm");
	light_location = glGetUniformLocation(program, "light_position");
	att_const_location = glGetUniformLocation(program, "attenuation_constant");
	att_lin_location = glGetUniformLocation(program, "attenuation_linear");
	att_quad_location = glGetUniformLocation(program, "attenuation_quadratic");

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

	glUniformMatrix4fv(ctm_location, 1, GL_FALSE, ctm);
	glDrawArrays(GL_TRIANGLES, 0, num_vertices);
	glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey) {
	if(key == 'q')
		exit(0);
}

void mouse(int button, int state, int x, int y) {
	trackMouse(track,button,state,x,y);
}

void motion(int x, int y){
	trackMotion(track,x,y);
}

void idle(void) {
	trackIdle(track);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512,512);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Rubik");
	glewInit();
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
