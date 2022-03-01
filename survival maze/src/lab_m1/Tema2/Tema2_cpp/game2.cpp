#include "lab_m1/Tema2/Tema2_h/game2.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/Tema2/Tema2_h/collision2.h"
#include "lab_m1/Tema2/Tema2_h/transform2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Game2::Game2()
{
    devMode = false;
}

Game2::~Game2()
{
    delete level;
}

void Game2::Init()
{
    window->DisablePointer();

    level = new Level2();
    player = new Player2(level->GetStartPosition());
    player->pos.y += player->size.y / 2.0f;  // jucatorul va sta pe platforma

    camera = new game_camera::Camera(player->pos);
    camera->projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    GetCameraInput()->SetActive(false);
    
    player->forward = camera->GetForward();
    player->right = camera->GetRight();
    player->up = camera->GetUp();

    // Create meshes
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;

        mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;

        mesh = new Mesh("square");
        mesh = drawings2::CreateSquare("square", glm::vec3(0), 1, glm::vec3(0, 1, 0), true);
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader *shader = new Shader("LabShader");
        std::string GAME_SOURCE_PATH = PATH_JOIN("src", "lab_m1", "Tema2");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
}

void Game2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0.45f, 0.6f, 0.75f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Game2::Update(float deltaTimeSeconds)
{
    CalculatePlayerAngle();
    player->UpdateBullets(deltaTimeSeconds);
    level->UpdateEnemies(deltaTimeSeconds);
    DetectTriggerCollisions(deltaTimeSeconds);
    UpdateTimer(deltaTimeSeconds);
}

void Game2::FrameEnd()
{

    RenderFloor();
    RenderMaze();
    RenderPlayer();
    RenderBullets();
    RenderEnemies();

    // Setup pentru 2D
    glClear(GL_DEPTH_BUFFER_BIT);  // Curatam depth buffer-ul pentru a putea desena 2D
    RenderLife();
    RenderTimer();

}

void Game2::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color, const bool mustDeform)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Get shader location for uniform mat4 "Model"
    int location = glGetUniformLocation(shader->GetProgramID(), "Model");

    // Set shader uniform "Model" to modelMatrix
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Get shader location for uniform mat4 "View"
    location = glGetUniformLocation(shader->GetProgramID(), "View");

    // Set shader uniform "View" to viewMatrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Get shader location for uniform mat4 "Projection"
    location = glGetUniformLocation(shader->GetProgramID(), "Projection");

    // Set shader uniform "Projection" to projectionMatrix
    glm::mat4 projectionMatrix = camera->projectionMatrix;
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Get shader location for uniform vec3 "Color"
    location = glGetUniformLocation(shader->GetProgramID(), "Color");

    // Set shader uniform "Color" to color
    glUniform3fv(location, 1, glm::value_ptr(color));

    // Get shader location for uniform float "ElapsedTime"
    location = glGetUniformLocation(shader->GetProgramID(), "ElapsedTime");
    GLfloat elapsedTime{ static_cast<GLfloat>(Engine::GetElapsedTime()) };

    // Set shader uniform "ElapsedTime" to elapsedTime
    glUniform1f(location, elapsedTime);

    // Get shader location for uniform bool "MustDeform"
    location = glGetUniformLocation(shader->GetProgramID(), "MustDeform");

    // Set shader uniform "MustDeform" to mustDeform
    glUniform1i(location, mustDeform);


    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Game2::OnInputUpdate(float deltaTime, int mods)
{
    // Add key press event
    float cameraSpeed = player->moveSpeed;
    if (window->KeyHold(GLFW_KEY_LEFT_SHIFT) && devMode)
    {
        cameraSpeed *= 10;
    }

    bool hasMoved = false;
    bool noFutureCollision = true;
    if (player->enabled)
    {
        glm::vec3 movement{ glm::vec3(0) };
        if (window->KeyHold(GLFW_KEY_W))
        {
            // Translatam jucatorul in fata 
            glm::vec3 dir = glm::normalize(glm::vec3(player->forward.x, 0, player->forward.z));
            movement += dir * deltaTime * cameraSpeed;
        }

        if (window->KeyHold(GLFW_KEY_A)) 
        {
            // Translatam jucatorul spre stanga
            movement += glm::normalize(player->right) * -deltaTime * cameraSpeed;
        }

        if (window->KeyHold(GLFW_KEY_S)) 
        {
            // Translatam jucatorul in spate
            glm::vec3 dir = glm::normalize(glm::vec3(player->forward.x, 0, player->forward.z));
            movement += -dir * deltaTime * cameraSpeed;
        }

        if (window->KeyHold(GLFW_KEY_D)) 
        {
            // Translatam jucatorul spre dreapta
            movement += glm::normalize(player->right) * deltaTime * cameraSpeed;;
        }

        // De asemenea prezicem coliziunile aici pentru a preveni tremuratul (mutand jucatorul in interiorul peretului apoi fortandu-l sa iasa)
        noFutureCollision = true;   // vrem un cub sa fie ca hitbox ->v<- deci vom folosi size.x deoarece z este cel mai mic.
        glm::vec3 box1MinPos{ player->pos + movement - glm::vec3(player->size.x) / 2.0f };
        glm::vec3 box1MaxPos{ player->pos + movement + glm::vec3(player->size.x) / 2.0f };

        for each (Wall2 *w in level->walls) 
        {
            glm::vec3 box2MinPos{ w->pos - w->size / 2.0f };
            glm::vec3 box2MaxPos{ w->pos + w->size / 2.0f };
            if (Collision::AABBAABB(box1MinPos, box1MaxPos, box2MinPos, box2MaxPos)) 
            {
                noFutureCollision = false;
                break;
            }
        }

        hasMoved = movement != glm::vec3(0);
        if (noFutureCollision && hasMoved) 
        {
            player->pos += movement;
        }
    }

    if (noFutureCollision && hasMoved) 
    {
        camera->OnInputUpdate(deltaTime * cameraSpeed, mods, window, devMode);
    }
}

