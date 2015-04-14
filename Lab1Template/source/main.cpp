/**
 * Lab 1
 * @author Brandon Clark
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include "GLSL.h"
#include "Camera.h"
#include "Shape.h"
#include "Terrain.h"
#include "MatrixStack.h"
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "RenderingHelper.h"
#define NUMOBJ 10

using namespace std;

void idleGL();

//The Window & window size
GLFWwindow* window;
int g_width;
int g_height;

Terrain terrain;

Camera camera;
bool cull = false;
bool line = false;
glm::vec2 mouse;
int shapeCount = 1;
std::vector<Shape> shapes(NUMOBJ);
//pid used for glUseProgram(pid);
GLuint pid;
GLint h_vertPos;
GLint h_vertNor;
GLint h_ProjMatrix;
GLint h_ViewMatrix;
GLint h_ModelMatrix;

GLint h_lightPos1;
GLint h_lightPos2;
GLint h_ka;
GLint h_kd;
GLint h_ks;
GLint h_s;
GLint h_option;

bool keyToggles[256] = {false};
float t = 0.0f;
float h = 0.1f;
glm::vec3 location(0.0f,0.0f,0.0f);
glm::vec3 g(-10.0f, -5.0f, 0.0f);

// Display time to control fps
float timeOldDraw = 0.0f;
float timeNew = 0.0f;
float timeOldSpawn = 0.0f;

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

int matCount = 2;
float optionS = 0.0f;

//Rendering Helper
RenderingHelper ModelTrans;

/**
 * Helper function to send materials to the shader - create below.
 */
void SetMaterial(int i)
{
	glUseProgram(pid);
	switch(i) {
		case 0: //Material 1
			glUniform3f(h_ka, 0.0, 0.05, 0.75);
			glUniform3f(h_kd, 0.2, 0.2, 0.2);
			glUniform3f(h_ks, 0.3, 1.0, 0.3);
			glUniform1f(h_s, 200.0);
			break;
		case 1: //Material 2
			glUniform3f(h_ka, 0.15, 0.15, 0.3);
			glUniform3f(h_kd, 0.3, 0.5, 0.5);
			glUniform3f(h_ks, 0.2, 0.2, 0.2);
			glUniform1f(h_s, 50.0);
			break;
		case 2: //Material 3
			glUniform3f(h_ka, 0.2, 0.2, 0.2);
			glUniform3f(h_kd, 0.8, 0.7, 0.7);
			glUniform3f(h_ks, 1.0, 0.9, 0.8);
			glUniform1f(h_s, 200.0);
			break;
	}
}

/**
 * For now, this just initializes the Shape object.
 * Later, we'll updated to initialize all objects moving.
 * (This is very specific right now to one object).....
 */
void initShape(char * filename)
{
	t = 0.0f;
	h = 0.001f;
	// g = glm::vec3(0.0f, -0.01f, 0.0f);

	//Load Shape object
	for (std::vector<Shape>::iterator it = shapes.begin(); it != shapes.end(); ++it){
		it->load(filename);
	}
}

/**
 * Generalized approach to intialization.
 */
void initModels()
{
	//Maybe later have a inheritance of Shape and go
	// through each and init each one.

	//Initialize Shape object
	for (std::vector<Shape>::iterator it = shapes.begin(); it != shapes.end(); ++it){
		it->init();
	}

	//Initialize Terrain object
	terrain.init();

	//initialize the modeltrans matrix stack
   ModelTrans.useModelViewMatrix();
   ModelTrans.loadIdentity();
}

void initGL()
{	
	// Set background color
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test
	glEnable(GL_DEPTH_TEST);
}

/**
 * Initialize the shaders passed to the function
 */
bool installShaders(const string &vShaderName, const string &fShaderName)
{		
	GLint rc;

	// Create shader handles
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
	h_ProjMatrix = GLSL::getUniformLocation(pid, "uProjMatrix");
	h_ViewMatrix = GLSL::getUniformLocation(pid, "uViewMatrix");
	h_ModelMatrix = GLSL::getUniformLocation(pid, "uModelMatrix");

	h_lightPos1 = GLSL::getUniformLocation(pid, "lightPos1");
	h_lightPos2 = GLSL::getUniformLocation(pid, "lightPos2");
	h_ka = GLSL::getUniformLocation(pid, "ka");
	h_kd = GLSL::getUniformLocation(pid, "kd");
	h_ks = GLSL::getUniformLocation(pid, "ks");
	h_s = GLSL::getUniformLocation(pid, "s");
	h_option = GLSL::getUniformLocation(pid, "option");

	// Check GLSL
	GLSL::checkVersion();
	assert(glGetError() == GL_NO_ERROR);
	return true;
}

