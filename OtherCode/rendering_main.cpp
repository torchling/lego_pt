//
//  main.cpp
//  LegoParts
//
//  Created by Richard Sung on 2017/12/15.
//  Copyright © 2017年 Richard Sung. All rights reserved.
//

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

#include "helpteddy.h"


using namespace std;

//////////////////////////////////////////////////
//global

float voxel_length = 0.03;//0.07
float voxel_length_half = voxel_length/2;//
float obmi_x, obmi_y, obmi_z;
int obn_x, obn_y, obn_z;

float alert_range = 3.0; //for interier stuff

float metrix_OO[12]= {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};
float metrix_O[12] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};
float metrix_V[12] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};

float rate = 0.2; //scale of parts (Square root) [0.2 to watch] [0.08 to obj] [0.05658 to obj]
float dis = -6.0;  //position of OBJ in z axis //-4.5 2017.11.8

float add=-2.2;
float oheight=0.0;
float upp = 0.0;

bool drawlegoFrame = true;
bool swch = true;
bool drawTri0 = false;

bool stud_for_plate=false;
bool stud_for_brick=false;

float rota = 0.0;
float rotate1 = 0.0;

float dark[3]={0.2,0.2,0.2};
float lightgrey[3]={0.65,0.65,0.65};
float grey[3]={0.5,0.5,0.5};
float blue[3]={0,0,1.0};
float yellow[3]={0.8,0.67,0.05};
float green[3]={0.3,0.8,0.3};
float red[3]={0.8,0,0};
float orange[3]={0.8,0.4,0.1};

struct face //max to 4 points
{
    int v1;
    int v2;
    int v3;
    int v4;
};

struct matri
{
    float m[12];
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
    std::vector< edge > t5e; //type 5 edge Pool
    
    //vertex[8] approximate_shape;//For now. It's still a box, designed for border detection.
    
};


// Vectors to store the ldraw lego geometry
/*----------------------------------------------------------------*/
// 01
vector< edge > bricks ;     //just store shapes for drawing, not for matching.

// 02
vector< part_v1 > parts  ;  //For real math stuffs. Prepare for assembling possibility

vector< vertex > tmpNormalPool  ;
vector< triangle > trianglePool  ;
vector< vertex > cnnc_x;
vector< vertex > cnnc_o;
vector< edge > edgePool;
vector< matri > bricksLocation ;
/*----------------------------------------------------------------*/

//char* p = "frog.obj";
//char* p = "ball.obj";


//char* p = "suzanne.obj";
char* p = "bug.obj";
//char* ma = "suzanne50.ma";
char* ma = "bug20.ma";
//char* ma = "dog30.ma";
//char* ma = "spider25.ma";
//char* p = "GermanShephardLowPoly.obj";
//char* p = "panther.obj";
std::ifstream infile(p);


//matrix operations start----------------------------------------------------------/

float angleCovert(float degree){
    float result;
    result = degree * M_PI/180;
    return result;
}

float angleBetween2Vector(vertex v1, vertex v2){
    float dot = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
    float length1 = pow(v1.x, 2.0) + pow(v1.y, 2.0) + pow(v1.z, 2.0);
    float length2 = pow(v2.x, 2.0) + pow(v2.y, 2.0) + pow(v2.z, 2.0);
    length1 = pow(length1, 0.5);
    length2 = pow(length2, 0.5);
    return acos(dot/(length1*length2))*180/M_PI;
}

bool testInRightSpaceOfVector(vertex test, vertex vec){
    if(false){
        return false;
    }
    return true;
}

vertex normalOf2Vector(vertex t, vertex v){
    vertex result;
    result.x = t.y*v.z-v.y*t.z;
    result.y = t.z*v.x-v.z*t.x;
    result.z = t.x*v.y-v.x*t.y;
    return result;
}

vertex normalize(vertex vec){
    vertex re; //result
    float length = pow(vec.x, 2.0) + pow(vec.y, 2.0) + pow(vec.z, 2.0);
    if(length > 0.0){
        length = pow(length, 0.5);
        re.x = vec.x/length;
        re.y = vec.y/length;
        re.z = vec.z/length;
        return re;
    }
    else{
        re.x = 0.0;
        re.y = 0.0;
        re.z = 0.0;
        return re;
    }
}

vertex  matrixVertexMotiply(float a[12], vertex v){
    vertex result;
    
    result.x = a[3]*v.x + a[4]*v.y + a[5]*v.z + a[0];
    result.y = a[6]*v.x + a[7]*v.y + a[8]*v.z + a[1];
    result.z = a[9]*v.x + a[10]*v.y + a[11]*v.z + a[2];
    return result;
}

matri matrixMotiply(float a[12], float m[12]){ //[ A ][ B ][v]=[v]
    matri result;
    result.m[3] = a[3]*m[3] + a[4]*m[6] + a[5]*m[9];
    result.m[4] = a[3]*m[4] + a[4]*m[7] + a[5]*m[10];
    result.m[5] = a[3]*m[5] + a[4]*m[8] + a[5]*m[11];
    result.m[0] = a[3]*m[0] + a[4]*m[1] + a[5]*m[2] + a[0];
    result.m[6] = a[6]*m[3] + a[7]*m[6] + a[8]*m[9];
    result.m[7] = a[6]*m[4] + a[7]*m[7] + a[8]*m[10];
    result.m[8] = a[6]*m[5] + a[7]*m[8] + a[8]*m[11];
    result.m[1] = a[6]*m[0] + a[7]*m[1] + a[8]*m[2] + a[1];
    result.m[9]  = a[9]*m[3] + a[10]*m[6] + a[11]*m[9];
    result.m[10] = a[9]*m[4] + a[10]*m[7] + a[11]*m[10];
    result.m[11] = a[9]*m[5] + a[10]*m[8] + a[11]*m[11];
    result.m[2]  = a[9]*m[0] + a[10]*m[1] + a[11]*m[2] + a[2];
    /*
     for(int i=0; i<12; i++){
     if(abs(result.m[i])<0.0000001) result.m[i]=0.0;
     }*/
    return result;
}

//Returns the rotation matrix around the vector v placed at point p, rotate by angle a
matri matrixRotate(float angle, vertex normalr, vertex originalVertex){
    
    matri m_result;
    
    float t = angleCovert(angle);
    
    vertex normal = normalize(normalr);
    float u = normal.x;
    float v = normal.y;
    float w = normal.z;
    float a = originalVertex.x;
    float b = originalVertex.y;
    float c = originalVertex.z;
    //cout<<u<<" "<<v<<" "<<w<<" "<<a<<" "<<b<<" "<<c<<"\n";
    
    m_result.m[3] = u*u+(v*v+w*w)*cos(t);
    m_result.m[4] = u*v*(1-cos(t))-w*sin(t);
    m_result.m[5] = u*w*(1-cos(t))+v*sin(t);
    m_result.m[0] = (a*(v*v+w*w)-u*(b*v+c*w))*(1-cos(t))+(b*w-c*v)*sin(t);
    m_result.m[6] = u*v*(1-cos(t))+w*sin(t);
    m_result.m[7] = v*v+(u*u+w*w)*cos(t);
    m_result.m[8] = v*w*(1-cos(t))-u*sin(t);
    m_result.m[1] = (b*(u*u+w*w)-v*(a*u+c*w))*(1-cos(t))+(c*u-a*w)*sin(t);
    m_result.m[9] = u*w*(1-cos(t))-v*sin(t);
    m_result.m[10] = v*w*(1-cos(t))+u*sin(t);
    m_result.m[11] = w*w+(u*u+v*v)*cos(t);
    m_result.m[2] = (c*(u*u+v*v)-w*(a*u+b*v))*(1-cos(t))+(a*v-b*u)*sin(t);
    /*
     u*u+(v*v+w*w)*cos(t), u*v*(1-cos(t))-w*sin(t), u*w*(1-cos(t))+v*sin(t),
     (a*(v*v+w*w)-u*(b*v+c*w))*(1-cos(t))+(b*w-c*v)*sin(t),
     u*v*(1-cos(t))+w*sin(t), v*v+(u*u+w*w)*cos(t), v*w*(1-cos(t))-u*sin(t),
     (b*(u*u+w*w)-v*(a*u+c*w))*(1-cos(t))+(c*u-a*w)*sin(t),
     u*w*(1-cos(t))-v*sin(t), v*w*(1-cos(t))+u*sin(t), w*w+(u*u+v*v)*cos(t),
     (c*(u*u+v*v)-w*(a*u+b*v))*(1-cos(t))+(a*v-b*u)*sin(t),
     0,                       0,                       0,                       1
     */
    for(int i=0; i<12; i++){
        //if(abs(m_result.m[i])<0.0000001) m_result.m[i]=0.0;
    }
    return m_result; //resultV
}
//matrix operations end-------------------------------------------------------------------------/