void Game2::OnKeyPress(int key, int mods)
{
    camera->OnKeyPress(key, mods, window);
    player->OnKeyPress(key, mods, window);
    if (key == GLFW_KEY_F2)
    {
        devMode = !devMode;
    }
    if (key == GLFW_KEY_SPACE && !camera->thirdPersonView)
    {
        ShootBullet();
    }
}

void Game2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}

void Game2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    float sensitivity = 0.001f;
    camera->OnMouseMove(mouseX, mouseY, deltaX, deltaY, window, sensitivity);
    player->OnMouseMove(mouseX, mouseY, deltaX, deltaY, window, sensitivity);
}

void Game2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    camera->OnMouseBtnPress(mouseX, mouseY, button, mods, window);
    player->OnMouseBtnPress(mouseX, mouseY, button, mods, window);

    if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT) && !camera->thirdPersonView)
    {
        ShootBullet();
    }
}

void Game2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Game2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Game2::OnWindowResize(int width, int height)
{
}


// Calculul unghiului jucatorului.
void Game2::CalculatePlayerAngle()
{
    glm::vec3 pfwd{ glm::vec3(player->forward.x, 0, player->forward.z) };
    player->lookAngle = glm::acos(glm::dot(pfwd, glm::vec3(0, 0, -1)));

    if (pfwd.x > 0)     // pentru gama completa de miscare
    {  
        player->lookAngle *= -1;
    }
}

// Actualizare timer.
void Game2::UpdateTimer(float deltaTimeSeconds)
{
    if (level->timer <= 0)
    {
        std::cout << "\nGame Over! Out of time!\n\n";
        window->Close();
    }
    level->timer = glm::max(level->timer - deltaTimeSeconds, 0.0f);
}

