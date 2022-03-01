#include "lab_m1/Tema2/Tema2_h/wall2.h"

#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace m1;

Wall2::Wall2(glm::vec3 pos, glm::vec3 size, glm::vec3 color)
{
	this->pos = pos;
	this->size = size;
	this->color = color;
}

Wall2::~Wall2()
{
}
