#include <cstdio>
#include <cstdlib>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/glut.h>
#endif
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <list>
#include "objloader.h"

using namespace std;
//////////////////////////////////////////////////
//global
float g_fWidth = 1000;
float g_fHeight = 500;
float g_fDepth = 100;
float g_fAngle = .0;

struct Button{
	float m_fPosX;		//表示在正交投影坐标系(左下角为坐标原点)的坐标，
	float m_fPosY;
	float m_fWidth;		//屏幕像素单位
	float m_fHeight;

	bool m_bPressed;
	void Render()
	{
		glPushMatrix();
		{
			//将中心位于原点的cube移动到使cube左下角坐标为m_fPosX,m_fPosY的位置
			//必须考虑cube的自身长宽
			glTranslatef(m_fPosX+m_fWidth/2, m_fPosY+m_fHeight/2, -2.0);		//-2.0只是为了按钮可见
			if( m_bPressed )
			{
				//double scaleAmt = 10.0 * sin( (double)rand() );
				//double scaleAmt = sin( (double)rand() );
				glScalef(0.9, 0.9, 1.0);
			}
			//cube中心位于原点
			glScalef (m_fWidth, m_fHeight, 5.0);
			glutSolidCube(1.0);
		}
		glPopMatrix();
	}
	bool OnMouseDown(int mousex, int mousey)
	{
		//鼠标的位置：mousex，mousey坐标系是原点位于左上角
		//必须将mousey变换到原点位于左下角的坐标系中
		mousey = g_fHeight-mousey;
		if( mousex > m_fPosX && mousex < m_fPosX+m_fWidth &&
			mousey > m_fPosY && mousey < m_fPosY+m_fHeight )
		{
			//cout<<"button is pressed"<<"\n";
			m_bPressed = true;

			return true;
		}
		return false;
	}
	void OnMouseUp()
	{
		m_bPressed = false;
	}
};

Button* pBtn;
Button* pBtn2;
Button* pBtn3;

objloader suz ( "suzanne.obj" ) ;
suz.build_list();
objloader::triangle * p;
p = list;

void init(void)
{
	glClearColor (0.5, 0.5, 0.5, 0.0);
	glShadeModel (GL_SMOOTH);

	pBtn = new Button;
	pBtn->m_bPressed = false;
	pBtn->m_fPosX = 40;
	pBtn->m_fPosY = 480;
	pBtn->m_fWidth = 60;
	pBtn->m_fHeight = 20;
	//printf("**********button pos: 40/t480/n");

	pBtn2 = new Button;
	pBtn2->m_bPressed = false;
	pBtn2->m_fPosX = 105;
	pBtn2->m_fPosY = 480;
	pBtn2->m_fWidth = 60;
	pBtn2->m_fHeight = 20;
	cout<<pBtn2->m_fPosY<<"\n";
	//printf("000000000000000");

	pBtn3= new Button;
	pBtn3->m_bPressed = false;
	pBtn3->m_fPosX = 170;
	pBtn3->m_fPosY = 480;
	pBtn3->m_fWidth = 60;
	pBtn3->m_fHeight = 20;
	cout<<pBtn2->m_fPosY<<"\n";
}