// Detectare coliziuni
void Game2::DetectTriggerCollisions(float deltaTimeSeconds)
{
    for each (Wall2 * w in level->walls)
    {
        glm::vec3 boxMinPos{ w->pos - w->size / 2.0f };
        glm::vec3 boxMaxPos{ w->pos + w->size / 2.0f };
        for each (Bullet2 *b in player->bullets)
        {
            if (b->available) 
                continue;  // daca glontul este disponibil sa fie tras


            if (Collision::SphereAABBB(b->pos, b->size.x, boxMinPos, boxMaxPos)) 
            {
                // Glontul a lovit peretele.
                // Resetam glontul.
                b->Reset();
            }
        }
        for each (Enemy2 *e in level->enemies)
        {
            if (!e->isAlive) continue;
            if (Collision::SphereAABBB(e->pos, e->size.x, boxMinPos, boxMaxPos))
            {
                // Inamicul a lovit peretele.
                // Schimbam directia lui de deplasare in sens invers.
                e->direction *= -1;

                // Deci daca inamicul loveste coltul, nu va fi declansat cu alt perete.
                e->pos += e->direction * deltaTimeSeconds * e->moveSpeed;
            }
        }
    }

    for each (Enemy2 *e in level->enemies)
    {
        if (!e->isAlive) continue;
        for each (Bullet2 *b in player->bullets)
        {
            if (b->available) 
                continue;  // daca glontul este disponibil sa fie tras

            if (Collision::SphereSphere(e->pos, e->size.x, b->pos, b->size.x)) 
            {
                // Glontul a lovit inamicul.
                // Acesta va muri dupa animatia cu deformarea caracterului
                e->mustDeform = true;

                // Resetam glontul.
                b->Reset();
            }
        }

        glm::vec3 boxMinPos{ player->pos - player->size / 2.0f };
        glm::vec3 boxMaxPos{ player->pos + player->size / 2.0f };

        if (Collision::SphereAABBB(e->pos, e->size.x, boxMinPos, boxMaxPos))
        {
            // Inamicul loveste jucatorul.
            // Inamicul moare.
            e->isAlive = false;

            // Scade viata jucatorului dupa coliziunea cu inamicul.
            player->lifes -= 1;
            std::cout << "Lifes = " << player->lifes << '\n';

            if (player->lifes == 0)
            {   
                // Daca jucatorul nu mai are vieti
                std::cout << "\nGame Over! Out of lifes!\n\n";
                // Se inchide fereastra jocului.
                window->Close();
            }
        }
        if (!e->isAlive || e->mustDeform) 
            continue;

        Wall2* w = level->finishHitbox;
        boxMinPos = w->pos - w->size / 2.0f;
        boxMaxPos = w->pos + w->size / 2.0f;

        if (Collision::SphereAABBB(e->pos, e->size.x, boxMinPos, boxMaxPos))
        {
            // Inamicul a incercat sa scape din labirint.
            // Ii schimbam directia de deplasare.
            e->direction *= -1;
            e->pos.x += e->direction * deltaTimeSeconds * e->moveSpeed;
        }
    }

    // Verificam daca jucatorul a iesit din labirint.
    {
        Wall2* w = level->finishHitbox;
        glm::vec3 box1MinPos{ player->pos - player->size / 2.0f };
        glm::vec3 box1MaxPos{ player->pos + player->size / 2.0f };
        glm::vec3 box2MinPos{ w->pos - w->size / 2.0f };
        glm::vec3 box2MaxPos{ w->pos + w->size / 2.0f };

        if (Collision::AABBAABB(box1MinPos, box1MaxPos, box2MinPos, box2MaxPos))
        {
            std::cout << "\nGame Over! You escaped the maze!\n\n";
            // Se inchide fereastra jocului.
            window->Close();
        }
    }
}

// Lansarea gloantelor.
void Game2::ShootBullet()
{
    if (player->cooldownTimer == 0)  // poate trage deoarece timpul de racire a trecut
    {  
        bool foundBullet = false;
        for each (Bullet2 *b in player->bullets)
        {
            if (b->available)   // am gasit un glont care poate fi lansat
            {  
                b->pos = player->pos;
                b->direction = glm::normalize(camera->GetForward());
                b->available = false;
                b->lifeTimer = b->life;
                foundBullet = true;
                break;
            }
        }
        if (!foundBullet)   // nu am gasit un glont care poate fi lansat
        {  
            Bullet2* b = new Bullet2(player->pos, glm::normalize(camera->GetForward()));
            b->available = false;
            player->bullets.push_back(b);
        }
        // Resetam timpul de cooldown (controleaza firing rate-ul).
        player->cooldownTimer = player->cooldown;
    }
}

// Desenam labirintul.
void Game2::RenderMaze()
{
    glm::mat4 modelMatrix;

    for each (Wall2 *w in level->walls)
    {
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, w->pos);
        modelMatrix = glm::scale(modelMatrix, w->size);
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, w->color);
    }
}

// Desenam podeaua labirintului
void Game2::RenderFloor()
{
    glm::mat4 modelMatrix;
    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(50, -0.06f, 50));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(200, 0.1f, 200));
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
}

// Desenam jucatorul.
void Game2::RenderPlayer()
{
    if (!camera->thirdPersonView)   // daca suntem in modul de first person, nu e nevoie sa desenam jucatorul.
        return;  

    RenderPlayerLeftLeg();
    RenderPlayerRightLeg();
    RenderPlayerTorso();
    RenderPlayerHead();
    RenderPlayerLeftArm();
    RenderPlayerRightArm();
}

