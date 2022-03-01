#include "lab_m1/Tema2/Tema2_h/enemy2.h"

#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace m1;

Enemy2::Enemy2(glm::vec3 pos, glm::vec3 size, glm::vec3 color, float moveSpeed)
{
	this->pos = pos;
	this->size = size;
	this->color = color;
	this->moveSpeed = moveSpeed;

	isAlive = true;
	direction = 1;

	mustDeform = false;
	mustDeformTimer = mustDeformFor;
}

Enemy2::~Enemy2()
{
}
