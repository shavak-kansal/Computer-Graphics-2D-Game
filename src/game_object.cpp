#include "game_object.h"
#include "game.h"

#include <iostream>

// GameObject::GameObject() 
//     : Position(0.0f, 0.0f), Size(1.0f, 1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity, float rotate) 
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(rotate), Sprite(sprite), IsSolid(false), Destroyed(false) { }

void GameObject::Draw(SpriteRenderer &renderer)
{   
    renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

void GameObject::HandleMovement(float dt,unsigned int Width,unsigned int Height){

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