// Desenam piciorul stang al jucatorului.
void Game2::RenderPlayerLeftLeg()
{
    glm::mat4 modelMatrix;

    glm::vec3 legColor = glm::vec3(0, 0, 0.8f);

    glm::vec3 legSize{ player->size };
    legSize.x *= 0.3f;
    legSize.y *= 0.4f;

    glm::vec3 legPos{ player->pos };
    legPos.x += legSize.x / 2.0f - player->size.x / 3.0f;
    legPos.y = legSize.y / 2.0f;

    glm::vec3 rotationPoint{ player->pos - legPos };

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, legPos);
    modelMatrix = glm::translate(modelMatrix, rotationPoint);
    modelMatrix = glm::rotate(modelMatrix, player->lookAngle, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, -rotationPoint);
    modelMatrix = glm::scale(modelMatrix, legSize);
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, legColor);
}

// Desenam piciorul drept al jucatorului.
void Game2::RenderPlayerRightLeg()
{
    glm::mat4 modelMatrix;

    glm::vec3 legColor = glm::vec3(0, 0, 0.8f);

    glm::vec3 legSize{ player->size };
    legSize.x *= 0.3f;
    legSize.y *= 0.4f;

    glm::vec3 legPos{ player->pos };
    legPos.x += -legSize.x / 2.0f + player->size.x / 3.0f;
    legPos.y = legSize.y / 2.0f;

    glm::vec3 rotationPoint{ player->pos - legPos };

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, legPos);
    modelMatrix = glm::translate(modelMatrix, rotationPoint);
    modelMatrix = glm::rotate(modelMatrix, player->lookAngle, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, -rotationPoint);
    modelMatrix = glm::scale(modelMatrix, legSize);
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, legColor);
}

// Desenam bratul stang al jucatorului.
void Game2::RenderPlayerLeftArm()
{
    glm::mat4 modelMatrix;

    glm::vec3 armColor = glm::vec3(0, 0.8f, 0.55f);

    glm::vec3 armSize{ player->size };
    armSize.x *= 0.15f;
    armSize.y *= 0.25f;

    glm::vec3 armPos{ player->pos };
    armPos.x += -armSize.x / 1.6f - player->size.x / 3.0f;
    armPos.y += player->size.y * 0.2f;

    glm::vec3 rotationPoint{ player->pos - armPos };

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, armPos);
    modelMatrix = glm::translate(modelMatrix, rotationPoint);
    modelMatrix = glm::rotate(modelMatrix, player->lookAngle, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, -rotationPoint);
    modelMatrix = glm::scale(modelMatrix, armSize);
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, armColor);


    glm::vec3 wristSize{ player->size };
    wristSize.x *= 0.15f;
    wristSize.y *= 0.15f;

    glm::vec3 wristPos{ armPos };
    wristPos.y += -wristSize.y / 2.0f - armSize.y / 2.0f;

    rotationPoint = player->pos - wristPos;

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, wristPos);
    modelMatrix = glm::translate(modelMatrix, rotationPoint);
    modelMatrix = glm::rotate(modelMatrix, player->lookAngle, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, -rotationPoint);
    modelMatrix = glm::scale(modelMatrix, wristSize);
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, player->color);
}

// Desenam bratul drept al jucatorului.
void Game2::RenderPlayerRightArm()
{
    glm::mat4 modelMatrix;

    glm::vec3 armColor = glm::vec3(0, 0.8f, 0.55f);

    glm::vec3 armSize{ player->size };
    armSize.x *= 0.15f;
    armSize.y *= 0.25f;

    glm::vec3 armPos{ player->pos };
    armPos.x += armSize.x / 1.6f + player->size.x / 3.0f;
    armPos.y += player->size.y * 0.2f;

    glm::vec3 rotationPoint{ player->pos - armPos };

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, armPos);
    modelMatrix = glm::translate(modelMatrix, rotationPoint);
    modelMatrix = glm::rotate(modelMatrix, player->lookAngle, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, -rotationPoint);
    modelMatrix = glm::scale(modelMatrix, armSize);
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, armColor);


    glm::vec3 wristSize{ player->size };
    wristSize.x *= 0.15f;
    wristSize.y *= 0.15f;

    glm::vec3 wristPos{ armPos };
    wristPos.y += -wristSize.y / 2.0f - armSize.y / 2.0f;

    rotationPoint = player->pos - wristPos;

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, wristPos);
    modelMatrix = glm::translate(modelMatrix, rotationPoint);
    modelMatrix = glm::rotate(modelMatrix, player->lookAngle, glm::vec3(0, 1, 0));
    modelMatrix = glm::translate(modelMatrix, -rotationPoint);
    modelMatrix = glm::scale(modelMatrix, wristSize);
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, player->color);
}

