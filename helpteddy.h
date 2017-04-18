/*
    GLUT Teddy
    Jui-Cheng,Sung. R.O.C.
    Lyre Mellark.
    Started from 2016.Feb.10
*/

#ifndef __helpteddy_h__
#define __helpteddy_h__


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

using namespace std;

struct vertex
{
    float x;
    float y;
    float z;
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

struct node
{
    int parent;     //parent's num
//    int lsn;        //left son's num
//    int rsn;        //right son's num
    int num;

    int start;      //data
    int end;        //data

    bool root;
    bool ls;        //is left son
    bool rs;        //is right son
    bool alive;     //useful:TRUE; useless:FALSE
    bool leaf;      //is leaf:TRUE; not leaf:FALSE
};
/*

std::vector< vertex >	theSetOfInputPoint ;
std::vector< edge >		edgePool ;
std::vector< triangle > trianglePool ;*/

//help teddy
bool onTheSameSide(vertex test_point, vertex line_start, vertex line_end, vertex compare_point);
bool onTheSameSideCDT(vertex test_point, vertex line_start, vertex line_end, vertex compare_point);
bool onTheEdge(vertex test_point, vertex line_start, vertex line_end);
bool onTheEdgeMid(vertex test_point, vertex line_start, vertex line_end);
bool outsideTheTriangle(vertex testvertex, vertex vertex1, vertex vertex2, vertex vertex3);
bool onTheTriangleEdges(vertex testvertex, vertex vertex1, vertex vertex2, vertex vertex3);
bool insideTheCircle(vertex test_vertex, vertex center_of_circle, float radius);
vertex centerOfCircumscribedCircle(vertex vertex_1, vertex vertex_2, vertex vertex_3);
GLfloat radiusOfCCircle(vertex testvertex, vertex center);
/*
bool isBadTriangle(vertex test_point, vertex center, float radius);
void addToEdgePool( vertex v1 , vertex v2 );
void addToTrianglePool( vertex v1 , vertex v2 , vertex v3 );
bool areSameEdges( edge edge1 , edge edge2 );
*/

#endif
