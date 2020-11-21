#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"


enum EntityType{PLAYER, GROUND, PLATFORM, ENEMY};
enum AIType { WALKER, WAITANDGO, WALKERONPF};
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
    float jumpPower =4.0f;
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

    bool isactive = true;
    bool init = true;
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    bool isJumping = false;
    bool isDead = false;
    bool isTurning = false;
    Entity();

    bool PointToBoxCollision(float x, float y, Entity* other);
    bool CheckEdge(float x, float y, Entity* objects, int objectCount);
    bool CheckCollision(Entity* other);
    void CheckCollisionsX(Entity* objects, int objectCount);
    void CheckCollisionsY(Entity* objects, int objectCount);
    void Update(float deltaTime, Entity* platforms, int platformCount, Entity* player, Entity *enemies, int enemyCount);
    void Render(ShaderProgram* program);
    void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
    
    void AI(Entity* player, Entity* platforms, int platformCount);
    void AIWalker();
    void AIWaitAndGo(Entity *player);
    void AIWalkerOnPF(Entity *player, Entity *platforms, int platformCount);
};