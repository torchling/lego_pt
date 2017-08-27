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

float voxel_length = 0.2;
float voxel_length_half = voxel_length*0.5;//

float metrix_O[12] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};
float metrix_V[12] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};

float rate = 0.15;


float add=2.4;
float oheight=-1.6;
float upp = 0.0;

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

struct part_v1 //This is part struct Ver.01 .
{
	std::vector< vertex > connection_1_x ; // x means Convex
	std::vector< vertex > connection_1_o ; // o means Concave

	std::vector< vertex > connection_2_x ; // x means Convex
	std::vector< vertex > connection_2_o ; // o means Concave

	// For the small set like rooster we only have 2 kinds of connecting way
	// but in the future, we need more then 2.
/*
	std::vector< vertex > connection_3_x ;
	std::vector< vertex > connection_3_o ;

	std::vector< vertex > connection_4_x ;
	std::vector< vertex > connection_4_o ;
*/

	std::vector< vertex > vertexPool; // all vertices in this part (maybe, we don't need this)

	std::vector< vertex > normal_pool;
	std::vector< triangle > tpfp; // triangle pool for part

	//vertex[8] approximate_shape;//For now. It's still a box, designed for border detection.

};


// Vectors to store the ldraw lego geometry
/*----------------------------------------------------------------*/
// 01
vector< edge > bricks ;		//just store shapes for drawing, not for matching.

// 02
vector< part_v1 > parts  ;	//For real math stuffs. Prepare for assembling possibility

vector< vertex > tmpNormalPool  ;
vector< triangle > trianglePool  ;
/*----------------------------------------------------------------*/

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
//char* p = "GermanShephardLowPoly.obj";
//char* p = "panther.obj";
std::ifstream infile(p);


// 超該死，C++不能循環呼叫所以只好把 read_one_lego_part_and_save_it() 和 searchfile() 合在一起------------/

// search_or_read() <-- read_one_lego_part_and_save_it() + searchfile()

