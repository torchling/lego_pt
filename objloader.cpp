#include "objloader.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdlib.h>

using namespace std;



objloader::objloader(char fn[]){
	int i = 0;
	while(fn[i] != '\0')
	{
		filename[i] = fn[i];
		i++;
	}

	filename[i] = fn[i];
}

void objloader::build_list(){

	int tempInt;
	int position = 0;
	ifstream txtFile;
	txtFile.open(filename, ios::in);
	string line;
	int first;
	int second;
	ptlistelement * phead = new ptlistelement();
	ptlistelement * pcurrent = phead;
	triangle * thead = new triangle();
	triangle * tcurrent = thead;

	while(txtFile.good())
	{
		getline(txtFile, line);

		if(line[0] == 'v')
		{
			pcurrent->position = ++position;
			//parse first float and assign it to x coord
			first = line.find(" ");
			second = line.find(" ", first+1);
			pcurrent->A.x = ::atof(line.substr(first+1, second-first-1).c_str());

			//parse second float and assign it to y coord
			first = second;
			second = line.find(" ", first+1);
			pcurrent->A.y = ::atof(line.substr(first+1, second-first-1).c_str());

			//parse third float and assign it to z coord
			pcurrent->A.z = ::atof(line.substr(second+1).c_str());

			pcurrent->next = new ptlistelement();
			pcurrent = pcurrent->next;
		}
		else if(line[0] == 'f')
		{
			first = line.find(" ");
			second = line.find(" ", first+1);
			tempInt = ::atoi(line.substr(first+1, second-first-1).c_str());

			tcurrent->A = getPoint(phead, tempInt);

			//parse second float and assign it to y coord
			first = second;
			second = line.find(" ", first+1);
			tempInt = ::atoi(line.substr(first+1, second-first-1).c_str());

			tcurrent->B = getPoint(phead, tempInt);

			//parse third float and assign it to z coord
			tempInt = ::atoi(line.substr(second+1).c_str());

			tcurrent->C = getPoint(phead, tempInt);

			tcurrent->next = new triangle();
			tcurrent = tcurrent->next;

			cout << line << endl;
		}
		else 
			;

	}

/*

	pcurrent = phead;

	for(int i = 0; i<position; i++)
	{
		cout << "vertex\t( " << pcurrent->A.x << ", "
			 << pcurrent->A.y << ", " << pcurrent->A.z 
			 << ") pos: " << pcurrent->position << "\n";
		pcurrent = pcurrent->next;
		delete phead;
		phead = pcurrent;
	}

	tcurrent = thead;

	while(tcurrent != NULL)
	{
		cout << "(" << tcurrent->A.x << ", " << tcurrent->A.y << ", " << tcurrent->A.z << ")  (" 
			 << tcurrent->B.x << ", " << tcurrent->B.y << ", " << tcurrent->B.z << ")  (" 
			 << tcurrent->C.x << ", " << tcurrent->C.y << ", " << tcurrent->C.z << ")\n"; 
		tcurrent = tcurrent->next;
	}

*/

	txtFile.close();

	list = thead;
}

objloader::point objloader::getPoint(ptlistelement * head, int pos)
{
	point nonpt;

	while(head != NULL)
	{
		if(head->position == pos)
		{
			return head->A;
		}
		else 
		{	
			head = head->next;
		}
	}

	exit(1);
	return nonpt;
}
