//
// sueda
// October, 2014
//
#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_
#define xBoundMax 50
#define xBoundMin 0
#define zBoundMax 0
#define zBoundMin -50
#include "GLSL.h"
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
	glm::vec3 getPosition();
	
private:
	std::vector<tinyobj::shape_t> shapes;
	GLuint posBufID;
	GLuint norBufID;
	GLuint indBufID;
	glm::vec3 x; // position
	glm::vec3 v; // velocity
	float scale;
	glm::vec4 color;
};

#endif
