#pragma once
#include "../Utilities/utilities.h"
#include <vector>

enum class ShaderState {CULLING, DEPTH_TEST, ALPHA};

class Shaders 
{
public:
	GLuint program, vertexShader, fragmentShader;
	char fileVS[260];
	char fileFS[260];
	GLuint locationAttributePos;
	GLuint locationAttributeUV;
	GLuint locationUniformTex;
	GLuint locationUniformMVP;
	GLuint locationUniformTime;

	std::vector<ShaderState> states;
	
	int Init(char * fileVertexShader, char * fileFragmentShader);
	void EnableStates();
	void DisableStates();
	~Shaders();
};