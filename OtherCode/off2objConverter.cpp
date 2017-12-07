#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <dirent.h>

#include <cstdlib>

#include <vector>

using namespace std;

char* file_name = "bug.off";

struct vertex
{
    float x;
    float y;
    float z;
};

struct edge
{
    int p1;
    int p2;
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

//Storage vectors list of ABCD.obj output
std::vector<vertex> obj_vPool;
std::vector<face> obj_fPool;

//Storage vector of Voxel
std::vector<vertex> voxel_center_vPool;

//int edgeNumber=0;
void read_off(char* name){
	ifstream infile(name);
	vertex v;
    face f;
    face size;
    string line;
    string first;
    string a,b,c,d;
    int ct=0;

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        if( ct==1 ){
        	iss>> size.v1 >> size.v2 >> size.v3;
        }
        if( ct>1 && ct< size.v1+2 ){
        	iss >> v.x >> v.y >> v.z;
        	obj_vPool.push_back(v);
        }
        if( ct > size.v1+1 ){
            iss >> f.v4 >> f.v1 >> f.v2 >> f.v3;
            obj_fPool.push_back(f);
        }
        ct++;
    }
}
void write_obj(){
    ofstream myfile;
    myfile.open ("result.obj");
    //myfile.open ("suzanne.off");
    //myfile.open ( "gepard.off");
    //myfile << "OFF\n";
    //myfile << obj_vPool.size() <<' '<< obj_fPool.size() <<' '<< edge_pool.size() <<"\n";
    cout << obj_vPool.size() <<' '<< obj_fPool.size() <<' '<< "0" <<"\n";
    for(int i=0; i<obj_vPool.size(); i++){
        myfile << "v" <<' '<< obj_vPool[i].x <<' '<< obj_vPool[i].y <<' '<< obj_vPool[i].z <<"\n";
    }
    for(int i=0; i<obj_fPool.size(); i++){
        myfile << "f" <<' '<< obj_fPool[i].v1+1 <<' '<< obj_fPool[i].v2+1 <<' '<< obj_fPool[i].v3+1 <<"\n";;
    }
    myfile.close();
}

int main()
{
    read_off(file_name);
    write_obj();

    return 0;
}
