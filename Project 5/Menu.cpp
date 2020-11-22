#include "Menu.h"





void Menu::Initialize() {
	state.nextScene = -1;
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	state.bgm = Mix_LoadMUS("crypto.mp3");


	state.gameover = Mix_LoadWAV("gameover.wav");
	state.fail = Mix_LoadWAV("failure.wav");
	state.hit = Mix_LoadWAV("bounce.wav");
	Mix_Volume(-1, MIX_MAX_VOLUME / 2);
	Mix_VolumeChunk(state.fail, MIX_MAX_VOLUME);

	state.player = new Entity();
	state.player->velocity = glm::vec3(0);
	state.player->life = 3;

	fontTextureID = Util::LoadTexture("font1.png");

}




void Menu::Update(float deltaTime) {


}

void Menu::Render(ShaderProgram* program) {
	Util::DrawText(program, fontTextureID, "Prof's Adventure", 0.25f, 0.1f, glm::vec3(2, -3, 0));
	
}

void Menu::Shutdown() {
	Mix_FreeChunk(state.fail);
	Mix_FreeChunk(state.gameover);
	Mix_FreeChunk(state.hit);
	Mix_FreeMusic(state.bgm);
	SDL_Quit();
}