// 超該死，C++不能循環呼叫所以只好把 read_one_lego_part_and_save_it() 和 searchfile() 合在一起------------/
// search_or_read() <-- read_one_lego_part_and_save_it() + searchfile()
bool wind_chosen;
int invertC=0;
void search_or_read( string part_name, bool Search, float array_O[12], bool invertYN, string namewithPath){
    //---- if start -------------------------------------
    if(Search){ //Search=false means to read.
        
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir ("/Users/luke/Desktop/legoParts/parts")) != NULL) {
            // C:/Users/luke/Desktop/lego_assembler/parts
            // C:/Users/user/Desktop/lego_assembler/parts
            // /Users/luke/desktop/lego_assembler/parts
            // search all the files and directories within directory
            
            while ((ent = readdir (dir)) != NULL) {
                string d_name;
                
                if(ent->d_name == part_name){
                    search_or_read( part_name, false, array_O, invertYN, part_name);
                }
            }
            closedir (dir);
        } else {
            // could not open directory
            cerr<<"Can't search the part in \\parts"<<endl;
            exit(1);
            //return EXIT_FAILURE;
        }
        
        if ((dir = opendir ("/Users/luke/Desktop/legoParts/parts/s")) != NULL) {
            // C:/Users/luke/Desktop/lego_assembler/parts/s
            // C:/Users/user/Desktop/lego_assembler/parts/s
            // /Users/luke/desktop/lego_assembler/parts/s
            // search all the files and directories within directory
            
            while ((ent = readdir (dir)) != NULL) {
                string d_name;      // ent->d_name: 3005.dat
                string ss="s\\";      // ss: s
                string sdname = ss + ent->d_name; // s\ + 3005.dat
                
                if(sdname == part_name){// part_name: s\3005.dat (backslash was escaped.)
                    
                    //cout<<"Found "<< ent->d_name <<" in \\parts\\s"<<endl;
                    search_or_read( ent->d_name, false, array_O, invertYN, sdname);
                }
            }
            closedir (dir);
        } else {
            // could not open directory
            cerr<<"Can't search the part in \\parts\\s"<<endl;
            exit(1);
            //return EXIT_FAILURE;
        }
        
        if ((dir = opendir ("/Users/luke/Desktop/legoParts/parts/48")) != NULL) {
            // C:/Users/luke/Desktop/lego_assembler/parts/48
            // C:/Users/user/Desktop/lego_assembler/parts/48
            // /Users/luke/desktop/lego_assembler/parts/48
            // search all the files and directories within directory
            
            while ((ent = readdir (dir)) != NULL) {
                string d_name;      // ent->d_name: 3005.dat
                string s48="48\\";      // s48: 48\;
                string sdname = s48 + ent->d_name; // 48\ + 3005.dat
                
                if(sdname == part_name){// part_name: s\3005.dat (backslash was escaped.)
                    
                    //cout<<"Found "<< ent->d_name <<" in \\parts\\s"<<endl;
                    search_or_read( ent->d_name, false, array_O, invertYN, sdname);
                }
            }
            closedir (dir);
        } else {
            // could not open directory
            cerr<<"Can't search the part in \\parts\\48"<<endl;
            exit(1);
            //return EXIT_FAILURE;
        }
        
        if ((dir = opendir ("/Users/luke/desktop/LegoParts/parts/8")) != NULL) {
            // C:/Users/luke/Desktop/lego_assembler/parts/8
            // C:/Users/user/Desktop/lego_assembler/parts/8
            // /Users/luke/desktop/legomac/parts/8
            // search all the files and directories within directory
            
            while ((ent = readdir (dir)) != NULL) {
                string d_name;      // ent->d_name: 3005.dat
                string s8="8\\";      // s8: 8\;
                string sdname = s8 + ent->d_name; // 8\3005.dat
                
                if(sdname == part_name){// part_name: s\3005.dat (backslash was escaped.)
                    
                    cout<<"Found "<< ent->d_name <<" in \\parts\\s"<<endl;
                    cout<<sdname<<endl;
                    search_or_read( ent->d_name, false, array_O, invertYN, sdname);
                }
            }
            closedir (dir);
        } else {
            // could not open directory
            cerr<<"Can't search the part in \\parts\\8"<<endl;
            exit(1);
            //return EXIT_FAILURE;
        }
    }
    //---- if end -------------------------------------
    
    //---- else start -------------------------------------
    else{
        // variable list:
        //cout<<"We are trying to read the part file."<<endl;
        //cout << part_name << endl;
        //cout << endl;
        
        //short geo_type = 0; // 2:line, 3:triangle, 4:Quadrilateral
        //part_v1 part;     // tmp
        
        //int type,color, a,b,c, d,e,f, g,h,i, j,k,l;
        int type;       // type is ldraw-types: 1, 2, 3, 4
        string color;   // ldraw-color; for type-0, it's some order
        string bfc = "BFC";
        string bfc_operation;
        string corcc;
        string cw         = "CW";
        string ccw        = "CCW";
        string invertnext = "INVERTNEXT";
        string certify    = "CERTIFY";
        string name       = "Name:";
        
        //part's content
        string stud       = "stud.dat";
        string stud2      = "stud2.dat";
        string stud2a     = "stud2a.dat";
        string stud2b     = "stud2b.dat";
        
        string sphe88     = "8-8sphe.dat";
        string fri8socket = "joint-8-socket-frica.dat";
        string p4733      = "4733.dat";
        
        //part's type
        string plate      = "Plate";
        string brick      = "Brick";
        
        //int metrix[12];   // a,b,c, d,e,f, g,h,i, j,k,l;
        
        bool once= false;
        if( part_name == p4733 )
            once=true;
        
        bool real= false;
        
        //int metrix[12];   // a,b,c, d,e,f, g,h,i, j,k,l;
        
        string fninf;
        //char fninf[20];       // only used in type 1, to store the file name
        //char *test;           // only used in type 1, to store the file name
        string line;        // to read file line by line, we use string
        
        float metrix[12];   // only used in type 1, to store the file name
        //3*4 : xyz abc def ghi
        vertex dot1;
        vertex dot2;
        vertex dot3;
        //vertex dot4;
        triangle tri; // line, triangle, quad. They all been saved as a triangle formate.
        //edge edg;
        
        vertex normalt;
        
        part_v1 parttmp;
        
        bool cclockwise=false;
        // function list:
        
        //string path      = "parts\\";
        //string pathToS   = "parts\\s\\";
        string path      = "parts/";        // on Mac
        string pathToS   = "parts/s/";      // on Mac
        string path8    = "parts/8/";     // on Mac
        string path48     = "parts/48/";      // on Mac
        
        string pathName  = path + part_name;
        string pathsName = pathToS + part_name;
        string path8Name = path8 + part_name;
        string path48Name= path48 + part_name;
        char *dat_name = new char[ pathName.length() + 1 ];
        strcpy(dat_name, pathName.c_str());
        char *s_dat_name = new char[ pathsName.length() + 1 ];
        strcpy(s_dat_name, pathsName.c_str());
        char *dat_name8 = new char[ path8Name.length() + 1 ];
        strcpy(dat_name8, path8Name.c_str());
        char *dat_name48 = new char[ path48Name.length() + 1 ];
        strcpy(dat_name48, path48Name.c_str());
        //cout<<"test "<<dat_name<<endl;
        ifstream inf(dat_name); // read the file with ifstream and save to inf
        ifstream infs(s_dat_name);  // read the file with ifstream and save to inf
        ifstream inf8(dat_name8); // read the file with ifstream and save to inf
        ifstream inf48(dat_name48); // read the file with ifstream and save to inf
        
        if( !inf && !infs && !inf8 && !inf48 ){
            cerr<<"Error: can't read part. There is no such part."<<endl;
            exit(1);
        }
        // read, save
        while( getline(inf, line)||getline(infs, line)||getline(inf8, line)||getline(inf48, line) ){   // use getline to save each line from 'inf' to 'line', one at a time.
            istringstream iss(line);// istringstream helps 'line'(string) transform into 'iss'(stream).
            if (iss >> type >> color) {
                if(type==0){
                    if(color == name){
                        iss >> bfc_operation;
                        if(bfc_operation == namewithPath)
                            real=true;
                            
                        color.clear();
                        bfc_operation.clear();
                        corcc.clear();
                    }
                    if(color == bfc){
                        iss >> bfc_operation;
                        /*
                        if(bfc_operation==ccw){
                            if(invertYN)
                                cclockwise=false;
                            else
                                cclockwise=true;
                            
                            invertYN=false;
                        }
                        if(bfc_operation==cw){
                            if(invertYN)
                                cclockwise=true;
                            else
                                cclockwise=false;
                            
                            invertYN=false;
                        }*/
                        
                        if(bfc_operation==invertnext){
                            //if(invertYN==true) invertYN=false;
                            //else invertYN=true;
                            invertYN=true;
                            color.clear();
                            bfc_operation.clear();
                            corcc.clear();
                        }
                        if(bfc_operation==certify){
                            iss >> corcc;
                            /*
                            if(corcc==ccw){
                                if(invertYN==true){
                                    cclockwise=false;
                                    cout<<"1"<<"\n";
                                }
                                else{
                                    cclockwise=true;
                                    cout<<"02 "<<part_name<<"\n";
                                }
                                invertYN=false;
                                
                            }
                            if(corcc==cw){
                                cout<<"cc"<<"\n";
                                if(invertYN==true){
                                    cclockwise=true;
                                }
                                else{
                                    cclockwise=false;
                                }
                                invertYN=false;
                            }*/
                            invertYN=false;
                            color.clear();
                            bfc_operation.clear();
                            corcc.clear();
                            //cout<<"{ "<<corcc<<" }"<<endl;
                        }
                        
                    }
                    if(color==brick){
                        stud_for_brick=true;
                        //cout<<"brick"<<"\n";
                    }
                    if(color==plate){
                        stud_for_plate=true;
                        //cout<<"plate"<<"\n";
                    }
                }
                
                if(type==1&&real){
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
                    
                    vertex tmp_cnnc;
                    if(fninf==stud){
                        tmp_cnnc.x = metrix_V[0]*rate;
                        tmp_cnnc.y = metrix_V[1]*rate;
                        tmp_cnnc.z = metrix_V[2]*rate;
                        tmp_cnnc.num = 1;
                        cnnc_x.push_back(tmp_cnnc);
                        if(stud_for_brick){
                            //float add = metrix[1]+24.0;
                            tmp_cnnc.x = metrix_V[0]*rate;
                            tmp_cnnc.y =(array_O[6]*metrix[0]+array_O[7]*(metrix[1]+24.0)+array_O[8]*metrix[2]+ array_O[1])*rate;
                            tmp_cnnc.z = metrix_V[2]*rate;
                            tmp_cnnc.num = 1;
                            cnnc_o.push_back(tmp_cnnc);
                        }
                        if(stud_for_plate){
                            tmp_cnnc.x = metrix_V[0]*rate;
                            tmp_cnnc.y =(array_O[6]*metrix[0]+array_O[7]*(metrix[1]+8.0)+array_O[8]*metrix[2]+ array_O[1])*rate;
                            tmp_cnnc.z = metrix_V[2]*rate;
                            tmp_cnnc.num = 1;
                            cnnc_o.push_back(tmp_cnnc);
                        }
                    }
                    if(fninf==stud2){
                        tmp_cnnc.x = metrix_V[0]*rate;
                        tmp_cnnc.y = metrix_V[1]*rate;
                        tmp_cnnc.z = metrix_V[2]*rate;
                        tmp_cnnc.num = 1;
                        cnnc_x.push_back(tmp_cnnc);
                        /*
                         if(stud_for_brick){
                         tmp_cnnc.x = metrix_V[0]*rate;
                         tmp_cnnc.y = (metrix_V[1]+24)*rate;
                         tmp_cnnc.z = metrix_V[2]*rate;
                         tmp_cnnc.num = 1;
                         cnnc_o.push_back(tmp_cnnc);
                         }
                         */
                        if(stud_for_plate){
                            tmp_cnnc.x = metrix_V[0]*rate;
                            tmp_cnnc.y =(array_O[6]*metrix[0]+array_O[7]*(metrix[1]+8.0)+array_O[8]*metrix[2]+ array_O[1])*rate;
                            tmp_cnnc.z = metrix_V[2]*rate;
                            tmp_cnnc.num = 1;
                            cnnc_o.push_back(tmp_cnnc);
                        }
                    }
                    if(fninf==stud2a){
                        tmp_cnnc.x=metrix_V[0]*rate;
                        tmp_cnnc.y=metrix_V[1]*rate;
                        tmp_cnnc.z=metrix_V[2]*rate;
                        tmp_cnnc.num = 1;
                        cnnc_x.push_back(tmp_cnnc);
                        
                        if(stud_for_brick && (part_name == p4733) && once){
                            tmp_cnnc.x = metrix_V[0]*rate;
                            tmp_cnnc.y =(array_O[6]*metrix[0]+array_O[7]*(metrix[1]+24.0)+array_O[8]*metrix[2]+ array_O[1])*rate;
                            tmp_cnnc.z = metrix_V[2]*rate;
                            tmp_cnnc.num = 1;
                            cnnc_o.push_back(tmp_cnnc);
                            once=false;
                        }
                        if(stud_for_plate){
                            tmp_cnnc.x = metrix_V[0]*rate;
                            tmp_cnnc.y =(array_O[6]*metrix[0]+array_O[7]*(metrix[1]+8.0)+array_O[8]*metrix[2]+ array_O[1])*rate;
                            tmp_cnnc.z = metrix_V[2]*rate;
                            tmp_cnnc.num = 1;
                            cnnc_o.push_back(tmp_cnnc);
                        }
                    }
                    if(fninf==sphe88){
                        tmp_cnnc.x=metrix_V[0]*rate;
                        tmp_cnnc.y=metrix_V[1]*rate;
                        tmp_cnnc.z=metrix_V[2]*rate;
                        tmp_cnnc.num = 2;
                        cnnc_x.push_back(tmp_cnnc);
                        
                    }
                    if(fninf==fri8socket){
                        tmp_cnnc.x=metrix_V[0]*rate;
                        tmp_cnnc.y=metrix_V[1]*rate;
                        tmp_cnnc.z=metrix_V[2]*rate;
                        tmp_cnnc.num = 2;
                        cnnc_o.push_back(tmp_cnnc);
                        
                    }

                    /*
                     cout << metrix_V[3] <<' '<< metrix_V[4] <<' '<< metrix_V[5] <<' '<< metrix_V[0] << endl;
                     cout << metrix_V[6] <<' '<< metrix_V[7] <<' '<< metrix_V[8] <<' '<< metrix_V[1] << endl;
                     cout << metrix_V[9] <<' '<< metrix_V[10] <<' '<< metrix_V[11] <<' '<< metrix_V[2] << endl;
                     cout << endl;
                     */
                    
                    /*
                    if(invertC>0&&(invertC%2==1))
                        search_or_read( fninf, true, metrix, true);//same geo_storage space as father
                    if(invertC>0){
                        
                        invertYN=false;
                        invertC=invertC-1;
                    }*/
                    cout<<fninf<<'\n';
                    search_or_read( fninf, true, metrix, invertYN, fninf);
                    
                    //invertYN=false;
                    
                    if(invertYN==true) invertYN=false;
                    cout<<"003"<<'\n';
                    //else invertYN=true;
                    
                    //metrix_V[0]=0; metrix_V[1]=0; metrix_V[2]=0; metrix_V[3]=1; metrix_V[4]=0; metrix_V[5]=0;
                    //metrix_V[6]=0; metrix_V[7]=1; metrix_V[8]=0; metrix_V[9]=0; metrix_V[10]=0; metrix_V[11]=1;
                }
                if(type==2&&real){
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
                    
                    dot1.x = metrix[0];     dot1.y = metrix[1];     dot1.z = metrix[2]; // dot1 = x1y1z1
                    dot2.x = metrix[3];     dot2.y = metrix[4];     dot2.z = metrix[5]; // dot2 = x2y2z2
                    
                    tri.v1 = dot1;
                    tri.v2 = dot2;
                    tri.v3 = dot2; // if it's a line, v2=v3.
                    
                    normalt.x = 0.0;
                    normalt.y = 0.0;
                    normalt.z = 0.0;
                    
                    //cout << metrix_V[0] <<' '<< metrix_V[1] <<' '<< metrix_V[2] << endl;
                    //push_back
                    
                    /*
                    tmpNormalPool.push_back(normalt);
                    trianglePool.push_back(tri);
                    */
                     
                    //parttmp.tpfp.push_back(tri);
                    //metrix_V[0]=0; metrix_V[1]=0; metrix_V[2]=0; metrix_V[3]=1; metrix_V[4]=0; metrix_V[5]=0;
                    //metrix_V[6]=0; metrix_V[7]=1; metrix_V[8]=0; metrix_V[9]=0; metrix_V[10]=0; metrix_V[11]=1;
                }
                if(type==3&&real){
                    cout<<"T"<<'\n';
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
                    
                    dot1.x = metrix[0];     dot1.y = metrix[1];     dot1.z = metrix[2]; // dot1 = x1y1z1
                    dot2.x = metrix[3];     dot2.y = metrix[4];     dot2.z = metrix[5]; // dot2 = x2y2z2
                    dot3.x = metrix[6];     dot3.y = metrix[7];     dot3.z = metrix[8]; // dot3 = x3y3z3
                    
                    //if(cclockwise){
                        //counter clockwise
                        cout<<"ccw"<<'\n';
                        tri.v1 = dot1;
                        tri.v2 = dot2;
                        tri.v3 = dot3;
                        
                        normalt.x = (dot2.y-dot1.y)*(dot3.z-dot1.z)-(dot3.y-dot1.y)*(dot2.z-dot1.z);
                        normalt.y = (dot2.z-dot1.z)*(dot3.x-dot1.x)-(dot3.z-dot1.z)*(dot2.x-dot1.x);
                        normalt.z = (dot2.x-dot1.x)*(dot3.y-dot1.y)-(dot3.x-dot1.x)*(dot2.y-dot1.y);
                        tmpNormalPool.push_back(normalt);
                        trianglePool.push_back(tri);
                    //}
                    //else{
                        //clockwise
                        cout<<"cw"<<'\n';
                        tri.v1 = dot3;
                        tri.v2 = dot2;
                        tri.v3 = dot1;
                        
                        //clockwise
                        normalt.x = (dot3.y-dot1.y)*(dot2.z-dot1.z)-(dot2.y-dot1.y)*(dot3.z-dot1.z);
                        normalt.y = (dot3.z-dot1.z)*(dot2.x-dot1.x)-(dot2.z-dot1.z)*(dot3.x-dot1.x);
                        normalt.z = (dot3.x-dot1.x)*(dot2.y-dot1.y)-(dot2.x-dot1.x)*(dot3.y-dot1.y);
                        tmpNormalPool.push_back(normalt);
                        trianglePool.push_back(tri);
                    //}
                    
                }
                if(type==4&&real){
                    cout<<"Q"<<'\n';
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
                    
                    dot1.x = metrix[0];     dot1.y = metrix[1];     dot1.z = metrix[2]; // dot1 = x1y1z1
                    dot2.x = metrix[3];     dot2.y = metrix[4];     dot2.z = metrix[5]; // dot2 = x2y2z2
                    dot3.x = metrix[6];     dot3.y = metrix[7];     dot3.z = metrix[8]; // dot3 = x3y3z3
                    
                    
                    //if(cclockwise){
                        cout<<"ccw"<<'\n';
                        tri.v1 = dot1;
                        tri.v2 = dot2;
                        tri.v3 = dot3;
                        //cclockwise
                         normalt.x = (dot2.y-dot1.y)*(dot3.z-dot1.z)-(dot3.y-dot1.y)*(dot2.z-dot1.z);
                         normalt.y = (dot2.z-dot1.z)*(dot3.x-dot1.x)-(dot3.z-dot1.z)*(dot2.x-dot1.x);
                         normalt.z = (dot2.x-dot1.x)*(dot3.y-dot1.y)-(dot3.x-dot1.x)*(dot2.y-dot1.y);
                        tmpNormalPool.push_back(normalt);
                        trianglePool.push_back(tri);
                    //}
                    //else{
                        cout<<"cw"<<'\n';
                        tri.v1 = dot3;
                        tri.v2 = dot2;
                        tri.v3 = dot1;
                        //clockwise
                         normalt.x = (dot3.y-dot1.y)*(dot2.z-dot1.z)-(dot2.y-dot1.y)*(dot3.z-dot1.z);
                         normalt.y = (dot3.z-dot1.z)*(dot2.x-dot1.x)-(dot2.z-dot1.z)*(dot3.x-dot1.x);
                         normalt.z = (dot3.x-dot1.x)*(dot2.y-dot1.y)-(dot2.x-dot1.x)*(dot3.y-dot1.y);
                        tmpNormalPool.push_back(normalt);
                        trianglePool.push_back(tri);
                    //}
                    
                    
                    dot1.x = metrix[6];     dot1.y = metrix[7];     dot1.z = metrix[8]; // dot1 = x3y3z3
                    dot2.x = metrix[9];     dot2.y = metrix[10];    dot2.z = metrix[11];// dot2 = x4y4z4
                    dot3.x = metrix[0];     dot3.y = metrix[1];     dot3.z = metrix[2]; // dot3 = x1y1z1
                    
                    
                    //if(cclockwise){
                        cout<<"ccw"<<'\n';
                        tri.v1 = dot1;
                        tri.v2 = dot2;
                        tri.v3 = dot3;
                        //cclockwise
                        normalt.x = (dot2.y-dot1.y)*(dot3.z-dot1.z)-(dot3.y-dot1.y)*(dot2.z-dot1.z);
                        normalt.y = (dot2.z-dot1.z)*(dot3.x-dot1.x)-(dot3.z-dot1.z)*(dot2.x-dot1.x);
                        normalt.z = (dot2.x-dot1.x)*(dot3.y-dot1.y)-(dot3.x-dot1.x)*(dot2.y-dot1.y);
                        tmpNormalPool.push_back(normalt);
                        trianglePool.push_back(tri);
                    //}
                    
                    //else{
                        cout<<"cw"<<'\n';
                        tri.v1 = dot3;
                        tri.v2 = dot2;
                        tri.v3 = dot1;
                        //clockwise
                        normalt.x = (dot3.y-dot1.y)*(dot2.z-dot1.z)-(dot2.y-dot1.y)*(dot3.z-dot1.z);
                        normalt.y = (dot3.z-dot1.z)*(dot2.x-dot1.x)-(dot2.z-dot1.z)*(dot3.x-dot1.x);
                        normalt.z = (dot3.x-dot1.x)*(dot2.y-dot1.y)-(dot2.x-dot1.x)*(dot3.y-dot1.y);
                        tmpNormalPool.push_back(normalt);
                        trianglePool.push_back(tri);
                    //}
                    
                }
                //cout<<type<<endl;
                /*
                if(type==5){
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
                    
                    dot1.x = metrix[0];     dot1.y = metrix[1];     dot1.z = metrix[2]; // dot1 = x1y1z1
                    dot2.x = metrix[3];     dot2.y = metrix[4];     dot2.z = metrix[5]; // dot2 = x2y2z2
                    dot3.x = metrix[6];     dot3.y = metrix[7];     dot3.z = metrix[8]; // dot3 = x3y3z3
                    dot4.x = metrix[9];     dot4.y = metrix[10];     dot4.z = metrix[11]; // dot2 = x2y2z2
                    
                    edg.v1 = dot1;
                    edg.v2 = dot2;
                    
                    //push_back
                    edgePool.push_back(edg);
                    //parttmp.tpfp.push_back(tri);
                    
                    edg.v1 = dot4;
                    edg.v2 = dot3;
                    
                    //push_back
                    //edgePool.push_back(edg);

                    
                    
                }
                */
            }
            
        }
        //parts.push_back(parttmp);
        real=false;
    }
    //---- else end -------------------------------------
}

