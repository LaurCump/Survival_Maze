#pragma once

#include "components/simple_scene.h"
#include "bullet2.h"

namespace m1
{
    class Player2
    {
    public:
        Player2(glm::vec3 pos,
               glm::vec3 size = glm::vec3(0.5f, 1.0f, 0.15f),
               glm::vec3 color = glm::vec3(0.85, 0.75f, 0.6f),
               float moveSpeed = 6.0f,
               int ammo = 10);
        ~Player2();
        
        void UpdateBullets(float deltaTimeSeconds);

        void RotateFirstPerson_OY(float angle);
        void RotateFirstPerson_OZ(float angle);

        void OnKeyPress(int key, int mods, WindowObject *window);
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY, WindowObject* window, float sensitivity);
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods, WindowObject* window);

    public:
        glm::vec3 pos;
        glm::vec3 size;
        glm::vec3 color;
        float moveSpeed;
        float lookAngle;

        int ammo;
        const float cooldown = 0.5f;  // firing rate in secunde
        float cooldownTimer;
        std::vector<Bullet2*> bullets;

        glm::vec3 forward;
        glm::vec3 right;
        glm::vec3 up;

        int lifes;
        const int totalLifes = 3;

        bool enabled;
    };
}
