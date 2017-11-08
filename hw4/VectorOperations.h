#include "PolygonDrawer.h"

vertex cross(vertex a, vertex b);
vertex sub(vertex a, vertex b);
float dot(vertex a, vertex b);
vertex multiply(vertex a, vertex b); //not used but might need it
vertex divide(vertex a, vertex b);
vertex add(vertex a, vertex b);
vertex scale(vertex a, float b);
vertex normalize(vertex a);
color vertextocolor(vertex v);
vertex colortovertex(color c);
vertex averageNormal(vertex v);
color lightingEq(vertex normal);
void printvert(vertex v);

