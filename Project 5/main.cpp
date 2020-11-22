#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include<iostream>
#include<vector>
#include <SDL.h>
#include <SDL_opengl.h>

//https://www.libsdl.org/projects/SDL_mixer/docs/index.html
#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Scene.h"
#include "Menu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Entity.h"
#include "Map.h"
#include "Util.h"


Scene* currentScene;
Scene* sceneList[4];


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, LeftPadModelMatrix, RightPadModelMatrix, BallModelMatrix, projectionMatrix;





void SwitchToScene(Scene* scene){
	int life;
	bool init = true;
	if (currentScene) {
		life = currentScene->state.player->life;
		init = false;
	}
	currentScene = scene;
    currentScene->Initialize();
	if (!init) {
		currentScene->state.player->life = life;
	}
}



void Initialize() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("Project 5", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 640, 480);
	SDL_Init(SDL_INIT_VIDEO);
	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// Inside of Initialize
	sceneList[0] = new Menu();
	sceneList[1] = new Level1();
	sceneList[2] = new Level2();
	sceneList[3] = new Level3();
	SwitchToScene(sceneList[0]);
	

	Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	Mix_PlayMusic(currentScene->state.bgm, -1);

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}

void ProcessInput() {
	currentScene->state.player->velocity.x = 0;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
			case SDL_WINDOWEVENT_CLOSE:
				gameIsRunning = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_SPACE:
					if (currentScene->state.player->collidedBottom && !(currentScene->state.player->isJumping)) {
						currentScene->state.player->isJumping = true;
						currentScene->state.player->velocity.y = currentScene->state.player->jumpPower;
						currentScene->state.player->acceleration.y = -3;
					}
				case SDLK_RETURN:
					if (currentScene == sceneList[0]) {
						currentScene->state.nextScene = 1;
					}
				}
				break;
				
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
					case SDLK_SPACE:
						currentScene->state.player->acceleration.y = -5;
						currentScene->state.player->isJumping = false;
						break;	
				}
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_LEFT]) {
		currentScene->state.player->velocity.x = -1.0f;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		currentScene->state.player->velocity.x = 1.0f;
	}

	if (keys[SDL_SCANCODE_SPACE]) {
		if (currentScene->state.player->isJumping) {
			
			currentScene->state.player->isJumping = true;
		}
		
	}
}



#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {


	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;
	deltaTime += accumulator;

	if (deltaTime < FIXED_TIMESTEP) {
		accumulator = deltaTime;
		return;
	}

	while (deltaTime >= FIXED_TIMESTEP) {
		// Update. Notice it's FIXED_TIMESTEP. Not deltaTime
		currentScene->Update(FIXED_TIMESTEP);
		deltaTime -= FIXED_TIMESTEP;
	}
	viewMatrix = glm::mat4(1.0f);
	if (currentScene->state.player->position.x < 5) {
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
		
	}
	else if (currentScene->state.player->position.x > 8) {
		viewMatrix = glm::translate(viewMatrix, glm::vec3(-8, 3.75, 0));
	}
	else {
		viewMatrix = glm::translate(viewMatrix,
			glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
	}
}

void Render() {
	program.SetViewMatrix(viewMatrix);
	glClear(GL_COLOR_BUFFER_BIT);
	glm::vec3 showLife = glm::vec3(0, 0, 0);
	if (currentScene->state.player->position.x < 5) {
		showLife = glm::vec3(1, -1, 0);

	}
	else if (currentScene->state.player->position.x > 8) {
		showLife = glm::vec3(4, -1, 0);
	}
	else {
		showLife = glm::vec3(currentScene->state.player->position.x-4, -1, 0);
	}
	currentScene->Render(&program);
	if (currentScene != sceneList[0]) {
		Util::DrawText(&program, currentScene->fontTextureID, "Life : " + std::to_string(currentScene->state.player->life), 0.5f, -0.25f, showLife);
	}

	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	currentScene->Shutdown();
}

int main(int argc, char* argv[]) {
	Initialize();
	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
		if (currentScene->state.nextScene >= 0) SwitchToScene(sceneList[currentScene->state.nextScene]);
	}
	Shutdown();
	return 0;
}