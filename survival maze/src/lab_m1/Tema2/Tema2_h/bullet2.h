#pragma once

#include "components/simple_scene.h"

namespace m1
{
    class Bullet2
    {
    public:
        Bullet2(glm::vec3 pos = glm::vec3(0, 0, 0),
               glm::vec3 direction = glm::vec3(0, 0, 0),
               glm::vec3 size = glm::vec3(0.25f, 0.25f, 0.25f),
               glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.8f),
               float speed = 30);
        ~Bullet2();
        void Reset();

    public:
        glm::vec3 pos;
        glm::vec3 size;
        glm::vec3 color;
        glm::vec3 direction;
        float speed;
        bool available;             // variabila care spune daca pot sa impusc
        float lifeTimer;            // in secunde
        const float life = 1.0f;    // in secunde
    };
}
