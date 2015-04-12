//
// sueda
// October, 2014
//

#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif
#include "tiny_obj_loader.h"

class Shape
{
public:
	Shape();
	virtual ~Shape();
	void load(const std::string &meshName);
	void init();
	void draw(GLint h_pos, GLint h_nor);
	void update(float t, float h, const glm::vec3 &g, const bool *keyToggles);
	
private:
	std::vector<tinyobj::shape_t> shapes;
	GLuint posBufID;
	GLuint norBufID;
	GLuint indBufID;
	float m; // mass
	float d; // viscous damping
	glm::vec3 x; // position
	glm::vec3 v; // velocity
	float scale;
	glm::vec4 color;
};

#endif