void load_lego_parts_list( char *part_list ){ //load lego parts from the list
    
    string line;
    string name;
    
    ifstream inf(part_list);    // read the file with ifstream and save to inf
    
    if(!inf){
        cerr<<"Error: can't got the list."<<endl;
        exit(1);
    }
    else{
        cout<<"We got the list."<<endl;
    }
    // read, save
    while(getline(inf, line)){  // use getline to save each line from 'inf' to 'line', one at a time.
        istringstream iss(line);
        iss >> name;
        search_or_read(name, true, metrix_O, false, name);
    }
}

void load(){}

//Storage vectors list of ABCD.obj input
std::vector<vertex> obj_vPool;        //all vertex in obj
std::vector<vertex> obj_vNormal;    //all vertex normal of obj
std::vector<face> obj_fPool;        //all faces in obj [face: A plane shape that has 4 or 3 point.]

std::vector<triangle> obj_tPool;    //all triangles in obj
std::vector<face> obj_f3Pool;        //all faces in obj [based on the triangles in obj_tPool]
std::vector<vertex> obj_normals;    //normals of triangles in obj_tPool
std::vector< vector <int> > obj_v_triP;

//Storage vector of Voxel
std::vector< vertex > voxel_candidate;         //ver 3.0
std::vector< vertex > voxel_center_vPool;    //ver 3.0 & 2.0 & 1.0
std::vector< vertex > x_strap;                //ver 3.0 & 2.0
std::vector< vector <vertex> > all_xy_strap;//ver 3.0 & 2.0
std::vector< vertex > voxel_bone_position;  //ver 3.0
std::vector< vertex > stuffing_vPool;       //ver 3.0

bool in_voxel(vertex test, vertex voxel_center, float radius){
    //float radius = edge_length*0.5;
    if( abs(test.x - voxel_center.x)<=radius
       && abs(test.y - voxel_center.y)<=radius
       && abs(test.z - voxel_center.z)<=radius
       )return true;
    
    return false;
}

bool areSameVertex(vertex v1, vertex v2){
    if(v1.x==v2.x && v1.y==v2.y && v1.z==v2.z)
        return true;
    return false;
}

