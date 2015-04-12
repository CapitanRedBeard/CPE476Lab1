//
// sueda
// October, 2014
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif
#include <iostream>
#include "GLSL.h"
#include "Camera.h"
#include "Shape.h"
#include "MatrixStack.h"

using namespace std;

//Window size
int g_width = 1024;
int g_height = 768;

Camera camera;
Shape shape;
GLuint pid;
GLint h_vertPos;
GLint h_vertNor;
GLint h_P;
GLint h_MV;
GLint h_lightPos1;
GLint h_lightPos2;
GLint h_ka;
GLint h_kd;
GLint h_ks;
GLint h_s;
GLint h_option;

typedef struct{
	float x;
	float y;
	float z;
}threeFloat;

typedef struct{
	threeFloat ka;
	threeFloat kd;
	threeFloat ks;
	float s;
}Material;

Material mat1;
Material mat2;
Material mat3;

int matCount = 2;
float optionS = 0.0f;

void loadScene(char * filename)
{
	shape.load(filename);
}

void initGL()
{
	//////////////////////////////////////////////////////
	// Initialize GL for the whole scene
	//
	//Construct Mats
	mat1.ka.x = 0.0f;
	mat1.ka.y = 0.05f;
	mat1.ka.z = 0.75f;
	mat1.kd.x = 0.2f;
	mat1.kd.y = 0.2f;
	mat1.kd.z = 0.2f;
	mat1.ks.x = 0.3f;
	mat1.ks.y = 1.0f;
	mat1.ks.z = 0.3f;
	mat1.s = 200.0f;

	mat2.ka.x = 0.15f;
	mat2.ka.y = 0.15f;
	mat2.ka.z = 0.3f;
	mat2.kd.x = 0.5f;
	mat2.kd.y = 0.5f;
	mat2.kd.z = 0.5f;
	mat2.ks.x = 0.2f;
	mat2.ks.y = 0.2f;
	mat2.ks.z = 0.2f;
	mat2.s = 50.0f;

	mat3.ka.x = 0.2f;
	mat3.ka.y = 0.2f;
	mat3.ka.z = 0.2f;
	mat3.kd.x = 0.8f;
	mat3.kd.y = 0.7f;
	mat3.kd.z = 0.7f;
	mat3.ks.x = 1.0f;
	mat3.ks.y = 0.9f;
	mat3.ks.z = 0.8f;
	mat3.s = 200.0f;
	
	// Set background color
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test
	glEnable(GL_DEPTH_TEST);
	
	
	//////////////////////////////////////////////////////
	// Initialize the geometry
	//

	shape.init();
	
	
	//////////////////////////////////////////////////////
	// Initialize shaders
	//
	
	// Create shader handles
	string vShaderName = "lab7_vert.glsl";
	string fShaderName = "lab7_frag.glsl";
	GLint rc;
	GLuint VS = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Read shader sources
	const char *vshader = GLSL::textFileRead(vShaderName.c_str());
	const char *fshader = GLSL::textFileRead(fShaderName.c_str());
	glShaderSource(VS, 1, &vshader, NULL);
	glShaderSource(FS, 1, &fshader, NULL);
	
	// Compile vertex shader
	glCompileShader(VS);
	GLSL::printError();
	glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(VS);
	if(!rc) {
		printf("Error compiling vertex shader %s\n", vShaderName.c_str());
	}
	
	// Compile fragment shader
	glCompileShader(FS);
	GLSL::printError();
	glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(FS);
	if(!rc) {
		printf("Error compiling fragment shader %s\n", fShaderName.c_str());
	}
	
	// Create the program and link
	pid = glCreateProgram();
	glAttachShader(pid, VS);
	glAttachShader(pid, FS);
	glLinkProgram(pid);
	GLSL::printError();
	glGetProgramiv(pid, GL_LINK_STATUS, &rc);
	GLSL::printProgramInfoLog(pid);
	if(!rc) {
		printf("Error linking shaders %s and %s\n", vShaderName.c_str(), fShaderName.c_str());
	}
	
	h_vertPos = GLSL::getAttribLocation(pid, "vertPos");
	h_vertNor = GLSL::getAttribLocation(pid, "vertNor");
	h_P = GLSL::getUniformLocation(pid, "P");
	h_MV = GLSL::getUniformLocation(pid, "MV");
	h_lightPos1 = GLSL::getUniformLocation(pid, "lightPos1");
	h_lightPos2 = GLSL::getUniformLocation(pid, "lightPos2");
	h_ka = GLSL::getUniformLocation(pid, "ka");
	h_kd = GLSL::getUniformLocation(pid, "kd");
	h_ks = GLSL::getUniformLocation(pid, "ks");
	h_s = GLSL::getUniformLocation(pid, "s");
	h_option = GLSL::getUniformLocation(pid, "option");

	// Check GLSL
	GLSL::checkVersion();
}