// Desenam trunchiul jucatorului.
void Game2::RenderPlayerTorso()
{
    glm::mat4 modelMatrix;

    glm::vec3 bodyColor{ glm::vec3(0, 0.8f, 0.55f) };

    glm::vec3 bodySize{ player->size };
    bodySize.x *= 0.675f;
    bodySize.y *= 0.4f;

    glm::vec3 bodyPos{ player->pos };
    bodyPos.y += player->size.y * 0.13f;

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, bodyPos);
    modelMatrix = glm::rotate(modelMatrix, player->lookAngle, glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, bodySize);
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, bodyColor);
}

// Desenam capul jucatorului.
void Game2::RenderPlayerHead()
{
    glm::mat4 modelMatrix;

    glm::vec3 headColor = player->color;

    glm::vec3 headSize{ player->size };
    headSize.x *= 0.32f;
    headSize.y *= 0.2f;

    glm::vec3 headPos{ player->pos };
    headPos.y += player->size.y * 0.45f;

    modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, headPos);
    modelMatrix = glm::rotate(modelMatrix, player->lookAngle, glm::vec3(0, 1, 0));
    modelMatrix = glm::scale(modelMatrix, headSize);
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, headColor);
}

// Desenam gloantele jucatorului.
void Game2::RenderBullets()
{
    glm::mat4 modelMatrix;

    for each (Bullet2 *b in player->bullets)
    {
        if (b->available) 
            continue;

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, b->pos);
        modelMatrix = glm::scale(modelMatrix, b->size);
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, b->color);
    }
}

// Desenam inamicii.
void Game2::RenderEnemies()
{
    glm::mat4 modelMatrix;

    for each (Enemy2 *e in level->enemies)
    {
        if (e->isAlive) 
        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, e->pos);
            modelMatrix = glm::scale(modelMatrix, e->size);
            RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, e->color, e->mustDeform);
        }
    }
}

// Desenam bara de viata a jucatorului.
void Game2::RenderLife()
{
    glm::mat3 modelMatrix;
    glm::vec3 color{ glm::vec3(1, 0, 0) };
    glm::vec2 size{ glm::vec2(0.15f, 0.15f) };

    // Desenam fiecare patrat rosu individual ce reprezinta o viata.
    for (int i = 0; i < player->lifes; ++i)
    {
        modelMatrix = glm::mat3(1);
        glm::vec2 pos = glm::vec2(i * size.x, 0);
        pos.x += 2.5f;
        pos.y -= 0.76f;
        modelMatrix *= transform2D::Translate(pos.x, pos.y);
        modelMatrix *= transform2D::Scale(size.x, size.y);
        RenderMesh2D(meshes["square"], modelMatrix, color);
    }

    // Desenam marginea neagra pentru bara de viata.
    for (int i = 0; i < player->totalLifes; ++i) 
    {
       modelMatrix = glm::mat3(1);
        glm::vec2 pos = glm::vec2(i * size.x, 0);
        pos.x += 2.5f;
        pos.y -= 0.76f;
        modelMatrix *= transform2D::Translate(pos.x, pos.y);
        modelMatrix *= transform2D::Scale(size.x, size.y);
        RenderMesh2D(meshes["square"], modelMatrix, glm::vec3(0));
    }
}

// Desenam timpul in labirint in care jucatorul trebuie sa gaseasca iesirea
void Game2::RenderTimer()
{
    glm::mat3 modelMatrix;
    glm::vec2 pos = glm::vec2(-2.0, 2.2);
    glm::vec3 color{ glm::vec3(1, 1, 1) };
    glm::vec2 size{ glm::vec2(4.0f, 0.015f) };

    size.x = size.x * level->timer / level->solveTime;

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(pos.x, pos.y);
    modelMatrix *= transform2D::Scale(size.x, size.y);
    RenderMesh2D(meshes["square"], modelMatrix, color);

    // Desenam marginile pentru timer
    size = glm::vec2(4.0f, 0.015f);
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(pos.x, pos.y);
    modelMatrix *= transform2D::Scale(size.x, size.y);
    RenderMesh2D(meshes["square"], modelMatrix, glm::vec3(0.5f));
}
