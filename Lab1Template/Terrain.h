/**
 * Terrain.h - Header file for Terrain object.
 * @author Brandon Clark
 */

#pragma once
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif
#ifdef __unix__
#include <GL/glut.h>
#endif
#include "tiny_obj_loader.h"

class Terrain
{
public:
	Terrain();
	virtual ~Terrain();
	void init();
	void draw(GLint h_pos, GLint h_nor);
	
private:
	static const int MAP_X = 100;
	static const int MAP_Z = 100;
	static const int MAP_SCALE = 10;
	float terrain[MAP_X][MAP_Z][3]; // Terrain data
	GLuint posBufID;
	GLuint norBufID;
	GLuint texBufID;
	glm::vec3 x; // position
	float scale;
};

#endif
