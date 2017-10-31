#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <dirent.h>

#include <cstdlib>

#include <vector>

using namespace std;

char* file_name = "Armadillo.obj";
//char* file_name = "suzanne.obj";
//char* file_name = "gepard.obj";

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

//Storage vectors list of ABCD.obj input
std::vector<vertex> obj_vPool;
std::vector<face> obj_fPool;
std::vector<triangle> obj_tPool;
std::vector<edge> edge_pool;


//Storage vector of Voxel
std::vector<vertex> voxel_center_vPool;

//int edgeNumber=0;

void read_obj(char* name){
    ifstream infile(name);
	vertex v;
    face f;
    string line;
    string first;
    string a,b,c,d;

    while (std::getline(infile, line))
    {
        std::istringstream iss(line);
        if(iss >> first){
            if(first[0]=='v' && !(first[1]=='t'||first[1]=='n') ){
                iss >> v.x >> v.y >> v.z;
                obj_vPool.push_back(v);
            }
            if(first[0]=='f'){
            	string tmstr;
                iss >> a >> b >> c;
                //cout<< a <<" "<< a.size() << "\n";

                if(iss >> d){
                    cout<< c;
                	int i=0;
                	for(i=0; i< a.size();i++){
                		if(a[i]=='/'){
                            a.resize(i);
                            break;
                        }
                	}tmstr= a;
                	f.v1= atoi(tmstr.c_str());
                    tmstr.clear();

                	for(i=0; i< b.size();i++){
                		if(b[i]=='/'){
                            b.resize(i);
                            break;
                        }
                	}tmstr= b;
                	f.v2= atoi(tmstr.c_str());
                	tmstr.clear();

                	for(i=0; i< c.size();i++){
                		if(c[i]=='/'){
                            c.resize(i);
                            break;
                        }
                	}tmstr= c;
                    //cout<<' '<< tmstr <<"\n";
                	f.v3= atoi(tmstr.c_str());
                	tmstr.clear();


                	for(i=0; i< d.size();i++){
                		if(d[i]=='/'){
                            d.resize(i);
                            break;
                        }
                	}tmstr= d;
                	f.v4= atoi(tmstr.c_str());
                	tmstr.clear();
                    obj_fPool.push_back(f);

                    f.v2=f.v1;//swap v1 & v3
                    f.v1=f.v3;//swap
                    f.v3=f.v2;//swap

                    f.v2=f.v4;
                    obj_fPool.push_back(f);
                }else{
                	tmstr.clear();
                	int i=0;
                	for(i=0; i< a.size();i++){
                		if(a[i]=='/'){
                            a.resize(i);
                            break;
                        }
                	}tmstr= a;
                	f.v1= atoi(tmstr.c_str());
                	tmstr.clear();

                	for(i=0; i< b.size();i++){
                		if(b[i]=='/'){
                            b.resize(i);
                            break;
                        }
                	}tmstr= b;
                	f.v2= atoi(tmstr.c_str());
                	tmstr.clear();

                	for(i=0; i< c.size();i++){
                		if(c[i]=='/'){
                            c.resize(i);
                            break;
                        }
                	}tmstr= c;
                	f.v3= atoi(tmstr.c_str());
                	tmstr.clear();

                    f.v4=0.0;
                    obj_fPool.push_back(f);
                }

            }/*
            if(first[0]=='f'){
            	string tmstr;
                iss >> f.v1 >> f.v2 >> f.v3;
                if(iss >> f.v4){

                    obj_fPool.push_back(f);

                    f.v2=f.v1;//swap v1 & v3
                    f.v1=f.v3;//swap
                    f.v3=f.v2;//swap

                    f.v2=f.v4;
                    obj_fPool.push_back(f);
                }else{

                	f.v4=0.0;
                    obj_fPool.push_back(f);
                }

            }*/
        }
    }

    edge e;
    for(int i=0; i<obj_fPool.size(); i++){
        e.p1 = obj_fPool[i].v1;
        e.p2 = obj_fPool[i].v2;
        edge_pool.push_back(e);

        e.p1 = obj_fPool[i].v2;
        e.p2 = obj_fPool[i].v3;
        edge_pool.push_back(e);

        e.p1 = obj_fPool[i].v3;
        e.p2 = obj_fPool[i].v1;
        edge_pool.push_back(e);
    }
    for(int i=0; i<edge_pool.size(); i++){
        for(int j=i+1; j<edge_pool.size(); j++){
            //if two edges in pool are the same, delete one.
            if( (edge_pool[i].p1==edge_pool[j].p1 && edge_pool[i].p2==edge_pool[j].p2)||
                (edge_pool[i].p1==edge_pool[j].p2 && edge_pool[i].p2==edge_pool[j].p1) ){
                edge_pool[j]=edge_pool[edge_pool.size()-1];
                edge_pool.pop_back();
            }
        }
    }

}

void write_off(){
    ofstream myfile;
    myfile.open ("Armadillo.off");
    //myfile.open ("suzanne.off");
    //myfile.open ( "gepard.off");
    myfile << "OFF\n";
    myfile << obj_vPool.size() <<' '<< obj_fPool.size() <<' '<< edge_pool.size() <<"\n";
    cout << obj_vPool.size() <<' '<< obj_fPool.size() <<' '<< edge_pool.size() <<"\n";
    for(int i=0; i<obj_vPool.size(); i++){
        myfile << obj_vPool[i].x <<' '<< obj_vPool[i].y <<' '<< obj_vPool[i].z <<"\n";
    }
    for(int i=0; i<obj_fPool.size(); i++){
        myfile << "3" <<' '<< obj_fPool[i].v1-1 <<' '<< obj_fPool[i].v2-1 <<' '<< obj_fPool[i].v3-1 <<"\n";;
    }
    myfile.close();
}

/*
void write_ma(){
    ofstream myfile2;
    myfile2.open ( "suzanne.ma");
    myfile2 << "OFF\n";
    myfile2 << obj_vPool.size() <<' '<< obj_fPool.size() <<' '<< edge_pool.size() <<"\n";
    for(int i=0; i<obj_vPool.size(); i++){
        myfile2 << "v" << obj_vPool[i].x <<' '<< obj_vPool[i].y <<' '<< obj_vPool[i].z <<"\n";
    }
    for(int i=0; i<edge_pool.size(); i++){
        myfile2 << "e" <<' '<< edge_pool[i].p1 <<' '<< edge_pool[i].p2  <<"\n";;
    }
    for(int i=0; i<obj_fPool.size(); i++){
        myfile2 << "f" <<' '<< obj_fPool[i].v1 <<' '<< obj_fPool[i].v2 <<' '<< obj_fPool[i].v3 <<"\n";;
    }
    myfile2.close();
}
*/

int main()
{
    read_obj(file_name);
    write_off();

    return 0;
}
