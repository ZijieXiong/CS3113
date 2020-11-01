#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include<iostream>
#include<vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
//https://www.libsdl.org/projects/SDL_mixer/docs/index.html
#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Entity.h"
#define PLATFORM_COUNT 12
#define ENEMY_COUNT 3



struct GameState {
	Entity *player;
	Entity *platforms;
	Entity *enemies;
	Entity *enemy;
	bool gameEnd = false;
	bool win = false;
	
	Mix_Music *bgm;
	Mix_Chunk* gameover;
	Mix_Chunk* fail;
	Mix_Chunk* hit;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, LeftPadModelMatrix, RightPadModelMatrix, BallModelMatrix, projectionMatrix;

GLuint fontTextureID;

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
	float size, float spacing, glm::vec3 position)
{
	float width = 1.0f / 16.0f;
	float height = 1.0f / 16.0f;

	std::vector<float> vertices;
	std::vector<float> texCoords;

	for (int i = 0; i < text.size(); i++) {

		int index = (int)text[i];
		float offset = (size + spacing) * i;
		float u = (float)(index % 16) / 16.0f;
		float v = (float)(index / 16) / 16.0f;
		vertices.insert(vertices.end(), {
			offset + (-0.5f * size), 0.5f * size,
			offset + (-0.5f * size), -0.5f * size,
			offset + (0.5f * size), 0.5f * size,
			offset + (0.5f * size), -0.5f * size,
			offset + (0.5f * size), 0.5f * size,
			offset + (-0.5f * size), -0.5f * size,
			});
		texCoords.insert(texCoords.end(), {
		u, v,
		u, v + height,
		u + width, v,
		u + width, v + height,
		u + width, v,
		u, v + height,
			});

	} // end of for loop
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	program->SetModelMatrix(modelMatrix);

	glUseProgram(program->programID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, fontTextureID);
	glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

GLuint LoadTexture(const char* filePath) {
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}


void Initialize() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	displayWindow = SDL_CreateWindow("Project 4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 640, 480);
	SDL_Init(SDL_INIT_VIDEO);
	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	state.bgm = Mix_LoadMUS("crypto.mp3");
	Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	Mix_PlayMusic(state.bgm, -1);

	state.gameover = Mix_LoadWAV("gameover.wav");
	state.fail = Mix_LoadWAV("failure.wav");
	state.hit = Mix_LoadWAV("bounce.wav");
	Mix_Volume(-1, MIX_MAX_VOLUME / 2);
	Mix_VolumeChunk(state.fail, MIX_MAX_VOLUME);

	viewMatrix = glm::mat4(1.0f);
	LeftPadModelMatrix = glm::mat4(1.0f);
	BallModelMatrix = glm::mat4(1.0f);
	RightPadModelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	state.player = new Entity();
	state.player->entityType = PLAYER;
	state.player->position = glm::vec3(-4, -2.3, 0);
	state.player->acceleration = glm::vec3(0, -5, 0);
	state.player->velocity = glm::vec3(0, 0, 0);
	state.player->width = 0.9f;
	state.player->textureID = LoadTexture("ctg.png");

	state.platforms = new Entity[PLATFORM_COUNT];
	for (int i = 0; i < PLATFORM_COUNT - 2; i++) {
		state.platforms[i].position = glm::vec3(-4.5 + i, -3.3, 0);
		state.platforms[i].entityType = GROUND;
	}
	state.platforms[10].position = glm::vec3(1, -1.2, 0);
	state.platforms[10].entityType = PLATFORM;
	state.platforms[11].position = glm::vec3(2, -1.2, 0);
	state.platforms[11].entityType = PLATFORM;
	GLuint groundTextureID = LoadTexture("ground.png");
	for (int i = 0; i < PLATFORM_COUNT; i++) {
		state.platforms[i].textureID = groundTextureID;
		state.platforms[i].Update(0, NULL, 0, NULL, NULL, 0);
	}


	state.enemies = new Entity[ENEMY_COUNT];
	GLint enemyTextureID = LoadTexture("enemy.png");
	for (int i = 0; i < ENEMY_COUNT; i++) {
		state.enemies[i].textureID = enemyTextureID;
		state.enemies[i].entityType = ENEMY;
		state.enemies[i].acceleration = glm::vec3(0,-5,0);
		state.enemies[i].width = 0.5;
		state.enemies[i].height = 0.8;
	}
	state.enemies[0].position = glm::vec3(1, -2.3, 0);
	state.enemies[0].aiType = WALKER;
	state.enemies[0].aiState = WALKING;
	state.enemies[1].position = glm::vec3(0, -2.3, 0);
	state.enemies[1].aiType = WAITANDGO;
	state.enemies[1].aiState = IDLE;
	state.enemies[2].position = glm::vec3(1 , 0, 0);
	state.enemies[2].aiType = WALKERONPF;
	state.enemies[2].aiState = IDLE;


	fontTextureID = LoadTexture("font1.png");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {
	state.player->velocity.x = 0;
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
					if (state.player->collidedBottom && !(state.player->isJumping)) {
						state.player->isJumping = true;
						state.player->velocity.y = state.player->jumpPower;
						state.player->acceleration.y = -3;
					}
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
					case SDLK_SPACE:
 						state.player->acceleration.y = -5;
						state.player->isJumping = false;
						break;	
				}
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_LEFT]) {
		state.player->velocity.x = -1.0f;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		state.player->velocity.x = 1.0f;
	}

	if (keys[SDL_SCANCODE_SPACE]) {
		if (state.player->isJumping) {
			
			state.player->isJumping = true;
		}
		
	}
}

bool win() {
	for (int i = 0; i < ENEMY_COUNT; i++) {
		if (state.enemies[i].isactive) {
			return false;
		}
	}
	return true;
}


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {

	bool collidedTop = false;
	bool collidedBottom = false;
	bool collidedLeft = false;
	bool collidedRight = false;

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
		if (!state.gameEnd) {
			state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.player, state.enemies, ENEMY_COUNT);
			for (int i = 0; i < ENEMY_COUNT; i++) {
				state.enemies[i].Update(FIXED_TIMESTEP,state.platforms, PLATFORM_COUNT, state.player, NULL, 0);
			}
			if (state.player->lastCollision == ENEMY) {
				Mix_PlayChannel(-1, state.hit, 0);
			}
			if (state.player->isDead) {
				state.gameEnd=true;
				Mix_PlayChannel(-1, state.fail, 0);
			}
			if (win()) {
				state.gameEnd = true;
				state.win = true;
				Mix_PlayChannel(-1, state.gameover, 0);
			}
		}
		

		deltaTime -= FIXED_TIMESTEP;
	}
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	state.player->Render(&program);
	for (int i = 0; i < PLATFORM_COUNT; i++) {
		state.platforms[i].Render(&program);
	}
	//state.enemy->Render(&program);
	for (int i = 0; i < ENEMY_COUNT; i++) {
		state.enemies[i].Render(&program);
	}
	if (state.gameEnd) {
		if (state.win) {
			DrawText(&program, fontTextureID, "You Win", 0.5f, -0.25f, glm::vec3(-0.5, 0, 0));
		}
		else {
			DrawText(&program, fontTextureID, "You Lose", 0.5f, -0.25f, glm::vec3(-0.5, 0, 0));
		}
	}

	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	Mix_FreeChunk(state.fail);
	Mix_FreeChunk(state.gameover);
	Mix_FreeChunk(state.hit);
	Mix_FreeMusic(state.bgm);
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
	}

	Shutdown();
	return 0;
}