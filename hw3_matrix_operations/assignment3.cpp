
#include "assignment3.h"
#include "init.h"
#include <math.h>
#include <iostream>
#include <GL/glut.h>

using namespace std;

vector<Matrix> iat;
vector<Pt> condset;
Matrix createIdentity() {
	Matrix i = Matrix();
	i.data[0][0] = 1;
	i.data[1][1] = 1;
	i.data[2][2] = 1;
	return i;
}
Matrix underscoreTranspose() {
	Matrix _transpose = Matrix();
	_transpose.data[0][1] = -1;
	_transpose.data[1][0] = 1;
	return _transpose;
}
Matrix translate(Vec v)
{
	Matrix rvalue;
	rvalue = createIdentity();
	rvalue.data[0][2] = v.x;
	rvalue.data[1][2] = v.y;
	return rvalue;
}

Matrix rotate(Pt o, float theta)
{
	Matrix rvalue = Matrix();
	rvalue.data[0][0] = cos(theta);
	rvalue.data[0][1] = (-sin(theta));
	rvalue.data[0][2] = o.x + (o.y * sin(theta)) - (o.x * cos(theta));
	rvalue.data[1][0] = sin(theta);
	rvalue.data[1][1] = cos(theta);
	rvalue.data[1][2] = o.y - (o.y * cos(theta)) - (o.x * sin(theta));
	rvalue.data[2][2] = 1;
	return rvalue;
}

Matrix scale(Pt o, float alpha)
{
	Matrix rvalue = Matrix();
	rvalue.data[0][0] = alpha;
	rvalue.data[0][2] = (1 - alpha)*o.x;
	rvalue.data[1][1] = alpha;
	rvalue.data[1][2] = (1 - alpha) * o.y;
	rvalue.data[2][2] = 1;
	return rvalue;
}

Matrix nscale(Pt o, Vec v, float alpha)
{
	Matrix rvalue = Matrix();
	rvalue.data[0][0] = (1 + ((alpha - 1) * (v.x * v.x)));
	rvalue.data[0][1] = (alpha - 1) * v.x * v.y;
	rvalue.data[0][2] = v.x * ((o.x * v.x) + (o.y * v.y))*(1 - alpha);
	rvalue.data[1][0] = (alpha - 1) * v.x * v.y;
	rvalue.data[1][1] = 1 + (alpha - 1) * (v.y * v.y);
	rvalue.data[1][2] = v.y * ((o.x * v.x) + (o.y * v.y))* (1 - alpha);
	rvalue.data[2][2] = 1;
	return rvalue;
}
//computes the inverse of matrix m
Matrix invert(Matrix m) {
	//https://stackoverflow.com/questions/983999/simple-3x3-matrix-inverse-code-c
	Matrix inverse;
	float det = m.data[0][0] * ((m.data[1][1] * m.data[2][2]) - (m.data[2][1] * m.data[1][2]))
		- m.data[0][1] * ((m.data[1][0] * m.data[2][2]) - (m.data[1][2] * m.data[2][0]))
		+ m.data[0][2] * ((m.data[1][0] * m.data[2][1]) - (m.data[1][1] * m.data[2][0]));
	float invdet = 1 / det;
	inverse.data[0][0] = (m.data[1][1] * m.data[2][2] - m.data[2][1] * m.data[1][2]) * invdet;
	inverse.data[0][1] = (m.data[0][2] * m.data[2][1] - m.data[0][1] * m.data[2][2]) * invdet;
	inverse.data[0][2] = (m.data[0][1] * m.data[1][2] - m.data[0][2] * m.data[1][1]) * invdet;
	inverse.data[1][0] = (m.data[1][2] * m.data[2][0] - m.data[1][0] * m.data[2][2]) * invdet;
	inverse.data[1][1] = (m.data[0][0] * m.data[2][2] - m.data[0][2] * m.data[2][0]) * invdet;
	inverse.data[1][2] = (m.data[1][0] * m.data[0][2] - m.data[0][0] * m.data[1][2]) * invdet;
	inverse.data[2][0] = (m.data[1][0] * m.data[2][1] - m.data[2][0] * m.data[1][1]) * invdet;
	inverse.data[2][1] = (m.data[2][0] * m.data[0][1] - m.data[0][0] * m.data[2][1]) * invdet;
	inverse.data[2][2] = (m.data[0][0] * m.data[1][1] - m.data[1][0] * m.data[0][1]) * invdet;
	return inverse;
}

