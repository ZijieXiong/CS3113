#include "Level1.h"

#define ENEMY_COUNT 1
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
unsigned int level1_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3,
 3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3,
 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};


bool Level1::win() {
	for (int i = 0; i < ENEMY_COUNT; i++) {
		if (state.enemies[i].isactive) {
			return false;
		}
	}
	return true;
}


void Level1::Initialize() {
	state.nextScene = -1;
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	state.bgm = Mix_LoadMUS("crypto.mp3");
	

	state.gameover = Mix_LoadWAV("gameover.wav");
	state.fail = Mix_LoadWAV("failure.wav");
	state.hit = Mix_LoadWAV("bounce.wav");
	Mix_Volume(-1, MIX_MAX_VOLUME / 2);
	Mix_VolumeChunk(state.fail, MIX_MAX_VOLUME);

	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);

	state.player = new Entity();
	state.player->entityType = PLAYER;
	state.player->position = glm::vec3(5, -1, 0);
	state.player->acceleration = glm::vec3(0, -5, 0);
	state.player->velocity = glm::vec3(0, 0, 0);
	state.player->width = 0.9f;
	state.player->life = 3;
	state.player->textureID = Util::LoadTexture("ctg.png");


	state.enemies = new Entity[ENEMY_COUNT];
	GLint enemyTextureID = Util::LoadTexture("enemy.png");
	for (int i = 0; i < ENEMY_COUNT; i++) {
		state.enemies[i].textureID = enemyTextureID;
		state.enemies[i].entityType = ENEMY;
		state.enemies[i].acceleration = glm::vec3(0, -5, 0);
		state.enemies[i].width = 0.6;
		state.enemies[i].height = 0.8;
	}
	state.enemies[0].position = glm::vec3(11, -2.3, 0);
	state.enemies[0].aiType = WALKER;
	state.enemies[0].aiState = IDLE;

	fontTextureID = Util::LoadTexture("font1.png");

}




void Level1::Update(float deltaTime) {
	if (!state.gameEnd) {
		state.player->Update(deltaTime, state.map, state.player, state.enemies, ENEMY_COUNT);
		for (int i = 0; i < ENEMY_COUNT; i++) {
			state.enemies[i].Update(deltaTime, state.map, state.player, NULL, 0);
		}
		if (state.player->lastCollision == ENEMY) {
			Mix_PlayChannel(-1, state.hit, 0);
			if (state.player->isDead) {
				state.player->life -= 1;
				if (state.player->life <= 0) {
					state.gameEnd = true;
					Mix_PlayChannel(-1, state.fail, 0);
				}
				else {
					state.nextScene = 1;
				}
				
			}			
		}
		if (win()) {
			state.nextScene = 2;
		}

	}

}

void Level1::Render(ShaderProgram* program) {
	state.player->Render(program);
	state.map->Render(program);
	//state.enemy->Render(&program);
	for (int i = 0; i < ENEMY_COUNT; i++) {
		state.enemies[i].Render(program);
	}
	
	if (state.gameEnd) {
		glm::vec3 showLife = glm::vec3(0, 0, 0);
		if (state.player->position.x < 5) {
			showLife = glm::vec3(4.4, -3.75, 0);

		}
		else if (state.player->position.x > 8) {
			showLife = glm::vec3(7.4, -3.75, 0);
		}
		else {
			showLife = glm::vec3(state.player->position.x - 0.4, -3.75, 0);
		}
		if (state.win) {
			Util::DrawText(program, fontTextureID, "You Win", 0.5f, -0.25f, showLife);
		}
		else {
			Util::DrawText(program, fontTextureID, "You Lose", 0.5f, -0.25f, showLife);
		}
	}
}

void Level1::Shutdown() {
	Mix_FreeChunk(state.fail);
	Mix_FreeChunk(state.gameover);
	Mix_FreeChunk(state.hit);
	Mix_FreeMusic(state.bgm);
	SDL_Quit();
}