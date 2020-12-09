#include "Entity.h"
#include <cmath>

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
                if (entityType == PLAYER && object->entityType == ENEMY) {
                    isDead = true;
                    return;
                }
                if (entityType == BULLET && object->entityType == ENEMY) {
                    object->lastCollision = BULLET;
                    isactive = false;
                    return;
                }
                if (velocity.y > 0) {
                    movement.y -= penetrationY;
                    velocity.y = 0;
                    collidedTop = true;
                }
                else if (velocity.y < 0) {
                    movement.y += penetrationY;
                    velocity.y = 0;
                    collidedBottom = true;
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
                float xdist = fabs(position.x - object->position.x);
                float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
                if (entityType == PLAYER && object->entityType == ENEMY) {
                    isDead = true;
                    return;
                }
                if (entityType == BULLET && object->entityType == ENEMY) {
                    object->lastCollision = BULLET;
                    isactive = false;
                    return;
                }
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
        if (lastCollision == TILE && entityType == BULLET) {
            isactive = false;
        }
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
        lastCollision = TILE;
        if (lastCollision == TILE && entityType == BULLET) {
            isactive = false;
        }
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
        lastCollision = TILE;
        if (lastCollision == TILE && entityType == BULLET) {
            isactive = false;
        }
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        lastCollision = TILE;
        if (lastCollision == TILE && entityType == BULLET) {
            isactive = false;
        }
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        lastCollision = TILE;
        if (lastCollision == TILE && entityType == BULLET) {
            isactive = false;
        }
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
        lastCollision = TILE;
        if (lastCollision == TILE && entityType == BULLET) {
            isactive = false;
        }
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
        if (lastCollision == TILE && entityType == BULLET) {
            isactive = false;
        }
    }

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
        lastCollision = TILE;
        if (lastCollision == TILE && entityType == BULLET) {
            isactive = false;
        }
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



void Entity::AIWalker(Entity *player) {

    switch (aiState) {
        case IDLE:
            glm::vec3 dir = player->position - position;
            velocity = 0.8f * glm::normalize(dir);
            break;
            
    }
   

}

void Entity::AIWaitAndGo(Entity *player) {
    switch (aiState) {
        case IDLE:
            if (glm::distance(position, player->position) < 5.0f) {
                aiState = WALKING;
            }
            if (life < 4) {
                aiState = WALKING;
            }
            break;
        case WALKING:
            glm::vec3 dir = player->position - position;
            velocity = 1.2f * glm::normalize(dir);
            break;
    }
}


void Entity::AIWalkerAround(Entity* player, float deltaTime) {
    switch (aiState){
        case IDLE:
            if (init) {
                velocity = 0.5f * glm::normalize(glm::vec3(glm::vec3(rand() % 99 - 49, rand() % 100 - 49, 0)));
                reloading = 0;
                init = false;
            }
            reloading += deltaTime;
            if (reloading > reloadTime) {
                velocity = 0.5f * glm::normalize(glm::vec3(glm::vec3(rand() % 99 - 49, rand() % 100 - 49, 0)));
                reloading = 0;
            }
            
            if (glm::distance(position, player->position) < 3.0f) {
                aiState = RUNNING;
            }

            if (life < 4) {
                aiState = RUNNING;
            }

            break;
        case RUNNING:
            glm::vec3 dir = player->position - position;
            velocity = 0.5f * glm::normalize(dir);
            break;
    }
      
}

void Entity::AI(Entity* player, float deltaTime) {
    switch (aiType) {
        case WALKER:
            AIWalker(player);
            break;
        case WAITANDGO:
            AIWaitAndGo(player);
            break;
        case WALKERAROUND:
            AIWalkerAround(player, deltaTime);

    }
}


int Entity::bulletsleft(Entity* bullets, int bulletCount) {
    int left = 0;
    for (int i = 0; i < bulletCount; i++) {
        if (!bullets[i].isactive) {
            left++;
        }
    }
    return left;
}

void Entity::vecRotate(glm::vec3& v, float angle) {
    float length = sqrt(pow(v.x, 2) + pow(v.y, 2));
    v = glm::vec3(length * cos(angle), length * sin(angle), 0);
}

bool Entity::fire(Entity* bullets, int bulletCount) {
    if (bulletsleft(bullets, bulletCount) > 0 && !isfired) {
        int ind = -1;
        for (int i = 0; i < bulletCount; i++) {
            if (!bullets[i].isactive) {
                ind = i;
                break;
            }
        }
        bullets[ind].position = position;
        bullets[ind].velocity = glm::vec3(18.0f, 0.0f , 0.0f);
        bullets[ind].isactive = true;
        bullets[ind].width = 0.2f;
        bullets[ind].height = 0.2f;
        bullets[ind].reloadTime = 0.5f;
        bullets[ind].rotation = rotation;
        vecRotate(bullets[ind].velocity, rotation);
        isfired = true;
        bullets[ind].isfired = true;
        return true;
    }
    return false;
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
        AI(player, deltaTime);
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

    if (isfired) {
        reloading += 1.0f * deltaTime;
        if (reloading >= reloadTime) {
            isfired = false;
            reloading = 0.0f;
        }
    }

    velocity += acceleration * deltaTime;

    position.y += velocity.y * deltaTime; // Move on Y
    CheckCollisionsY(map);// Fix if needed

    CheckCollisionsY(enemies, enemyCount);

    position.x += velocity.x * deltaTime; // Move on X
    CheckCollisionsX(map);// Fix if needed

    CheckCollisionsX(enemies, enemyCount);
    if (entityType == ENEMY) {
        float len = glm::length(velocity);
        if (len > 0) {
            rotation = asin(velocity.y / len);
            if (velocity.x < 0) {
                rotation = 1.0f*M_PI - rotation;
            }
        }
        
    }
    if (isUp) {
        if (isLeft) {
            rotation = 0.75f * M_PI;
        }
        else if (isRight) {
            rotation = 0.25f * M_PI;
        }
        else {
            rotation = 0.5f * M_PI;
        }
    }
    else if (isDown){
        if (isLeft) {
            rotation = 1.25f * M_PI;
        }
        else if (isRight) {
            rotation = 1.75f * M_PI;
        }
        else {
            rotation = 1.5f * M_PI;
        }
    }
    else if (isLeft) {
        rotation = 1.0f * M_PI;
    }
    else if (isRight){
        rotation = 0.0f * M_PI;
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));

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