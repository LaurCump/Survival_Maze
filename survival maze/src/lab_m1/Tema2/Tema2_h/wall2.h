#pragma once

#include "components/simple_scene.h"

namespace m1
{
    class Wall2
    {
    public:
        Wall2(glm::vec3 pos = glm::vec3(0),
             glm::vec3 size = glm::vec3(2.0f, 2.0f, 2.0f),
             glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f));
        ~Wall2();

    public:
        glm::vec3 pos;
        glm::vec3 size;
        glm::vec3 color;
    };
}