void read_obj(){
    std::string line;
    char section[20];
    char faceSec[5];
    vertex v;
    face f;
    triangle tri;
    cout<<"Start to get obj"<<'\n';
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
    
    for(int i=0; i<obj_vPool.size(); i++){
        vector<int> tmm;
        obj_v_triP.push_back(tmm);
    }
    cout<< "Size of obj_v_triP "<< obj_v_triP.size() << '\n';
    
    face f3tmp;
    int tpnum=0;
    for(int i=0; i<obj_fPool.size(); i++){
        if(obj_fPool[i].v4!=0){
            
            tri.v1 = obj_vPool[ obj_fPool[i].v1-1 ];
            tri.v2 = obj_vPool[ obj_fPool[i].v2-1 ];
            tri.v3 = obj_vPool[ obj_fPool[i].v3-1 ];
            
            tri.n1 = obj_fPool[i].v1-1;
            tri.n2 = obj_fPool[i].v2-1;
            tri.n3 = obj_fPool[i].v3-1;
            
            obj_v_triP[ obj_fPool[i].v1-1 ].push_back(tpnum);
            obj_v_triP[ obj_fPool[i].v2-1 ].push_back(tpnum);
            obj_v_triP[ obj_fPool[i].v3-1 ].push_back(tpnum);
            tpnum++;
            
            obj_tPool.push_back(tri);
            
            f3tmp.v1 = obj_fPool[i].v1 ;
            f3tmp.v2 = obj_fPool[i].v2 ;
            f3tmp.v3 = obj_fPool[i].v3 ;
            
            obj_f3Pool.push_back(f3tmp);
            
            
            tri.v1 = obj_vPool[ obj_fPool[i].v1-1 ];
            tri.v2 = obj_vPool[ obj_fPool[i].v3-1 ];
            tri.v3 = obj_vPool[ obj_fPool[i].v4-1 ];
            
            tri.n1 = obj_fPool[i].v1-1;
            tri.n2 = obj_fPool[i].v3-1;
            tri.n3 = obj_fPool[i].v4-1;
            
            obj_v_triP[ obj_fPool[i].v1-1 ].push_back(tpnum);
            obj_v_triP[ obj_fPool[i].v3-1 ].push_back(tpnum);
            obj_v_triP[ obj_fPool[i].v4-1 ].push_back(tpnum);
            tpnum++;
            
            obj_tPool.push_back(tri);
            
            f3tmp.v1 = obj_fPool[i].v1 ;
            f3tmp.v2 = obj_fPool[i].v3 ;
            f3tmp.v3 = obj_fPool[i].v4 ;
            
            obj_f3Pool.push_back(f3tmp);
        }
        else{
            
            tri.v1 = obj_vPool[ obj_fPool[i].v1-1 ];
            tri.v2 = obj_vPool[ obj_fPool[i].v2-1 ];
            tri.v3 = obj_vPool[ obj_fPool[i].v3-1 ];
            
            tri.n1 = obj_fPool[i].v1-1;
            tri.n2 = obj_fPool[i].v2-1;
            tri.n3 = obj_fPool[i].v3-1;
            
            obj_v_triP[ obj_fPool[i].v1-1 ].push_back(tpnum);
            obj_v_triP[ obj_fPool[i].v2-1 ].push_back(tpnum);
            obj_v_triP[ obj_fPool[i].v3-1 ].push_back(tpnum);
            tpnum++;
            
            obj_tPool.push_back(tri);
            
            f3tmp.v1 = obj_fPool[i].v1 ;
            f3tmp.v2 = obj_fPool[i].v2 ;
            f3tmp.v3 = obj_fPool[i].v3 ;
            
            obj_f3Pool.push_back(f3tmp);
        }
    }
    
    //cout<< "obj_v_triP[0][0] "<< obj_v_triP[0][0] << " obj_v_triP[0][1] "<< obj_v_triP[0][1] << '\n';
    //cout<< "obj_tPool[0].n2 "<< obj_tPool[0].n2 << " obj_tPool[1].n3 "<< obj_tPool[1].n3 << '\n';
    //cout<< "obj_tPool[96].n2 "<< obj_tPool[96].n2 << " obj_tPool[96].n3 "<< obj_tPool[96].n3 << '\n';
    //cout<< "obj_tPool[112].n2 "<< obj_tPool[112].n2 << " obj_tPool[112].n3 "<< obj_tPool[112].n3 << '\n';
    
    //creat normal from obj model
    vertex normal;
    //creat normal for each triangle
    for(int i=0; i<obj_tPool.size(); i++){
        normal.x = (obj_tPool[i].v2.y-obj_tPool[i].v1.y)*(obj_tPool[i].v3.z-obj_tPool[i].v1.z)
        -(obj_tPool[i].v3.y-obj_tPool[i].v1.y)*(obj_tPool[i].v2.z-obj_tPool[i].v1.z);
        normal.y = (obj_tPool[i].v2.z-obj_tPool[i].v1.z)*(obj_tPool[i].v3.x-obj_tPool[i].v1.x)
        -(obj_tPool[i].v3.z-obj_tPool[i].v1.z)*(obj_tPool[i].v2.x-obj_tPool[i].v1.x);
        normal.z = (obj_tPool[i].v2.x-obj_tPool[i].v1.x)*(obj_tPool[i].v3.y-obj_tPool[i].v1.y)
        -(obj_tPool[i].v3.x-obj_tPool[i].v1.x)*(obj_tPool[i].v2.y-obj_tPool[i].v1.y);
        obj_normals.push_back(normal);
    }
    
    //creat normal for each vertex
    for(int i=0; i<obj_vPool.size(); i++){
        obj_vNormal.push_back(obj_vPool[i]);
    }
    for(int i=0; i<obj_f3Pool.size(); i++){
        obj_vNormal[obj_f3Pool[i].v1-1] = obj_normals[obj_f3Pool[i].v1-1];
        obj_vNormal[obj_f3Pool[i].v2-1] = obj_normals[obj_f3Pool[i].v2-1];
        obj_vNormal[obj_f3Pool[i].v3-1] = obj_normals[obj_f3Pool[i].v3-1];
    }
    
    
    //voxelize the model
    float max_x = 0.0;
    float min_x = 0.0;
    float max_y = 0.0;
    float min_y = 0.0;
    float max_z = 0.0;
    float min_z = 0.0;
    
    if( 1 <= obj_vPool.size() ){
        max_x = obj_vPool[0].x;
        min_x = obj_vPool[0].x;
        max_y = obj_vPool[0].y;
        min_y = obj_vPool[0].y;
        max_z = obj_vPool[0].z;
        min_z = obj_vPool[0].z;
    }
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
    
    //max_x, max_y, max_z, mix_x, mix_y, mix_z
    //are from [obj_vPool]
    
    max_x = max_x - min_x;//use max to replace range of whole model
    max_y = max_y - min_y;//..
    max_z = max_z - min_z;//..
    
    //float voxel_length;
    float midx, midy, midz;
    midx = min_x + max_x*0.5;
    midy = min_y + max_y*0.5;
    midz = min_z + max_z*0.5;
    
    int hxn = (midx-min_x)/voxel_length;
    int hyn = (midy-min_y)/voxel_length;
    int hzn = (midz-min_z)/voxel_length;
    
    int xn = max_x/voxel_length + 1;
    int yn = max_y/voxel_length + 1;
    int zn = max_z/voxel_length + 1;
    
    cout<<endl;
    cout<< xn <<" "<< yn <<" "<< zn <<endl;
    
    //saved for stuffing()
    obmi_x = min_x;
    obmi_y = min_y;
    obmi_z = min_z;
    
    obn_x = xn;
    obn_y = yn;
    obn_z = zn;
    
    if( (midx - hxn*voxel_length - min_x) < voxel_length_half )
    {    min_x = midx - hxn*voxel_length - voxel_length_half; }
    else{    min_x = midx - (hxn+1)*voxel_length; }
    
    if( (midy - hyn*voxel_length - min_y) < voxel_length_half )
    {    min_y = midy - hyn*voxel_length - voxel_length_half; }
    else{    min_y = midy - (hyn+1)*voxel_length; }
    
    if( (midz - hzn*voxel_length - min_z) < voxel_length_half )
    {    min_z = midz - hzn*voxel_length - voxel_length_half; }
    else{    min_z = midz - (hzn+1)*voxel_length; }
    
    /*
     min_x = min_x - voxel_length_half;
     min_y = min_y - voxel_length_half;
     min_z = min_z - voxel_length_half;
     */
    
    //Voxel Ver.3
    
    for(int i=0; i<(xn*yn); i++){//creat enough space for all_xy_strap
        all_xy_strap.push_back(x_strap);
    }
    //xn, yn, zn are number of voxel on the 3d border
    
    for(int i=0; i<obj_tPool.size(); i++){
        //get voxel line
        float max_tx, max_ty, max_tz, min_tx, min_ty, min_tz;
        max_tx = obj_tPool[i].v1.x;if(max_tx < obj_tPool[i].v2.x) max_tx = obj_tPool[i].v2.x;if(max_tx < obj_tPool[i].v3.x) max_tx = obj_tPool[i].v3.x;
        max_ty = obj_tPool[i].v1.y;if(max_ty < obj_tPool[i].v2.y) max_ty = obj_tPool[i].v2.y;if(max_ty < obj_tPool[i].v3.y) max_ty = obj_tPool[i].v3.y;
        max_tz = obj_tPool[i].v1.z;if(max_tz < obj_tPool[i].v2.z) max_tz = obj_tPool[i].v2.z;if(max_tz < obj_tPool[i].v3.z) max_tz = obj_tPool[i].v3.z;
        min_tx = obj_tPool[i].v1.x;if(min_tx > obj_tPool[i].v2.x) min_tx = obj_tPool[i].v2.x;if(min_tx > obj_tPool[i].v3.x) min_tx = obj_tPool[i].v3.x;
        min_ty = obj_tPool[i].v1.y;if(min_ty > obj_tPool[i].v2.y) min_ty = obj_tPool[i].v2.y;if(min_ty > obj_tPool[i].v3.y) min_ty = obj_tPool[i].v3.y;
        min_tz = obj_tPool[i].v1.z;if(min_tz > obj_tPool[i].v2.z) min_tz = obj_tPool[i].v2.z;if(min_tz > obj_tPool[i].v3.z) min_tz = obj_tPool[i].v3.z;
        
        vertex vc12;
        vc12.x = obj_tPool[i].v2.x-obj_tPool[i].v1.x;
        vc12.y = obj_tPool[i].v2.y-obj_tPool[i].v1.y;
        vc12.z = obj_tPool[i].v2.z-obj_tPool[i].v1.z;
        
        vertex vc13;
        vc13.x = obj_tPool[i].v3.x-obj_tPool[i].v1.x;
        vc13.y = obj_tPool[i].v3.y-obj_tPool[i].v1.y;
        vc13.z = obj_tPool[i].v3.z-obj_tPool[i].v1.z;
        
        vertex vc23;
        vc23.x = obj_tPool[i].v3.x-obj_tPool[i].v2.x;
        vc23.y = obj_tPool[i].v3.y-obj_tPool[i].v2.y;
        vc23.z = obj_tPool[i].v3.z-obj_tPool[i].v2.z;
        
        float max;
        int time;
        int col, row, dep;
        vertex vonline;
        vonline.num = i;
        //line 1 to 2
        max=abs(vc12.x); if(max < abs(vc12.y))max = abs(vc12.y); if(max < abs(vc12.z))max = abs(vc12.z);
        time = max/voxel_length + 1;
        vc12.x = vc12.x / time;
        vc12.y = vc12.y / time;
        vc12.z = vc12.z / time;
        //cout<<vc12.x<<"\n";
        
        for(int j=0; j<time-1;j++){
            vonline.x = obj_tPool[i].v1.x + vc12.x*j;
            vonline.y = obj_tPool[i].v1.y + vc12.y*j;
            vonline.z = obj_tPool[i].v1.z + vc12.z*j;
            
            col = (vonline.x - min_x)/voxel_length;// x
            row = (vonline.y - min_y)/voxel_length;// y
            dep = (vonline.z - min_z)/voxel_length;// z
            
            if( (col + row * xn) < all_xy_strap.size() )
                all_xy_strap[ col + row * xn ].push_back(vonline);
            
        }
        //line 3 to 1
        max=abs(vc13.x); if(max < abs(vc13.y))max = abs(vc13.y); if(max < abs(vc13.z))max = abs(vc13.z);
        time = max/voxel_length + 1;
        vc13.x = vc13.x / time;
        vc13.y = vc13.y / time;
        vc13.z = vc13.z / time;
        
        for(int j=0; j<time-1;j++){
            vonline.x = obj_tPool[i].v3.x - vc13.x*j;
            vonline.y = obj_tPool[i].v3.y - vc13.y*j;
            vonline.z = obj_tPool[i].v3.z - vc13.z*j;
            
            col = (vonline.x - min_x)/voxel_length;// x
            row = (vonline.y - min_y)/voxel_length;// y
            dep = (vonline.z - min_z)/voxel_length;// z
            
            if( (col + row * xn) < all_xy_strap.size() )
                all_xy_strap[ col + row * xn ].push_back(vonline);
            
        }
        //line 2 to 3
        
        max=abs(vc23.x); if(max < abs(vc23.y))max = abs(vc23.y); if(max < abs(vc23.z))max = abs(vc23.z);
        time = max/voxel_length + 1;
        vc23.x = vc23.x / time;
        vc23.y = vc23.y / time;
        vc23.z = vc23.z / time;
        
        for(int j=0; j<time-1;j++){
            vonline.x = obj_tPool[i].v2.x + vc23.x*j;
            vonline.y = obj_tPool[i].v2.y + vc23.y*j;
            vonline.z = obj_tPool[i].v2.z + vc23.z*j;
            
            col = (vonline.x - min_x)/voxel_length;// x
            row = (vonline.y - min_y)/voxel_length;// y
            dep = (vonline.z - min_z)/voxel_length;// z
            
            if( (col + row * xn) < all_xy_strap.size() )
                all_xy_strap[ col + row * xn ].push_back(vonline);
            
        }
        
        
        //get voxels on triangle's face
        max_tx = max_tx - min_tx;//use max to replace range of whole model
        max_ty = max_ty - min_ty;//..
        max_tz = max_tz - min_tz;//..
        
        int xt = max_tx/voxel_length + 1;
        int yt = max_ty/voxel_length + 1;
        int zt = max_tz/voxel_length + 1;
        
        int xtt= (min_tx - min_x)/voxel_length;
        int ytt= (min_ty - min_y)/voxel_length;
        int ztt= (min_tz - min_z)/voxel_length;
        //xtt 並不是多算，而是利用float轉int的特性，去對齊voxel，ytt 也是如此
        
        if(xtt<0)xtt=0; if(ytt<0)ytt=0; if(ztt<0)ztt=0;
        //if  x || y || z  <0 ,  x || y || z  =0
        
        min_tx = min_x + xtt*voxel_length + voxel_length_half;
        min_ty = min_y + ytt*voxel_length + voxel_length_half;
        min_tz = min_z + ztt*voxel_length + voxel_length_half;
        
        vertex candidate;
        candidate.num = i;
        
        for(int j=0; j<xt; j++){
            for(int k=0; k<yt; k++){
                for(int l=0; l<zt; l++){
                    //candidate.x = min_tx + (j%xt)*voxel_length;
                    //candidate.y = min_ty + (j/xt)*voxel_length;
                    //candidate.z = min_tz + (j/(xt*yt))*voxel_length;
                    //voxel_candidate.push_back(candidate);
                    
                    candidate.x = min_tx + j*voxel_length;
                    candidate.y = min_ty + k*voxel_length;
                    candidate.z = min_tz + l*voxel_length;
                    
                    voxel_candidate.push_back(candidate);
                    //voxel_center_vPool.push_back(candidate);
                }
            }
        }
        for(int j=0; j<voxel_candidate.size(); j++){
            if(
               (!outsideTheTriangle(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3) && !outsideTheTrianglezy(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3)) ||
               (!outsideTheTrianglezy(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3) && !outsideTheTrianglexz(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3)) ||
               (!outsideTheTrianglexz(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3) && !outsideTheTriangle(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3)) ||
               ( xt<2 && !outsideTheTrianglezy(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3) && abs( voxel_candidate[j].x - obj_tPool[i].v1.x )<=voxel_length_half ) ||
               ( yt<2 && !outsideTheTrianglexz(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3) && abs( voxel_candidate[j].y - obj_tPool[i].v1.y )<=voxel_length_half ) ||
               ( zt<2 && !outsideTheTriangle(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3) && abs( voxel_candidate[j].z - obj_tPool[i].v1.z )<=voxel_length_half )
               ){
                /*
                 float nxz = obj_normals[i].x / obj_normals[i].z;
                 float nyz = obj_normals[i].y / obj_normals[i].z;
                 voxel_candidate[j].z =
                 (obj_tPool[i].v1.x - voxel_candidate[j].x)*nxz +
                 (obj_tPool[i].v1.y - voxel_candidate[j].y)*nyz +
                 obj_tPool[i].v1.z;*/
                //push
                
                voxel_center_vPool.push_back(voxel_candidate[j]);
                
                //xt, yt, zt are numbers in each triangle
                //xtt = xt-xm
                
                if((xtt+j%xt) + (ytt+j/xt)*xn < all_xy_strap.size() )
                    all_xy_strap[ (xtt+j%xt) + (ytt+j/xt)*xn ].push_back(voxel_candidate[j]);
            }
        }
        
    }
    //cout<< "obj_v_triP[0][0] "<< obj_v_triP[0][0] << " obj_v_triP[0][1] "<< obj_v_triP[0][1] << '\n';
    //cout<< "obj_tPool[0].n2 "<< obj_tPool[0].n2 << " obj_tPool[1].n3 "<< obj_tPool[1].n3 << '\n';
    //cout<< "obj_tPool[96].n2 "<< obj_tPool[96].n2 << " obj_tPool[96].n3 "<< obj_tPool[96].n3 << '\n';
    //cout<< "obj_tPool[112].n2 "<< obj_tPool[112].n2 << " obj_tPool[112].n3 "<< obj_tPool[112].n3 << '\n';
    
    //Take out voxels from [all_xy_strap]
    //and save voxels to [voxel_center_vPool].
    int start;
    int end;
    bool recording = false;
    vertex voxel_center_tmp;
    for(int i=0; i<all_xy_strap.size(); i++){
        for(int j=0; j<all_xy_strap[i].size(); j++){
            
            int k;
            k = (all_xy_strap[i][j].z- min_z) / voxel_length;
            //define the voxel's position
            voxel_center_tmp.x = (i%xn)*voxel_length + min_x + voxel_length_half;
            voxel_center_tmp.y = (i/xn)*voxel_length + min_y + voxel_length_half;
            voxel_center_tmp.z =      k*voxel_length + min_z + voxel_length_half;
            voxel_center_tmp.num = all_xy_strap[i][j].num;
            //push the voxel into voxel pool
            voxel_center_vPool.push_back(voxel_center_tmp);
        }
        recording = false;
        start = 0;
        end = 0;
    }
    
    //find inside frame
    /*
    int top, buttom, left, right, front, hind;
    bool t_found = false;
    bool b_found = false;
    bool l_found = false;
    bool r_found = false;
    bool f_found = false;
    bool h_found = false;
    */
    
    //remove same point in voxel_center_vPool
    vertex vs;
    for(int i=0; i<voxel_center_vPool.size(); i++){
        for(int j=i+1; j<voxel_center_vPool.size(); j++){
            if( areSameVertex( voxel_center_vPool[i], voxel_center_vPool[j] ) ){
                voxel_center_vPool[j] = voxel_center_vPool[voxel_center_vPool.size()-1];
                voxel_center_vPool.pop_back();
            }
        }
    }
    
    cout<< "\n";
    cout<< "size of voxel: " << voxel_center_vPool.size() <<"\n";
    
}

