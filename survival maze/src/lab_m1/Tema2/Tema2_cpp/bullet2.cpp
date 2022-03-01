#include "lab_m1/Tema2/Tema2_h/bullet2.h"

#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace m1;

Bullet2::Bullet2(glm::vec3 pos, glm::vec3 direction, glm::vec3 size, glm::vec3 color, float speed)
{
	this->pos = pos;
	this->direction = direction;
	this->size = size;
	this->color = color;
	this->speed = speed;
	available = true;
	lifeTimer = life;
}

Bullet2::~Bullet2()
{
}

void Bullet2::Reset()
{
	available = true;
	lifeTimer = life;
}
