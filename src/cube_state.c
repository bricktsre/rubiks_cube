#include "cube_state.h"

int right[8]={20,23,26,17,8,5,2,11}, up[8]={20,19,18,21,24,25,26,23}, front[8]={18,19,20,11,2,1,0,9}, left[8]={18,9,0,3,6,15,24,21}, down[8]={0,1,2,5,8,7,6,3}, back[8]={26,25,24,15,6,7,8,17};

void rotateSlice(int slice[]){
	int temp1, temp2;
	temp1 = slice[6];
	temp2 = slice[7];
	slice[7] = slice[5];
	slice[6] = slice[4];
	slice[5] = slice[3];
	slice[4] = slice[2];
	slice[3] = slice[1];
	slice[2] = slice[0];
	slice[1] = temp2;
	slice[0] = temp1;
}

void updateRight(){
	front[2] = right[0];
	front[3] = right[7];
	front[4] = right[6];
	up[0] = right[0];
	up[6] = right[2];
	up[7] = right[1];
	down[2] = right[6];
	down[3] = right[5];
	down[4] = right[4];
	back[0] = right[2];
	back[6] = right[4];
	back[7] = right[3];
}

void updateUp(){
	right[0] = up[0];
	right[1] = up[7];
	right[2] = up[6];
	front[0] = up[2];
	front[1] = up[1];
	front[2] = up[0];
	left[0] = up[2];
	left[6] = up[4];
	left[7] = up[3];
	back[0] = up[6];
	back[1] = up[5];
	back[2] = up[4];
}

void updateFront(){
	right[0] = front[2];
	right[6] = front[4];
	right[7] = front[3];
	up[0] = front[2];
	up[1] = front[1];
	up[2] = front[0];
	left[0] = front[0];
	left[1] = front[7];
	left[2] = front[6];
	down[0] = front[6];
	down[1] = front[5];
	down[2] = front[4];
}

void updateLeft(){
	up[2] = left[0];
	up[3] = left[7];
	up[4] = left[6];
	front[0] = left[0];
	front[6] = left[2];
	front[7] = left[1];
	down[0] = left[2];
	down[6] = left[4];
	down[7] = left[3];
	back[2] = left[6];
	back[3] = left[5];
	back[4] = left[4];
}

void updateDown(){
	right[6] = down[2];
	right[5] = down[3];
	right[4] = down[4];
	front[4] = down[2];
	front[5] = down[1];
	front[6] = down[0];
	left[2] = down[0];
	left[3] = down[7];
	left[4] = down[6];
	back[4] = down[6];
	back[5] = down[5];
	back[6] = down[4];
}

void updateBack(){
	right[2] = back[0];
	right[3] = back[7];
	right[4] = back[6];
	up[4] = back[2];
	up[5] = back[1];
	up[6] = back[0];
	left[4] = back[4];
	left[5] = back[3];
	left[6] = back[2];
	down[4] = back[6];
	down[5] = back[5];
	down[6] = back[4];
}
