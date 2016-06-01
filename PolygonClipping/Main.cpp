#include <iostream>
#include <fstream>
#include <GL\glut.h>
#include <GL\GLU.h>
#include <ctime>
#include "Polygon.h"

using namespace std;

int K, N, M;

std::vector <std::vector <std::shared_ptr <Vertex>>> result, source, clip;

int windowWidth, windowHeight;

double axisXMin, axisXMax, axisYMin, axisYMax, gridSize = 10;

/* Initialize OpenGL Graphics */
void initGL() {
	// Set "clearing" or background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black and opaque
	glClear(GL_COLOR_BUFFER_BIT);
}


void processInput() {
	ifstream fin("input.txt");
	fin >> K >> N >> M;
	for (int i = 0; i < N + M; ++i) {
		char ch;
		fin >> ch;
		int nVertices;
		fin >> nVertices;
		vector <shared_ptr<Vertex>> vertices;
		for (int i = 0; i < nVertices; ++i) {
			double x, y;
			fin >> x >> y;
			vertices.push_back(shared_ptr<Vertex>(new Vertex(x, y)));
		}
		if (ch == 'A')
			source.push_back(vertices);
		else
			clip.push_back(vertices);
	}

	bool sourceForwards, clipForwards;

	if (K == 1) {
		sourceForwards = clipForwards = 1;
		for (int i = 0; i < source.size(); ++i) {

			for (int j = 0; j < clip.size(); ++j) {
				std::vector <std::vector <std::shared_ptr <Vertex>>> thisPassRes =
					PolygonClipping::Polygon(source[i]).clip(clip[j], sourceForwards, clipForwards);

				for (int index = 0; index < thisPassRes.size(); ++index)
					result.push_back(thisPassRes[index]);
			}

		}
	}
	else if (K == 2) {
		sourceForwards = clipForwards = 0;
		for (int i = 0; i < source.size(); ++i) {
			std::vector <std::vector <std::shared_ptr <Vertex>>> temp1;
			temp1.push_back(source[i]);

			for (int j = 0; j < clip.size(); ++j) {
				std::vector <std::vector <std::shared_ptr <Vertex>>> temp2;

				for (int k = 0; k < temp1.size(); ++k) {
					std::vector <std::vector <std::shared_ptr <Vertex>>> thisPassRes =
						PolygonClipping::Polygon(temp1[k]).clip(clip[j], sourceForwards, clipForwards);

					if (thisPassRes.size() > 0) {
						for (int index = 0; index < thisPassRes.size(); ++index)
							temp2.push_back(thisPassRes[index]);
					}
					else {
						temp2.push_back(temp1[k]);
						temp2.push_back(clip[j]);
					}
				}

				temp1 = temp2;
			}

			for (int index = 0; index < temp1.size(); ++index)
				result.push_back(temp1[index]);
		}
	}
	else {
		sourceForwards = 0;
		clipForwards = 1;

		for (int i = 0; i < source.size(); ++i) {
			std::vector <std::vector <std::shared_ptr <Vertex>>> temp1;
			temp1.push_back(source[i]);

			for (int j = 0; j < clip.size(); ++j) {
				std::vector <std::vector <std::shared_ptr <Vertex>>> temp2;

				for (int k = 0; k < temp1.size(); ++k) {
					std::vector <std::vector <std::shared_ptr <Vertex>>> thisPassRes =
						PolygonClipping::Polygon(temp1[k]).clip(clip[j], sourceForwards, clipForwards);

					if (thisPassRes.size() > 0) {
						for (int index = 0; index < thisPassRes.size(); ++index)
							temp2.push_back(thisPassRes[index]);
					}
					else {
						temp2.push_back(temp1[k]);
					}
				}

				temp1 = temp2;
			}

			for (int index = 0; index < temp1.size(); ++index)
				result.push_back(temp1[index]);
		}
	}


	for (int i = 0; i < result.size(); ++i) {
		cout << "____________________________________________\n";
		for (int j = 0; j < result[i].size(); ++j)
				cout << result[i][j]->x << " " << result[i][j]->y << endl;
	}
}

void plot_axis(int red = 1, int green = 1, int blue = 1) {
	glColor3f(red, green, blue);

	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_LINE_STRIP);
	glVertex2d(axisXMin, 0);
	glVertex2d(axisXMax, 0);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2d(0, axisYMin);
	glVertex2d(0, axisYMax);
	glEnd();
}

