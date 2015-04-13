//
// sueda
// November, 2014
//

#include "Camera.h"
#include "MatrixStack.h"
#include <iostream>
#define GLM_FORCE_RADIANS
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/transform.hpp"

using namespace std;

//Global variables for now ......
//Camera
// horizontal angle : toward -Z
float horizontalAngle = -3.14f/2.0f;
// vertical angle : 0, look at the horizon
float verticalAngle = 0.0f;
//Mouse sensitivity
float mouseSpeed = 0.005f;
//Strafe speed
float strafeSpeed = 1.5f;
//Walk speed
float walkSpeed = 1.5f;
//Sprint speed
float sprintSpeed = 3.0f;

Camera::Camera() :
	aspect(1.0f),
	fovy(45.0f),
	znear(0.1f),
	zfar(1000.0f),
	rotations(0.01, 0.01),
	rfactor(0.0005f),
	pfactor(0.0005f),
	position(0.0f, 0.0f, 0.0f),
	sfactor(0.005f),
	state(0),
	//Brandon Stuff
	lookAtPoint(0.0f, 0.0f, 0.0f),
	theEye(0.0f, 0.0f, 0.0f),
	theStrafe(0.0f, 0.0f, 0.0f),
	theZoom(0.0f, 0.0f, 0.0f)
{
}

Camera::~Camera()
{
	
}

glm::vec3 Camera::getLookAtPoint()
{
	return lookAtPoint;
}

glm::vec3 Camera::getTheEye()
{
	return theEye;
}

void Camera::updateStrafe(glm::vec3 dStrafe)
{
	theStrafe += dStrafe;
}

void Camera::updateZoom(glm::vec3 dZoom)
{
	theZoom += dZoom;
}

void Camera::mouseMoved(int x, int y)
{
	glm::vec2 mouseCurr(x, y);
	rotations += rfactor * mouseCurr;
}

void Camera::setWindowSize(float w, float h)
{
	aspect = w/h;
}

//void Camera::update(const bool *keys, const glm::vec2 &mouse)
void Camera::update(double xpos, double ypos)
{
	static double xOld = 0;
  	static double yOld = 0;

  	// Compute new orientation
  	horizontalAngle += (mouseSpeed * (xpos - xOld));
  	verticalAngle -= (mouseSpeed * (ypos - yOld));
  	xOld = xpos;
  	yOld = ypos;

  	if (verticalAngle > (80.0 * (3.14f)/180.0))
    	verticalAngle = (80.0 * (3.14f)/180.0);
  	else if (verticalAngle < -(80.0 * (3.14f)/180.0))
    	verticalAngle = -(80.0 * (3.14f)/180.0);

  	//Updated look at point for pitch and yaw.
  	lookAtPoint.x = 1.0 * cos(verticalAngle) * cos(horizontalAngle);
  	lookAtPoint.y = 1.0 * sin(verticalAngle);
  	lookAtPoint.z = 1.0 * cos(verticalAngle) * cos(90.0 - horizontalAngle);
}

void Camera::applyProjectionMatrix(MatrixStack *P) const
{
	P->perspective(fovy, aspect, znear, zfar);
}

void Camera::applyViewMatrix(MatrixStack *MV) const
{
	glm::mat4 View = glm::lookAt((theEye + theStrafe + theZoom), 
		(lookAtPoint + theStrafe + theZoom), glm::vec3(0, 1, 0));
  	MV->multMatrix(View);
}
