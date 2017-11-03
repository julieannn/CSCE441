#ifndef POLYGON_DRAWER_H
#define POLYGON_DRAWER_H


#include <vector>



using namespace std;

struct color {
	float r, g, b;		// Color (R,G,B values)
	color(float nR, float nG, float nB) {
		r = nR;
		g = nG;
		b = nB;
	}
};

struct face;
struct vertex {
	float x, y, z;
	//vertex* normal;
	vertex(float nX, float nY, float nZ) {
		x = nX;
		y = nY;
		z = nZ;
	}
	vertex() {
		x = y = z = 0.0;
	}
	vector<face> faces;
};

struct face {
	int p1, p2, p3;
	vertex normal;
	face(int x, int y, int z) {
		p1 = x;
		p2 = y;
		p3 = z;
	}
	face() {
		p1 = p2 = p3 = 0;
	}
};
class Pt
{
public:
	int x, y;
	float z;
	vertex c;
	Pt ( void )
	{
		x = y = z = 0;
	}

	Pt (int nX, int nY )
	{
		x = nX;
		y = nY;
	}
	Pt(int nX, int nY, float nZ)
	{
		x = nX;
		y = nY;
		z = nZ;
	}
};


void drawPolygon ( vector<Pt> points , color c, bool phong);

#endif