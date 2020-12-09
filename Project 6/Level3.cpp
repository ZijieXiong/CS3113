#include "Level3.h"

#define ENEMY_COUNT 5
#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 9
#define BULLET_COUNT 1
unsigned int level3_data[] =
{
 16, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 16,
 16, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 16,
 16, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 16,
 16, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 16,
 16, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 16,
 16, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 16,
 16, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 16,
 16, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 16,
 16, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 16
};

bool Level3::fire() {
	if (state.player->fire(state.bullets, BULLET_COUNT)) {
		Mix_PlayChannel(-1, state.shoot, 0);
		return true;
	}

	return false;
}




void Level3::Initialize() {
	state.nextScene = -1;


	state.gameover = Mix_LoadWAV("gameover.wav");
	state.fail = Mix_LoadWAV("failure.wav");
	state.hit = Mix_LoadWAV("scream.wav");
	state.shoot = Mix_LoadWAV("shoot.wav");
	Mix_VolumeChunk(state.hit, MIX_MAX_VOLUME);
	Mix_VolumeChunk(state.fail, MIX_MAX_VOLUME);

	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 9, 9);

	state.player = new Entity();
	state.player->entityType = PLAYER;
	state.player->position = glm::vec3(3, -3, 0);
	state.player->velocity = glm::vec3(0, 0, 0);
	state.player->width = 0.9;
	state.player->textureID = Util::LoadTexture("ctg.png");


	state.enemies = new Entity[ENEMY_COUNT];
	GLint enemyTextureID = Util::LoadTexture("enemy.png");
	for (int i = 0; i < ENEMY_COUNT; i++) {
		state.enemies[i].textureID = enemyTextureID;
		state.enemies[i].entityType = ENEMY;
		state.enemies[i].life = 4;
		state.enemies[i].width = 0.6;
		state.enemies[i].height = 0.8;
	}
	state.enemies[0].position = glm::vec3(8, -2, 0);
	state.enemies[0].aiType = WALKER;
	state.enemies[0].aiState = IDLE;
	state.enemies[0].life = 3;
	state.enemies[1].position = glm::vec3(9, -6, 0);
	state.enemies[1].aiType = WAITANDGO;
	state.enemies[1].aiState = IDLE;
	state.enemies[2].position = glm::vec3(7, -6, 0);
	state.enemies[2].aiType = WALKERAROUND;
	state.enemies[2].aiState = IDLE;
	state.enemies[3].position = glm::vec3(3, -7, 0);
	state.enemies[3].aiType = WALKERAROUND;
	state.enemies[3].aiState = IDLE;
	state.enemies[4].position = glm::vec3(1, -6, 0);
	state.enemies[4].aiType = WALKERAROUND;
	state.enemies[4].aiState = IDLE;

	state.bullets = new Entity[BULLET_COUNT];
	GLint bulletTextureID = Util::LoadTexture("beams.png");
	for (int i = 0; i < BULLET_COUNT; i++) {
		state.bullets[i].entityType = BULLET;
		state.bullets[i].isactive = false;
		state.bullets[i].textureID = bulletTextureID;
	}

	fontTextureID = Util::LoadTexture("font1.png");

}

bool Level3::win() {
	for (int i = 0; i < ENEMY_COUNT; i++) {
		if (state.enemies[i].isactive) {
			return false;
		}
	}
	return true;
}


void Level3::Update(float deltaTime) {
	if (!state.gameEnd) {
		state.player->Update(deltaTime, state.map, state.player, state.enemies, ENEMY_COUNT);
		for (int i = 0; i < ENEMY_COUNT; i++) {
			state.enemies[i].Update(deltaTime, state.map, state.player, NULL, 0);
			if (state.enemies[i].lastCollision == BULLET) {
				state.enemies[i].life--;
			if (state.enemies[i].life == 0) {
				state.enemies[i].isactive = false;
			}
				Mix_PlayChannel(-1, state.hit, 0);
				state.enemies[i].lastCollision = TILE;
			}
		}
		for (int i = 0; i < BULLET_COUNT; i++) {
			state.bullets[i].Update(deltaTime, state.map, state.player, state.enemies, ENEMY_COUNT);
		}
		if (state.player->lastCollision == ENEMY) {
			Mix_PlayChannel(-1, state.hit, 0);
		}
		if (state.player->lastCollision == ENEMY) {

			if (state.player->isDead) {
				state.player->life -= 1;
				if (state.player->life <= 0) {
					state.gameEnd = true;
					Mix_PlayChannel(-1, state.fail, 0);
				}
				else {
					state.nextScene = 3;
				}

			}
		}
		if (win()) {
			state.nextScene = 4;
		}
	}

}

void Level3::Render(ShaderProgram* program) {
	state.map->Render(program);
	state.player->Render(program);
	for (int i = 0; i < BULLET_COUNT; i++) {
		state.bullets[i].Render(program);
	}
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
			showLife = glm::vec3(state.player->position.x-0.4 , -3.75, 0);
		}
		if (state.win) {
			Util::DrawText(program, fontTextureID, "You Win", 0.5f, -0.25f, showLife);
		}
		else {
			Util::DrawText(program, fontTextureID, "You Lose", 0.5f, -0.25f, showLife);
		}
	}
}

void Level3::Shutdown() {
	Mix_FreeChunk(state.fail);
	Mix_FreeChunk(state.gameover);
	Mix_FreeChunk(state.hit);
	Mix_FreeMusic(state.bgm);
	SDL_Quit();
}