#ifndef __OBJLOADER_H_INCLUDED__
#define __OBJLOADER_H_INCLUDED__

#include <string>

class objloader{

// points for vertices of triangle
struct point{
	float x;
	float y;
	float z;
};

// element of list of points
struct ptlistelement{
	point A;
	int position;

	ptlistelement * next;
};

public:
// triangle element for list of triangles. public for ease
// of linked list traversal
	struct triangle{
		point A;
		point B;
		point C;

		triangle * next;
	};

private:
// assigned upon initialization
	char filename[];

public:
// list of triangles defining object
	triangle * list;

// constructor
	objloader(char fn[]);
	
// does all the work. builds the list of triangles
	void build_list();

private:
// utility function
	point getPoint(ptlistelement * head, int pos);
};

#endif