void drawGL()
{
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(pid);

	//Update Camera
	// Get mouse position
  	double xpos, ypos;

  	glfwGetCursorPos(window, &xpos, &ypos);
	camera.update(xpos, ypos);

	// Enable backface culling
	if(cull) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
	if(line) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glUniform3fv(h_lightPos1, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
	glUniform3fv(h_lightPos2, 1, glm::value_ptr(glm::vec3(-1.0f, 1.0f, 1.0f)));
	glUniform1f(h_option, optionS);


	// Create matrix stacks
	/*MatrixStack P, MV;
	// Apply camera transforms
	P.pushMatrix();
	camera.applyProjectionMatrix(&P);
	MV.pushMatrix();*/
	//camera.applyViewMatrix(&MV);
	
	// Bind the program
	
	//Set projection matrix
	MatrixStack proj, view;
	proj.pushMatrix();
	camera.applyProjectionMatrix(&proj);
	glUniformMatrix4fv( h_ProjMatrix, 1, GL_FALSE, glm::value_ptr( proj.topMatrix()));
	proj.pushMatrix();
	camera.applyViewMatrix(&view);
	glUniformMatrix4fv(h_ViewMatrix, 1, GL_FALSE, glm::value_ptr(view.topMatrix()));

	//glUniformMatrix4fv(h_MV, 1, GL_FALSE, glm::value_ptr(MV.topMatrix()));
	

	/*if(matCount == 1){
		materialSet(material1);
	}else if (matCount == 2){
		materialSet(material2);
	}else if (matCount == 3){
		materialSet(material3);
	}*/
	//Instead of the above, we use the function SetMaterial.
	SetMaterial(1);

	// Draw shapes
	int i = 0;
	for (std::vector<Shape>::iterator it = shapes.begin(); i < shapeCount; ++it, i++){
    // std::cout << ' ' << ;
	// for(int i = 0; i < shapeCount; i++){
		ModelTrans.loadIdentity();
		ModelTrans.pushMatrix();
		ModelTrans.translate(it->getPosition());
		glUniformMatrix4fv(h_ModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelTrans.modelViewMatrix));
		ModelTrans.popMatrix();
		it->draw(h_vertPos, h_vertNor);	
	}

	ModelTrans.loadIdentity();
	ModelTrans.pushMatrix();
	glUniformMatrix4fv(h_ModelMatrix, 1, GL_FALSE, glm::value_ptr(ModelTrans.modelViewMatrix));
	//Still need to set position for terrain.
	ModelTrans.popMatrix();
	terrain.draw(h_vertPos, h_vertNor);
	
	// Unbind the program
	glUseProgram(0);

	// Pop stacks
	proj.popMatrix();
}

/**
 * This will get called when any button on keyboard is pressed.
 */
void checkUserInput()
{
   vec3 view = camera.getLookAtPoint() - camera.getTheEye();
   vec3 strafe = glm::cross(view, vec3(0.0, 1.0, 0.0));
   if (glfwGetKey(window, GLFW_KEY_C ) == GLFW_PRESS)
   {
      cull = !cull;
   }
   if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
   {
      line = !line;
   }
   if (glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS)
   {
      //theStrafe -= strafe * strafeSpeed;
      camera.updateStrafe(-strafe);
   }
   if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
   {
      //theStrafe += strafe * strafeSpeed;
      camera.updateStrafe(strafe);
   }
   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
   {
      //theZoom += view * sprintSpeed;
      camera.updateZoom(view);
   }
   else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
   {
      //theZoom += view * walkSpeed;
      camera.updateZoom(view);
   }
   if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
   {
      //theZoom -= view * sprintSpeed;
      camera.updateZoom(-view);
   }
   else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
   {
      //theZoom -= view * walkSpeed;
      camera.updateZoom(-view);
   }
}

void motionGL(int x, int y)
{
	float dx = (g_width / 2) - x;
	float dy = (g_height / 2) - y;
	printf("DX: %f, DY: %f\n", dx, dy);
	//if((dx > 0.001 || dx < 0.001) && (dy > 0.001 || dy < 0.001)){
		printf("DX: %f, DY: %f\n", dx, dy);
		camera.mouseMoved(dx, dy);		
	//}
}

/*void idleGL(){
	timeNew = glutGet(GLUT_ELAPSED_TIME);
	int dt = timeNew - timeOldDraw;
	t += h;
	// Update every 60Hz
	if(dt > 1000.0/60.0) {
		timeOldDraw = timeNew;
		camera.update(keyToggles, mouse);
		//glutPostRedisplay();
		shape.update(t, h, g, keyToggles);
	}
}*/

void window_size_callback(GLFWwindow* window, int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	g_width = w;
	g_height = h;
	camera.setWindowSize(w, h);
}

int main(int argc, char **argv)
{
	// Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Open a window and create its OpenGL context
   g_width = 1024;
   g_height = 768;
   window = glfwCreateWindow( g_width, g_height, "CPE476 Lab1 [ Clark | Harper ]", NULL, NULL);
   if( window == NULL ){
      fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_size_callback);
    //glfwSetKeyCallback(window, key_callback);

    // Initialize glad
   if (!gladLoadGL()) {
      fprintf(stderr, "Unable to initialize glad");
      glfwDestroyWindow(window);
      glfwTerminate();
      return 1;
   }

  	// Ensure we can capture the escape key being pressed below
  	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  	glfwSetCursorPos(window, 0.0, 0.0);

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	initGL();
	installShaders("lab7_vert.glsl", "lab7_frag.glsl");
	std::string str = "assets/bunny.obj";
	initShape(&str[0u]); //initShape(argv[0]);
  	initModels();
   do{
   	//maybe we should have the time step in here

   	timeNew = glfwGetTime();
	double dtSpawn = timeNew - timeOldSpawn;

	printf("shapeCount: %f", dtSpawn);
	// Update every 1s
	if(shapeCount != NUMOBJ && dtSpawn >= timeOldSpawn) {
		shapeCount++;
		timeOldDraw += 1.0;
	}

   	//Check for user input
   	checkUserInput();
	double dtDraw = timeNew - timeOldDraw;
	t += h;
	// Update every 60Hz
	if(dtDraw >= (1.0 / 60) ) {
		timeOldDraw += (1.0 / 60);
		//Draw an image
		drawGL();
	}
	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();

   } // Check if the ESC key was pressed or the window was closed
   while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0 );

   // Close OpenGL window and terminate GLFW
   glfwTerminate();

	return 0;
}
