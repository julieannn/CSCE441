#include "ScanConvert.h"
#include "PolygonDrawer.h"
#include "VectorOperations.h"
#include <algorithm>
#include <math.h>
#include <iostream>

using namespace std;

class Edge
{
public:
	float slopeRecip;
	float maxY;
	float currentX;
	float currentZ; //interpolate z values
	float zinc;
	vertex currentF; //interpolate color/normal values
	vertex finc;

	bool operator < ( const Edge &e )
	{
		if ( currentX == e.currentX )
		{
			return slopeRecip < e.slopeRecip;
		}
		else
		{
			return currentX < e.currentX;
		}
	}
};

vector<vector<Edge> > activeEdgeTable;
vector<Edge> activeEdgeList;

void buildActiveEdgeTable ( vector<Pt> &points )
{
	int i;

	activeEdgeTable.clear ( );

	// add rows equal to height of image to active edge table
	for ( i = 0; i < ImageH; i++ )
	{
		vector<Edge> row;

		activeEdgeTable.push_back ( row );
	}

	for ( i = 0; i < points.size ( ); i++ )
	{

		Edge e;
		int next = ( i + 1 ) % points.size ( );

		// ignore horizontal lines
		if ( points [ i ].y ==  points [ next ].y )
		{
			continue;
		}
		e.maxY = max ( points [ i ].y, points [ next ].y );
		e.slopeRecip = ( points [ i ].x - points [ next ].x ) / (float)( points [ i ].y - points [ next ].y );
		e.finc = scale( sub(points[i].c , points[next].c) , (1.0/(float)(points[i].y - points[next].y)) );
		e.zinc = (points[i].z - points[next].z) / (float)(points[i].y - points[next].y);
		if ( points [ i ].y == e.maxY )
		{
			e.currentX = points [ next ].x;
			e.currentF = points[next].c;
			e.currentZ = points[next].z;
			activeEdgeTable [ points [ next ].y ].push_back ( e );
		}
		else
		{
			e.currentX =  points [ i ].x;
			e.currentF = points[i].c;
			e.currentZ = points[i].z;
			activeEdgeTable [ points [ i ].y ].push_back ( e );
		}
	}
}

// assumes all vertices are within window!!!
void drawPolygon ( vector<Pt> points , color c, bool phong)
{
	int x, y, i;
	float z = 3; //static per poly
	vertex dF = vertex(-3, 0, 0); //static per poly
	activeEdgeList.clear ( );
	buildActiveEdgeTable ( points );

	for ( y = 0; y < ImageH; y++ )
	{
		// add edges into active Edge List
		for ( i = 0; i < activeEdgeTable [ y ].size ( ); i++ )
		{
			activeEdgeList.push_back ( activeEdgeTable [ y ] [ i ] );
		}

		// delete edges from active Edge List
		for ( i = 0; i < activeEdgeList.size ( ); i++ )
		{
			if ( activeEdgeList [ i ].maxY <= y )
			{
				activeEdgeList.erase ( activeEdgeList.begin ( ) + i );
				i--;
			}
		}

		// sort according to x value... a little expensive since not always necessary
		sort ( activeEdgeList.begin ( ), activeEdgeList.end ( ) );

		// draw scan line
		for ( i = 0; i < activeEdgeList.size ( ); i += 2 )
		{
			//get current values to interpolate
			vertex f = activeEdgeList[i].currentF;
			float curz = activeEdgeList[i].currentZ;
			
			for ( x = (int)ceil ( activeEdgeList [ i ].currentX ); x < activeEdgeList [ i + 1 ].currentX; x++ )
			{
				//set these values at only the first line we draw
				if (dF.x == -3) { 
					dF = sub(activeEdgeList[i + 1].currentF, activeEdgeList[i].currentF);
					dF = scale(dF, (1 / (activeEdgeList[i + 1].currentX- activeEdgeList[i].currentX)));
					f = activeEdgeList[i].currentF;
				}
				if (z == 3) {
					z = (activeEdgeList[i + 1].currentZ - activeEdgeList[i].currentZ) / (activeEdgeList[i + 1].currentX - activeEdgeList[i].currentX);
				}

				//check the z buffer and draw if the value is closer
				//closer is more negative since eye is at -infinity
				float checkz = getZbuffer(x, y);
				if (checkz >= curz) {

					//update the zbuffer
					setZbuffer(x, y, curz);
					if (c.r != -1.0 && c.g != -1.0 && c.b != -1.0) //not dummy value from main = flat
						setFramebuffer(x, y, c.r, c.g, c.b);
					else {
						if (!phong) //!phong = gouraud 
							c = vertextocolor(f);
						else { //calculate lighting equation per pixel
							c = lightingEq(f);
						}
						setFramebuffer(x, y, c.r, c.g, c.b);

						//update interpolated values
						float tempx = activeEdgeList[i].currentX;
						if (tempx < 1) {
							tempx = x - tempx;
						}
						else
							tempx = 1.0;
						f = add(f, scale(dF, tempx));
						curz = curz + tempx*z;

						//reset color to dummy value
						c = color(-1.0, -1.0, -1.0);
					}
				}
			}
		}

		// update edges in active edge list
		for ( i = 0; i < activeEdgeList.size ( ); i++ )
		{
			activeEdgeList [ i ].currentX += activeEdgeList [ i ].slopeRecip;
			activeEdgeList[i].currentF = add(activeEdgeList[i].currentF, activeEdgeList[i].finc);
			activeEdgeList[i].currentZ += activeEdgeList[i].zinc;
		}
	}
}