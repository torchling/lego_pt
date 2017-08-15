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
	std::vector< triangle > tpfp; // triangle pool for part 

	vertex[8] approximate_shape;//For now. It's still a box, designed for border detection. 

};


// Vectors to store the ldraw lego geometry
/*----------------------------------------------------------------*/
// 01
vector< vector<edge> > bricks ;		//just store shapes for drawing, not for matching. 

// 02
vector< vector<part_v1> > parts  ;	//For real math stuffs. Prepare for assembling possibility
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


void read_one_lego_part_and_save_it( char *part_name, int vector_id/* part's_name, geo_storage_id:?? */ ){
	// need 2 variable 
	// 1.part's_number or part's_name 2.save to "selected" part's_geo_storage in part list
	 
// variable list:

	short geo_type = 0;	// 2:line, 3:triangle, 4:Quadrilateral
	part_v1 part;		// tmp 
    
    //int type,color, a,b,c, d,e,f, g,h,i, j,k,l;
    int type, color;			// type is ldraw-types: 1, 2, 3, 4 and ldraw-color
    //int metrix[12];				// a,b,c, d,e,f, g,h,i, j,k,l;
    
    char fninf[10];				// only used in type 1, to store the file name
    char *test;					// only used in type 1, to store the file name
    string line;				// to read file line by line, we use string

    float metrix[12];	// 3*4 : xyz abc def ghi
	vertex dot1;
	vertex dot2;
	vertex dot3;
	triangle tri; // line, triangle, quad. They all been saved as a triangle formate.


// function list:
	ifstream inf(part_name);	// read the file with ifstream and save to inf

	if(!inf){
		cerr<<"Error: can't read part."<<endl;
		exit(1);
	}
	// read, save
	while(getline(inf, line)){	// use getline to save each line from 'inf' to 'line', one at a time. 
		istringstream iss(line);// istringstream helps 'line'(string) transform into 'iss'(stream). 
		if (iss >> x >> color) {

			if(type==1){
				//command
				iss >> >> >>
				search_one_lego_part_and_read_it();//same geo_storage space as father
			}
			if(type==2){
				//line
				iss >> metrix[0] >> metrix[1] >> metrix[2] >> metrix[3] >> metrix[4] >> metrix[5];

				dot1.x = metrix[0];		dot1.y = metrix[1];		dot1.z = metrix[2]; // dot1 = x1y1z1
				dot2.x = metrix[3];		dot2.y = metrix[4];		dot2.z = metrix[5]; // dot2 = x2y2z2

				tri.v1 = dot1;
				tri.v2 = dot2;
				tri.v3 = dot2; // if it's a line, v2=v3.

				//push_back
				parts[vector_id].tpfp.push_back(tri);
			}
			if(type==3){
				//triangle
				iss >> metrix[0] >> metrix[1] >> metrix[2] >> metrix[3] >> metrix[4] >> metrix[5]
					>> metrix[6] >> metrix[7] >> metrix[8];
				
				dot1.x = metrix[0];		dot1.y = metrix[1];		dot1.z = metrix[2]; // dot1 = x1y1z1
				dot2.x = metrix[3];		dot2.y = metrix[4];		dot2.z = metrix[5]; // dot2 = x2y2z2
				dot3.x = metrix[6];		dot3.y = metrix[7];		dot3.z = metrix[8]; // dot3 = x3y3z3
		
				tri.v1 = dot1;
				tri.v2 = dot2;
				tri.v3 = dot3;

				//push_back
				parts[vector_id].tpfp.push_back(tri);
			}
			if(type==4){
				//Quadrilateral
				iss >> metrix[0] >> metrix[1] >> metrix[2] >> metrix[3] >> metrix[4] >> metrix[5]
					>> metrix[6] >> metrix[7] >> metrix[8] >> metrix[9] >> metrix[10] >> metrix[11];
				
				dot1.x = metrix[0];		dot1.y = metrix[1];		dot1.z = metrix[2]; // dot1 = x1y1z1
				dot2.x = metrix[3];		dot2.y = metrix[4];		dot2.z = metrix[5]; // dot2 = x2y2z2
				dot3.x = metrix[6];		dot3.y = metrix[7];		dot3.z = metrix[8]; // dot3 = x3y3z3
		
				tri.v1 = dot1;
				tri.v2 = dot2;
				tri.v3 = dot3;

				//push_back
				parts[vector_id].tpfp.push_back(tri);

				dot1.x = metrix[6];		dot1.y = metrix[7];		dot1.z = metrix[8]; // dot1 = x3y3z3
				dot2.x = metrix[9];		dot2.y = metrix[10];	dot2.z = metrix[11];// dot2 = x4y4z4
				dot3.x = metrix[0];		dot3.y = metrix[1];		dot3.z = metrix[2]; // dot3 = x1y1z1
		
				tri.v1 = dot1;
				tri.v2 = dot2;
				tri.v3 = dot3;

				//push_back
				parts[vector_id].tpfp.push_back(tri);
			}
		}
	}
}

void searchfile(char *wanted_file){//
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("C:\\Users\\luke\\Desktop\\420\\p")) != NULL) {
        // sear all the files and directories within directory
        while ((ent = readdir (dir)) != NULL) {
            
            if(ent->d_name == wanted_file){
                read_one_lego_part_and_save_it( wanted_file );
            }
        }
        closedir (dir);
    } else {
        // could not open directory
        perror ("");
        return EXIT_FAILURE;
    }
}

void load_lego_parts_list( char *part_list ){ //load lego parts from the list
	
	ifstream inf(part_list);	// read the file with ifstream and save to inf

	if(!inf){
		cerr<<"Error: can't read part."<<endl;
		exit(1);
	}
	// read, save
	while(getline(inf, line)){	// use getline to save each line from 'inf' to 'line', one at a time. 
		istringstream iss(line);
		searchfile(iss);
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
	for(int i=0; i<20; i++){
		cout<< voxel_center_vPool[i].y <<"\n";
	}
	cout<< voxel_center_vPool[ voxel_center_vPool.size()-1 ].y <<"\n";

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
   		glBegin(GL_QUADS);
    	glutWireCube (1.5);
    	glEnd();
    glPopMatrix();

    glPushMatrix();			//The cube on the left hand
    	glRotatef(16.0, 0.0, 1.0, 0.0);
    	glTranslatef(-1.2,0.0,0.0);//1.2, 0.0, 0.0

    	glColor3f(1.0f,1.0f,1.0f);
   		glBegin(GL_QUADS);
    	glutWireCube (1.5);
    	glEnd();
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