Matrix image(Pt p1, Pt p2, Pt p3, Pt q1, Pt q2, Pt q3)
{
	Matrix rvalue;
	Matrix mOne = Matrix();
	Matrix mTwo = Matrix();
	vector<Pt> p = { p1, p2, p3 };
	vector<Pt> q = { q1, q2, q3 };
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (i == 0) { //top row x
				mOne.data[i][j] = q[j].x;
				mTwo.data[i][j] = p[j].x;
			}
			else if (i == 1) { // middle row y
				mOne.data[i][j] = q[j].y;
				mTwo.data[i][j] = p[j].y;
			}
			else if (i == 2) { //bottom row ones
				mOne.data[i][j] = 1;
				mTwo.data[i][j] = 1;
			}
		}

	}
	//|p2 q2 r2|  * |p1x q1x r1x |^ -1
	//multiply mOne by the inverse of mTwo
	rvalue = compose(mOne, invert(mTwo));
	return rvalue;
}

Matrix compose(Matrix m1, Matrix m2)
{
	Matrix product = Matrix();
	//http://cplussplussatplay.blogspot.com/2012/01/3x3-matrix-multiplication.html
	float temp = 0;
	int a, b, c;

	for (a = 0; a < 3; a++)
	{
		for (b = 0; b < 3; b++)
		{
			for (c = 0; c < 3; c++)
			{
				temp += m1.data[b][c] * m2.data[c][a];
			}
			product.data[b][a] = temp;
			temp = 0;
		}
	}
	return product;
}

//these should only be one line
void setCondensationSet(vector<Pt> pts)
{
	condset = pts;
}

void setIATTransformations(vector<Matrix> transformations)
{
	iat = transformations;
}

