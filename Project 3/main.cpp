#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include<iostream>
#include<vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Entity.h"
#define PLATFORM_COUNT 12


struct GameState {
	Entity *player;
	Entity *platforms;
	bool gameEnd = false;
	bool win = true;
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
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Project 3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, 640, 480);
	SDL_Init(SDL_INIT_VIDEO);
	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
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
	state.player->entityType = PlAYER;
	state.player->position = glm::vec3(0, 3.3, 0);
	state.player->acceleration = glm::vec3(0, -0.1, 0);
	state.player->velocity = glm::vec3(0, 0, 0);
	state.player->textureID = LoadTexture("ctg.png");

	state.platforms = new Entity[PLATFORM_COUNT];
	state.platforms[0].position = glm::vec3(-4.5, -3.3, 0);
	state.platforms[1].position = glm::vec3(-3.5, -3.3, 0);
	state.platforms[2].position = glm::vec3(-2.5, -3.3, 0);
	state.platforms[3].position = glm::vec3(-1.5, -3.3, 0);
	state.platforms[4].position = glm::vec3(-0.5, -3.3, 0);
	state.platforms[5].position = glm::vec3(0.5, -3.3, 0);
	state.platforms[6].position = glm::vec3(1.5, -3.3, 0);
	state.platforms[7].position = glm::vec3(2.5, -3.3, 0);
	state.platforms[8].position = glm::vec3(3.5, -3.3, 0);
	state.platforms[9].position = glm::vec3(4.5, -3.3, 0);
	state.platforms[10].position = glm::vec3(1, 0, 0);
	state.platforms[11].position = glm::vec3(2, 0, 0);
	GLuint groundTextureID = LoadTexture("ground.png");
	for (int i = 0; i < PLATFORM_COUNT; i++) {
		state.platforms[i].entityType = GROUND;
		state.platforms[i].textureID = groundTextureID;
		state.platforms[i].Update(0);
	}
	GLuint destTextureID = LoadTexture("destination.png");
	state.platforms[7].textureID = destTextureID;
	state.platforms[7].entityType = DESTINATION;
	state.platforms[8].textureID = destTextureID;
	state.platforms[8].entityType = DESTINATION;
	fontTextureID = LoadTexture("font1.png");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {
	state.player->acceleration.x = 0;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
			case SDL_WINDOWEVENT_CLOSE:
				gameIsRunning = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				
				}
				break;
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_LEFT]) {
		state.player->acceleration.x = -1.0f;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		state.player->acceleration.x = 1.0f;
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
		if (!state.gameEnd) {
			state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
			if (state.player->lastCollision == DESTINATION || state.player->lastCollision == GROUND) {
				state.gameEnd = true;
				if (state.player->collideGround) {
					state.win = false;
				}
			}
		}
		

		deltaTime -= FIXED_TIMESTEP;
	}

	accumulator = deltaTime;
	

	

	
	

}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	state.player->Render(&program);
	for (int i = 0; i < PLATFORM_COUNT; i++) {
		state.platforms[i].Render(&program);
	}
	if (state.gameEnd) {
		if (state.win) {
			DrawText(&program, fontTextureID, "Mission Successful", 0.5f, -0.25f, glm::vec3(-2, 0, 0));
		}
		else {
			DrawText(&program, fontTextureID, "Mission Failed", 0.5f, -0.25f, glm::vec3(-2, 0, 0));
		}
	}

	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
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