std::vector< vertex > ma_vPool;
std::vector< triangle > ma_ePool;
std::vector< triangle > ma_fPool;

void readMa(char *fileName){
    ifstream inMa(fileName);
    string line;
    string type;
    int nOv, nOe, nOf;
    int one, two, three;
    float mmx, mmy, mmz, rds;
    bool fileread = false;
    vertex vtx;
    triangle triMa;
    
    while(getline(inMa, line)){
        std::istringstream iss(line);
        if(fileread==false){
            iss >> nOv >> nOe >> nOf;
            fileread = true;
        }
        iss >> type;
        if(type[0] == 'v'){
            iss >> mmx >> mmy >> mmz >> rds;
            vtx.x = mmx;
            vtx.y = mmy;
            vtx.z = mmz;
            ma_vPool.push_back(vtx);
        }
        if(type[0] == 'e'){
            iss >> one >> two;
            triMa.v1 = ma_vPool[one];//ma檔是從0開始算，不像OBJ是從1開始
            triMa.v2 = ma_vPool[two];
            ma_ePool.push_back(triMa);
        }
        if(type[0] == 'f'){
            iss >> one >> two >> three;
            triMa.v1 = ma_vPool[one];
            triMa.v2 = ma_vPool[two];
            triMa.v3 = ma_vPool[three];
            ma_fPool.push_back(triMa);
        }
        
    }
    
    cout<<".ma size: "<< ma_vPool.size() <<" "<< ma_ePool.size() <<" "<< ma_fPool.size() <<"\n";
}

vector<vertex> randomlyPicked;
vector<short> randomlyPicked_color;
void randomPick_even(){
    for(int i=0; i<voxel_center_vPool.size(); i = i+rand()%10+ 3){ //10~30
        randomlyPicked.push_back(voxel_center_vPool[i]);
    }
    int j;
    for(int i=0; i<voxel_center_vPool.size(); i++){
        j=rand()%4;
        randomlyPicked_color.push_back(j);
    }
    cout<< "size of randomly picked: " << randomlyPicked.size() <<"\n";
}
void randomPick_symmetric(){
    ;
}

