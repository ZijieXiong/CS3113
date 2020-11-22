#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0,0,0);
    velocity = glm::vec3(0,0,0);
    acceleration = glm::vec3(0);
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::PointToBoxCollision(float x, float y, Entity* other) {
    return(x >= other->position.x - other->width/2.0f &&
        x <= other->position.x + other->width/2.0f &&
        y <= other->position.y + other->height/2.0f &&
        y >= other->position.y - other->height/2.0f);
}


bool Entity::CheckCollision(Entity* other) {
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    if (xdist < 0 && ydist < 0)
    {
        lastCollision = other->entityType;
        return true;
    }

    return false;

}

void Entity::CheckCollisionsY(Entity* objects, int objectCount)
{
    glm::vec3 movement = glm::vec3(0, 0, 0);
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (object->isactive) {
            if (CheckCollision(object))
            {
                float ydist = fabs(position.y - object->position.y);
                float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
                if (velocity.y > 0) {
                    movement.y -= penetrationY;
                    velocity.y = 0;
                    collidedTop = true;
                }
                else if (velocity.y < 0) {
                    if (entityType == PLAYER && object->entityType == ENEMY) {
                        object->isactive = false;
                    }
                    else {
                        movement.y += penetrationY;
                        velocity.y = 0;
                        collidedBottom = true;
                    }
                    if (entityType == PLAYER || entityType == ENEMY) {
                        acceleration.y = -5;
                    }
                    
                }
            }
        }
        
    }
    position += movement;
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount)
{
    glm::vec3 movement = glm::vec3(0, 0, 0);
    for (int i = 0; i < objectCount; i++)
    {
        Entity* object = &objects[i];
        if (object->isactive) {
            if (CheckCollision(object))
            {
                if (entityType == PLAYER && object->entityType == ENEMY) {
                    isDead = true;
                    return;
                }
                float xdist = fabs(position.x - object->position.x);
                float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
                if (velocity.x > 0) {
                    movement.x -= penetrationX;
                    velocity.x = 0;
                    collidedRight = true;
                }
                else if (velocity.x < 0) {
                    movement.x += penetrationX;
                    velocity.x = 0;
                    collidedLeft = true;
                }
            }
        }
    }
    position += movement;
}

void Entity::CheckCollisionsY(Map* map)
{
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);

    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
        lastCollision = TILE;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
        lastCollision = TILE;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
        lastCollision = TILE;
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        lastCollision = TILE;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        lastCollision = TILE;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        lastCollision = TILE;
    }
}

void Entity::CheckCollisionsX(Map* map)
{
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
        lastCollision = TILE;
    }

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
        lastCollision = TILE;
    }
}

bool Entity::AI_CheckCollisionX(Map* map) {
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2) - 0.1, position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2) + 0.1, position.y, position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        collidedLeft = true;
        lastCollision = TILE;
        return true;
    }

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        collidedRight = true;
        lastCollision = TILE;
        return true;
    }
    return false;

}

bool Entity::AI_CheckCollisionY(Map* map) {

    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2) - 0.1, position.y - (height / 2) - 0.1, position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2) + 0.1, position.y - (height / 2) - 0.1, position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (!map->IsSolid(bottom_left, &penetration_x, &penetration_y) || !map->IsSolid(bottom_right, &penetration_x, &penetration_y)) {
        return false;
    }
    else {
        return true;
    }
}



void Entity::AIWalker(Map* map) {

    switch (aiState) {
        case IDLE:
            if (init) {
                velocity.x = -1;
                init = false;
            }
            if (AI_CheckCollisionX(map)) {
                velocity.x = -velocity.x;
            }
            break;
    }
   

}

void Entity::AIWaitAndGo(Entity *player) {
    switch (aiState) {
        case IDLE:
            if (glm::distance(position, player->position) < 3.0f) {
                aiState = WALKING;
            }
            break;
        case WALKING:
            if (position.x < player->position.x) {
                velocity.x = 0.7;
            }
            else if(position.x > player->position.x){
                velocity.x = -0.7;
            }
            else {
                velocity.x = 0;
            }

            break;
    }
}


void Entity::AIWalkerOnPF(Entity* player, Map* map) {
    switch (aiState){
        case IDLE:
            if (init) {
                velocity.x = -1;
                init = false;
            }

            if (glm::distance(position, player->position) < 2.0f) {
                aiState = RUNNING;
            }

            if (isTurning) {
                if (AI_CheckCollisionY(map)) {
                    isTurning = false;
                }
            }
            else {
                if (!AI_CheckCollisionY(map)) {
                    velocity.x = -velocity.x;
                    isTurning = true;
                }
            }
            
            if (AI_CheckCollisionX(map)) {
                velocity.x = -velocity.x;
            }


            break;

        case RUNNING:
            if (position.x < player->position.x) {
                velocity.x = -0.7;
            }
            else if (position.x > player->position.x) {
                velocity.x = 0.7;
            }
            if (glm::distance(position, player->position) > 2.0f) {
                aiState = IDLE;
            }
            break;
    }
      
}

void Entity::AI(Entity* player, Map* map) {
    switch (aiType) {
        case WALKER:
            AIWalker(map);
            break;
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
        case WALKERONPF:
            AIWalkerOnPF(player, map);

    }
}


void Entity::Update(float deltaTime, Map* map, Entity* player, Entity *enemies, int enemyCount)
{
    if (!isactive) {
        return;
    }
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    if (entityType == ENEMY) {
        AI(player, map);
    }
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;
            if (animTime >= 0.25f) {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        }
        else {
            animIndex = 0;
        }
    }
    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);// Fix if needed

    CheckCollisionsY(enemies, enemyCount);

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);// Fix if needed

    CheckCollisionsX(enemies, enemyCount);
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) {
    if (!isactive) {
        return;
    }
    program->SetModelMatrix(modelMatrix);

    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}