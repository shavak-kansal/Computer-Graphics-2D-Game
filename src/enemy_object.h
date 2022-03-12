#ifndef ENEMYOBJECT_H
#define ENEMYOBJECT_H

#include "game_object.h"
#include <glm/glm.hpp>

class EnemyObject : public GameObject {
	public :

		float Radius; 

		EnemyObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);
		
		void HandleMovement(float dt,unsigned int Width,unsigned int Height);
};

#endif