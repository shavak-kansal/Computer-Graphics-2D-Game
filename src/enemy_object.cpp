#include "enemy_object.h"
#include <iostream>

EnemyObject::EnemyObject(glm::vec2 pos1, float radius, glm::vec2 velocity, Texture2D sprite)
: GameObject(pos1, glm::vec2(radius * 2.0f, radius * 2.0f) , sprite, glm::vec3(1.0f), velocity), Radius(radius) { }

void EnemyObject::HandleMovement(float dt,unsigned int Width,unsigned int Height){

  this->Position += this->Velocity * dt;


  if (this->Position.x <= 0.0f){
    this->Velocity.x = -this->Velocity.x;
    this->Position.x = 0.0f;
}
else if (this->Position.x + this->Size.x >= Width){
    this->Velocity.x = -this->Velocity.x;
    this->Position.x = Width - this->Size.x;
}

if (this->Position.y <= 0.0f){
    this->Velocity.y = -this->Velocity.y;
    this->Position.y = 0.0f;
}
else if (this->Position.y + this->Size.y >= Height){
    this->Velocity.y = -this->Velocity.y;
    this->Position.y = Height - this->Size.y;
}
}