void drawfractal() {
	cout << "drawing fractal...\n";
	//draw the fractal
	vector<vector<Matrix>> fracshapes; //idk if I need this 
	vector<Matrix> pointstodraw;
	if (!condset.empty()) {
		//Start with condensation set
		//if there is a condensation set they need to be 3x3 matrices to work with my multiplication
		//since all matrices are represented as 3x3 matrices
		for (int c = 0; c < condset.size(); c++) {
			Matrix condpoint = Matrix();
			condpoint.data[0][0] = condset[c].x;
			condpoint.data[1][0] = condset[c].y;
			condpoint.data[2][0] = 1;
			pointstodraw.push_back(condpoint);
		}
	}
	else {
		//start with any shape, I chose a triangle
		//represent points as 3x3 matrices
		for (int t = 0; t < 3; t++) {
			Matrix triangle = Matrix();
			//http://c-faq.com/lib/rand48.html
			triangle.data[0][0] = -1 + 2 * (float)rand() / (RAND_MAX); //random point in our window
			cout << triangle.data[0][0] << "start x \n";
			triangle.data[1][0] = -1 + 2 * (float)rand() / (RAND_MAX); //random point in our window
			cout << triangle.data[1][0] << "start y \n";
			triangle.data[2][0] = 1; //1 for consistency
			pointstodraw.push_back(triangle);
		}
	}

	
	fracshapes.push_back(pointstodraw);


	//apply the transformation
	int sizeOfFractal; //number of times to loop
	if (iat.size() < 4) sizeOfFractal = 7;
	else sizeOfFractal = 5;
	for (int i = 0; i < sizeOfFractal; i++) { 
		vector < vector<Matrix>> newfracshapes; //store the new shapes after we transform them (x_i+1)
		for (int j = 0; j < fracshapes.size(); j++) { //loop through our current set (x_i)
			vector<Matrix> shape = fracshapes[j]; //get the current shape
			for (int k = 0; k < iat.size(); k++) { //F_j (x_i)
				vector<Matrix> newShape; //store the new part of the fractal
			   //loop through the points of this part of the fractal
				for (int s = 0; s < shape.size(); s++) {
					newShape.push_back(compose(iat[k], shape[s])); //apply the transformation
				}
				//add to our new set x_i+1
				newfracshapes.push_back(newShape);
			}
		}
		fracshapes = newfracshapes;
		//add the condensation set
		if(!condset.empty())
			fracshapes.push_back(pointstodraw);

	}

	//draw the fractal
	glColor3f(1.0f, 1.0f, 1.0f);
	int numshapes = fracshapes.size();
	for (int i = 0; i < numshapes; i++) {
		vector<Matrix> shapetodraw = fracshapes.front();
		if (condset.empty()) {
			glBegin(GL_POINTS);
		}
		else {
			glBegin(GL_LINE_LOOP);
		}
		
		for (int k = 0; k < shapetodraw.size(); k++) {
			glVertex2f(shapetodraw[k].data[0][0], shapetodraw[k].data[1][0]);
		}
		fracshapes.erase(fracshapes.begin());
		glEnd();
		glFlush();
	}


		//draw the condensation set
		//x_0 = C
		if (!condset.empty()) { //there is a condensation set
			if (condset.size() == 1) {
				//condensation set is a point
				glColor3f(0.0f, 0.0f, 1.0f);
				glBegin(GL_POINTS);
				glVertex2f(condset[0].x, condset[0].y);
				glEnd();
				glFlush();
			}
			else if (condset.size() == 2) {
				//condensation set is a line
				glColor3f(0.0f, 0.0f, 1.0f);
				glBegin(GL_LINES);
				glVertex2f(condset[0].x, condset[0].y);
				glVertex2f(condset[1].x, condset[1].y);
				glEnd();
				glFlush();
			}
			else {
				//condensation set is a poly-line
				glColor3f(1.0f, 1.0f, 1.0f);
				glBegin(GL_LINE_LOOP);
				for (int i = 0; i < condset.size(); i++) {
					glVertex2f(condset[i].x, condset[i].y);
				}
				glEnd();
				glFlush();
			}
		}
		iat.clear();
		condset.clear();
}
// Draws the current IAT
void display(void)
{
	
	if (!iat.empty()) {
		glClear(GL_COLOR_BUFFER_BIT);
		drawfractal();
		glFlush();
		cout << "fractal drawn\n";
	}
	
}


