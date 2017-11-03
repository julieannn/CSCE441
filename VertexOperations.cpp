#include "VectorOperations.h"
#include <iostream>
using namespace std;
vertex cross(vertex a, vertex b) {
	vertex result;
	result.x = (a.y * b.z) - (a.z * b.y);
	result.y = (a.z * b.x) - (a.x * b.z);
	result.z = (a.x * b.y) - (a.y * b.x);
	return result;
}
vertex sub(vertex a, vertex b) {
	vertex result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}
float dot(vertex a, vertex b) {
	return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z));
}
vertex multiply(vertex a, vertex b) {
	vertex result;
	result.x = a.x * b.x;
	result.y = a.y * b.y;
	result.z = a.z * b.z;
	return result;
}
vertex divide(vertex a, vertex b) {
	vertex result;
	result.x = a.x / b.x;
	result.y = a.y / b.y;
	result.z = a.z / b.z;
	return result;
}
vertex add(vertex a, vertex b) {
	vertex result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}
vertex scale(vertex a, float b) {
	a.x *= b;
	a.y *= b;
	a.z *= b;
	return a;
}
vertex normalize(vertex a) {
	float length = sqrt( a.x*a.x + a.y*a.y + a.z*a.z);
	a.x = a.x / length;
	a.y = a.y / length;
	a.z = a.z / length;
	return a;
}
color vertextocolor(vertex v) {
	color c = color(v.x, v.y, v.z);
	return c;
}
vertex colortovertex(color c) {
	vertex v = vertex(c.r, c.g, c.b);
	return v;
}
vertex averageNormal(vertex v) {
	vertex normal;
	for (int j = 0; j < v.faces.size(); j++) {
		normal = add(normal, v.faces[j].normal);
	} 
	normal = scale(normal, (1.0/ v.faces.size()));
	return normal;
}

//lighting equation
color lightingEq(vertex normal) {
	color result = color(0.0, 0.0, 0.0);
	vertex A = vertex(.5, .5, .5);
	vertex C = vertex(1.0, 1.0, 1.0);
	vertex E = vertex(0.0, 0.0, -1.0);
	vertex k_a = vertex(.1, 0.0, 0.0);
	vertex k_s = vertex(.5, .5, .5);
	vertex k_d = vertex(.7, 0.0, 0.0);
	vertex L = vertex(1.0, 1.0, -1.0);
	vertex N = normalize( normal);
	vertex R = vertex(2.0 * dot(L, N) * N.x - L.x,
		2.0 * dot(L, N) * N.y - L.y,
		2.0 * dot(L, N) * N.z - L.z);
	R = normalize(R);
	float LdotN = dot(L, N);
	if (LdotN < 0) {
		result.r = k_a.x * A.x;
		result.g = k_a.y * A.y;
		result.b = k_a.z * A.z;
	}
	else {
		result.r = k_a.x * A.x + C.x * (k_d.x * LdotN + k_s.x * pow(dot(R, E), 5));
		result.b = k_a.y * A.y + C.y * (k_d.y * LdotN + k_s.y * pow(dot(R, E), 5));
		result.g = k_a.z * A.x + C.z * (k_d.z * LdotN + k_s.z * pow(dot(R, E), 5));
	}
	
	return result;
}

//for testing
void printvert(vertex v) {
	cout << v.x << " , " << v.y << " , " << v.z << "\n";
}