void CubeOrigin(void)
{
    glColor3f(1.0f,0.0f,0.0f);    // Top Color red
    glVertex3f( 0.2f, 0.2f,-0.2f);    // Top Right Of The Quad (Top)
    glVertex3f(-0.2f, 0.2f,-0.2f);    // Top Left Of The Quad (Top)
    glVertex3f(-0.2f, 0.2f, 0.2f);    // Bottom Left Of The Quad (Top)
    glVertex3f( 0.2f, 0.2f, 0.2f);    // Bottom Right Of The Quad (Top)
    glColor3f(1.0f,0.0f,0.0f);    // Bottom Color red
    glVertex3f( 0.2f,-0.2f, 0.2f);    // Top Right Of The Quad (Bottom)
    glVertex3f(-0.2f,-0.2f, 0.2f);    // Top Left Of The Quad (Bottom)
    glVertex3f(-0.2f,-0.2f,-0.2f);    // Bottom Left Of The Quad (Bottom)
    glVertex3f( 0.2f,-0.2f,-0.2f);    // Bottom Right Of The Quad (Bottom)
    glColor3f(1.0f,0.0f,0.0f);    // Front Color red
    glVertex3f( 0.2f, 0.2f, 0.2f);    // Top Right Of The Quad (Front)
    glVertex3f(-0.2f, 0.2f, 0.2f);    // Top Left Of The Quad (Front)
    glVertex3f(-0.2f,-0.2f, 0.2f);    // Bottom Left Of The Quad (Front)
    glVertex3f( 0.2f,-0.2f, 0.2f);    // Bottom Right Of The Quad (Front)
    glColor3f(1.0f,0.0f,0.0f);    // Back Color red
    glVertex3f( 0.2f,-0.2f,-0.2f);    // Top Right Of The Quad (Back)
    glVertex3f(-0.2f,-0.2f,-0.2f);    // Top Left Of The Quad (Back)
    glVertex3f(-0.2f, 0.2f,-0.2f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 0.2f, 0.2f,-0.2f);    // Bottom Right Of The Quad (Back)
    glColor3f(1.0f,0.0f,0.0f);    // Left Color red
    glVertex3f(-0.2f, 0.2f, 0.2f);    // Top Right Of The Quad (Left)
    glVertex3f(-0.2f, 0.2f,-0.2f);    // Top Left Of The Quad (Left)
    glVertex3f(-0.2f,-0.2f,-0.2f);    // Bottom Left Of The Quad (Left)
    glVertex3f(-0.2f,-0.2f, 0.2f);    // Bottom Right Of The Quad (Left)
    glColor3f(1.0f,0.0f,0.0f);    // Right Color red
    glVertex3f( 0.2f, 0.2f,-0.2f);    // Top Right Of The Quad (Right)
    glVertex3f( 0.2f, 0.2f, 0.2f);    // Top Left Of The Quad (Right)
    glVertex3f( 0.2f,-0.2f, 0.2f);    // Bottom Left Of The Quad (Right)
    glVertex3f( 0.2f,-0.2f,-0.2f);    // Bottom Right Of The Quad (Right)
}

void display(void)
{
	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, g_fWidth, 0, g_fHeight, 0, g_fDepth);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	pBtn->Render();
	pBtn2->Render();
	pBtn3->Render();

	// 绘制cube物体，
	glMatrixMode (GL_PROJECTION);		//回复原有的设置
	glLoadIdentity ();
	gluPerspective(60,1.0,1.5,20);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	/* viewing transformation  */
	gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glRotatef(g_fAngle, 0.0, 1.0, 0.0);
	//glRotatef(0.0, 0.0, 1.0, 0.0);
	glScalef (1.0, 2.0, 1.0);      /* modeling transformation */
	glutWireCube (0.3);

	glPushMatrix();
	glRotatef(-16.0, 0.0, 1.0, 0.0);
    glTranslatef(1.2,0.0,0.0);//0.0, 0.0, 0.0
    glBegin(GL_QUADS);        //The cube at center of window,the original cube
    glutWireCube (1.5);
    //CubeOrigin();
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glRotatef(16.0, 0.0, 1.0, 0.0);
    glTranslatef(-1.2,0.0,0.0);//0.0, 0.0, 0.0
    glBegin(GL_QUADS);        //The cube at center of window,the original cube
    glutWireCube (1.5);
    //CubeOrigin();
    glEnd();
    glPopMatrix();
	
	//glFlush();
	glutSwapBuffers();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(60,1.0,1.5,20);
	glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
case 27:
	exit(0);
	break;
	}
}

void mouse(int button, int state, int x, int y)
{
	if(button==GLUT_LEFT_BUTTON)
		switch(state)
	{
		case GLUT_DOWN:
			//左键按下：
			//printf("Mouse pos : %d/t%d/n", x, -500-y);
			if( pBtn->OnMouseDown(x, y) ){
				g_fAngle += 2.0;
				cout<<"button_1"<<"\n";
			}
			else if( pBtn2->OnMouseDown(x, y) )
            	cout<<"button_2"<<"\n";
			else if( pBtn3->OnMouseDown(x, y) )
				cout<<"button_3"<<"\n";
			break;

		case GLUT_UP:
			pBtn->OnMouseUp();
			pBtn2->OnMouseUp();
			pBtn3->OnMouseUp();
			break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (1000, 500);
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc( mouse );
	glutMainLoop();
	return 0;
}

