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
//#include <list> //no need

#include <fstream>
#include <sstream>
#include <string>
#include <dirent.h>

#include <cstdlib>

#include <vector>

using namespace std;
//////////////////////////////////////////////////
//global
float g_fWidth = 1000;
float g_fHeight = 500;
float g_fDepth = 100;
float g_fAngle = .0;

struct vertex
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

struct edge
{
    vertex v1;
    vertex v2;
};

struct triangle
{
    vertex v1;
    vertex v2;
    vertex v3;
};

struct face //max to 4 points
{
    int v1;
    int v2;
    int v3;
    int v4;
};

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

char* p = "suzanne.obj";
std::ifstream infile(p);

void read_lego_data(){
	;
}

std::vector<vertex> obj_vPool;
std::vector<face> obj_fPool;
std::vector<triangle> obj_tPool;

void read_obj(){
	std::string line;
	char section[10];
	char faceSec[5];
	vertex v;
	face f;
	triangle tri;

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);

        if (!iss) { break; } // error
        //std::cout << iss<<" ";
        //std::cout << line<<"\n";
        //std::cout << line[0]<<"\n";

		int nOb=0; //number of blank_space
		int nOb2=0; //number of blank_space
        for ( int i=0; i!=line.end()-line.begin(); i++){
    		if( line[0]=='v' && line[1]==' ' ){
    			if(line[i]==' '){
    				nOb++;
    				int c = 0;
    				int j = i;
    				while( line[j+1] != ' ' && (j+1) != line.end()-line.begin() ){
    					section[c]=line[j+1];
    					j++;
    					c++;
    				}
    				if(nOb==1){
    					v.x = atof(section);
    					section[10]={0};
    				}
    				else if(nOb==2){
    					v.y = atof(section);
    					section[10]={0};
    				}
    				else if(nOb==3){
    					v.z = atof(section);
    					section[10]={0};
    					obj_vPool.push_back(v);
                       }
    				else;
    			}

    		}
    		else if( line[0]=='f' && line[1]==' ' ){
    			
    			if(line[i]==' '){
    				nOb2++;
    				int c = 0;
    				int j = i;
    				while( line[j+1] != ' ' && (j+1) != line.end()-line.begin() ){
    					section[c]=line[j+1];
    					j++;
    					c++;
    				}
    				if(nOb2==1){
    					f.v1 = atof(section);
    					faceSec[5]={0};
    				}
    				else if(nOb2==2){
    					f.v2 = atof(section);
    					faceSec[5]={0};
    				}
    				else if(nOb2==3){
    					f.v3 = atof(section);
    					faceSec[5]={0};
    					if( j+1 == line.end()-line.begin() ){
    						f.v4=0;
    						obj_fPool.push_back(f);
    					}
    				}
    				else if(nOb2==4){
    					f.v4 = atof(section);
    					faceSec[5]={0};
    					obj_fPool.push_back(f);
                    }
    				else;
    			}
    		}
  		}

    }

    for(int i=0; i<obj_fPool.size(); i++){
		if(obj_fPool[i].v4==0){
			tri.v1 = obj_vPool[ obj_fPool[i].v1-1 ];
			tri.v2 = obj_vPool[ obj_fPool[i].v2-1 ];
			tri.v3 = obj_vPool[ obj_fPool[i].v3-1 ];

			obj_tPool.push_back(tri);
		}
		else{
			tri.v1 = obj_vPool[ obj_fPool[i].v1-1 ];
			tri.v2 = obj_vPool[ obj_fPool[i].v2-1 ];
			tri.v3 = obj_vPool[ obj_fPool[i].v3-1 ];

			obj_tPool.push_back(tri);

			tri.v1 = obj_vPool[ obj_fPool[i].v3-1 ];
			tri.v2 = obj_vPool[ obj_fPool[i].v4-1 ];
			tri.v3 = obj_vPool[ obj_fPool[i].v1-1 ];

			obj_tPool.push_back(tri);
		}
	}


/*
    for(int i=0; i<obj_vPool.size(); i++){

    	cout<< obj_vPool[i].x << '\n';
    }*/
    /*for(int i=0; i<obj_fPool.size(); i++){

    	cout<< obj_fPool[i].v1 <<" "<< obj_fPool[i].v2 <<" "<< obj_fPool[i].v3 <<" "<< obj_fPool[i].v4 << '\n';
    }*/
    cout<< "Size is "<< obj_vPool.size() << '\n';
/*
    std::string str ("Test string");
    cout<< str.end()-str.begin() <<"\n";
  	for ( std::string::iterator it=str.begin(); it!=str.end(); ++it){
    	std::cout << *it;
    	if(*it==' ')
    		cout << "space"<<'\n';
  	}
  	std::cout << '\n';
*/
}

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

	read_obj();
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
void drawObj_p()
{
	glColor3f(0.0f,0.0f,0.0f);
	for(int i=0; i<obj_vPool.size(); i++){
		glVertex3f( obj_vPool[i].x, obj_vPool[i].y, obj_vPool[i].z);
	}
}

void drawObj_f()
{
	glColor3f(1.0f,1.0f,1.0f);
	glBegin(GL_TRIANGLES);
		for(int i=0; i<obj_tPool.size(); i++){	
			glVertex3f( obj_tPool[i].v1.x, obj_tPool[i].v1.y, obj_tPool[i].v1.z);
			glVertex3f( obj_tPool[i].v2.x, obj_tPool[i].v2.y, obj_tPool[i].v2.z);
			glVertex3f( obj_tPool[i].v3.x, obj_tPool[i].v3.y, obj_tPool[i].v3.z);
		}
	glEnd();
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
    
    glBegin(GL_QUADS);        //The cube on the right hand
    glutWireCube (1.5);
    //CubeOrigin();
    glEnd();
    
    //drawObj_f();
    glPopMatrix();

    glPushMatrix();
    glRotatef(16.0, 0.0, 1.0, 0.0);
    glTranslatef(-1.2,0.0,0.0);//0.0, 0.0, 0.0
    
    glBegin(GL_QUADS);        //The cube on the left hand
    glutWireCube (1.5);
    //CubeOrigin();
    glEnd();
    
    
    glBegin(GL_POINTS);
    	drawObj_p();
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
			else if( pBtn2->OnMouseDown(x, y) ){
				g_fAngle -= 2.0;
            	cout<<"button_2"<<"\n";
            }
			else if( pBtn3->OnMouseDown(x, y) ){
				g_fAngle = .0;
				cout<<"button_3"<<"\n";
			}
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