void display() {
	plot_axis();

	for (int i = 0; i < source.size(); ++i) {
		glColor3f(0.0f, 0.0f, 1.0f);

		GLUtriangulatorObj* tess = gluNewTess();

		/* Nice casting here -- NOT!! */
		gluTessCallback(tess, GLU_BEGIN, (void(__stdcall *) (void))glBegin);
		gluTessCallback(tess, GLU_VERTEX, (void(__stdcall *) (void))glVertex3dv);
		gluTessCallback(tess, GLU_END, (void(__stdcall *) (void))glEnd);

		gluBeginPolygon(tess);
		double(*v)[3] = new double[source[i].size()][3];
		for (int j = 0; j < source[i].size(); ++j) {
			v[j][0] = source[i][j]->x; v[j][1] = source[i][j]->y; v[j][2] = 0;
		}
		for (int j = 0; j < source[i].size(); ++j) {
			gluTessVertex(tess, v[j], v[j]);
		}
		gluEndPolygon(tess);
		gluDeleteTess(tess);
		delete v;
	}

	for (int i = 0; i < clip.size(); ++i) {
		glColor3f(1.0f, 0.0f, 0.0f);

		GLUtriangulatorObj* tess = gluNewTess();

		/* Nice casting here -- NOT!! */
		gluTessCallback(tess, GLU_BEGIN, (void(__stdcall *) (void))glBegin);
		gluTessCallback(tess, GLU_VERTEX, (void(__stdcall *) (void))glVertex3dv);
		gluTessCallback(tess, GLU_END, (void(__stdcall *) (void))glEnd);

		gluBeginPolygon(tess);
		double(*v)[3] = new double[clip[i].size()][3];
		for (int j = 0; j < clip[i].size(); ++j) {
			v[j][0] = clip[i][j]->x; v[j][1] = clip[i][j]->y; v[j][2] = 0;
		}
		for (int j = 0; j < clip[i].size(); ++j) {
			gluTessVertex(tess, v[j], v[j]);
		}
		gluEndPolygon(tess);
		gluDeleteTess(tess);
		delete v;
	}

	for (int i = 0; i < result.size(); ++i) {
		glColor3f(0.0f, 1.0f, 0.0f);

		GLUtriangulatorObj* tess = gluNewTess();

		/* Nice casting here -- NOT!! */
		gluTessCallback(tess, GLU_BEGIN, (void(__stdcall *) (void))glBegin);
		gluTessCallback(tess, GLU_VERTEX, (void(__stdcall *) (void))glVertex3dv);
		gluTessCallback(tess, GLU_END, (void(__stdcall *) (void))glEnd);

		gluBeginPolygon(tess);
		double (*v)[3] = new double[result[i].size()][3];
		for (int j = 0; j < result[i].size(); ++j) {
			v[j][0] = result[i][j]->x; v[j][1] = result[i][j]->y; v[j][2] = 0;
			cout << result[i][j]->x << " " << result[i][j]->y << endl;
		}
		for (int j = 0; j < result[i].size(); ++j) {
			gluTessVertex(tess, v[j], v[j]);
		}
		gluEndPolygon(tess);
		gluDeleteTess(tess);
		delete v;
	}

	glFlush();
}

void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
	windowWidth = width;
	windowHeight = height;
	// Compute aspect ratio of the new window
	if (height == 0) height = 1;                // To prevent divide by 0
	GLfloat aspect = (GLfloat)width / (GLfloat)height;

	// Set the viewport to cover the new window
	glViewport(0, 0, width, height);

	// Set the aspect ratio of the clipping area to match the viewport
	glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
	glLoadIdentity();
	if (width >= height) {
		// aspect >= 1, set the height from -1 to 1, with larger width
		axisXMin = -gridSize * aspect;
		axisXMax = gridSize * aspect;
		axisYMin = -gridSize;
		axisYMax = gridSize;
		gluOrtho2D(axisXMin, axisXMax, axisYMin, axisYMax);
	}
	else {
		// aspect < 1, set the width to -1 to 1, with larger height
		axisXMin = -gridSize;
		axisXMax = gridSize;
		axisYMin = -gridSize * aspect;
		axisYMax = gridSize * aspect;
		gluOrtho2D(axisXMin, axisXMax, axisYMin, axisYMax);
	}
}

int main(int argc, char** argv) {
	processInput();

	srand(time(NULL));
	glutInit(&argc, argv);          // Initialize GLUT
	glutInitWindowSize(windowWidth, windowHeight);   // Set the window's initial width & height - non-square
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutCreateWindow("Final Project. 1351052 - 1351055");  // Create window with the given title
	glutDisplayFunc(display);       // Register callback handler for window re-paint event
	glutReshapeFunc(reshape);       // Register callback handler for window re-size event
	initGL();
	glutMainLoop();                 // Enter the infinite event-processing loop
	return 0;
}