float max3(float f1, float f2, float f3){
    if(f1>=f2){
        if(f1>=f3)
            return f1;
        if(f1<f3)
            return f3;
    }
    if(f2>f1){
        if(f2>=f3)
            return f2;
        if(f2<f3)
            return f3;
    }
    return f1;
}
vector< matri > mpool_RandomPick;
void surface_arrange_random(){
    float va, vb, vc, max;
    int vertex_num;
    int n=0;
    
    for(int i=0; i<randomlyPicked.size(); i++){
        
        va =distanceBetween2V( obj_vPool[ obj_tPool[ randomlyPicked[i].num ].n1 ], randomlyPicked[i] );
        vb =distanceBetween2V( obj_vPool[ obj_tPool[ randomlyPicked[i].num ].n2 ], randomlyPicked[i] );
        vc =distanceBetween2V( obj_vPool[ obj_tPool[ randomlyPicked[i].num ].n3 ], randomlyPicked[i] );
        
        max = max3( va, vb, vc);
        
        n=0;
        if(max==va)n=1;
        if(max==vb)n=2;
        if(max==vc)n=3;
        
        if(n==1) vertex_num = obj_tPool[ randomlyPicked[i].num ].n1;
        if(n==2) vertex_num = obj_tPool[ randomlyPicked[i].num ].n2;
        if(n==3) vertex_num = obj_tPool[ randomlyPicked[i].num ].n3;
        
        if(n!=0){
            /*
             vector< vertex > vv_trip;
             //store vertex around randomlyPicked voxel center
             //into vv_trip
             for(int j=0; j<obj_v_triP[vertex_num].size(); j++){
             if(obj_tPool[ obj_v_triP[vertex_num][j] ].n1 != vertex_num){
             vv_trip.push_back( obj_tPool[ obj_v_triP[vertex_num][j] ].v1 );
             }
             if(obj_tPool[ obj_v_triP[vertex_num][j] ].n2 != vertex_num){
             vv_trip.push_back( obj_tPool[ obj_v_triP[vertex_num][j] ].v2 );
             }
             if(obj_tPool[ obj_v_triP[vertex_num][j] ].n3 != vertex_num){
             vv_trip.push_back( obj_tPool[ obj_v_triP[vertex_num][j] ].v3 );
             }
             }
             
             vertex noPV;//normal of picked vertex
             noPV.x = 0.0;
             noPV.y = 0.0;
             noPV.z = 0.0;
             for(int j=0; j<vv_trip.size(); j++){
             noPV.x = noPV.x + obj_vPool[vertex_num].x - vv_trip[j].x;
             noPV.y = noPV.y + obj_vPool[vertex_num].y - vv_trip[j].y;
             noPV.z = noPV.z + obj_vPool[vertex_num].z - vv_trip[j].z;
             }
             noPV.x = noPV.x/vv_trip.size();
             noPV.y = noPV.y/vv_trip.size();
             noPV.z = noPV.z/vv_trip.size();
             */
            
            vertex noPV;//normal of picked vertex
            noPV.x = obj_normals[ randomlyPicked[i].num ].x;
            noPV.y = obj_normals[ randomlyPicked[i].num ].y;
            noPV.z = obj_normals[ randomlyPicked[i].num ].z;
            
            vertex ny;
            ny.x = 0.0;        ny.y = 1.0;        ny.z = 0.0;
            
            vertex nny;
            nny.x = 0.0;    nny.y = -1.0;    nny.z = 0.0;
            
            vertex nz;
            nz.x = 0.0;     nz.y = 0.0;     nz.z = 1.0;
            
            vertex nnz;
            nnz.x = 0.0;    nnz.y = 0.0;    nnz.z = -1.0;
            
            vertex o;
            o.x = 0.0;      o.y = 0.0;      o.z = 0.0;
            
            vertex o2;
            o2.x = randomlyPicked[i].x;
            o2.y = randomlyPicked[i].y;
            o2.z = randomlyPicked[i].z;
            
            vertex nopv_xz;
            nopv_xz.x = noPV.x;
            nopv_xz.y = 0.0;
            nopv_xz.z = noPV.z;
            
            vertex nopv_xy;
            nopv_xy.x = noPV.x;
            nopv_xy.y = noPV.y;
            nopv_xy.z = 0.0;
            
            float mp[12]={ o2.x, o2.y, o2.z, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
            
            //ver 1.0
            matri tRM0;
            if(nopv_xz.x >= 0.0)
                tRM0 = matrixRotate( angleBetween2Vector(nopv_xz ,nnz), nny, o );
            else
                tRM0 =matrixRotate( angleBetween2Vector(nopv_xz ,nnz), ny, o );
            
            matri tRM = matrixRotate( angleBetween2Vector(noPV ,nny), normalOf2Vector(nny, noPV), o );
            //警告，所有零件原始的放置方式都是繞X軸180度旋轉的
            
            /*
             //ver 2.0
             matri tRM0;
             if(nopv_xy.x >= 0.0)
             tRM0 = matrixRotate( angleBetween2Vector(nopv_xy ,ny), nnz, o );
             else
             tRM0 = matrixRotate( angleBetween2Vector(nopv_xy ,ny), nz, o );
             
             matri tRM = matrixRotate( angleBetween2Vector(noPV ,nnz), normalOf2Vector(nnz, noPV), o );
             */
            
            tRM = matrixMotiply(tRM.m, tRM0.m);
            tRM = matrixMotiply(mp, tRM.m);
            
            mpool_RandomPick.push_back(tRM);
        }
        
    }
    cout<<"OOKK " << "\n";
    cout<< "size of mpool_RandomPick: " << mpool_RandomPick.size() <<"\n";
    
}

void stuffing_1(){
    float strapmin, strapmax;
    int time = 0;
    vertex v_stf;
    cout<<"\n";
    cout<<"all_xy_strap.size() = "<<all_xy_strap.size()<<"\n";
    for(int i=0; i<all_xy_strap.size(); i++){
        //cout<<all_xy_strap[i].size()<<"\n";
        if(all_xy_strap[i].size()>0){
            strapmin = all_xy_strap[i][0].z;
            strapmax = all_xy_strap[i][0].z;
            
            for(int j=0; j<all_xy_strap[i].size(); j++){
                if(all_xy_strap[i][j].z > strapmax ) strapmax = all_xy_strap[i][j].z;
                if(all_xy_strap[i][j].z < strapmin ) strapmin = all_xy_strap[i][j].z;
            }
            
            time = (strapmax-strapmin)/voxel_length;
            for(int j=0; j<time; j++){
                int nxn=(        all_xy_strap[i][0].x - obmi_x )/voxel_length + 1;
                int nyn=(        all_xy_strap[i][0].y - obmi_y )/voxel_length + 1;
                int nzn=( (strapmin + j*voxel_length) - obmi_z )/voxel_length + 1;
                v_stf.x = obmi_x + nxn*voxel_length;
                v_stf.y = obmi_y + nyn*voxel_length;
                v_stf.z = obmi_z + nzn*voxel_length;
                stuffing_vPool.push_back(v_stf);
            }
        }
    }
    cout<<"\n";
    cout<<"#1 stuffing_vPool.size()="<<stuffing_vPool.size()<<"\n";
    for(int i=0; i < voxel_center_vPool.size(); i++){
        for(int j=0; j < stuffing_vPool.size(); j++){
            if(areSameVertex(stuffing_vPool[j], voxel_center_vPool[i])){
                stuffing_vPool[j] = stuffing_vPool[stuffing_vPool.size()];
                stuffing_vPool.pop_back();
            }
        }
    }
    cout<<"#2 stuffing_vPool.size()="<<stuffing_vPool.size()<<"\n";
    cout<<"\n";
    
    vector< vertex > z_layer;
    /*
     for(int i=0; i<stuffing_vPool.size(); i++){
     for(int j=0; j<voxel_center_vPool.size(); j++){
     if( voxel_center_vPool.z - stuffing_vPool.z <=0.03){
     z_layer.push_back(voxel_center_vPool[j]);
     }
     for(int k=0; k<z_layer; k++){
     ;
     }
     }
     }*/
}

void stuffing(){
    vector< vertex > stuffing_candidate_01;
    vector< vertex > stuffing_candidate_02;
    
    vertex noPV;    //normal of picked vertex
    vertex pv;        //picked vertex
    
    float x, y, z;
    
    for(int i=0; i<voxel_center_vPool.size(); i++){
        stuffing_candidate_01.push_back(voxel_center_vPool[i]);
    }
    
    for (int k = 0; k < 2; k++){
        
        for(int i=0; i<stuffing_candidate_01.size(); i++){
            noPV.x = obj_normals[ stuffing_candidate_01[i].num ].x;
            noPV.y = obj_normals[ stuffing_candidate_01[i].num ].y;
            noPV.z = obj_normals[ stuffing_candidate_01[i].num ].z;
            
            x = noPV.x * noPV.x;
            y = noPV.y * noPV.y;
            z = noPV.z * noPV.z;
            
            pv.x = stuffing_candidate_01[i].x;
            pv.y = stuffing_candidate_01[i].y;
            pv.z = stuffing_candidate_01[i].z;
            pv.num = stuffing_candidate_01[i].num;
            
            if(x >= y){
                if(x >= z){//x
                    if(noPV.x > 0){
                        pv.x = stuffing_candidate_01[i].x - voxel_length;
                        pv.y = stuffing_candidate_01[i].y;
                        pv.z = stuffing_candidate_01[i].z;
                        pv.num = stuffing_candidate_01[i].num;
                    }
                    else{
                        pv.x = stuffing_candidate_01[i].x + voxel_length;
                        pv.y = stuffing_candidate_01[i].y;
                        pv.z = stuffing_candidate_01[i].z;
                        pv.num = stuffing_candidate_01[i].num;
                    }
                }
                if(x < z){//z
                    if(noPV.z > 0){
                        pv.x = stuffing_candidate_01[i].x;
                        pv.y = stuffing_candidate_01[i].y;
                        pv.z = stuffing_candidate_01[i].z - voxel_length;
                        pv.num = stuffing_candidate_01[i].num;
                    }
                    else{
                        pv.x = stuffing_candidate_01[i].x;
                        pv.y = stuffing_candidate_01[i].y;
                        pv.z = stuffing_candidate_01[i].z + voxel_length;
                        pv.num = stuffing_candidate_01[i].num;
                    }
                }
            }
            if(x < y){
                if(y >= z){//y
                    if(noPV.y > 0){
                        pv.x = stuffing_candidate_01[i].x;
                        pv.y = stuffing_candidate_01[i].y - voxel_length;
                        pv.z = stuffing_candidate_01[i].z;
                        pv.num = stuffing_candidate_01[i].num;
                    }
                    else{
                        pv.x = stuffing_candidate_01[i].x;
                        pv.y = stuffing_candidate_01[i].y + voxel_length;
                        pv.z = stuffing_candidate_01[i].z;
                        pv.num = stuffing_candidate_01[i].num;
                    }
                }
                if(y < z){//z
                    if(noPV.z > 0){
                        pv.x = stuffing_candidate_01[i].x;
                        pv.y = stuffing_candidate_01[i].y;
                        pv.z = stuffing_candidate_01[i].z - voxel_length;
                        pv.num = stuffing_candidate_01[i].num;
                    }
                    else{
                        pv.x = stuffing_candidate_01[i].x;
                        pv.y = stuffing_candidate_01[i].y;
                        pv.z = stuffing_candidate_01[i].z + voxel_length;
                        pv.num = stuffing_candidate_01[i].num;
                    }
                }
            }
            
            stuffing_candidate_02.push_back(pv);
        }
        
        //delete repeated vertex in the same stuffing layer.
        for(int i=0; i < stuffing_candidate_02.size(); i++){
            for(int j=i+1; j < stuffing_candidate_02.size(); j++){
                if(areSameVertex(stuffing_candidate_02[j], stuffing_candidate_02[i])){
                    stuffing_candidate_02[j] = stuffing_candidate_02[stuffing_candidate_02.size()];
                    stuffing_candidate_02.pop_back();
                }
            }
        }
        
        //delete repeated vertex between two consecutive stuffing layer.
        for(int i=0; i < stuffing_candidate_02.size(); i++){
            for(int j=0; j < stuffing_candidate_01.size(); j++){
                if(areSameVertex(stuffing_candidate_01[j], stuffing_candidate_02[i])){
                    stuffing_candidate_02[i] = stuffing_candidate_02[stuffing_candidate_02.size()];
                    stuffing_candidate_02.pop_back();
                }
            }
        }
        
        //stuff_01  = {}
        //stuff_vPool << stuff_02
        //stuff_01' = stuff_02
        //stuff_02' = {}
        
        stuffing_candidate_01.clear();
        for(int i=0; i < stuffing_candidate_02.size(); i++){
            stuffing_candidate_01.push_back(stuffing_candidate_02[i]);
            stuffing_vPool.push_back(stuffing_candidate_02[i]);
        }
        
        stuffing_candidate_02.clear();
    }
    
    int stx, sty, stz;
    stx= sty= stz= 0;
    float strapmin, strapmax;
    
    for(int i=0; i < stuffing_vPool.size(); i++){
        stx = (stuffing_vPool[i].x - obmi_x) / voxel_length;
        sty = (stuffing_vPool[i].y - obmi_y) / voxel_length;
        stz = (stuffing_vPool[i].z - obmi_z) / voxel_length;
        
        if((stx > obn_x)||(sty > obn_y)||(stx < 0)||(sty < 0)){
            stuffing_vPool[i]=stuffing_vPool[stuffing_vPool.size()-1];
            stuffing_vPool.pop_back();
        }
    }
    /*
     cout<<stuffing_vPool.size()<<"\n";
     cout<<"0013254646"<<"\n";
     for(int i=0; i < stuffing_vPool.size(); i++){
     stx = (stuffing_vPool[i].x - obmi_x) / voxel_length;
     sty = (stuffing_vPool[i].y - obmi_y) / voxel_length;
     stz = (stuffing_vPool[i].z - obmi_z) / voxel_length;
     
     if(stx + sty*obn_x < all_xy_strap.size()){
     strapmax= all_xy_strap[stx + sty*obn_x][0].z;
     strapmin= all_xy_strap[stx + sty*obn_x][0].z;
     cout<<"22222222222222222"<<"\n";
     for(int j=0; j < all_xy_strap[stx + sty*obn_x].size(); j++){
     if(all_xy_strap[stx + sty*obn_x][j].z > strapmax)
     strapmax = all_xy_strap[stx + sty*obn_x][j].z;
     if(all_xy_strap[stx + sty*obn_x][j].z < strapmin)
     strapmin = all_xy_strap[stx + sty*obn_x][j].z;
     }
     cout<<"3333333333333333333333333"<<"\n";
     if((stuffing_vPool[i].z < strapmin)||(stuffing_vPool[i].z > strapmax)){
     stuffing_vPool[i]=stuffing_vPool[stuffing_vPool.size()-1];
     stuffing_vPool.pop_back();
     }
     }
     }*/
    
}

void load_part(string name){
    part_v1 part0;
    
    part0.tpfp.clear();
    part0.normal_pool.clear();
    search_or_read(name, false, metrix_O, false, name);
    for(int i=0; i<trianglePool.size(); i++){
        part0.tpfp.push_back(trianglePool[i]);
        part0.normal_pool.push_back(tmpNormalPool[i]);
    }
    for(int i=0; i<cnnc_x.size(); i++){
        if(cnnc_x[i].num==1)
            part0.connection_1_x.push_back(cnnc_x[i]);
        if(cnnc_x[i].num==2)
            part0.connection_2_x.push_back(cnnc_x[i]);
    }
    for(int i=0; i<cnnc_o.size(); i++){
        if(cnnc_o[i].num==1)
            part0.connection_1_o.push_back(cnnc_o[i]);
        if(cnnc_o[i].num==2)
            part0.connection_2_o.push_back(cnnc_o[i]);
    }
    //cout<<cnnc_x.size()<<"\n";
    //cout<<cnnc_o.size()<<"\n";
    //cout<<part0.connection_1_o.size()<<"\n";
    parts.push_back(part0);
    trianglePool.clear();
    tmpNormalPool.clear();
    cnnc_x.clear();
    cnnc_o.clear();
    stud_for_plate=false;
    stud_for_brick=false;
    cout<<"part "<<name<<"\n";
    
}

void init(void)
{
    
    read_obj();
    randomPick_even();
    surface_arrange_random();
    stuffing();
    readMa(ma);
    
    
    /*
     char* list = "40234_Rooster_reduced.txt";
     load_lego_parts_list(list);
     */
    
    for(int i=0; i<12; i++){
        metrix_O[i] = metrix_O[i]*rate;
    }
    
    string partt = "4733.dat";//"3005.dat";3024 3070b
    load_part(partt);
    // 0
    
    partt = "87087.dat";//"3005.dat";3024 3070b; 11477
    load_part(partt);
    // 1
    
    partt = "11477.dat";//"3005.dat";3024 3070b
    load_part(partt);
    // 2
    
    partt = "22890.dat";//"3005.dat";3024 3070b; 11477
    load_part(partt);
    // 3
    
    partt = "14418.dat";//"3005.dat";3024 3070b; 11477
    load_part(partt);
    // 4
    
}

void drawObj_p()
{
    glColor3f(1.0f,1.0f,1.0f);
    if(drawlegoFrame){
        for(int i=0; i<obj_vPool.size(); i++){
            glVertex3f( obj_vPool[i].x, obj_vPool[i].y, obj_vPool[i].z);
        }
    }
    else{
        for(int i=0; i<voxel_center_vPool.size(); i++){
            glVertex3f( voxel_center_vPool[i].x, voxel_center_vPool[i].y, voxel_center_vPool[i].z);
        }
    }
}

void drawObj_in_p()
{
    glColor3f(1.0f,0.0f,0.0f);
    for(int i=0; i<voxel_bone_position.size(); i++){
        //glVertex3f( voxel_bone_position[i].x, voxel_bone_position[i].y, voxel_bone_position[i].z);
        glBegin(GL_LINE_LOOP);
        glNormal3f( 1.0f, 0.0f, 0.0f );//right
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( -1.0f, 0.0f, 0.0f );//left
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, 1.0f, 0.0f );//up
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, -1.0f, 0.0f );//down
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, 0.0f, 1.0f );//front
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z + voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, 0.0f, -1.0f );//hind
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y + voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glVertex3f( voxel_bone_position[i].x - voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glVertex3f( voxel_bone_position[i].x + voxel_length_half, voxel_bone_position[i].y - voxel_length_half, voxel_bone_position[i].z - voxel_length_half);
        glEnd();
    }
}