void reshapeGL(int w, int h)
{
	// Set view size
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	// Set camera aspect ratio
	camera.setAspect((float)w/h);
}

void materialSet(Material mat){
	glUniform3fv(h_ka,  1, glm::value_ptr(glm::vec3(mat.ka.x, mat.ka.y, mat.ka.z)));
	glUniform3fv(h_kd,  1, glm::value_ptr(glm::vec3(mat.kd.x, mat.kd.y, mat.kd.z)));
	glUniform3fv(h_ks, 1, glm::value_ptr(glm::vec3(mat.ks.x, mat.ks.y, mat.ks.z)));
	glUniform1f(h_s, mat.s);
}


void drawGL()
{
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Create matrix stacks
	MatrixStack P, MV;
	// Apply camera transforms
	P.pushMatrix();
	camera.applyProjectionMatrix(&P);
	MV.pushMatrix();
	camera.applyCameraMatrix(&MV);
	
	// Bind the program
	
	glUseProgram(pid);
	glUniformMatrix4fv( h_P, 1, GL_FALSE, glm::value_ptr( P.topMatrix()));
	glUniformMatrix4fv(h_MV, 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));
	glUniform3fv(h_lightPos1, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	glUniform3fv(h_lightPos2, 1, glm::value_ptr(glm::vec3(-1.0f, 1.0f, 1.0f)));
	glUniform1f(h_option, optionS);

	if(matCount == 1){
		materialSet(mat1);
	}else if (matCount == 2){
		materialSet(mat2);
	}else if (matCount == 3){
		materialSet(mat3);
	}

	
	// Draw shape
	shape.draw(h_vertPos, h_vertNor);
	
	// Unbind the program
	glUseProgram(0);

	// Pop stacks
	MV.popMatrix();
	P.popMatrix();

	// Double buffer
	glutSwapBuffers();
}

void mouseGL(int button, int state, int x, int y)
{
	int modifier = glutGetModifiers();
	bool shift = modifier & GLUT_ACTIVE_SHIFT;
	bool ctrl  = modifier & GLUT_ACTIVE_CTRL;
	bool alt   = modifier & GLUT_ACTIVE_ALT;
	camera.mouseClicked(x, y, shift, ctrl, alt);
}

void motionGL(int x, int y)
{
	camera.mouseMoved(x, y);
	// Refresh screen
	glutPostRedisplay();
}

void keyboardGL(unsigned char key, int x, int y)
{
	//cout << key << endl;
	switch(key) {
		case 27:
			// ESCAPE
			exit(0);
			break;
		case 77:
			// M
			if(matCount == 1)
				matCount = 3;
			else
				matCount--;
			break;
		case 109:
			// m
			if(matCount == 3)
				matCount = 1;
			else
				matCount++;
			break;
		case 83:
			// S
			if(optionS > 0.5f)
				optionS = 0.0f;
			else
				optionS = 1.0f;
			break;
		case 115:
			// s
			fprintf(stderr, "%lf\n", optionS);
			if(optionS > 0.5f){
				optionS = 0.0f;
			}
			else{
				optionS = 1.0f;
			}
			break;
	}
	// Refresh screen
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(g_width, g_height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Emmett Harper A3");
	glutMouseFunc(mouseGL);
	glutMotionFunc(motionGL);
	glutKeyboardFunc(keyboardGL);
	glutReshapeFunc(reshapeGL);
	glutDisplayFunc(drawGL);
	if(argc == 2){
		loadScene(*(argv + 1));
	}else{
		fprintf(stderr, "%s\n", "Wrong # arguements");
		return 0;
	}
	initGL();
	glutMainLoop();
	return 0;
}