void search_or_read( string part_name, bool SorR, float array_O[12]/*, int id  /*true:search false:read*/){
//---- if start -------------------------------------
	if(SorR==true){


		DIR *dir;
    	struct dirent *ent;
    	if ((dir = opendir ("C:\\Users\\user\\Desktop\\button_test\\parts")) != NULL) {
    		//C:\Users\luke\Desktop\button_test
    		//C:\Users\user\Desktop\button_test
        	// sear all the files and directories within directory

    	    while ((ent = readdir (dir)) != NULL) {
    	    	string d_name;

    	    	if(ent->d_name == part_name){
   		        	search_or_read( part_name, false, array_O );
            	}
        	}
        	closedir (dir);
    	} else {
        	// could not open directory
        	cerr<<"Can't search the part in \\parts"<<endl;
        	exit(1);
        	//return EXIT_FAILURE;
    	}

    	if ((dir = opendir ("C:\\Users\\user\\Desktop\\button_test\\parts\\s")) != NULL) {
    		//C:\Users\luke\Desktop\button_test
    		//C:\Users\user\Desktop\button_test
        	// search all the files and directories within directory

    	    while ((ent = readdir (dir)) != NULL) {
    	    	string d_name;      // ent->d_name: 3005.dat
                string ss="s\\";      // ss: s
                string sdname = ss + ent->d_name; // s3005.dat

                if(sdname == part_name){// part_name: s\3005.dat (backslash was escaped.)

                    //cout<<"Found "<< ent->d_name <<" in \\parts\\s"<<endl;
                    search_or_read( ent->d_name, false, array_O );
                }
        	}
        	closedir (dir);
    	} else {
        	// could not open directory
        	cerr<<"Can't search the part in \\parts\\s"<<endl;
        	exit(1);
        	//return EXIT_FAILURE;
    	}
	}
//---- if end -------------------------------------

//---- else start -------------------------------------
	else{
// variable list:
		//cout<<"We are trying to read the part file."<<endl;
	cout << part_name << endl;
	cout << endl;

	short geo_type = 0;	// 2:line, 3:triangle, 4:Quadrilateral
	//part_v1 part;		// tmp

    //int type,color, a,b,c, d,e,f, g,h,i, j,k,l;
    int type, color;	// type is ldraw-types: 1, 2, 3, 4 and ldraw-color
    //int metrix[12];	// a,b,c, d,e,f, g,h,i, j,k,l;

    string fninf;
    //char fninf[20];		// only used in type 1, to store the file name
    //char *test;			// only used in type 1, to store the file name
    string line;		// to read file line by line, we use string

    float metrix[12];	// only used in type 1, to store the file name
    					//3*4 : xyz abc def ghi
	vertex dot1;
	vertex dot2;
	vertex dot3;
	triangle tri; // line, triangle, quad. They all been saved as a triangle formate.

	vertex normalt;

    part_v1 parttmp;

// function list:
    string path      = "parts\\";
    string pathToS   = "parts\\s\\";
    string pathName  = path + part_name;
    string pathsName = pathToS + part_name;
    char *dat_name = new char[ pathName.length() + 1 ];
    strcpy(dat_name, pathName.c_str());
    char *s_dat_name = new char[ pathsName.length() + 1 ];
    strcpy(s_dat_name, pathsName.c_str());

	ifstream inf(dat_name);	// read the file with ifstream and save to inf
	ifstream infs(s_dat_name);	// read the file with ifstream and save to inf

	if( !inf && !infs ){
		cerr<<"Error: can't read part. 02"<<endl;
		exit(1);
	}
	// read, save
	while( getline(inf, line)||getline(infs, line) ){	// use getline to save each line from 'inf' to 'line', one at a time.
		istringstream iss(line);// istringstream helps 'line'(string) transform into 'iss'(stream).
		if (iss >> type >> color) {

			if(type==1){
				//command
				iss >> metrix[0] >> metrix[1] >> metrix[2] >> metrix[3] >> metrix[4] >> metrix[5]
				    >> metrix[6] >> metrix[7] >> metrix[8] >> metrix[9] >> metrix[10]>> metrix[11]
				    >> fninf;

				metrix_V[3] = array_O[3]*metrix[3] + array_O[4]*metrix[6] + array_O[5]*metrix[9];
				metrix_V[4] = array_O[3]*metrix[4] + array_O[4]*metrix[7] + array_O[5]*metrix[10];
				metrix_V[5] = array_O[3]*metrix[5] + array_O[4]*metrix[8] + array_O[5]*metrix[11];
				metrix_V[0] = array_O[3]*metrix[0] + array_O[4]*metrix[1] + array_O[5]*metrix[2] + array_O[0];
				metrix_V[6] = array_O[6]*metrix[3] + array_O[7]*metrix[6] + array_O[8]*metrix[9];
				metrix_V[7] = array_O[6]*metrix[4] + array_O[7]*metrix[7] + array_O[8]*metrix[10];
				metrix_V[8] = array_O[6]*metrix[5] + array_O[7]*metrix[8] + array_O[8]*metrix[11];
				metrix_V[1] = array_O[6]*metrix[0] + array_O[7]*metrix[1] + array_O[8]*metrix[2] + array_O[1];
				metrix_V[9]  = array_O[9]*metrix[3] + array_O[10]*metrix[6] + array_O[11]*metrix[9];
				metrix_V[10] = array_O[9]*metrix[4] + array_O[10]*metrix[7] + array_O[11]*metrix[10];
				metrix_V[11] = array_O[9]*metrix[5] + array_O[10]*metrix[8] + array_O[11]*metrix[11];
				metrix_V[2]  = array_O[9]*metrix[0] + array_O[10]*metrix[1] + array_O[11]*metrix[2] + array_O[2];

				for(int i=0; i<12 ; i++){ metrix[i] = metrix_V[i]; }

				cout << metrix_V[3] <<' '<< metrix_V[4] <<' '<< metrix_V[5] <<' '<< metrix_V[0] << endl;
				cout << metrix_V[6] <<' '<< metrix_V[7] <<' '<< metrix_V[8] <<' '<< metrix_V[1] << endl;
				cout << metrix_V[9] <<' '<< metrix_V[10] <<' '<< metrix_V[11] <<' '<< metrix_V[2] << endl;
				cout << endl;

				search_or_read( fninf, true, metrix );//same geo_storage space as father
				//metrix_V[0]=0; metrix_V[1]=0; metrix_V[2]=0; metrix_V[3]=1; metrix_V[4]=0; metrix_V[5]=0;
				//metrix_V[6]=0; metrix_V[7]=1; metrix_V[8]=0; metrix_V[9]=0; metrix_V[10]=0; metrix_V[11]=1;
			}
			if(type==2){
				//line
				iss >> metrix[0] >> metrix[1] >> metrix[2] >> metrix[3] >> metrix[4] >> metrix[5];

				metrix_V[0] = array_O[3]*metrix[0] + array_O[4]*metrix[1] + array_O[5]*metrix[2] + array_O[0];
				metrix_V[1] = array_O[6]*metrix[0] + array_O[7]*metrix[1] + array_O[8]*metrix[2] + array_O[1];
				metrix_V[2] = array_O[9]*metrix[0] + array_O[10]*metrix[1] + array_O[11]*metrix[2] + array_O[2];

				metrix[0] = metrix_V[0]*rate;
				metrix[1] = metrix_V[1]*rate;
				metrix[2] = metrix_V[2]*rate;


				metrix_V[0] = array_O[3]*metrix[3] + array_O[4]*metrix[4] + array_O[5]*metrix[5] + array_O[0];
				metrix_V[1] = array_O[6]*metrix[3] + array_O[7]*metrix[4] + array_O[8]*metrix[5] + array_O[1];
				metrix_V[2] = array_O[9]*metrix[3] + array_O[10]*metrix[4] + array_O[11]*metrix[5] + array_O[2];

				metrix[3] = metrix_V[0]*rate;
				metrix[4] = metrix_V[1]*rate;
				metrix[5] = metrix_V[2]*rate;

				dot1.x = metrix[0];		dot1.y = metrix[1];		dot1.z = metrix[2]; // dot1 = x1y1z1
				dot2.x = metrix[3];		dot2.y = metrix[4];		dot2.z = metrix[5]; // dot2 = x2y2z2

				tri.v1 = dot1;
				tri.v2 = dot2;
				tri.v3 = dot2; // if it's a line, v2=v3.

				normalt.x = 0.0;
				normalt.y = 0.0;
				normalt.z = 0.0;
				//cout << metrix_V[0] <<' '<< metrix_V[1] <<' '<< metrix_V[2] << endl;
				//push_back
				tmpNormalPool.push_back(normalt);
				trianglePool.push_back(tri);
				//parttmp.tpfp.push_back(tri);
				//metrix_V[0]=0; metrix_V[1]=0; metrix_V[2]=0; metrix_V[3]=1; metrix_V[4]=0; metrix_V[5]=0;
				//metrix_V[6]=0; metrix_V[7]=1; metrix_V[8]=0; metrix_V[9]=0; metrix_V[10]=0; metrix_V[11]=1;
			}
			if(type==3){
				//triangle
				iss >> metrix[0] >> metrix[1] >> metrix[2] >> metrix[3] >> metrix[4] >> metrix[5]
					>> metrix[6] >> metrix[7] >> metrix[8];

				metrix_V[0] = array_O[3]*metrix[0] + array_O[4]*metrix[1] + array_O[5]*metrix[2] + array_O[0];
				metrix_V[1] = array_O[6]*metrix[0] + array_O[7]*metrix[1] + array_O[8]*metrix[2] + array_O[1];
				metrix_V[2] = array_O[9]*metrix[0] + array_O[10]*metrix[1] + array_O[11]*metrix[2] + array_O[2];

				metrix[0] = metrix_V[0]*rate;
				metrix[1] = metrix_V[1]*rate;
				metrix[2] = metrix_V[2]*rate;

				metrix_V[0] = array_O[3]*metrix[3] + array_O[4]*metrix[4] + array_O[5]*metrix[5] + array_O[0];
				metrix_V[1] = array_O[6]*metrix[3] + array_O[7]*metrix[4] + array_O[8]*metrix[5] + array_O[1];
				metrix_V[2] = array_O[9]*metrix[3] + array_O[10]*metrix[4] + array_O[11]*metrix[5] + array_O[2];

				metrix[3] = metrix_V[0]*rate;
				metrix[4] = metrix_V[1]*rate;
				metrix[5] = metrix_V[2]*rate;

				metrix_V[0] = array_O[3]*metrix[6] + array_O[4]*metrix[7] + array_O[5]*metrix[8] + array_O[0];
				metrix_V[1] = array_O[6]*metrix[6] + array_O[7]*metrix[7] + array_O[8]*metrix[8] + array_O[1];
				metrix_V[2] = array_O[9]*metrix[6] + array_O[10]*metrix[7] + array_O[11]*metrix[8] + array_O[2];

				metrix[6] = metrix_V[0]*rate;
				metrix[7] = metrix_V[1]*rate;
				metrix[8] = metrix_V[2]*rate;

				dot1.x = metrix[0];		dot1.y = metrix[1];		dot1.z = metrix[2]; // dot1 = x1y1z1
				dot2.x = metrix[3];		dot2.y = metrix[4];		dot2.z = metrix[5]; // dot2 = x2y2z2
				dot3.x = metrix[6];		dot3.y = metrix[7];		dot3.z = metrix[8]; // dot3 = x3y3z3

				tri.v1 = dot1;
				tri.v2 = dot2;
				tri.v3 = dot3;

				normalt.x = (dot2.y-dot1.y)*(dot3.z-dot1.z)-(dot3.y-dot1.y)*(dot2.z-dot1.z);
				normalt.y = (dot2.z-dot1.z)*(dot3.x-dot1.x)-(dot3.z-dot1.z)*(dot2.x-dot1.x);
				normalt.z = (dot2.x-dot1.x)*(dot3.y-dot1.y)-(dot3.x-dot1.x)*(dot2.y-dot1.y);

				//cout << metrix_V[0] <<' '<< metrix_V[1] <<' '<< metrix_V[2] << endl;
				//push_back
				tmpNormalPool.push_back(normalt);
				trianglePool.push_back(tri);
				//parttmp.tpfp.push_back(tri);
				//metrix_V[0]=0; metrix_V[1]=0; metrix_V[2]=0; metrix_V[3]=1; metrix_V[4]=0; metrix_V[5]=0;
				//metrix_V[6]=0; metrix_V[7]=1; metrix_V[8]=0; metrix_V[9]=0; metrix_V[10]=0; metrix_V[11]=1;
			}
			if(type==4){
				//Quadrilateral
				iss >> metrix[0] >> metrix[1] >> metrix[2] >> metrix[3] >> metrix[4] >> metrix[5]
					>> metrix[6] >> metrix[7] >> metrix[8] >> metrix[9] >> metrix[10] >> metrix[11];

				metrix_V[0] = array_O[3]*metrix[0] + array_O[4]*metrix[1] + array_O[5]*metrix[2] + array_O[0];
				metrix_V[1] = array_O[6]*metrix[0] + array_O[7]*metrix[1] + array_O[8]*metrix[2] + array_O[1];
				metrix_V[2] = array_O[9]*metrix[0] + array_O[10]*metrix[1] + array_O[11]*metrix[2] + array_O[2];

				metrix[0] = metrix_V[0]*rate;
				metrix[1] = metrix_V[1]*rate;
				metrix[2] = metrix_V[2]*rate;

				metrix_V[0] = array_O[3]*metrix[3] + array_O[4]*metrix[4] + array_O[5]*metrix[5] + array_O[0];
				metrix_V[1] = array_O[6]*metrix[3] + array_O[7]*metrix[4] + array_O[8]*metrix[5] + array_O[1];
				metrix_V[2] = array_O[9]*metrix[3] + array_O[10]*metrix[4] + array_O[11]*metrix[5] + array_O[2];

				metrix[3] = metrix_V[0]*rate;
				metrix[4] = metrix_V[1]*rate;
				metrix[5] = metrix_V[2]*rate;

				metrix_V[0] = array_O[3]*metrix[6] + array_O[4]*metrix[7] + array_O[5]*metrix[8] + array_O[0];
				metrix_V[1] = array_O[6]*metrix[6] + array_O[7]*metrix[7] + array_O[8]*metrix[8] + array_O[1];
				metrix_V[2] = array_O[9]*metrix[6] + array_O[10]*metrix[7] + array_O[11]*metrix[8] + array_O[2];

				metrix[6] = metrix_V[0]*rate;
				metrix[7] = metrix_V[1]*rate;
				metrix[8] = metrix_V[2]*rate;

				metrix_V[0] = array_O[3]*metrix[9] + array_O[4]*metrix[10] + array_O[5]*metrix[11] + array_O[0];
				metrix_V[1] = array_O[6]*metrix[9] + array_O[7]*metrix[10] + array_O[8]*metrix[11] + array_O[1];
				metrix_V[2] = array_O[9]*metrix[9] + array_O[10]*metrix[10] + array_O[11]*metrix[11] + array_O[2];

				metrix[9] = metrix_V[0]*rate;
				metrix[10] = metrix_V[1]*rate;
				metrix[11] = metrix_V[2]*rate;

				dot1.x = metrix[0];		dot1.y = metrix[1];		dot1.z = metrix[2]; // dot1 = x1y1z1
				dot2.x = metrix[3];		dot2.y = metrix[4];		dot2.z = metrix[5]; // dot2 = x2y2z2
				dot3.x = metrix[6];		dot3.y = metrix[7];		dot3.z = metrix[8]; // dot3 = x3y3z3

				tri.v1 = dot1;
				tri.v2 = dot2;
				tri.v3 = dot3;

				normalt.x = (dot2.y-dot1.y)*(dot3.z-dot1.z)-(dot3.y-dot1.y)*(dot2.z-dot1.z);
				normalt.y = (dot2.z-dot1.z)*(dot3.x-dot1.x)-(dot3.z-dot1.z)*(dot2.x-dot1.x);
				normalt.z = (dot2.x-dot1.x)*(dot3.y-dot1.y)-(dot3.x-dot1.x)*(dot2.y-dot1.y);

				//cout << metrix_V[0] <<' '<< metrix_V[1] <<' '<< metrix_V[2] << endl;

				//push_back
				tmpNormalPool.push_back(normalt);
				trianglePool.push_back(tri);
				//parttmp.tpfp.push_back(tri);

				dot1.x = metrix[6];		dot1.y = metrix[7];		dot1.z = metrix[8]; // dot1 = x3y3z3
				dot2.x = metrix[9];		dot2.y = metrix[10];	dot2.z = metrix[11];// dot2 = x4y4z4
				dot3.x = metrix[0];		dot3.y = metrix[1];		dot3.z = metrix[2]; // dot3 = x1y1z1

				tri.v1 = dot1;
				tri.v2 = dot2;
				tri.v3 = dot3;

				normalt.x = (dot2.y-dot1.y)*(dot3.z-dot1.z)-(dot3.y-dot1.y)*(dot2.z-dot1.z);
				normalt.y = (dot2.z-dot1.z)*(dot3.x-dot1.x)-(dot3.z-dot1.z)*(dot2.x-dot1.x);
				normalt.z = (dot2.x-dot1.x)*(dot3.y-dot1.y)-(dot3.x-dot1.x)*(dot2.y-dot1.y);

				//cout << metrix_V[0] <<' '<< metrix_V[1] <<' '<< metrix_V[2] << endl;

				//push_back
				tmpNormalPool.push_back(normalt);
				trianglePool.push_back(tri);
				//parttmp.tpfp.push_back(tri);
				//metrix_V[0]=0; metrix_V[1]=0; metrix_V[2]=0; metrix_V[3]=1; metrix_V[4]=0; metrix_V[5]=0;
				//metrix_V[6]=0; metrix_V[7]=1; metrix_V[8]=0; metrix_V[9]=0; metrix_V[10]=0; metrix_V[11]=1;

			}
			//cout<<type<<endl;
		}

		}
		//parts.push_back(parttmp);
	}
//---- else end -------------------------------------
}

