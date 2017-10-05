/*
    Lego
    Jui-Cheng,Sung. R.O.C.
    Lyre Mellark.
    Started from 2017.Feb.10
*/

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

float voxel_length = 0.07;
float voxel_length_half = voxel_length/2;//

float alert_range = 3.0; //for interier stuff

float metrix_O[12] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};
float metrix_V[12] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1};

float rate = 0.2; //scale of parts (Square root) [0.2 to watch] [0.08 to obj]
float dis = -4.5;  //position of OBJ in z axis

float add=-2.2;
float oheight=0.0;
float upp = 0.0;

bool drawlegoFrame = false;

float rota = 0.0;
float rotate1 = 0.0;

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
vector< edge > bricks ;     //just store shapes for drawing, not for matching.

// 02
vector< part_v1 > parts  ;  //For real math stuffs. Prepare for assembling possibility

vector< vertex > tmpNormalPool  ;
vector< triangle > trianglePool  ;
/*----------------------------------------------------------------*/

//char* p = "frog.obj";
char* p = "suzanne.obj";
char* ma = "bug20.ma";
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
        if ((dir = opendir ("C:\\Users\\luke\\Desktop\\lego_assembler\\parts")) != NULL) {
            // C:\\Users\\luke\\Desktop\\lego_assembler\\parts
            // C:\\Users\\user\\Desktop\\lego_assembler\\parts
            // /Users/luke/desktop/legomac/parts
            // search all the files and directories within directory

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

        if ((dir = opendir ("C:\\Users\\luke\\Desktop\\lego_assembler\\parts\\s")) != NULL) {
            // C:\\Users\\luke\\Desktop\\lego_assembler\\parts\\s
            // C:\\Users\\user\\Desktop\\lego_assembler\\parts\\s
            // /Users/luke/desktop/legomac/parts/s
            // search all the files and directories within directory

            while ((ent = readdir (dir)) != NULL) {
                string d_name;      // ent->d_name: 3005.dat
                string ss="s\\";      // ss: s
                string sdname = ss + ent->d_name; // s\ + 3005.dat

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

        if ((dir = opendir ("C:\\Users\\luke\\Desktop\\lego_assembler\\parts\\48")) != NULL) {
            // C:\\Users\\luke\\Desktop\\lego_assembler\\parts\\48
            // C:\\Users\\user\\Desktop\\lego_assembler\\parts\\48
            // /Users/luke/desktop/legomac/parts/48
            // search all the files and directories within directory

            while ((ent = readdir (dir)) != NULL) {
                string d_name;      // ent->d_name: 3005.dat
                string s48="48\\";      // s48: 48\;
                string sdname = s48 + ent->d_name; // 48\ + 3005.dat

                if(sdname == part_name){// part_name: s\3005.dat (backslash was escaped.)

                    //cout<<"Found "<< ent->d_name <<" in \\parts\\s"<<endl;
                    search_or_read( ent->d_name, false, array_O );
                }
            }
            closedir (dir);
        } else {
            // could not open directory
            cerr<<"Can't search the part in \\parts\\48"<<endl;
            exit(1);
            //return EXIT_FAILURE;
        }

        if ((dir = opendir ("C:\\Users\\luke\\Desktop\\lego_assembler\\parts\\8")) != NULL) {
            // C:\\Users\\luke\\Desktop\\lego_assembler\\parts\\8
            // C:\\Users\\user\\Desktop\\lego_assembler\\parts\\8
            // /Users/luke/desktop/legomac/parts/8
            // search all the files and directories within directory

            while ((ent = readdir (dir)) != NULL) {
                string d_name;      // ent->d_name: 3005.dat
                string s8="8\\";      // s8: 8\;
                string sdname = s8 + ent->d_name; // 8\3005.dat

                if(sdname == part_name){// part_name: s\3005.dat (backslash was escaped.)

                    //cout<<"Found "<< ent->d_name <<" in \\parts\\s"<<endl;
                    search_or_read( ent->d_name, false, array_O );
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

        short geo_type = 0; // 2:line, 3:triangle, 4:Quadrilateral
        //part_v1 part;     // tmp

        //int type,color, a,b,c, d,e,f, g,h,i, j,k,l;
        int type, color;    // type is ldraw-types: 1, 2, 3, 4 and ldraw-color
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
        triangle tri; // line, triangle, quad. They all been saved as a triangle formate.

        vertex normalt;

        part_v1 parttmp;

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

        ifstream inf(dat_name); // read the file with ifstream and save to inf
        ifstream infs(s_dat_name);  // read the file with ifstream and save to inf
        ifstream inf8(dat_name8); // read the file with ifstream and save to inf
        ifstream inf48(dat_name48); // read the file with ifstream and save to inf

        if( !inf && !infs && !inf8 && !inf48 ){
            cerr<<"Error: can't read part. 02"<<endl;
            exit(1);
        }
        // read, save
        while( getline(inf, line)||getline(infs, line)||getline(inf8, line)||getline(inf48, line) ){   // use getline to save each line from 'inf' to 'line', one at a time.
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
                    /*
                    cout << metrix_V[3] <<' '<< metrix_V[4] <<' '<< metrix_V[5] <<' '<< metrix_V[0] << endl;
                    cout << metrix_V[6] <<' '<< metrix_V[7] <<' '<< metrix_V[8] <<' '<< metrix_V[1] << endl;
                    cout << metrix_V[9] <<' '<< metrix_V[10] <<' '<< metrix_V[11] <<' '<< metrix_V[2] << endl;
                    cout << endl;
                    */
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

                    dot1.x = metrix[0];     dot1.y = metrix[1];     dot1.z = metrix[2]; // dot1 = x1y1z1
                    dot2.x = metrix[3];     dot2.y = metrix[4];     dot2.z = metrix[5]; // dot2 = x2y2z2
                    dot3.x = metrix[6];     dot3.y = metrix[7];     dot3.z = metrix[8]; // dot3 = x3y3z3

                    tri.v1 = dot1;
                    tri.v2 = dot2;
                    tri.v3 = dot3;

                    normalt.x = (dot2.y-dot1.y)*(dot3.z-dot1.z)-(dot3.y-dot1.y)*(dot2.z-dot1.z);
                    //normalt.x = (dot3.y-dot1.y)*(dot2.z-dot1.z)-(dot2.y-dot1.y)*(dot3.z-dot1.z);
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

                    dot1.x = metrix[0];     dot1.y = metrix[1];     dot1.z = metrix[2]; // dot1 = x1y1z1
                    dot2.x = metrix[3];     dot2.y = metrix[4];     dot2.z = metrix[5]; // dot2 = x2y2z2
                    dot3.x = metrix[6];     dot3.y = metrix[7];     dot3.z = metrix[8]; // dot3 = x3y3z3

                    tri.v1 = dot1;
                    tri.v2 = dot3;
                    tri.v3 = dot2;

                    normalt.x = (dot2.y-dot1.y)*(dot3.z-dot1.z)-(dot3.y-dot1.y)*(dot2.z-dot1.z);
                    normalt.y = (dot2.z-dot1.z)*(dot3.x-dot1.x)-(dot3.z-dot1.z)*(dot2.x-dot1.x);
                    normalt.z = (dot2.x-dot1.x)*(dot3.y-dot1.y)-(dot3.x-dot1.x)*(dot2.y-dot1.y);

                    //cout << metrix_V[0] <<' '<< metrix_V[1] <<' '<< metrix_V[2] << endl;

                    //push_back
                    tmpNormalPool.push_back(normalt);
                    trianglePool.push_back(tri);
                    //parttmp.tpfp.push_back(tri);

                    dot1.x = metrix[6];     dot1.y = metrix[7];     dot1.z = metrix[8]; // dot1 = x3y3z3
                    dot2.x = metrix[9];     dot2.y = metrix[10];    dot2.z = metrix[11];// dot2 = x4y4z4
                    dot3.x = metrix[0];     dot3.y = metrix[1];     dot3.z = metrix[2]; // dot3 = x1y1z1

                    tri.v1 = dot1;
                    tri.v2 = dot3;
                    tri.v3 = dot2;

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
        search_or_read(name, true, metrix_O);
    }
}

void load(){}

//Storage vectors list of ABCD.obj input
std::vector<vertex> obj_vPool;		//all vertex in obj
std::vector<vertex> obj_vNormal;	//all vertex normal of obj
std::vector<face> obj_fPool;		//all faces in obj [face: A plane shape that has 4 or 3 point.]

std::vector<triangle> obj_tPool;	//all triangles in obj
std::vector<face> obj_f3Pool;		//all faces in obj [based on the triangles in obj_tPool]
std::vector<vertex> obj_normals;	//normals of triangles in obj_tPool

//Storage vector of Voxel
std::vector< vertex > voxel_candidate; 		//ver 3.0
std::vector< vector <vertex> > all_y_layer; //ver 		2.0
std::vector< vertex > y_layer;				//ver 		2.0
std::vector< vertex > voxel_center_vPool;	//ver 		2.0 & 1.0
std::vector< vertex > x_strap;				//ver 3.0 & 2.0
std::vector< vector <vertex> > all_xy_strap;//ver 3.0 & 2.0
std::vector< vertex > voxel_bone_position;      //ver 3.0

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

    face f3tmp;
    for(int i=0; i<obj_fPool.size(); i++){
        if(obj_fPool[i].v4!=0){
            tri.v1 = obj_vPool[ obj_fPool[i].v1-1 ];
            tri.v2 = obj_vPool[ obj_fPool[i].v2-1 ];
            tri.v3 = obj_vPool[ obj_fPool[i].v3-1 ];

            obj_tPool.push_back(tri);

            f3tmp.v1 = obj_fPool[i].v1 ;
            f3tmp.v2 = obj_fPool[i].v2 ;
            f3tmp.v3 = obj_fPool[i].v3 ;

            obj_f3Pool.push_back(f3tmp);


            tri.v1 = obj_vPool[ obj_fPool[i].v1-1 ];
            tri.v2 = obj_vPool[ obj_fPool[i].v3-1 ];
            tri.v3 = obj_vPool[ obj_fPool[i].v4-1 ];

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

            obj_tPool.push_back(tri);

            f3tmp.v1 = obj_fPool[i].v1 ;
            f3tmp.v2 = obj_fPool[i].v2 ;
            f3tmp.v3 = obj_fPool[i].v3 ;

            obj_f3Pool.push_back(f3tmp);
        }
    }

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

	if( (midx - hxn*voxel_length - min_x) < voxel_length_half )
    {	min_x = midx - hxn*voxel_length - voxel_length_half; }
    else{	min_x = midx - (hxn+1)*voxel_length; }

    if( (midy - hyn*voxel_length - min_y) < voxel_length_half )
    {	min_y = midy - hyn*voxel_length - voxel_length_half; }
    else{	min_y = midy - (hyn+1)*voxel_length; }

    if( (midz - hzn*voxel_length - min_z) < voxel_length_half )
    {	min_z = midz - hzn*voxel_length - voxel_length_half; }
    else{	min_z = midz - (hzn+1)*voxel_length; }
/*
    min_x = min_x - voxel_length_half;
    min_y = min_y - voxel_length_half;
    min_z = min_z - voxel_length_half;
*/
    vertex voxel_center_test;

	//Voxel Ver.3

	for(int i=0; i<(xn*yn); i++){//creat enough space for all_xy_strap
    	all_xy_strap.push_back(x_strap);
    }
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
        vertex vonloop;
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


    //get voxel face
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
    	min_tx = min_x + xtt*voxel_length + voxel_length_half;
    	min_ty = min_y + ytt*voxel_length + voxel_length_half;
        min_tz = min_z + ztt*voxel_length + voxel_length_half;

    	vertex candidate;

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
               (!outsideTheTrianglexz(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3) && !outsideTheTrianglezy(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3)) ||
               (!outsideTheTriangle(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3) && !outsideTheTrianglexz(voxel_candidate[j], obj_tPool[i].v1, obj_tPool[i].v2, obj_tPool[i].v3)) ||
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

                //if((xtt+j%xt) + (ytt+j/xt)*xn < all_xy_strap.size() )
    			//all_xy_strap[ (xtt+j%xt) + (ytt+j/xt)*xn ].push_back(voxel_candidate[j]);
    		}
    	}

    }

    //fill shell voxel
    int start;
    int end;
    bool recording = false;
    vertex voxel_center_tmp;
    for(int i=0; i<all_xy_strap.size(); i++){
    	for(int j=0; j<all_xy_strap[i].size(); j++){
/*
    		if(recording==false && obj_vNormal[ all_xy_strap[i][j].num ].z >= 0){//if normal.z >=0
    			start = (all_xy_strap[i][j].z- min_z) / voxel_length;
                recording = true;
    		}
            if(recording==true && obj_vNormal[ all_xy_strap[i][j].num ].z < 0){
                end = (all_xy_strap[i][j].z- min_z) / voxel_length;
                for(int k=start; k<end+1; k++){
                    //define the voxel's position
                    voxel_center_tmp.x = (i/yn)*voxel_length + min_x - voxel_length_half;
                    voxel_center_tmp.y = (i%yn)*voxel_length + min_y - voxel_length_half;
                    voxel_center_tmp.z =      k*voxel_length + min_z - voxel_length_half;
                    //push the voxel into voxel pool
                    voxel_center_vPool.push_back(voxel_center_tmp);
                }
                recording = false;
            }
*/

            int k;
            k = (all_xy_strap[i][j].z- min_z) / voxel_length;
            //define the voxel's position
            voxel_center_tmp.x = (i%xn)*voxel_length + min_x + voxel_length_half;
            voxel_center_tmp.y = (i/xn)*voxel_length + min_y + voxel_length_half;
            voxel_center_tmp.z =      k*voxel_length + min_z + voxel_length_half;
            //push the voxel into voxel pool
            voxel_center_vPool.push_back(voxel_center_tmp);
        }
        recording = false;
        start = 0;
        end = 0;
    }

    //fill inside part
    int top, buttom, left, right, front, hind;
    bool t_found = false;
    bool b_found = false;
    bool l_found = false;
    bool r_found = false;
    bool f_found = false;
    bool h_found = false;
/*
    cout<< xn <<" "<< yn <<" "<< zn <<endl;
    cout<< voxel_center_vPool.size() <<endl;
    for(int i=0; i<xn; i++){
        for(int j=0; j<voxel_center_vPool.size(); j++){
            if( abs( midx + i*voxel_length - voxel_center_vPool[j].x )<voxel_length &&
                abs( midy - voxel_center_vPool[j].y )<alert_range*voxel_length &&
                abs( midz - voxel_center_vPool[j].z )<alert_range*voxel_length &&
                (r_found==false)){
                cout<<" yeah" <<endl;
                right=i;
                r_found=true;
            }
            if( abs( midx - i*voxel_length - voxel_center_vPool[j].x )<voxel_length &&
                abs( midy - voxel_center_vPool[j].y )<alert_range*voxel_length &&
                abs( midz - voxel_center_vPool[j].z )<alert_range*voxel_length &&
                (l_found==false)){
                left=i;
                l_found=true;
            }
        }
    }
    for(int i=0; i<yn; i++){
        for(int j=0; j<voxel_center_vPool.size(); j++){
            if( abs( midx - voxel_center_vPool[j].x )<alert_range*voxel_length &&
                abs( midy + i*voxel_length - voxel_center_vPool[j].y )<voxel_length &&
                abs( midz - voxel_center_vPool[j].z )<alert_range*voxel_length &&
                (t_found==false)){
                top=i;
                t_found=true;
            }
            if( abs( midx - voxel_center_vPool[j].x )<alert_range*voxel_length &&
                abs( midy - i*voxel_length - voxel_center_vPool[j].y )<voxel_length &&
                abs( midz - voxel_center_vPool[j].z )<alert_range*voxel_length &&
                (b_found==false)){
                buttom=i;
                b_found=true;
            }
        }
    }
    for(int i=0; i<zn; i++){
        for(int j=0; j<voxel_center_vPool.size(); j++){
            if( abs( midx - voxel_center_vPool[j].x )<alert_range*voxel_length &&
                abs( midy - voxel_center_vPool[j].y )<alert_range*voxel_length &&
                abs( midz + i*voxel_length - voxel_center_vPool[j].z )<voxel_length &&
                (f_found==false)){
                front=i;
                f_found=true;
            }
            if( abs( midx - voxel_center_vPool[j].x )<alert_range*voxel_length &&
                abs( midy - voxel_center_vPool[j].y )<alert_range*voxel_length &&
                abs( midz - i*voxel_length - voxel_center_vPool[j].z )<voxel_length &&
                (h_found==false)){
                hind=i;
                h_found=true;
            }
        }
    }
    cout<< top <<" "<< buttom <<"\n";
    cout<< right <<" "<< left <<"\n";
    cout<< front <<" "<< hind <<"\n";
    vertex voxel_bone;
    for(int i=0; i<(top+ buttom)*(right+ left)*(front+ hind); i++){
        voxel_bone.x = voxel_length_half + midx - left*voxel_length + (i%(right+ left))*voxel_length;
        voxel_bone.y = voxel_length_half + midy - buttom*voxel_length + ((i/(right+ left))%(top+buttom))*voxel_length;
        voxel_bone.z = voxel_length_half + midz - hind*voxel_length + (i/((right+ left)*(top+ buttom)))*voxel_length;
        voxel_bone_position.push_back(voxel_bone);
    }
    cout<< voxel_bone_position.size() <<"\n";
*/

    //Voxel Ver.2
/*
    //y_layer.clear();
    for(int i=0; i<yn; i++){//creat enough space for all_y_layer
    	all_y_layer.push_back(y_layer);
    }
    cout<< "\n" <<"problem" <<endl;
    int layer;
    for(int i=0; i<obj_vPool.size(); i++){
    	layer = (obj_vPool[i].y-min_y) / voxel_length;
        obj_vPool[i].num = i;
        //cout<< "\n" <<"1" <<endl;
        if(layer < all_y_layer.size())
    	all_y_layer[layer].push_back(obj_vPool[i]);
        //cout<< "\n" <<"2" <<endl;
    }

    //x_strap.clear();
    for(int i=0; i<(xn*yn); i++){//creat enough space for all_xy_strap
    	all_xy_strap.push_back(x_strap);
    }
    //cout<< "\n" <<"problem start" <<endl;
    int layer2;
    for(int i=0; i<all_y_layer.size(); i++){
    	for(int j=0; j<all_y_layer[i].size(); j++){
    		layer  = (all_y_layer[i][j].x-min_x) / voxel_length;
    		layer2 = i;
            //cout<< "\n" <<"1" <<endl;
            if(layer < xn && layer2 < yn)
    		all_xy_strap[ layer + layer2*xn ].push_back(all_y_layer[i][j]);
            //cout<< "\n" <<"2" <<endl;
    	}
    }
    //cout<< "\n" <<"problem end" <<endl;

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
            //push the voxel into voxel pool
            voxel_center_vPool.push_back(voxel_center_tmp);
        }
        recording = false;
        start = 0;
        end = 0;
    }
*/

	//Voxel  Ver.1
/*
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
*/
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

void init(void)
{

    read_obj();
    /*
     char* list = "40234_Rooster_reduced.txt";
     load_lego_parts_list(list);
     */
    readMa(ma);

    for(int i=0; i<12; i++){
        metrix_O[i] = metrix_O[i]*rate;
    }

    part_v1 part0;

    string partt = "87087.dat";//"3005.dat";3024 3070b
    search_or_read(partt, false, metrix_O);
    for(int i=0; i<trianglePool.size(); i++){
        part0.tpfp.push_back(trianglePool[i]);
        part0.normal_pool.push_back(tmpNormalPool[i]);
    }
    parts.push_back(part0);
    trianglePool.clear();
    tmpNormalPool.clear();

    part0.tpfp.clear();
    partt = "11477.dat";//"3005.dat";3024 3070b; 11477
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

    cout<< "\n";
    cout<< "size of parts: " << parts.size() << " " <<endl;
    cout<< "size of part 11477 : " << parts[1].tpfp.size() << " " <<endl;
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
}

void drawPart(int p_number){
    for(int i=0; i < parts[p_number].tpfp.size() ; i++){
        glColor3f(0.0f,0.0f,1.0f);
        glBegin(GL_TRIANGLES);
        glNormal3f( parts[p_number].normal_pool[i].x, parts[p_number].normal_pool[i].y, parts[p_number].normal_pool[i].z );
        glVertex3f( parts[p_number].tpfp[i].v1.x, parts[p_number].tpfp[i].v1.y, parts[p_number].tpfp[i].v1.z);
        glVertex3f( parts[p_number].tpfp[i].v2.x, parts[p_number].tpfp[i].v2.y, parts[p_number].tpfp[i].v2.z);
        glVertex3f( parts[p_number].tpfp[i].v3.x, parts[p_number].tpfp[i].v3.y, parts[p_number].tpfp[i].v3.z);
        glEnd();
        if(!drawlegoFrame){
            glColor3f(1.0f,1.0f,1.0f);
            glBegin(GL_LINE_LOOP);
            glNormal3f( parts[p_number].normal_pool[i].x, parts[p_number].normal_pool[i].y, parts[p_number].normal_pool[i].z );
            glVertex3f( parts[p_number].tpfp[i].v1.x, parts[p_number].tpfp[i].v1.y, parts[p_number].tpfp[i].v1.z);
            glVertex3f( parts[p_number].tpfp[i].v2.x, parts[p_number].tpfp[i].v2.y, parts[p_number].tpfp[i].v2.z);
            glVertex3f( parts[p_number].tpfp[i].v3.x, parts[p_number].tpfp[i].v3.y, parts[p_number].tpfp[i].v3.z);
            glEnd();
        }
    }
}

void drawMa(){
    for(int i=0; i < ma_ePool.size() ; i++){
        glColor3f(0.0f,1.0f,0.0f);
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
//-----------------------------------------------------/
    glPushMatrix();
    glTranslated(-2.8,0.0, dis);//glTranslated(-2.4,1.2,-6);

    glRotated(-90 + rota,1,0,0);
    glRotated(25 + rotate1,0,0,1);

    //drawObj_t(true);
    drawMa();
    //glutSolidSphere(1,slices,stacks);
    glPopMatrix();
//-----------------------------------------------------/
    glPushMatrix();
    glTranslated(0,0.0, dis);//glTranslated(-2.4,1.2,-6);

    glRotated(-90.0 + rota,1,0,0);
    glRotated(0 + rotate1,0,0,1);

    drawObj_t(false);
    if(!drawlegoFrame)
    drawVoxel();
    //glutSolidSphere(1,slices,stacks);
    glPopMatrix();
//-----------------------------------------------------/
    glPushMatrix();
    glTranslated(2.8,0.0, dis);//glTranslated(2.4,1.2,-6);

    glRotated(-90 + rota,1,0,0);
    glRotated(-25 + rotate1,0,0,1);

    glBegin(GL_POINTS);
    drawObj_p();
    drawObj_in_p();
    glEnd();
    if(drawlegoFrame)
    drawVoxel();
    //glutSolidTorus(0.2,0.8,slices,stacks);
    glPopMatrix();
//-----------------------------------------------------/
    for(int i=0; i<parts.size(); i++){
        glPushMatrix();
        glTranslated(1.4,-1+oheight,-4);//glTranslated(0,1.2,-6);
        glTranslatef(0.0, add*i, 0.0);
        glRotated(30 + rotate1,0,1,0);
        glRotated(180,1,0,0);
        drawPart(i);

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
            dis = -4.5;
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
const GLfloat light_position[] = { 20.0f, 20.0f, 0.0f, 0.0f };

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

    glutCreateWindow("Lego Assembler Ver 2.0");

    init();

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    //glClearColor(0.5,0.5,0.5,1);
    glClearColor(0.0,0.0,0.0,1);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
/*
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
*/
    glutMainLoop();

    return EXIT_SUCCESS;
}
