#ifndef CUBE_STATE_H
#define CUBE_STATE_H

extern int right[], up[], front[], left[], down[], back[];

void rotateSlice(int slice[]);
void updateRight();
void updateUp();
void updateFront();
void updateLeft();
void updateDown();
void updateBack();

#endif 