void load_lego_parts_list( char *part_list ){ //load lego parts from the list

	string line;
	string name;

	ifstream inf(part_list);	// read the file with ifstream and save to inf

	if(!inf){
		cerr<<"Error: can't got the list."<<endl;
		exit(1);
	}
	else{
		cout<<"We got the list."<<endl;
	}
	// read, save
	while(getline(inf, line)){	// use getline to save each line from 'inf' to 'line', one at a time.
		istringstream iss(line);
		iss >> name;
		search_or_read(name, true, metrix_O);
	}
}

//Storage vectors list of ABCD.obj input
std::vector<vertex> obj_vPool;
std::vector<face> obj_fPool;
std::vector<triangle> obj_tPool;

//Storage vector of Voxel
std::vector<vertex> voxel_center_vPool;

bool in_voxel(vertex test, vertex voxel_center, float radius){
	//float radius = edge_length*0.5;
	/*
	if( ((test.x - voxel_center.x)<=radius || (voxel_center.x - test.x)<=radius)
	 && ((test.y - voxel_center.y)<=radius || (voxel_center.y - test.y)<=radius)
	 && ((test.z - voxel_center.z)<=radius || (voxel_center.z - test.z)<=radius)
	)return true;
	*/
	if( abs(test.x - voxel_center.x)<=radius
	 && abs(test.y - voxel_center.y)<=radius
	 && abs(test.z - voxel_center.z)<=radius
	)return true;

	return false;
}

