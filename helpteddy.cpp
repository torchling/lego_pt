/*
    GLUT Teddy
    Jui-Cheng,Sung. R.O.C.
    Lyre Mellark.
    Started from 2016.Feb.10
*/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <vector>

using namespace std;

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

bool onTheSameSide(vertex test_point, vertex line_start, vertex line_end, vertex compare_point)
{
	//calculate y distance from test point to line.
    if(line_start.x!=line_end.x)
    {
        float vx = line_start.x-line_end.x;
        float vy = line_start.y-line_end.y;

        if( (test_point.y - ( line_start.y + vy*(test_point.x-line_start.x)/vx))*
           (compare_point.y - (line_start.y + vy*(compare_point.x-line_start.x)/vx)) <= 0 )
            return false;

        return true;
    }

    //calculate x distance from test point to line.
    if(line_start.x==line_end.x)
    {
        if( (test_point.x-line_start.x)*(compare_point.x-line_start.x) <= 0 )
            return false;

        return true;
    }

}

bool onTheSameSideCDT(vertex test_point, vertex line_start, vertex line_end, vertex compare_point)
{
    //calculate y distance from test point to line.
    if(line_start.x!=line_end.x)
    {
        float vx = line_start.x-line_end.x;
        float vy = line_start.y-line_end.y;

        if( (test_point.y - ( line_start.y + vy*(test_point.x-line_start.x)/vx))*
           (compare_point.y - (line_start.y + vy*(compare_point.x-line_start.x)/vx)) < 0 )
            return false;

        return true;
    }

    //calculate x distance from test point to line.
    if(line_start.x==line_end.x)
    {
        if( (test_point.x-line_start.x)*(compare_point.x-line_start.x) < 0 )
            return false;

        return true;
    }

}

bool outsideTheTriangle(vertex testvertex, vertex vertex1, vertex vertex2, vertex vertex3)
{
    if( onTheSameSide(testvertex, vertex1, vertex2, vertex3) &&
        onTheSameSide(testvertex, vertex3, vertex1, vertex2) &&
        onTheSameSide(testvertex, vertex2, vertex3, vertex1) )
        return false;

    return true;
}

bool onTheEdge(vertex test_point, vertex line_start, vertex line_end)
{
    
    //calculate y distance from test point to line.
    if(line_start.x!=line_end.x)
    {
        float vx = line_start.x-line_end.x;
        float vy = line_start.y-line_end.y;

        if( fabs(test_point.y - ( line_start.y + vy*(test_point.x-line_start.x)/vx)) <=0.00000001  )
            return true;

        return false;
    }

    //calculate x distance from test point to line.
    if(line_start.x==line_end.x)
    {
        if( fabs(test_point.x-line_start.x) <= 0.0000001 )
            return true;

        return false;
    }

}

bool onTheEdgeMid(vertex test_point, vertex line_start, vertex line_end)
{
    /*
	//calculate y distance from test point to line.
    if(line_start.x!=line_end.x)
    {
        float vx = line_start.x-line_end.x;
        float vy = line_start.y-line_end.y;

        if( fabs(test_point.y - ( line_start.y + vy*(test_point.x-line_start.x)/vx)) <=0.00000001  )
            return true;

        return false;
    }

    //calculate x distance from test point to line.
    if(line_start.x==line_end.x)
    {
        if( fabs(test_point.x-line_start.x) <= 0.0000001 )
            return true;

        return false;
    }
    */
    vertex mid_point;
    mid_point.x = (line_start.x + line_end.x)*0.5;
    mid_point.y = (line_start.y + line_end.y)*0.5;
    //mid_point.z = 0.0;//or transform to

    if((mid_point.x == test_point.x)&&(mid_point.y == test_point.y))
        return true;

    return false;

}

bool onTheTriangleEdges(vertex testvertex, vertex vertex1, vertex vertex2, vertex vertex3)
{
    if( onTheEdge(testvertex, vertex1, vertex2) ||
        onTheEdge(testvertex, vertex3, vertex1) ||
        onTheEdge(testvertex, vertex2, vertex3) )
        return true;

    return false;
}

