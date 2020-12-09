#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <cstdlib>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Map.h"

enum EntityType{PLAYER, TILE, ENEMY, BULLET};
enum AIType { WALKER, WAITANDGO, WALKERAROUND};
enum AIState {IDLE, WALKING, RUNNING};

class Entity {
public:
    EntityType entityType;
    EntityType lastCollision;
    AIType aiType;
    AIState aiState;
    bool collideGround = false;

    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;

    float width = 1.0f;
    float height = 1.0f;
    float rotation =0.0f;
    GLuint textureID;

    glm::mat4 modelMatrix;

    int* animRight = NULL;
    int* animLeft = NULL;
    int* animUp = NULL;
    int* animDown = NULL;

    int* animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    float reloadTime = 1.5f;
    float reloading = 0.0f;

    bool isactive = true;
    bool init = true;
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    bool isJumping = false;
    bool isDead = false;
    bool isTurning = false;
    bool isUp = false;
    bool isDown = false;
    bool isRight = false;
    bool isLeft = false;
    bool isfired = false;

    int life;
    Entity();

    bool PointToBoxCollision(float x, float y, Entity* other);
    bool CheckCollision(Entity* other);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void CheckCollisionsY(Entity* objects, int objectCount);
    void Update(float deltaTime, Map* map, Entity* player, Entity *enemies, int enemyCount);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
    void CheckCollisionsX(Map* map);
    void CheckCollisionsY(Map* map);
    void AI(Entity* player, float deltaTime);
    void AIWalker(Entity* player);
    void AIWaitAndGo(Entity *player);
    void AIWalkerAround(Entity* player, float deltaTime);
    bool AI_CheckCollisionX(Map* map);
    bool AI_CheckCollisionY(Map* map);
    int bulletsleft(Entity* bullets, int bulletCount);
    void vecRotate(glm::vec3& v, float angle);
    bool fire(Entity* bullets, int bulletCount);
};