void read_obj(){
	std::string line;
	char section[20];
	char faceSec[5];
	vertex v;
	face f;
	triangle tri;
	cout<<"Start to get line"<<'\n';
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
    		//memset(section, 0, 20);
    		//memset(faceSec, 0, 5);
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
    					memset(section, 0, 20);
    				}
    				else if(nOb==2){
    					v.y = atof(section);
    					memset(section, 0, 20);
    				}
    				else if(nOb==3){
    					v.z = atof(section);
    					memset(section, 0, 20);
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
    				while( (line[j+1] != ' ') && ((j+1) != line.end()-line.begin()) && (line[j+1] != '/') ){
    					faceSec[c]=line[j+1];// 5/26:section[c]; 5/27:faceSec[c]
    					j++;
    					c++;
    				}
    				if(nOb2==1){
    					f.v1 = atoi(faceSec);// 5/26:section; 5/27:faceSec
    					memset(faceSec, 0, 5);
    				}
    				else if(nOb2==2){
    					f.v2 = atoi(faceSec);// 5/26:section; 5/27:faceSec
    					memset(faceSec, 0, 5);
    				}
    				else if(nOb2==3){
    					f.v3 = atoi(faceSec);// 5/26:section; 5/27:faceSec
    					memset(faceSec, 0, 5);
    					if( j+1 == line.end()-line.begin() ){
    						f.v4=0;
    						obj_fPool.push_back(f);
    					}
    				}
    				else if(nOb2==4){
    					f.v4 = atoi(faceSec);// 5/26:section; 5/27:faceSec
    					memset(faceSec, 0, 5);
    					obj_fPool.push_back(f);
                    }
    				else;
    			}
    		}
    		else{
    			;
    		}
  		}

    }
	cout<< "Size of vertex is "<< obj_vPool.size() << '\n';
	cout<< "Size of faces is "<< obj_fPool.size() << '\n';

    for(int i=0; i<obj_fPool.size(); i++){
		if(obj_fPool[i].v4!=0){
    		tri.v1 = obj_vPool[ obj_fPool[i].v1-1 ];
			tri.v2 = obj_vPool[ obj_fPool[i].v2-1 ];
			tri.v3 = obj_vPool[ obj_fPool[i].v3-1 ];

			obj_tPool.push_back(tri);

			tri.v1 = obj_vPool[ obj_fPool[i].v1-1 ];
			tri.v2 = obj_vPool[ obj_fPool[i].v3-1 ];
			tri.v3 = obj_vPool[ obj_fPool[i].v4-1 ];

			obj_tPool.push_back(tri);
		}
		else{

			tri.v1 = obj_vPool[ obj_fPool[i].v1-1 ];
			tri.v2 = obj_vPool[ obj_fPool[i].v2-1 ];
			tri.v3 = obj_vPool[ obj_fPool[i].v3-1 ];

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
	//voxelize the model
	float max_x = obj_vPool[0].x;
	float min_x = obj_vPool[0].x;
	float max_y = obj_vPool[0].y;
	float min_y = obj_vPool[0].y;
	float max_z = obj_vPool[0].z;
	float min_z = obj_vPool[0].z;

	for(int i=0; i<obj_vPool.size(); i++){
		//get Max
		if(max_x < obj_vPool[i].x)
			max_x = obj_vPool[i].x;

		if(max_y < obj_vPool[i].y)
			max_y = obj_vPool[i].y;

		if(max_z < obj_vPool[i].z)
			max_z = obj_vPool[i].z;

		//get min
		if(min_x > obj_vPool[i].x)
			min_x = obj_vPool[i].x;

		if(min_y > obj_vPool[i].y)
			min_y = obj_vPool[i].y;

		if(min_z > obj_vPool[i].z)
			min_z = obj_vPool[i].z;
	}

	max_x = max_x - min_x;//use max to replace radius of whole model
	max_y = max_y - min_y;//..
	max_z = max_z - min_z;//..

	//float voxel_length;

	int xn = max_x/voxel_length;
	int yn = max_y/voxel_length;
	int zn = max_z/voxel_length;

	vertex voxel_center_test;

	for(int i=0; i<xn; i++){
		for(int j=0; j<yn; j++){
			for(int k=0; k<zn; k++){
				voxel_center_test.x = min_x + i*voxel_length;
				voxel_center_test.y = min_y + j*voxel_length;
				voxel_center_test.z = min_z + k*voxel_length;

				for(int l=0; l<obj_vPool.size(); l++){
					if( in_voxel(obj_vPool[l], voxel_center_test, voxel_length_half) )
						voxel_center_vPool.push_back(voxel_center_test);
				}
			}
		}
	}

	cout<< "size of voxel: " << voxel_center_vPool.size() <<"\n";
	//cout<< obj_vPool[0].y <<"\n";
	/*
	for(int i=0; i<20; i++){
		cout<< voxel_center_vPool[i].y <<"\n";
	}*/
	//cout<< voxel_center_vPool[ voxel_center_vPool.size()-1 ].y <<"\n";

	//resize voxel_center_vPool;
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
	//cout<<pBtn2->m_fPosY<<"\n";
	//printf("000000000000000");

	pBtn3= new Button;
	pBtn3->m_bPressed = false;
	pBtn3->m_fPosX = 170;
	pBtn3->m_fPosY = 480;
	pBtn3->m_fWidth = 60;
	pBtn3->m_fHeight = 20;
	//cout<<pBtn2->m_fPosY<<"\n";

	read_obj();
/*
	char* list = "40234_Rooster_reduced.txt";
	load_lego_parts_list(list);
*/
	for(int i=0; i<12; i++){
		metrix_O[i] = metrix_O[i]*rate;
	}

	part_v1 part0;

	string partt = "3024.dat";//"3005.dat";3024 3070b
	search_or_read(partt, false, metrix_O);
	for(int i=0; i<trianglePool.size(); i++){
		part0.tpfp.push_back(trianglePool[i]);
		part0.normal_pool.push_back(tmpNormalPool[i]);
	}
	parts.push_back(part0);
	trianglePool.clear();
	tmpNormalPool.clear();

	part0.tpfp.clear();
	partt = "11477.dat";//"3005.dat";3024 3070b
	search_or_read(partt, false, metrix_O);
	for(int i=0; i<trianglePool.size(); i++){
		part0.tpfp.push_back(trianglePool[i]);
		part0.normal_pool.push_back(tmpNormalPool[i]);
	}
	parts.push_back(part0);
	trianglePool.clear();
	tmpNormalPool.clear();

	part0.tpfp.clear();
	partt = "3005.dat";//"3005.dat";3024 3070b
	search_or_read(partt, false, metrix_O);
	for(int i=0; i<trianglePool.size(); i++){
		part0.tpfp.push_back(trianglePool[i]);
		part0.normal_pool.push_back(tmpNormalPool[i]);
	}
	parts.push_back(part0);
	trianglePool.clear();
	tmpNormalPool.clear();

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

void drawObj_t()
{
	glColor3f(1.0f,0.0f,0.0f);

	for(int i=0; i<obj_tPool.size(); i++){
		glBegin(GL_LINE_LOOP);
			glVertex3f( obj_tPool[i].v1.x, obj_tPool[i].v1.y, obj_tPool[i].v1.z);
			glVertex3f( obj_tPool[i].v2.x, obj_tPool[i].v2.y, obj_tPool[i].v2.z);
			glVertex3f( obj_tPool[i].v3.x, obj_tPool[i].v3.y, obj_tPool[i].v3.z);
		glEnd();
	}
	/*
	glBegin(GL_TRIANGLES);
		for(int i=0; i<obj_tPool.size(); i++){
			glVertex3f( obj_tPool[i].v1.x, obj_tPool[i].v1.y, obj_tPool[i].v1.z);
			glVertex3f( obj_tPool[i].v2.x, obj_tPool[i].v2.y, obj_tPool[i].v2.z);
			glVertex3f( obj_tPool[i].v3.x, obj_tPool[i].v3.y, obj_tPool[i].v3.z);
		}
	glEnd();
	*/
}

void drawVoxel()
{

	glColor3f(0.0f,1.0f,0.0f);
	for(int i=0; i<voxel_center_vPool.size(); i++){
		glBegin(GL_LINE_LOOP);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
			glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
		glEnd();
	}
}

void drawPart(int p_number){
	for(int i=0; i < parts[p_number].tpfp.size() ; i++){
		glColor3f(0.0f,0.0f,1.0f);
		glBegin(GL_TRIANGLES);
			glNormal3f( parts[p_number].normal_pool[i].x, parts[p_number].normal_pool[i].y, parts[p_number].normal_pool[i].z );
			glVertex3f( parts[p_number].tpfp[i].v1.x, parts[p_number].tpfp[i].v1.y, parts[p_number].tpfp[i].v1.z);
			glVertex3f( parts[p_number].tpfp[i].v2.x, parts[p_number].tpfp[i].v2.y, parts[p_number].tpfp[i].v2.z);
			glVertex3f( parts[p_number].tpfp[i].v3.x, parts[p_number].tpfp[i].v3.y, parts[p_number].tpfp[i].v3.z);
		glEnd();/*
		glColor3f(1.0f,1.0f,1.0f);
		glBegin(GL_LINE_LOOP);
			glNormal3f( parts[p_number].normal_pool[i].x, parts[p_number].normal_pool[i].y, parts[p_number].normal_pool[i].z );
			glVertex3f( parts[p_number].tpfp[i].v1.x, parts[p_number].tpfp[i].v1.y, parts[p_number].tpfp[i].v1.z);
			glVertex3f( parts[p_number].tpfp[i].v2.x, parts[p_number].tpfp[i].v2.y, parts[p_number].tpfp[i].v2.z);
			glVertex3f( parts[p_number].tpfp[i].v3.x, parts[p_number].tpfp[i].v3.y, parts[p_number].tpfp[i].v3.z);
		glEnd();*/
	}
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
	gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);	//monkey
	//gluLookAt (0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);	// dog
	glRotatef(g_fAngle, 0.0, 1.0, 0.0);
	//glRotatef(0.0, 0.0, 1.0, 0.0);
	glScalef (1.0, 2.0, 1.0);      /* modeling transformation */
	glutWireCube (0.3);

	glPushMatrix();			//The cube on the right hand
    	glRotatef(-16.0, 0.0, 1.0, 0.0);
    	glTranslatef(1.2,0.0,0.0);//1.2, 0.0, 0.0

    	glColor3f(1.0f,1.0f,1.0f);
   		/*glBegin(GL_QUADS);
    	glutWireCube (1.5);
    	glEnd();*/
    glPopMatrix();

    glPushMatrix();			//The cube on the left hand
    	glRotatef(16.0, 0.0, 1.0, 0.0);
    	glTranslatef(-1.2,0.0,0.0);//1.2, 0.0, 0.0

    	glColor3f(1.0f,1.0f,1.0f);
   		/*glBegin(GL_QUADS);
    	glutWireCube (1.5);
    	glEnd();*/
    glPopMatrix();

	glPushMatrix();			//Obj model points
		glRotatef(-16.0, 0.0, 1.0, 0.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);	// monkey only
    	//glTranslatef(1.2,-3.0,-2.0);		// dog
    	glTranslatef(1.2, 0.0, 0.0);		// monkey

    	glBegin(GL_POINTS);
    		drawObj_p();
		glEnd();

    	drawVoxel();

    glPopMatrix();

    glPushMatrix();			//Obj model faces
    	glRotatef(16.0, 0.0, 1.0, 0.0);
    	glRotatef(-90.0, 1.0, 0.0, 0.0);	// monkey only
    	//glTranslatef(-1.2,-3.0,-2.0);		// dog
    	glTranslatef(-1.2, 0.0, 0.0);		// monkey

    	drawObj_t();
    glPopMatrix();

    // drawPart
    glPushMatrix();
    	glRotatef(30.0, 0.0, 1.0, 0.0);
    	glRotatef(180, 1.0, 0.0, 0.0);
		
    	glTranslatef(0.0, oheight, 0.0);
    	
    	for(int i=0; i<parts.size(); i++){
    		//glRotatef(upp, 1.0, 0.0, 0.0);
    		glTranslatef(0.0, add, 0.0);

    		drawPart(i);
    	}
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
		case 'a':
			g_fAngle += 2.0;
			break;

		case 'd':
			g_fAngle -= 2.0;
			break;

		case 's':
			g_fAngle = .0;
			upp = .0;
			oheight=-1.6;
			break;

		case 'w':
			upp += 0.5;
			break;

		case 'i':
			oheight -= 0.1;
			break;

		case 'k':
			oheight += 0.1;
			break;

		case 't':
			add += 0.1;
			break;

		case 'g':
			add -= 0.1;
			break;

		case 27:
			exit(0);
			break;
	}
	glutPostRedisplay();
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
				//cout<<"button_1"<<"\n";
			}
			else if( pBtn2->OnMouseDown(x, y) ){
				g_fAngle -= 2.0;
            	//cout<<"button_2"<<"\n";
            }
			else if( pBtn3->OnMouseDown(x, y) ){
				g_fAngle = .0;
				//cout<<"button_3"<<"\n";
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

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 20.0f, 20.0f, -80.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (1000, 500);
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc( mouse );

	//glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	glutMainLoop();
	return 0;
}

