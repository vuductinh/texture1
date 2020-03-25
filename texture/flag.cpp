#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "texture.h"
#include <windows.h>

struct Point
{
	Point(float _x, float _y, float _z)
	{
		i = _x; j = _y; z = _z;
	}

	float i, j, z;
};

#define MAX_WIDTH		40
#define MAX_HEIGHT		20

#define NUMBER_SIN_CYCLE	2
#define toRAD(i) (i * NUMBER_SIN_CYCLE*2*3.1415 / MAX_WIDTH)

#define MAX_WIDTH_HALF	MAX_WIDTH/2
#define MAX_HEIGHT_HALF	MAX_HEIGHT/2


int flag_idle = 0;

GLuint texture_id;

static int angle = 0;

float flag_points[MAX_WIDTH][MAX_HEIGHT][3];
void initFlag()
{
	for (int j = 0; j < MAX_HEIGHT; j++)
	{
		for (int i = 0; i < MAX_WIDTH; i++)
		{
			int x = i;
			int y = j;

			float tx = x - MAX_WIDTH_HALF;
			float ax = toRAD(tx);
			flag_points[i][j][1] = y - MAX_HEIGHT_HALF;
			flag_points[i][j][0] = tx;
			flag_points[i][j][2] = float(sin(ax)) * 3;
		}
	}
}

void swap()
{
	flag_idle++;
	if (flag_idle < 15)
		return;
	flag_idle = 0;
	int i, j = 0;

	for (int j = 0; j < MAX_HEIGHT; j++)
	{
		float hold = flag_points[0][j][2];
		for (int i = 0; i < MAX_WIDTH - 1; i++)
		{
			flag_points[i][j][2] = flag_points[i + 1][j][2];
		}
		flag_points[MAX_WIDTH - 1][j][2] = hold;
	}
}

int init_alpha = 0;
void moving_sine()
{
	flag_idle++;
	if (flag_idle < 15)
		return;
	flag_idle = 0;

	init_alpha++;
	if (init_alpha > MAX_WIDTH)
		init_alpha = 0;

	float alpha = toRAD(init_alpha);
	for (int j = 0; j < MAX_HEIGHT; j++)
	{
		for (int i = 0; i < MAX_WIDTH - 1; i++)
		{
			int x = i;
			float tx = x - MAX_WIDTH_HALF;
			float ax = toRAD(tx);
			flag_points[i][j][2] = float(sin(ax + alpha)) * 1;
		}
	}
}

DWORD last_idle_time = GetTickCount();

void startCount()
{
	last_idle_time = GetTickCount();
}

void frameMove()
{
	DWORD time_now;
	time_now = GetTickCount();
	float dt = (float)(time_now - last_idle_time) / 1000.0;
	printf("%f\n", dt);

	swap();
	moving_sine();

	glutPostRedisplay();
}



void drawFlag()
{
	int i, j;
	float float_x, float_y, float_xb, float_yb;

	//glTranslatef(0.0f,0.0f,-12.0f);

	/*glRotatef(xrot,1.0f,0.0f,0.0f);
	glRotatef(yrot,0.0f,1.0f,0.0f);
	glRotatef(zrot,0.0f,0.0f,1.0f);*/

	glBindTexture(GL_TEXTURE_2D, texture_id);

	glBegin(GL_QUADS);
	for (i = 0; i < MAX_WIDTH - 1; i++)
	{
		for (j = 0; j < MAX_HEIGHT - 1; j++)
		{
			float_x = float(i) / MAX_WIDTH;
			float_y = float(j) / MAX_HEIGHT;
			float_xb = float(i + 1) / MAX_WIDTH;
			float_yb = float(j + 1) / MAX_HEIGHT;

			glTexCoord2f(float_x, float_y);
			glVertex3f(flag_points[i][j][0], flag_points[i][j][1], flag_points[i][j][2]);

			glTexCoord2f(float_x, float_yb);
			glVertex3f(flag_points[i][j + 1][0], flag_points[i][j + 1][1], flag_points[i][j + 1][2]);

			glTexCoord2f(float_xb, float_yb);
			glVertex3f(flag_points[i + 1][j + 1][0], flag_points[i + 1][j + 1][1], flag_points[i + 1][j + 1][2]);

			glTexCoord2f(float_xb, float_y);
			glVertex3f(flag_points[i + 1][j][0], flag_points[i + 1][j][1], flag_points[i + 1][j][2]);

		}
	}
	glEnd();


}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	drawFlag();

	glutSwapBuffers();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();
}

void initTexture()
{
	//- Generate Texture ID
	glGenTextures(1, &texture_id);

	//- Enable texture
	glEnable(GL_TEXTURE_2D);

	//- Bind Active Texture 
	glBindTexture(GL_TEXTURE_2D, texture_id);

	//- Load image data from file
	unsigned char* imgData;
	int imgWidth, imgHeight;
	char bmp[] = "data/flag.bmp";
	int imgSize = readBMP(bmp, &imgData, imgWidth, imgHeight);

	//- Specify Texture Maps
	glTexImage2D(GL_TEXTURE_2D, 0, 3, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);

	//- Set Texture Parameters 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glPolygonMode(GL_BACK, GL_FILL);					// Back Face Is Solid
	glPolygonMode(GL_FRONT, GL_LINE);					// Front Face Is Made Of Lines

}

void initGaphics()
{
	initTexture();
	initFlag();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Hello");
	glutReshapeFunc(reshape);
	glutIdleFunc(frameMove);
	glutDisplayFunc(display);

	initGaphics();

	glutMainLoop();
	return 0;
}

