#include "Menu.h"



bool Menu::fire() {

	return false;
}

void Menu::Initialize() {
	state.nextScene = -1;

	state.bgm = Mix_LoadMUS("nora.mp3");


	state.gameover = Mix_LoadWAV("gameover.wav");
	state.fail = Mix_LoadWAV("failure.wav");
	state.hit = Mix_LoadWAV("bounce.wav");
	Mix_VolumeMusic(MIX_MAX_VOLUME / 16);
	Mix_VolumeChunk(state.fail, MIX_MAX_VOLUME);

	state.player = new Entity();
	state.player->velocity = glm::vec3(0);
	state.player->life = 3;

	fontTextureID = Util::LoadTexture("font1.png");

}




void Menu::Update(float deltaTime) {


}

void Menu::Render(ShaderProgram* program) {
	Util::DrawText(program, fontTextureID, "Zombie Land", 0.6f, 0.000000000000000000000000000000000000000000001f, glm::vec3(-2.7, 1, 0));
	Util::DrawText(program, fontTextureID, "WASD: shoot dir", 0.4f, 0.000000000000000000000000000000000000000000001f, glm::vec3(-2.5, 0, 0));
	Util::DrawText(program, fontTextureID, "dir: walk dir", 0.4f, 0.00000000000000000000000000000001f, glm::vec3(-2.5, -0.5, 0));
	Util::DrawText(program, fontTextureID, "space: fire", 0.4f, 0.00000000000000000000000000000001f, glm::vec3(-2.5, -1, 0));
	Util::DrawText(program, fontTextureID, "enter: start", 0.4f, 0.00000000000000000000000000000001f, glm::vec3(-2.5, -1.5, 0));
	
}

void Menu::Shutdown() {
	Mix_FreeChunk(state.fail);
	Mix_FreeChunk(state.gameover);
	Mix_FreeChunk(state.hit);
	Mix_FreeMusic(state.bgm);
	SDL_Quit();
}