#pragma once

#include <vector>
#include "components/simple_scene.h"
#include "game_camera.h"
#include "level2.h"
#include "player2.h"
#include "drawings2.h"

namespace m1
{
    class Game2 : public gfxc::SimpleScene
    {
     public:
        Game2();
        ~Game2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color, const bool mustDeform = false);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void CalculatePlayerAngle();
        void UpdateTimer(float deltaTimeSeconds);
        void DetectTriggerCollisions(float deltaTimeSeconds);

        void ShootBullet();

        void RenderMaze();
        void RenderFloor();

        void RenderPlayer();
        void RenderPlayerLeftLeg();
        void RenderPlayerRightLeg();
        void RenderPlayerLeftArm();
        void RenderPlayerRightArm();
        void RenderPlayerTorso();
        void RenderPlayerHead();

        void RenderBullets();
        void RenderEnemies();
        void RenderLife();
        void RenderTimer();


    protected:
        game_camera::Camera* camera;
        Level2* level;
        Player2* player;
        bool devMode;
    };
}
