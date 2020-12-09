#pragma once
#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <iostream>
#include <string>
#include <SDL_opengl.h>
#include <math.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include <SDL_mixer.h>

struct GameState {

	Map* map;
	Entity* player;
	Entity* platforms;
	Entity* enemies;
	Entity* enemy;
	Entity* bullets;
	bool gameEnd = false;
	bool win = false;
	int nextScene = -1;

	Mix_Music* bgm;
	Mix_Chunk* gameover;
	Mix_Chunk* fail;
	Mix_Chunk* hit;
	Mix_Chunk* shoot;
};
class Scene {
public:
	GameState state;
	GLuint fontTextureID;
	virtual void Initialize() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Render(ShaderProgram* program) = 0;
	virtual void Shutdown() = 0;
	virtual bool fire() = 0;
};