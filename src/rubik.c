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
#include "solve_rc.h"
#include "cube_state.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

typedef GLfloat vec4[4];
typedef GLfloat mat4[16];
typedef GLfloat vec2[2];

GLfloat attenuation_constant = 0.5, attenuation_linear=0.1, attenuation_quadratic=0.1, shininess=1.0;
GLuint att_const_location, att_lin_location, att_quad_location, shininess_location, shadow_location;
GLuint light_location;
vec4 light_position = {0,0,4,1.0};

int num_vertices = 0, animate = 0;;
GLuint model_view_location, projection_location, ctm_location;
mat4 model_view, projection, ctm;
mat4 cube_ctms[27];

int move = 0;
char *solution = NULL;
int sol_index = 0, sol_count = 0, sol_curr = 0;

struct trackState temp = {0,{0.0,0.0,0.0,0.0},{1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0},{0.0,0.0,0.0,0.0},0.0,&ctm};
struct trackState *track = &temp;

void makeCube(vec4 *vertices, vec4 *normals, vec4 *colors, vec4 point, int *v_index, int *n_index, int *c_index, int *r_index){
	int i;
	mat4 trans;
	matrixTranslation(point[0],point[1],point[2],trans);
	vec4 temp;
	for(i = 0; i<132; i+=3){
		matrixVectorMultiplication(trans,cube_vertices[i],temp);
		vectorCopy(temp, vertices[*v_index]);
		matrixVectorMultiplication(trans,cube_vertices[i+1],temp);
		vectorCopy(temp, vertices[(*v_index)+1]);
		matrixVectorMultiplication(trans,cube_vertices[i+2],temp);
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
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices)+sizeof(normals), sizeof(colors), colors);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices)+sizeof(normals));

	identityMatrix(model_view);
	vec4 e = {0.0,0.0,4.0,0.0};
	vec4 a = {0.0,0.0,0,0.0};
	vec4 vup = {0.0,1.0,0.0,0.0};
	lookAt(e,a,vup,model_view);

	identityMatrix(projection);
	vec4 lrb = {-1.0,1.0,-1.0,0.0};
	vec4 tnf = {1.0,-1.0,-10.0,0.0};
	frustum(lrb,tnf,projection);

	identityMatrix(ctm);
	int i;
	for(i=0;i<27;i++)
		identityMatrix(cube_ctms[i]);
	
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
	int i;
	for(i =0; i<27; i++){
		mat4 temp;
		matrixMultiplication(ctm,cube_ctms[i],temp);
		glUniformMatrix4fv(ctm_location, 1, GL_FALSE, temp);
		glDrawArrays(GL_TRIANGLES, i*132, 132);
	}
	glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey) {
	if(key == 'r')
		move = 'r';
	else if(key == 'u')
		move = 'u';
	else if(key == 'f')
		move = 'f';
	else if(key == 'l')
		move = 'l';
	else if(key == 'd')
		move ='d';
	else if(key == 'b')
		move = 'b';
	else if(key == 's')
		move ='s';
	else if(key == 'a'){
		solution = solve_rc();
}	else if(key == 'q')
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
	if(move == 'r'){
		int i;
		for(i=0;i<8;i++){
			mat4 temp,copy;
			matrixRotateX(-M_PI/2,temp);
			matrixMultiplication(temp,cube_ctms[right[i]],copy);
			matrixCopy(copy, cube_ctms[right[i]]);
		}
		r_string_right();
		rotateSlice(right);
		updateRight();
		move = 0;
	}else if(move == 'u'){
		int i;
		for(i=0;i<8;i++){
			mat4 temp,copy;
			matrixRotateY(-M_PI/2,temp);
			matrixMultiplication(temp,cube_ctms[up[i]],copy);
			matrixCopy(copy, cube_ctms[up[i]]);
		}
		r_string_up();
		rotateSlice(up);
		updateUp();
		move = 0;
	}else if(move == 'f'){
		int i;
		for(i=0;i<8;i++){
			mat4 temp,copy;
			matrixRotateZ(-M_PI/2,temp);
			matrixMultiplication(temp,cube_ctms[front[i]],copy);
			matrixCopy(copy, cube_ctms[front[i]]);
		}
		r_string_front();
		rotateSlice(front);
		updateFront();
		move = 0;
	}else if(move == 'l'){
		int i;
		for(i=0;i<8;i++){
			mat4 temp,copy;
			matrixRotateX(M_PI/2,temp);
			matrixMultiplication(temp,cube_ctms[left[i]],copy);
			matrixCopy(copy, cube_ctms[left[i]]);
		}
		r_string_left();
		rotateSlice(left);
		updateLeft();
		move = 0;
	}else if(move == 'd'){
		int i;
		for(i=0;i<8;i++){
			mat4 temp,copy;
			matrixRotateY(M_PI/2,temp);
			matrixMultiplication(temp,cube_ctms[down[i]],copy);
			matrixCopy(copy, cube_ctms[down[i]]);
		}
		r_string_down();
		rotateSlice(down);
		updateDown();
		move = 0;
	}else if(move == 'b'){
		int i;
		for(i=0;i<8;i++){
			mat4 temp,copy;
			matrixRotateZ(M_PI/2,temp);
			matrixMultiplication(temp,cube_ctms[back[i]],copy);
			matrixCopy(copy, cube_ctms[back[i]]);
		}
		r_string_back();
		rotateSlice(back);
		updateBack();
		move = 0;
	}else{
		if(solution != NULL){
			if(sol_count != 0){
				move = sol_curr;
				sol_count--;
				return;
			}
			sol_curr = solution[sol_index];
			sol_count = solution[sol_index]-1;
			move = sol_curr;
			sol_index += 2;
		}
	}

	glutPostRedisplay();
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
