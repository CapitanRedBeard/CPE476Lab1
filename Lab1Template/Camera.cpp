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

Camera::Camera() :
	aspect(1.0f),
	fovy(30.0f/180.0f*M_PI),
	znear(0.1f),
	zfar(1000.0f),
	rotations(0.01, 0.01),
	rfactor(0.0001f),
	pfactor(0.0005f),
	position(0.0f, 0.0f, 10.0f),
	sfactor(0.005f)
{
}

Camera::~Camera()
{
	
}


void Camera::mouseClicked(int x, int y, bool shift, bool ctrl, bool alt)
{
	// mousePrev.x = x;
	// mousePrev.y = y;
	if(shift) {
		state = Camera::TRANSLATE;
	} else if(ctrl) {
		state = Camera::SCALE;
	} else {
		state = Camera::ROTATE;
	}
}

void Camera::mouseMoved(int x, int y)
{
	glm::vec2 mouseCurr(x, y);
	glm::vec2 dv = mouseCurr - mousePrev;
	//printf("Got here");
	switch(state) {
		case Camera::ROTATE:
			rotations += rfactor * dv;
			break;
		case Camera::TRANSLATE:
			position.x += pfactor * dv.x;
			position.y -= pfactor * dv.y;
			break;
		case Camera::SCALE:
			position.z *= (1.0f - sfactor * dv.y);
			break;
	}
	mousePrev.x = x;
	mousePrev.y = y;
}

void Camera::setWindowSize(float w, float h)
{
	aspect = w/h;
}

// void Camera::updateCamera(int x, int y)
// {
// 	// How much has the mouse moved?
// 	float dx = mousePrev.x - x;
// 	float dy = mousePrev.y - y;

// 	rotations.x += rfactor * dx;
// 	rotations.y += rfactor * dy;
// 	mousePrev.x = x;
// 	mousePrev.y = y;
// }
void Camera::update(const bool *keys, const glm::vec2 &mouse)
{
	// How much has the mouse moved?
	float dx = mousePrev.x - mouse.x;
	float dy = mousePrev.y - mouse.y;
	//Up
	if(keys['w']){
		position.z -= pfactor * cos(rotations.x); 
	}else if(keys['a']){
		position.x += pfactor * sin(rotations.x);
	}else if(keys['s']){
		position.z += pfactor * cos(rotations.x);
	}else if(keys['d']){
		position.x -= pfactor * sin(rotations.x);
	}
	rotations.x += rfactor * dx;
	rotations.y += rfactor * dy;

	// Update camera position and orientation here

	// Save last mouse
	mousePrev = mouse;
}

void Camera::applyProjectionMatrix(MatrixStack *P) const
{
	P->perspective(fovy, aspect, znear, zfar);
}

void Camera::applyViewMatrix(MatrixStack *MV) const
{
	// Create the translation and rotation matrices
	glm::mat4 T = glm::translate(position);
	glm::mat4 RX = glm::rotate(rotations.y, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 RY = glm::rotate(rotations.x, glm::vec3(0.0f, 1.0f, 0.0f));
	// The matrix C is the product of these matrices
	glm::mat4 C = T * RX * RY; // Also apply rotations here
	// The view matrix is the inverse
	glm::mat4 V = glm::inverse(C);
	// Add to the matrix stack
	MV->multMatrix(V);
}