void test_draw()
{
    glColor3f(0.8f,0.6f,0.1f);
    glBegin(GL_TRIANGLES);
    glNormal3f( 0.0, -1.0, 0.0 );
    glVertex3f( 1.0, 0.0, 0.0);//1 0 0 0.9914 0 0.1305 0 0 0
    
    glVertex3f( 0.9914, 0.0, 0.1305);
    glVertex3f( 0.0, 0.0, 0.0);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f( 0.0, 0.0, 1.0 );
    glVertex3f( 1.0, 0.0, 0.0);//1 0 0 0.9914 0 0.1305 0 0 0
    glVertex3f( 1.0, 0.5, 0.0);
    glVertex3f( -1.0, 0.5, 0.0);
    glVertex3f( -1.0, 0.0, 0.0);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f( 0.0, 0.0, -1.0 );
    glVertex3f( -1.0, 0.0, 0.0);//1 0 0 0.9914 0 0.1305 0 0 0
    glVertex3f( -1.0, 0.5, 0.0);
    glVertex3f( 1.0, 0.5, 0.0);
    glVertex3f( 1.0, 0.0, 0.0);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glNormal3f( 0, 1.0, 0 );
    glVertex3f(-1.0, 1.0, 1.0);//1 0 0 0.9914 0 0.1305 0 0 0
    glVertex3f( 1.0, 1.0, 1.0);
    glVertex3f( 1.0, 1.0, -1.0);
    glEnd();
}

void drawObj_t(bool drawTri)//true: draw Triangles ; false: draw loops
{
    for(int i=0; i<obj_tPool.size(); i++){
        if(drawTri){
            glColor3f(0.8f,0.6f,0.1f);
            glBegin(GL_TRIANGLES);
            glNormal3f( obj_normals[i].x, obj_normals[i].y, obj_normals[i].z );
            glVertex3f( obj_tPool[i].v1.x, obj_tPool[i].v1.y, obj_tPool[i].v1.z);
            glVertex3f( obj_tPool[i].v2.x, obj_tPool[i].v2.y, obj_tPool[i].v2.z);
            glVertex3f( obj_tPool[i].v3.x, obj_tPool[i].v3.y, obj_tPool[i].v3.z);
            glEnd();
        }else{
            glColor3f(0.0f,1.0f,0.0f);
            glBegin(GL_LINE_LOOP);
            glNormal3f( 0.0f, 1.0f, 1.0f );//up
            glVertex3f( obj_tPool[i].v1.x, obj_tPool[i].v1.y, obj_tPool[i].v1.z);
            glVertex3f( obj_tPool[i].v2.x, obj_tPool[i].v2.y, obj_tPool[i].v2.z);
            glVertex3f( obj_tPool[i].v3.x, obj_tPool[i].v3.y, obj_tPool[i].v3.z);
            glEnd();
        }
        if(!drawlegoFrame){
            glColor3f(1.0f,1.0f,1.0f);
            glBegin(GL_LINE_LOOP);
            glNormal3f( 1.0f, 1.0f, 1.0f );
            glVertex3f( obj_tPool[i].v1.x, obj_tPool[i].v1.y, obj_tPool[i].v1.z);
            glVertex3f( obj_tPool[i].v2.x, obj_tPool[i].v2.y, obj_tPool[i].v2.z);
            glVertex3f( obj_tPool[i].v3.x, obj_tPool[i].v3.y, obj_tPool[i].v3.z);
            glEnd();
        }
    }
    
}

void drawVoxel()
{
    
    glColor3f(1.0f,0.0f,0.0f);
    for(int i=0; i<voxel_center_vPool.size(); i++){
        glBegin(GL_LINE_LOOP);
        glNormal3f( 1.0f, 0.0f, 0.0f );//right
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( -1.0f, 0.0f, 0.0f );//left
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, 1.0f, 0.0f );//up
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, -1.0f, 0.0f );//down
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, 0.0f, 1.0f );//front
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z + voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, 0.0f, -1.0f );//hind
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y + voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x - voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glVertex3f( voxel_center_vPool[i].x + voxel_length_half, voxel_center_vPool[i].y - voxel_length_half, voxel_center_vPool[i].z - voxel_length_half);
        glEnd();
    }
    
    glColor3f(0.1f,0.7f,0.3f);
    for(int i=0; i<randomlyPicked.size(); i++){
        glBegin(GL_QUADS);
        glNormal3f( 1.0f, 0.0f, 0.0f );//right
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_QUADS);
        glNormal3f( -1.0f, 0.0f, 0.0f );//left
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_QUADS);
        glNormal3f( 0.0f, 1.0f, 0.0f );//up
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_QUADS);
        glNormal3f( 0.0f, -1.0f, 0.0f );//down
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_QUADS);
        glNormal3f( 0.0f, 0.0f, 1.0f );//front
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z + voxel_length_half);
        glEnd();
        glBegin(GL_QUADS);
        glNormal3f( 0.0f, 0.0f, -1.0f );//hind
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y + voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glVertex3f( randomlyPicked[i].x - voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glVertex3f( randomlyPicked[i].x + voxel_length_half, randomlyPicked[i].y - voxel_length_half, randomlyPicked[i].z - voxel_length_half);
        glEnd();
    }
}

void drawVoxel_stuffing()
{
    
    glColor3f(0.0f,1.0f,0.0f);
    for(int i=0; i<stuffing_vPool.size(); i++){
        glBegin(GL_LINE_LOOP);
        glNormal3f( 1.0f, 0.0f, 0.0f );//right
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( -1.0f, 0.0f, 0.0f );//left
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, 1.0f, 0.0f );//up
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, -1.0f, 0.0f );//down
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, 0.0f, 1.0f );//front
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z + voxel_length_half);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glNormal3f( 0.0f, 0.0f, -1.0f );//hind
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y + voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glVertex3f( stuffing_vPool[i].x - voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glVertex3f( stuffing_vPool[i].x + voxel_length_half, stuffing_vPool[i].y - voxel_length_half, stuffing_vPool[i].z - voxel_length_half);
        glEnd();
    }
}

void drawPart(int p_number, float place[12], float color[3]){
    
    for(int i=0; i < parts[p_number].tpfp.size() ; i++){
        
        vertex normal = matrixVertexMotiply(place, parts[p_number].normal_pool[i]);
        triangle tri;
        tri.v1 = matrixVertexMotiply(place, parts[p_number].tpfp[i].v1);
        tri.v2 = matrixVertexMotiply(place, parts[p_number].tpfp[i].v2);
        tri.v3 = matrixVertexMotiply(place, parts[p_number].tpfp[i].v3);
        
        glColor3f(color[0],color[1],color[2]);
        
        glBegin(GL_TRIANGLES);
        glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( tri.v1.x
                   , tri.v1.y
                   , tri.v1.z );
        glVertex3f( tri.v2.x
                   , tri.v2.y
                   , tri.v2.z );
        glVertex3f( tri.v3.x
                   , tri.v3.y
                   , tri.v3.z );
        glEnd();
        if(!drawlegoFrame){
            glColor3f(1.0f,1.0f,1.0f);
            glBegin(GL_LINE_LOOP);
            glNormal3f( normal.x, normal.y, normal.z );
            glVertex3f( tri.v1.x
                       , tri.v1.y
                       , tri.v1.z );
            glVertex3f( tri.v2.x
                       , tri.v2.y
                       , tri.v2.z );
            glVertex3f( tri.v3.x
                       , tri.v3.y
                       , tri.v3.z );
            glEnd();
        }
    }
    for(int i=0; i < parts[p_number].t5e.size() ; i++){
        glColor3f(1.0f,1.0f,1.0f);
        glBegin(GL_LINES);
        glVertex3f( parts[p_number].t5e[i].v1.x
                   , parts[p_number].t5e[i].v1.y
                   , parts[p_number].t5e[i].v1.z );
        glVertex3f( parts[p_number].t5e[i].v2.x
                   , parts[p_number].t5e[i].v2.y
                   , parts[p_number].t5e[i].v2.z );
        glEnd();
    }
}

void drawJoint(int p_number, float place[12], float color[3]){
    for(int i=0; i < parts[p_number].connection_1_x.size() ; i++){
        vertex joint;
        joint = matrixVertexMotiply(place, parts[p_number].connection_1_x[i]);
        vertex x1, x2, y1, y2, z1, z2;
        x1.x = joint.x + 0.5;
        x1.y = joint.y;
        x1.z = joint.z;
        
        x2.x = joint.x - 0.5;
        x2.y = joint.y;
        x2.z = joint.z;
        
        y1.x = joint.x;
        y1.y = joint.y + 0.5;
        y1.z = joint.z;
        
        y2.x = joint.x;
        y2.y = joint.y - 0.5;
        y2.z = joint.z;
        
        z1.x = joint.x;
        z1.y = joint.y;
        z1.z = joint.z + 0.5;
        
        z2.x = joint.x;
        z2.y = joint.y;
        z2.z = joint.z - 0.5;
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( x1.x
                   , x1.y
                   , x1.z );
        glVertex3f( x2.x
                   , x2.y
                   , x2.z );
        glEnd();
        
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( y1.x
                   , y1.y
                   , y1.z );
        glVertex3f( y2.x
                   , y2.y
                   , y2.z );
        glEnd();
        
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( z1.x
                   , z1.y
                   , z1.z );
        glVertex3f( z2.x
                   , z2.y
                   , z2.z );
        glEnd();
    }
    //cout<<"size:"<<parts[p_number].connection_1_o.size()<<"\n";
    for(int i=0; i < parts[p_number].connection_1_o.size() ; i++){
        vertex joint;
        joint = matrixVertexMotiply(place, parts[p_number].connection_1_o[i]);
        vertex x1, x2, y1, y2, z1, z2;
        x1.x = joint.x + 0.5;
        x1.y = joint.y;
        x1.z = joint.z;
        
        x2.x = joint.x - 0.5;
        x2.y = joint.y;
        x2.z = joint.z;
        
        y1.x = joint.x;
        y1.y = joint.y + 0.5;
        y1.z = joint.z;
        
        y2.x = joint.x;
        y2.y = joint.y - 0.5;
        y2.z = joint.z;
        
        z1.x = joint.x;
        z1.y = joint.y;
        z1.z = joint.z + 0.5;
        
        z2.x = joint.x;
        z2.y = joint.y;
        z2.z = joint.z - 0.5;
        
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( x1.x
                   , x1.y
                   , x1.z );
        glVertex3f( x2.x
                   , x2.y
                   , x2.z );
        glEnd();
        
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( y1.x
                   , y1.y
                   , y1.z );
        glVertex3f( y2.x
                   , y2.y
                   , y2.z );
        glEnd();
        
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( z1.x
                   , z1.y
                   , z1.z );
        glVertex3f( z2.x
                   , z2.y
                   , z2.z );
        glEnd();
    }
    //No.2
    for(int i=0; i < parts[p_number].connection_2_x.size() ; i++){
        vertex joint;
        joint = matrixVertexMotiply(place, parts[p_number].connection_2_x[i]);
        vertex x1, x2, y1, y2, z1, z2;
        x1.x = joint.x + 0.5;
        x1.y = joint.y;
        x1.z = joint.z;
        
        x2.x = joint.x - 0.5;
        x2.y = joint.y;
        x2.z = joint.z;
        
        y1.x = joint.x;
        y1.y = joint.y + 0.5;
        y1.z = joint.z;
        
        y2.x = joint.x;
        y2.y = joint.y - 0.5;
        y2.z = joint.z;
        
        z1.x = joint.x;
        z1.y = joint.y;
        z1.z = joint.z + 0.5;
        
        z2.x = joint.x;
        z2.y = joint.y;
        z2.z = joint.z - 0.5;
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( x1.x
                   , x1.y
                   , x1.z );
        glVertex3f( x2.x
                   , x2.y
                   , x2.z );
        glEnd();
        
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( y1.x
                   , y1.y
                   , y1.z );
        glVertex3f( y2.x
                   , y2.y
                   , y2.z );
        glEnd();
        
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( z1.x
                   , z1.y
                   , z1.z );
        glVertex3f( z2.x
                   , z2.y
                   , z2.z );
        glEnd();
    }
    for(int i=0; i < parts[p_number].connection_2_o.size() ; i++){
        vertex joint;
        joint = matrixVertexMotiply(place, parts[p_number].connection_2_o[i]);
        vertex x1, x2, y1, y2, z1, z2;
        x1.x = joint.x + 0.5;
        x1.y = joint.y;
        x1.z = joint.z;
        
        x2.x = joint.x - 0.5;
        x2.y = joint.y;
        x2.z = joint.z;
        
        y1.x = joint.x;
        y1.y = joint.y + 0.5;
        y1.z = joint.z;
        
        y2.x = joint.x;
        y2.y = joint.y - 0.5;
        y2.z = joint.z;
        
        z1.x = joint.x;
        z1.y = joint.y;
        z1.z = joint.z + 0.5;
        
        z2.x = joint.x;
        z2.y = joint.y;
        z2.z = joint.z - 0.5;
        
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( x1.x
                   , x1.y
                   , x1.z );
        glVertex3f( x2.x
                   , x2.y
                   , x2.z );
        glEnd();
        
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( y1.x
                   , y1.y
                   , y1.z );
        glVertex3f( y2.x
                   , y2.y
                   , y2.z );
        glEnd();
        
        glBegin(GL_LINES);
        //glNormal3f( normal.x, normal.y, normal.z );
        glVertex3f( z1.x
                   , z1.y
                   , z1.z );
        glVertex3f( z2.x
                   , z2.y
                   , z2.z );
        glEnd();
    }
}