/* do not modify the reshape function */
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void keyboard(unsigned char key, int x, int y)
{
	vector<Matrix> iat2;
	vector<Pt> condset2;
	switch (key) {
	case '1':
		//fractal hangman
		iat2.push_back(image(Pt(0.9, -0.9), Pt(-0.9, -0.9), Pt(-0.9, 0.9), Pt(0.9, -0.9), Pt(0, -0.9), Pt(0, 0)));
		iat2.push_back(image(Pt(0.9, -0.9), Pt(-0.9, -0.9), Pt(-0.9, 0.9), Pt(0, -0.9), Pt(-0.9,-0.9), Pt(-0.9, 0)));
		iat2.push_back(image(Pt(0.9, -0.9), Pt(-0.9, -0.9), Pt(-0.9, 0.9), Pt(-0.9, 0), Pt(-0.9, 0.9), Pt(0, 0.9)));


		break;
	case '2':
		//fractal staircase
		iat2.push_back(image(Pt(0.9, -0.9), Pt(0, -0.9), Pt(0, 0), Pt(0, -0.9), Pt(-0.45, -0.9), Pt(-0.45, -0.45)));
		iat2.push_back(image(Pt(0.9, -0.9), Pt(0, -0.9), Pt(0, 0), Pt(0.9, 0), Pt(0.45, 0), Pt(0.45, 0.45)));
		condset2.push_back(Pt(0.9, -0.9));
		condset2.push_back(Pt(0, -0.9));
		condset2.push_back(Pt(0, 0));
		condset2.push_back(Pt(0.9, 0));

		break;
	case '3':
		//fractal Snowflake
		//pentagon
		//scale by 1/ 1 + golden ratio
		iat2.push_back(scale(Pt(0, 0.9), 0.38));//top point
		iat2.push_back(scale(Pt(-0.9, 0.31), 0.38)); //upper left
		iat2.push_back(scale(Pt(0.9, 0.31), 0.38)); //upper right
		iat2.push_back(scale(Pt(-0.6, -0.77), 0.38)); //bottom left
		iat2.push_back(scale(Pt(0.6, -0.77), 0.38)); //bottom right
		iat2.push_back(compose(scale(Pt(0, -0.022), 0.38), rotate(Pt(0,0), 3.1415))); //middle

		break;
	case '4':
		//hex flower, a little long but whatever
		//scale by 1/3 because each hex is 1/3 the length of the original
		iat2.push_back(scale(Pt(-0.45, 0.9), 0.33)); // top left vertex
		iat2.push_back(scale(Pt(0.45, 0.9), 0.33)); //top right vertex
		iat2.push_back(scale(Pt(-0.45, -0.9), 0.33)); // bottom left vertex
		iat2.push_back(scale(Pt(0.45, -0.9), 0.33)); //bottom right lower
		iat2.push_back(scale(Pt(0.9, 0), 0.33)); // right vertex 
		iat2.push_back(scale(Pt(-0.9, 0), 0.33)); //left vertex

		break;
	case '5':
		//custom 1 square things
		iat2.push_back(scale(Pt(-0.9, 0.9), 0.3));
		iat2.push_back(scale(Pt(-0.9, -0.9), 0.3));
		iat2.push_back(scale(Pt(0.9, 0.9), 0.3));
		iat2.push_back(scale(Pt(0.9, -0.9), 0.3));
		iat2.push_back(compose(scale(Pt(0, 0), 0.3), rotate(Pt(0, 0), 0.78)));

		break;
	case '6':
		//custom 2 smiley face
		iat2.push_back(image(Pt(-0.8, 0), Pt(0, -0.8), Pt(0, 0.8), Pt(-0.6, 0.4), Pt(-0.4, 0.2), Pt(-0.2, 0.4)));//left eye
		iat2.push_back(image(Pt(-0.8, 0), Pt(0, -0.8), Pt(0, 0.8), Pt(0.2, 0.4), Pt(0.4, 0.2), Pt(0.6, 0.4))); //right eye
		iat2.push_back(image(Pt(-0.8, 0), Pt(0, -0.8), Pt(0, 0.8), Pt(-0.8,-0.2 ), Pt(-0.6, -0.4), Pt(-0.4, -0.2))); //mouth left
		iat2.push_back(image(Pt(-0.8, 0), Pt(0, -0.8), Pt(0, 0.8), Pt(-0.8, -0.6), Pt(-0.6, -0.8), Pt(-0.4, -0.6))); //bottom 1
		iat2.push_back(image(Pt(-0.8, 0), Pt(0, -0.8), Pt(0, 0.8), Pt(-0.4, -0.6), Pt(-0.2, -0.8), Pt(0,-0.6))); //bottom 2
		iat2.push_back(image(Pt(-0.8, 0), Pt(0, -0.8), Pt(0, 0.8), Pt(0,-0.6), Pt(0.2, -0.8), Pt(0.4, -0.6))); //bottom 3
		iat2.push_back(image(Pt(-0.8, 0), Pt(0, -0.8), Pt(0, 0.8), Pt(0.4, -0.6), Pt(0.6,-0.8), Pt(0.8, -0.6))); //bottom 4
		iat2.push_back(image(Pt(-0.8, 0), Pt(0, -0.8), Pt(0, 0.8), Pt(0.4, -0.2), Pt(0.6, -0.4), Pt(0.8, -0.2))); // mouth right
		break;
	}
	setIATTransformations(iat2);
	setCondensationSet(condset2);
	display();
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Julie Thompson - Homework 3");
	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
