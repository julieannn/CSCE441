#include <Windows.h>
#include <GL/glut.h>
#include <math.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include "ScanConvert.h"
#include "PolygonDrawer.h"
#include "VectorOperations.h"


using namespace std;

/******************************************************************
	Notes:
	Image size is 400 by 400 by default.  You may adjust this if
		you want to.
	You can assume the window will NOT be resized.
	Call clearFramebuffer to clear the entire framebuffer.
	Call setFramebuffer to set a pixel.  This should be the only
		routine you use to set the color (other than clearing the
		entire framebuffer).  drawit() will cause the current
		framebuffer to be displayed.
	As is, your scan conversion should probably be called from
		within the display function.  There is a very short sample
		of code there now.
	You may add code to any of the subroutines here,  You probably
		want to leave the drawit, clearFramebuffer, and
		setFramebuffer commands alone, though.
  *****************************************************************/

float framebuffer[ImageH][ImageW][3];
float zbuffer[ImageH][ImageW][1];//for z buffer hidden surface removal

vector<Pt> points; //vertices in window mode without z coord
vector<face> faces; //all of our faces
vector<vertex> vertices;//all of our vertices

//mode booleans changed with keyboard 
boolean flat = true;
boolean gouraud = false;
boolean phong = false;

// Draws the scene
void drawit(void)
{
   glDrawPixels(ImageW,ImageH,GL_RGB,GL_FLOAT,framebuffer);
   glFlush();
}
float getZbuffer(int x, int y) {
	return zbuffer[x][y][0];
}
void setZbuffer(int x, int y, float z) {
	zbuffer[x][y][0] = z;
}
void initzbuffer() {
	for (int i = 0; i < ImageH; i++) {
		for (int j = 0; j < ImageW; j++) {
			zbuffer[i][j][0] = (float) 3.0; //set to facing away (all z values should be less than this)
		}
	}
}
// Clears framebuffer to black
void clearFramebuffer()
{
	int i,j;

	for(i=0;i<ImageH;i++) {
		for (j=0;j<ImageW;j++) {
			framebuffer[i][j][0] = 0.0;
			framebuffer[i][j][1] = 0.0;
			framebuffer[i][j][2] = 0.0;
		}
	}
}


// Sets pixel x,y to the color RGB
// I've made a small change to this function to make the pixels match
// those returned by the glutMouseFunc exactly - Scott Schaefer 

void setFramebuffer(int x, int y, float R, float G, float B)
{
	// changes the origin from the lower-left corner to the upper-left corner
	y = ImageH - 1 - y;
	if (R<=1.0)
		if (R>=0.0)
			framebuffer[y][x][0]=R;
		else
			framebuffer[y][x][0]=0.0;
	else
		framebuffer[y][x][0]=1.0;
	if (G<=1.0)
		if (G>=0.0)
			framebuffer[y][x][1]=G;
		else
			framebuffer[y][x][1]=0.0;
	else
		framebuffer[y][x][1]=1.0;
	if (B<=1.0)
		if (B>=0.0)
			framebuffer[y][x][2]=B;
		else
			framebuffer[y][x][2]=0.0;
	else
		framebuffer[y][x][2]=1.0;
}

void display(void)
{
	drawit();
}

//change from screen to window coordinates
void convertPoints(vector<vertex> v) {
	for (int i = 0; i < v.size(); i++) {
		float x = v[i].x;
		float y = v[i].y;
		//formula from ppt slides
		Pt point;
		point.x = (int) ((ImageW - 1) / 2.0) * (1 + x);
		point.y = (int) ((ImageH - 1) / 2.0) * (1 - y);
		point.z = v[i].z;
		points.push_back(point);
	}
}

//compute the normal of the face
face computeNormal(face f) {
	vertex norm1;
	vertex norm2;
	vertex v1 = vertices[f.p1 - 1];
	vertex v2 = vertices[f.p2 - 1];
	vertex v3 = vertices[f.p3 - 1];
	norm1 = sub(v3, v1);
	norm2 = sub(v2, v1);
	f.normal = cross(norm1, norm2);
	f.normal = normalize(f.normal);
	return f;
}
color randomColor() { //for testing purposes
	float r, g, b;
	r = ((float)rand() / (RAND_MAX));
	g = ((float)rand() / (RAND_MAX));
	b = ((float)rand() / (RAND_MAX));
	return color(r,g,b);
}

