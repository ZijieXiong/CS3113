#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include<iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include "ShaderProgram.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, LeftPadModelMatrix, RightPadModelMatrix, BallModelMatrix, projectionMatrix;

GLuint LeftPadTextureID;
GLuint RightPadTextureID;
GLuint BallTextureID;

glm::vec3 left_pad_pos = glm::vec3(-5, 0, 0);
glm::vec3 right_pad_pos = glm::vec3(5, 0, 0);
glm::vec3 ball_pos = glm::vec3(0, 0, 0);

glm::vec3 left_pad_mov = glm::vec3(0, 0, 0);
glm::vec3 right_pad_mov = glm::vec3(0, 0, 0);
glm::vec3 ball_mov = glm::vec3(1, 0, 0);

float pad_speed = 4.0f;
float ball_speed = 2.0f;
float lastTicks = 0;
bool isGameStart = false;
float pad_width = 1.0f;
float pad_height = 1.0f;
float ball_width = 1.0f;
float ball_height = 1.0f;


bool LeftPadTouched() {
	float x = fabs(left_pad_pos.x - ball_pos.x) - ((pad_width + ball_width) / 2.0f);
	float y = fabs(left_pad_pos.y - ball_pos.y) - ((pad_height + ball_height)/ 2.0f);
	return (x < 0 && y<0);
}

bool RightPadTouched() {
	float x = fabs(right_pad_pos.x - ball_pos.x) - ((pad_width + ball_width) / 2.0f);
	float y = fabs(right_pad_pos.y - ball_pos.y) - ((pad_height + ball_height) / 2.0f);
	return (x<0 && y<0);
}

bool UpBoundTouched() {
	return (ball_pos.y + ball_height / 2.0f) >= 3.75f;
}

bool BotBoundTouched() {
	return (ball_pos.y - ball_height / 2.0f) <= -3.75f;
}

bool LeftBoundTouched() {
	return (ball_pos.x - ball_width / 2.0f) <= -5.0f;
}

bool RightBoundTouched() {
	return (ball_pos.x + ball_width / 2.0f) >= 5.0f;
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
	displayWindow = SDL_CreateWindow("Project 2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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



	LeftPadTextureID = LoadTexture("ctg.png");
	RightPadTextureID = LoadTexture("ctg.png");
	BallTextureID = LoadTexture("ham.png");

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ProcessInput() {
	
	left_pad_mov = glm::vec3(0, 0, 0);
	right_pad_mov = glm::vec3(0, 0, 0);

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
					if (!isGameStart) {
						glm::vec3 left_pad_pos = glm::vec3(-5, 0, 0);
						glm::vec3 right_pad_pos = glm::vec3(5, 0, 0);
						ball_pos = glm::vec3(0, 0, 0);
						isGameStart = true;
					}
					break;
				}
				break;
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_UP]) {
		right_pad_mov.y = 1.0f;
	}
	else if (keys[SDL_SCANCODE_DOWN]) {
		right_pad_mov.y = -1.0f;
	}
	if (keys[SDL_SCANCODE_W]) {
		left_pad_mov.y = 1.0f;
	}
	else if (keys[SDL_SCANCODE_S]) {
		left_pad_mov.y = -1.0f;
	}
}



bool is_moving_left = true;
void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;

	

	if (isGameStart) {
		if (RightPadTouched()) {
			ball_mov.x = -1;
			if (ball_pos.y < right_pad_pos.y) {
				ball_mov.y = -1;
			}
			else {
				ball_mov.y = 1;
			}
		}
		if (LeftPadTouched()) {
			ball_mov.x = 1;
			if (ball_pos.y < left_pad_pos.y) {
				ball_mov.y = -1;
			}
			else {
				ball_mov.y = 1;
			}
		}
		if (UpBoundTouched()) {
			ball_mov.y = -1;
		}
		if (BotBoundTouched()) {
			ball_mov.y = 1;
		}
		if (RightBoundTouched() || LeftBoundTouched()) {
			isGameStart = false;
		}
		if (glm::length(ball_mov) > 1.0f) {
			ball_mov=glm::normalize(ball_mov);
		}
		ball_pos += ball_mov * ball_speed * deltaTime;
		left_pad_pos += left_pad_mov * pad_speed * deltaTime;
		if (left_pad_pos.y >= 3.75f || left_pad_pos.y <= -3.75f) {
			left_pad_pos -= left_pad_mov * pad_speed * deltaTime;
		}
		right_pad_pos += right_pad_mov * pad_speed * deltaTime;
		if (right_pad_pos.y >= 3.75f || right_pad_pos.y <= -3.75f) {
			right_pad_pos -= right_pad_mov * pad_speed * deltaTime;
		}

	}
	
	

	

	LeftPadModelMatrix = glm::mat4(1.0f);
	LeftPadModelMatrix = glm::translate(LeftPadModelMatrix, left_pad_pos);
	LeftPadModelMatrix = glm::rotate(LeftPadModelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	RightPadModelMatrix = glm::mat4(1.0f);
	RightPadModelMatrix = glm::translate(RightPadModelMatrix, right_pad_pos);
	RightPadModelMatrix = glm::rotate(RightPadModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	BallModelMatrix = glm::mat4(1.0f);
	BallModelMatrix = glm::scale(BallModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
	BallModelMatrix = glm::translate(BallModelMatrix, ball_pos);
	BallModelMatrix = glm::rotate(BallModelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);


	float pad_vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
	float pad_texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	float ball_vertices[] = { -0.25, -0.25, 0.25, -0.25, 0.25, 0.25, -0.25, -0.25, 0.25, 0.25, -0.25, 0.25 };
	float ball_texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, pad_vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, pad_texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	program.SetModelMatrix(LeftPadModelMatrix);
	glBindTexture(GL_TEXTURE_2D, LeftPadTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	program.SetModelMatrix(RightPadModelMatrix);
	glBindTexture(GL_TEXTURE_2D, RightPadTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);


	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ball_vertices);
	glEnableVertexAttribArray(program.positionAttribute);
	glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, ball_texCoords);
	glEnableVertexAttribArray(program.texCoordAttribute);

	program.SetModelMatrix(BallModelMatrix);
	glBindTexture(GL_TEXTURE_2D, BallTextureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);


	glDisableVertexAttribArray(program.positionAttribute);
	glDisableVertexAttribArray(program.texCoordAttribute);

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