#include "Level2.h"

#define ENEMY_COUNT 2
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
unsigned int level2_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3,
 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3,
 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};





void Level2::Initialize() {
	state.nextScene = -1;
	

	state.bgm = Mix_LoadMUS("crypto.mp3");

	state.gameover = Mix_LoadWAV("gameover.wav");
	state.fail = Mix_LoadWAV("failure.wav");
	state.hit = Mix_LoadWAV("bounce.wav");
	Mix_Volume(-1, MIX_MAX_VOLUME / 2);
	Mix_VolumeChunk(state.fail, MIX_MAX_VOLUME);

	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);

	state.player = new Entity();
	state.player->entityType = PLAYER;
	state.player->position = glm::vec3(5, -1, 0);
	state.player->acceleration = glm::vec3(0, -5, 0);
	state.player->velocity = glm::vec3(0, 0, 0);
	state.player->width = 1.0f;
	state.player->textureID = Util::LoadTexture("ctg.png");


	state.enemies = new Entity[ENEMY_COUNT];
	GLint enemyTextureID = Util::LoadTexture("enemy.png");
	for (int i = 0; i < ENEMY_COUNT; i++) {
		state.enemies[i].textureID = enemyTextureID;
		state.enemies[i].entityType = ENEMY;
		state.enemies[i].acceleration = glm::vec3(0, -5, 0);
		state.enemies[i].width = 0.6;
		state.enemies[i].height = 0.8;
		state.enemies[i].velocity.x = -1;
	}
	state.enemies[0].position = glm::vec3(9, -2.3, 0);
	state.enemies[0].aiType = WALKER;
	state.enemies[0].aiState = IDLE;
	state.enemies[1].position = glm::vec3(8, -2.3, 0);
	state.enemies[1].aiType = WAITANDGO;
	state.enemies[1].aiState = IDLE;

	fontTextureID = Util::LoadTexture("font1.png");

}

bool Level2::win() {
	for (int i = 0; i < ENEMY_COUNT; i++) {
		if (state.enemies[i].isactive) {
			return false;
		}
	}
	return true;
}


void Level2::Update(float deltaTime) {
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
					state.nextScene = 2;
				}

			}
		}
		if (win()) {
			state.nextScene = 3;
		}
	}

}

void Level2::Render(ShaderProgram* program) {
	state.player->Render(program);
	state.map->Render(program);
	//state.enemy->Render(&program);
	for (int i = 0; i < ENEMY_COUNT; i++) {
		state.enemies[i].Render(program);
	}
	if (state.gameEnd) {
		if (state.win) {
			Util::DrawText(program, fontTextureID, "You Win", 0.5f, -0.25f, glm::vec3(-0.5 + state.player->position.x, -3.75, 0));
		}
		else {
			Util::DrawText(program, fontTextureID, "You Lose", 0.5f, -0.25f, glm::vec3(-0.5 + state.player->position.x, -3.75, 0));
		}
	}
}

void Level2::Shutdown() {
	Mix_FreeChunk(state.fail);
	Mix_FreeChunk(state.gameover);
	Mix_FreeChunk(state.hit);
	Mix_FreeMusic(state.bgm);
	SDL_Quit();
}