float divv=0.075;// 0.15 ; 0.15/2 one brick's length
void drawBonewithLego()
{
    for(int i=0; i<ma_fPool.size(); i++){
        //draw 4733
        vertex center;
        center = centerOfCircumscribedCircle(ma_fPool[i].v1, ma_fPool[i].v2, ma_fPool[i].v3);
        float x = (ma_fPool[i].v1.x + ma_fPool[i].v2.x + ma_fPool[i].v3.x)/3.0;
        float y = (ma_fPool[i].v1.y + ma_fPool[i].v2.y + ma_fPool[i].v3.y)/3.0;
        float z = (ma_fPool[i].v1.z + ma_fPool[i].v2.z + ma_fPool[i].v3.z)/3.0;
        float m_for_f[12]={ x, y, z, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
        drawPart(0, m_for_f, dark);
    }
    
    for(int i=0; i<ma_ePool.size(); i++){
        //draw 87087
        bool edgeNotFace = true;
        for(int j=0; j<ma_fPool.size(); j++){
            if( areSameVertex(ma_fPool[j].v1, ma_ePool[i].v1) ){
                if( areSameVertex(ma_fPool[j].v2, ma_ePool[i].v2) ){
                    edgeNotFace = false;
                }
                if( areSameVertex(ma_fPool[j].v3, ma_ePool[i].v2) ){
                    edgeNotFace = false;
                }
            }
            if( areSameVertex(ma_fPool[j].v2, ma_ePool[i].v1) ){
                if( areSameVertex(ma_fPool[j].v1, ma_ePool[i].v2) ){
                    edgeNotFace = false;
                }
                if( areSameVertex(ma_fPool[j].v3, ma_ePool[i].v2) ){
                    edgeNotFace = false;
                }
            }
            if( areSameVertex(ma_fPool[j].v3, ma_ePool[i].v1) ){
                if( areSameVertex(ma_fPool[j].v2, ma_ePool[i].v2) ){
                    edgeNotFace = false;
                }
                if( areSameVertex(ma_fPool[j].v1, ma_ePool[i].v2) ){
                    edgeNotFace = false;
                }
            }
        }
        
        if(edgeNotFace){
            //float lego_height = 24*rate;
            int time =pow(
                          pow(ma_ePool[i].v1.x-ma_ePool[i].v2.x ,2.0)
                          + pow(ma_ePool[i].v1.y-ma_ePool[i].v2.y ,2.0)
                          + pow(ma_ePool[i].v1.z-ma_ePool[i].v2.z ,2.0)
                          , 0.5)/divv;
            if(time==0)time=1;
            
            /*int time=2;*/
            vertex vct;
            
            vct.x = (ma_ePool[i].v1.x - ma_ePool[i].v2.x);
            vct.y = (ma_ePool[i].v1.y - ma_ePool[i].v2.y);
            vct.z = (ma_ePool[i].v1.z - ma_ePool[i].v2.z);
            
            vct = normalize( vct );
            vct.x = vct.x*divv;
            vct.y = vct.y*divv;
            vct.z = vct.z*divv;
            //cout<< time <<"\n";
            for(int j=0; j<time; j++){
                vertex ny;
                ny.x = 0.0;
                ny.y = 1.0;
                ny.z = 0.0;
                vertex o;
                o.x = 0.0;
                o.y = 0.0;
                o.z = 0.0;
                vertex o2;
                o2.x = ma_ePool[i].v2.x + j*vct.x;
                o2.y = ma_ePool[i].v2.y + j*vct.y;
                o2.z = ma_ePool[i].v2.z + j*vct.z;
                float mp[12]={ o2.x, o2.y, o2.z, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
                matri m_for_e = matrixRotate( angleBetween2Vector(vct ,ny), normalOf2Vector(ny, vct), o );
                m_for_e=matrixMotiply(mp, m_for_e.m);
                drawPart(1, m_for_e.m, lightgrey);
            }
        }
        
    }/*
      for(int i=0; i<ma_vPool.size(); i++){
      float m_for_v[12]={ ma_vPool[i].x, ma_vPool[i].y, ma_vPool[i].z, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
      drawPart(2, m_for_v, yellow);
      }*/
}

void drawSurfaceParts_random(){
    int j;
    for(int i=0; i < randomlyPicked.size(); i++){
        j=randomlyPicked_color[i];
        if(j==0)
            drawPart(2, mpool_RandomPick[i].m, yellow);
        if(j==1)
            drawPart(2, mpool_RandomPick[i].m, red);
        if(j==2)
            drawPart(2, mpool_RandomPick[i].m, blue);
        if(j==3)
            drawPart(2, mpool_RandomPick[i].m, green);
    }
}

void drawMa(){
    for(int i=0; i < ma_ePool.size() ; i++){
        glColor3f(0.0f,0.7f,0.0f);
        glBegin(GL_LINES);
        glVertex3f( ma_ePool[i].v1.x, ma_ePool[i].v1.y, ma_ePool[i].v1.z );
        glVertex3f( ma_ePool[i].v2.x, ma_ePool[i].v2.y, ma_ePool[i].v2.z );
        glEnd();
    }
    for(int i=0; i < ma_fPool.size() ; i++){
        glColor3f(1.0f,0.0f,0.0f);
        glBegin(GL_TRIANGLES);
        glVertex3f( ma_fPool[i].v1.x, ma_fPool[i].v1.y, ma_fPool[i].v1.z );
        glVertex3f( ma_fPool[i].v2.x, ma_fPool[i].v2.y, ma_fPool[i].v2.z );
        glVertex3f( ma_fPool[i].v3.x, ma_fPool[i].v3.y, ma_fPool[i].v3.z );
        glEnd();
    }
}

/* GLUT callback Handlers */
static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3d(1,0,0);
    
    //--00---------------------------------------------------/
    glPushMatrix();
    glTranslated(-3.6,0.0, dis);//glTranslated(-2.4,1.2,-6);
    
    glRotated(rota,1,0,0);
    glRotated(0 + rotate1,0,0,1);//glRotated(25 + rotate1,0,0,1) 2017.11.08;
    
    drawPart(3, metrix_OO, blue);
    drawJoint(3, metrix_OO, red);
    //drawObj_t(drawTri0);
    //drawMa();
    
    //glutSolidSphere(1,slices,stacks);
    glPopMatrix();
    //--01---------------------------------------------------/
    glPushMatrix();
    glTranslated(-1.8,0.0, dis);//glTranslated(-2.4,1.2,-6);
    
    glRotated(rota,1,0,0);
    glRotated(0 + rotate1,0,0,1);//glRotated(25 + rotate1,0,0,1) 2017.11.08;
    
    drawPart(0, metrix_OO, red);
    drawJoint(0, metrix_OO, red);
    //drawObj_t(drawTri0);
    //drawMa();
    
    //glutSolidSphere(1,slices,stacks);
    glPopMatrix();
    //--02---------------------------------------------------/
    glPushMatrix();
    glTranslated(0,0.0, dis);//glTranslated(-2.4,1.2,-6);
    
    glRotated(rota,1,0,0);
    glRotated(0 + rotate1,0,0,1);
    
    drawPart(1, metrix_OO, yellow);
    drawJoint(1, metrix_OO, red);
    //drawMa();
    //drawBonewithLego();
    //drawSurfaceParts_random();
    
    //drawObj_t(false);
    if(!swch){
        drawVoxel();
    }
    //glutSolidSphere(1,slices,stacks);
    glPopMatrix();
    //--03---------------------------------------------------/
    glPushMatrix();
    glTranslated(1.8,0.0, dis);//glTranslated(2.4,1.2,-6); //glTranslated(2.8,0.0, dis)2017.11.08;
    
    glRotated(rota,1,0,0);
    glRotated(0 + rotate1,0,0,1);//glRotated(-25 + rotate1,0,0,1) 2017.11.08;
    
    drawPart(2, metrix_OO, green);
    drawJoint(2, metrix_OO, red);
    //drawBonewithLego();
    if(!swch){
        //drawSurfaceParts_random();
        //drawObj_t(drawTri0);
    }
    
    glBegin(GL_POINTS);
    //drawObj_p();
    //drawObj_in_p();
    glEnd();
    if(swch){
        //drawVoxel();
    }
    drawVoxel_stuffing();
    //glutSolidTorus(0.2,0.8,slices,stacks);
    glPopMatrix();
    
    //--03-2-test--------------------------------------------/
    glPushMatrix();
    glTranslated(1.8,0.0, dis);//glTranslated(2.4,1.2,-6); //glTranslated(2.8,0.0, dis)2017.11.08;
    
    glRotated(rota,1,0,0);
    glRotated(0 + rotate1,0,0,1);//glRotated(-25 + rotate1,0,0,1) 2017.11.08;
    
    //drawPart(2, metrix_OO, blue);
    //test_draw();
    
    glPopMatrix();
    
    //--04---------------------------------------------------/
    glPushMatrix();
    glTranslated(3.6,0.0, dis);//glTranslated(-2.4,1.2,-6);
    
    glRotated(rota,1,0,0);
    glRotated(0 + rotate1,0,0,1);//glRotated(25 + rotate1,0,0,1) 2017.11.08;
    
    drawPart(4, metrix_OO, grey);
    drawJoint(4, metrix_OO, red);
    //drawObj_t(drawTri0);
    //drawMa();
    
    //glutSolidSphere(1,slices,stacks);
    glPopMatrix();
    
    
    //--part---------------------------------------------------/
    for(int i=0; i<parts.size(); i++){
        glPushMatrix();
        glTranslated(1.4,-1+oheight,-4);//glTranslated(0,1.2,-6);
        glTranslatef(0.0, add*i, 0.0);
        glRotated(rotate1,0,1,0);
        //glRotated(180,1,0,0);
        
        //drawPart(i, metrix_OO, blue);
        
        //glutSolidCone(1,1,slices,stacks);
        glPopMatrix();
    }
    
    
    /*
     glTranslated(-2.4,1.2,-6);
     glTranslated(0,1.2,-6);
     glTranslated(2.4,1.2,-6);
     glTranslated(-2.4,-1.2,-6);
     glTranslated(0,-1.2,-6);
     glTranslated(2.4,-1.2,-6);
     */
    
    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;
            
        case '+':
            dis += .5;
            break;
            
        case '-':
            dis -= .5;
            break;
            
        case 'a':
            //            g_fAngle += 2.0;
            rotate1 += 2.0;
            break;
            
        case 'd':
            //            g_fAngle -= 2.0;
            rotate1 -= 2.0;
            break;
            
        case 'w':
            //            g_fAngle += 2.0;
            rota += 2.0;
            break;
            
        case 's':
            //            g_fAngle -= 2.0;
            rota -= 2.0;
            break;
            
        case 'x':
            //            g_fAngle = .0;
            rota = 0.0;
            rotate1 = 0.0;
            
            oheight=.0;
            dis = -4.2;//4.5 2017.11.08;
            break;
            
        case 'i':
            oheight += 0.1;
            break;
            
        case 'k':
            oheight -= 0.1;
            break;
            
        case 't':
            add += 0.1;
            break;
            
        case 'g':
            add -= 0.1;
            break;
            
        case 'f':
            if(drawlegoFrame){
                drawlegoFrame=false;
            }else{
                drawlegoFrame=true;
            }
            break;
            
        case 'r':
            if(swch){
                swch=false;
            }else{
                swch=true;
            }
            break;
            
        case 'v':
            if(drawTri0){
                drawTri0=false;
            }else{
                drawTri0=true;
            }
            break;
    }
    
    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 30.0f, 50.0f, 20.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(1300,600);
    glutInitWindowPosition(100,250);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    glutCreateWindow("Lego Parts Viewer 1.0");
    
    init();
    
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    
    glClearColor(0.8,0.8,0.8,1);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
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
    
    return EXIT_SUCCESS;
}