bool insideTheCircle(vertex test_vertex, vertex center_of_circle, float radius)
{
    float distance2;
    distance2 = pow(test_vertex.x-center_of_circle.x, 2.0) + pow(test_vertex.y-center_of_circle.y, 2.0) ;
    if( sqrt(distance2) > radius )
        return false;

    return true;
}

//according to wiki's data
vertex centerOfCircumscribedCircle(vertex vertex_1, vertex vertex_2, vertex vertex_3)
{

    float xc =(pow(vertex_1.x, 2.0)+pow(vertex_1.y, 2.0))*vertex_2.y +
              (pow(vertex_2.x, 2.0)+pow(vertex_2.y, 2.0))*vertex_3.y +
              (pow(vertex_3.x, 2.0)+pow(vertex_3.y, 2.0))*vertex_1.y -
              (pow(vertex_1.x, 2.0)+pow(vertex_1.y, 2.0))*vertex_3.y -
              (pow(vertex_2.x, 2.0)+pow(vertex_2.y, 2.0))*vertex_1.y -
              (pow(vertex_3.x, 2.0)+pow(vertex_3.y, 2.0))*vertex_2.y;
    float xm =vertex_1.x*vertex_2.y +
              vertex_2.x*vertex_3.y +
              vertex_3.x*vertex_1.y -
              vertex_1.x*vertex_3.y -
              vertex_2.x*vertex_1.y -
              vertex_3.x*vertex_2.y;

    float yc =(pow(vertex_1.x, 2.0)+pow(vertex_1.y, 2.0))*vertex_3.x +
              (pow(vertex_2.x, 2.0)+pow(vertex_2.y, 2.0))*vertex_1.x +
              (pow(vertex_3.x, 2.0)+pow(vertex_3.y, 2.0))*vertex_2.x -
              (pow(vertex_1.x, 2.0)+pow(vertex_1.y, 2.0))*vertex_2.x -
              (pow(vertex_2.x, 2.0)+pow(vertex_2.y, 2.0))*vertex_3.x -
              (pow(vertex_3.x, 2.0)+pow(vertex_3.y, 2.0))*vertex_1.x;
    float ym =vertex_1.x*vertex_2.y +
              vertex_2.x*vertex_3.y +
              vertex_3.x*vertex_1.y -
              vertex_1.x*vertex_3.y -
              vertex_2.x*vertex_1.y -
              vertex_3.x*vertex_2.y;
    xm=2*xm;
    ym=2*ym;

    vertex center;
    center.x =xc/xm;
    center.y =yc/ym;
    center.z =0.0;

    return center;
}

GLfloat radiusOfCCircle(vertex testvertex, vertex center)
{
    float radius = pow(testvertex.x-center.x, 2.0) + pow(testvertex.y-center.y, 2.0);
    radius = sqrt(radius);

    return radius;
}

/*

GLfloat vertices[15][3]=
{
    //{ 0.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f },

    //{ 3.5f, -0.5f, 0.0f },
    { 3.5f, -0.5f, 0.0f },

    //{ 3.7f, 1.6f, 0.0f },
    { 3.7f, 1.6f, 0.0f },

    { 0.6f, 3.2f, 0.0f },
    { 0.5f, 6.8f, 0.0f },
    { 3.7f, 9.2f, 0.0f },
    { 7.5f, 8.9f, 0.0f },
    { 10.1f, 6.5f, 0.0f },
    { 9.6f, 3.3f, 0.0f },
    { 6.7f, 1.5f, 0.0f },
    { 6.2f, -1.2f, 0.0f },
    { 8.0f, -4.2f, 0.0f },
    { 4.5f, -4.1f, 0.0f },

    //{ 3.4f, -1.8f, 0.0f },
    { 3.4f, -1.8f, 0.0f },

    { 1.2f, -1.5f, 0.0f }
};

*/