//draws the object loaded from the file
void drawObject() {
	if (!points.empty()) {
		initzbuffer();
		//send to the scan converter
		//need to modify it to take vertices so we can do z buffer
		cout << "drawing...\n";
		for (int i = 0; i < faces.size(); i++) {
			//draw each polygon face, faces index at 1 so index at 0 for the vector
			//calculate the color of the polygon
			color result = color(-1.0, -1.0, -1.0); //tells us to interpolate
			face f = faces[i];
			vector<Pt> pts;
			pts.push_back(points[(f.p1 - 1)]);
			pts.push_back(points[(f.p2 - 1)]);
			pts.push_back(points[(f.p3 - 1)]);
			if (flat) {
				//send the color of the polygon
				result = lightingEq(f.normal);
			}
			if (gouraud) {
				//set c to the color from lighting equation
				pts[0].c = colortovertex(lightingEq(averageNormal(vertices[f.p1 - 1])));
				pts[1].c = colortovertex(lightingEq(averageNormal(vertices[f.p2 - 1])));
				pts[2].c = colortovertex(lightingEq(averageNormal(vertices[f.p3 - 1])));
			}
			if (phong) {
				//set c to the normal
				pts[0].c = averageNormal(vertices[f.p1 - 1]);
				pts[1].c = averageNormal(vertices[f.p2 - 1]);
				pts[2].c = averageNormal(vertices[f.p3 - 1]);
			}
			drawPolygon(pts, result, phong);
		}
		cout << "done.\n";
		drawit();
	}
	else
		cout << "failed...\n";
}

//parse the obj file
//initialize all the variables
void parseObj(char* file) {
	points.clear();
	vertices.clear();
	faces.clear();
	//open the file
	int f = 0;
	cout << "File name: " << file << "\n";
	string line;
	ifstream objfile;
	objfile.open(file);
	if (objfile.is_open())
	{
		cout << "open success...\n";
		while (getline(objfile, line))
		{
			if (line[0] == 'v') {
				//add to vertex list
				vector<float> coords;
				std::istringstream iss(line);
				int i = 0;
				for (string s; iss >> s; ) {
					if(i != 0) //ignore the v
						coords.push_back(stof(s));
					i++;
				}
				vertex p = vertex(coords[0], coords[1], coords[2]);
				vertices.push_back(p);
			}
			else if (line[0] == 'f') {
				//add to face list
				face fc;
				vector<int> verts;
				std::istringstream iss(line);
				int i = 0;
				for (string s; iss >> s; ) {
					if (i != 0) //ignore the f
						verts.push_back(stoi(s));
					i++;
				}
				fc = face(verts[0], verts[1], verts[2]);
				fc = computeNormal(fc);
				vertices[fc.p1 - 1].faces.push_back(fc);
				vertices[fc.p2 - 1].faces.push_back(fc);
				vertices[fc.p3 - 1].faces.push_back(fc);
				//cout << "Normal: "<< fc.normal.x << " " << fc.normal.y << " " << fc.normal.z << "\n";
				if (fc.normal.z < 0) {
					//backface culling maybe
					faces.push_back(fc);
				}
			}
		}
		objfile.close();
	}
	if (!vertices.empty()) {
		cout << "successfully got vertices\n";

		//convert to window coordinates
		cout << "converting points...\n";
		convertPoints(vertices);
		cout << "done.\n";
	}
	cout << "press 1, 2, or 3 to draw the object.\n";
	cout << "press another number to change mode after object is drawn.\n";
	
}
void keyboard(unsigned char key, int x, int y)
{
	switch(key) {
		case '1': {
			flat = true;
			gouraud = false;
			phong = false;
			break;
		}
		case '2': {
			flat = false;
			gouraud = true;
			phong = false;
			break;
		}
		case '3': {
			flat = false;
			gouraud = false;
			phong = true;
			break;
		}
	}
	clearFramebuffer();
	drawObject();
}

void init(void){}

int main(int argc, char** argv)
{
	glutInit(&argc,argv);
	parseObj(argv[1]);//parse the object file
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(ImageW,ImageH);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Julie Thompson - Homework 4");
	init();	
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
