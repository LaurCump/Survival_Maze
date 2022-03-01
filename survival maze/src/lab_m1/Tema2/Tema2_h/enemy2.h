#pragma once

#include "components/simple_scene.h"

namespace m1
{
    class Enemy2
    {
    public:
        Enemy2(glm::vec3 pos,
              glm::vec3 size = glm::vec3(1),
              glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f),
              float moveSpeed = 2.5f);
        ~Enemy2();

    public:
        glm::vec3 pos;
        glm::vec3 size;
        glm::vec3 color;
        float moveSpeed;
        bool isAlive;   // daca este in viata
        int direction;  // va fi 1 sau -1 

        bool mustDeform;
        float mustDeformTimer;
        const float mustDeformFor = 1.5f;  // in secunde
    };
}
