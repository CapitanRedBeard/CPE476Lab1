//
// sueda
// November, 2014
//

#pragma  once
#ifndef __Camera__
#define __Camera__

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"

class MatrixStack;

class Camera
{
public:

	enum {
		ROTATE = 0,
		TRANSLATE,
		SCALE
	};
	
	Camera();
	virtual ~Camera();
	void setWindowSize(float w, float h);
	// void updateCamera(int x, int y);
	void update(const bool *keys, const glm::vec2 &mouse);
	void mouseClicked(int x, int y, bool shift, bool ctrl, bool alt);
	void mouseMoved(int x, int y);
	void applyProjectionMatrix(MatrixStack *P) const;
	void applyViewMatrix(MatrixStack *MV) const;
	
private:
	float aspect;
	float fovy;
	float znear;
	float zfar;
	float rfactor;
	float pfactor;
	float sfactor;
	int state;
	glm::vec2 mousePrev;
	glm::vec2 rotations;
	glm::vec3 position;
};

#endif
