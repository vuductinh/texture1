#pragma once
#pragma once
#include<stdio.h>
long readBMP(char* filename, unsigned char** imgData, int& width, int& height);
GLuint createTextureFromBMP(char* filename);