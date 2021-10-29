#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>


using namespace std;

class object
{
public:
	object();
	object(string);
	string model;
	GLuint tex;
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> texCoords;
	std::vector<unsigned int> indices;
	GLuint getTex();
	std::vector<glm::vec4> getVerts();
	std::vector<glm::vec4> getNorms();
	std::vector<glm::vec2> getTexCoords();
	std::vector<unsigned int> getIndices();
	GLuint readTexture(const char* filename);
	void loadModel(std::string plik);
};

