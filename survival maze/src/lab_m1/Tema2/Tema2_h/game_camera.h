#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

namespace game_camera
{
    class Camera
    {
     public:
        Camera(glm::vec3 playerPosition)
        {
            InitOtherVars();

            firstPersonPosition = glm::vec3(playerPosition.x, playerPosition.y + distanceToTarget / 2, playerPosition.z);
            firstPersonForward  = glm::vec3(0, 0, -1);
            firstPersonUp       = glm::vec3(0, 1, 0);
            firstPersonRight    = glm::vec3(1, 0, 0);

            thirdPersonPosition = glm::vec3(playerPosition.x, playerPosition.y + distanceToTarget / 2, playerPosition.z + distanceToTarget);
            thirdPersonForward  = glm::vec3(0, 0, -1);
            thirdPersonUp       = glm::vec3(0, 1, 0);
            thirdPersonRight    = glm::vec3(1, 0, 0);

        }

        ~Camera()
        { }

        void InitOtherVars()
        {
            thirdPersonView = true;
            enabled = true;
            projectionMatrix = glm::mat4(1);
            distanceToTarget = 2;
        }

        void MoveForward(float distance)
        {
            // Translates the camera using the `dir` vector computed from
            // `forward`. Movement will always keep the camera at the same
            // height. For example, if you rotate your head up/down, and then
            // walk forward, then you will still keep the same relative
            // distance (height) to the ground!
            glm::vec3 thirdPersonDir = glm::normalize(glm::vec3(thirdPersonForward.x, 0, thirdPersonForward.z));
            thirdPersonPosition += thirdPersonDir * distance;

            glm::vec3 firstPersonDir = glm::normalize(glm::vec3(firstPersonForward.x, 0, firstPersonForward.z));
            firstPersonPosition += firstPersonDir * distance;
        }

        void TranslateForward(float distance)
        {
            // Translate the camera using the `forward` vector.
            // What's the difference between `TranslateForward()` and
            // `MoveForward()`?
            thirdPersonPosition += glm::normalize(thirdPersonForward) * distance;
            firstPersonPosition += glm::normalize(firstPersonForward) * distance;
        }

        void TranslateUpward(float distance)
        {
            // Trebuie sa urcam indiferent daca privim in jos.
            thirdPersonPosition += glm::vec3(0, 1, 0) * distance;
            firstPersonPosition += glm::vec3(0, 1, 0) * distance;
        }

        void TranslateRight(float distance)
        {
            // See instructions below. Read the entire thing!
            // You need to translate the camera using the `right` vector.
            // Usually, however, translation using camera's `right` vector
            // is not very useful, because if the camera is rotated around the
            // `forward` vector, then the translation on the `right` direction
            // will have an undesired effect, more precisely, the camera will
            // get closer or farther from the ground. The solution is to
            // actually use the projected `right` vector (projected onto the
            // ground plane), which makes more sense because we will keep the
            // same distance from the ground plane.
            thirdPersonPosition += glm::normalize(firstPersonRight) * distance;
            firstPersonPosition += glm::normalize(thirdPersonRight) * distance;
        }

        void RotateFirstPerson_OX(float angle)
        {
            glm::vec4 tmp_vec4 = glm::vec4(firstPersonForward, 1.0);
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, firstPersonRight);
            tmp_vec4 = model * tmp_vec4;
            glm::vec3 newForward = glm::normalize(glm::vec3(tmp_vec4));
            glm::vec3 newUp = glm::cross(firstPersonRight, newForward);

            // Trebuie sa implementam o limita pentru a fi siguri ca nu putem privi in jos sau sus mai mult de 90 de grade.
            if (newUp.y >= 0) 
            {
                firstPersonForward = newForward;
                firstPersonUp = newUp;
            }
        }

        void RotateFirstPerson_OY(float angle)
        {
            // Compute the new `forward`, `up` and `right`
            // vectors. Use `glm::rotate()`. Don't forget to normalize the
            // vectors!
            glm::vec4 tmp_vec4 = glm::vec4(firstPersonForward, 1.0);
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
            tmp_vec4 = model * tmp_vec4;
            firstPersonForward = glm::normalize(glm::vec3(tmp_vec4));

            tmp_vec4 = glm::vec4(firstPersonRight, 1.0);
            model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
            tmp_vec4 = model * tmp_vec4;
            firstPersonRight = glm::normalize(glm::vec3(tmp_vec4));

            firstPersonUp = glm::cross(firstPersonRight, firstPersonForward);
        }

        void RotateThirdPerson_OX(float angle)
        {
            // Rotate the camera in third-person mode around
            // the OX axis. Use `distanceToTarget` as translation distance.
            thirdPersonPosition += glm::normalize(thirdPersonForward) * distanceToTarget;

            glm::vec4 tmp_vec4 = glm::vec4(thirdPersonForward, 1.0);
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, thirdPersonRight);
            tmp_vec4 = model * tmp_vec4;
            glm::vec3 newForward = glm::normalize(glm::vec3(tmp_vec4));
            glm::vec3 newUp = glm::cross(thirdPersonRight, newForward);

            // Trebuie sa implementam o limita pentru a fi siguri ca nu putem privi in jos sau sus mai mult de 90 de grade.
            if (newUp.y >= 0) 
            {
                thirdPersonForward = newForward;
                thirdPersonUp = newUp;
            }

            thirdPersonPosition -= glm::normalize(thirdPersonForward) * distanceToTarget;
        }

        void RotateThirdPerson_OY(float angle)
        {
            // Rotate the camera in third-person mode around
            // the OY axis.
            thirdPersonPosition += glm::normalize(thirdPersonForward) * distanceToTarget;

            glm::vec4 tmp_vec4 = glm::vec4(thirdPersonForward, 1.0);
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
            tmp_vec4 = model * tmp_vec4;
            thirdPersonForward = glm::normalize(glm::vec3(tmp_vec4));

            tmp_vec4 = glm::vec4(thirdPersonRight, 1.0);
            model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
            tmp_vec4 = model * tmp_vec4;
            thirdPersonRight = glm::normalize(glm::vec3(tmp_vec4));

            thirdPersonUp = glm::cross(thirdPersonRight, thirdPersonForward);

            thirdPersonPosition -= glm::normalize(thirdPersonForward) * distanceToTarget;
        }

        glm::mat4 GetViewMatrix()
        {
            // Returnam View Matrix
            if (thirdPersonView) 
            {
                return glm::lookAt(thirdPersonPosition, thirdPersonPosition + thirdPersonForward, thirdPersonUp);
            }
            else {
                return glm::lookAt(firstPersonPosition, firstPersonPosition + firstPersonForward, firstPersonUp);
            }
        }

        glm::vec3 GetPosition() 
        {
            return thirdPersonView ? thirdPersonPosition : firstPersonPosition;
        }

        glm::vec3 GetForward() 
        {
            return thirdPersonView ? thirdPersonForward : firstPersonForward;
        }

        glm::vec3 GetRight() 
        {
            return thirdPersonView ? thirdPersonRight : firstPersonRight;
        }

        glm::vec3 GetUp() 
        {
            return thirdPersonView ? thirdPersonUp : firstPersonUp;
        }


        void OnInputUpdate(float distance, int mods, WindowObject* window, bool devMode)
        {
            if (!enabled) return;

            if (window->KeyHold(GLFW_KEY_W))
            {
                // Translate the camera forward
                MoveForward(distance);
            }

            if (window->KeyHold(GLFW_KEY_A))
            {
                // Translate the camera to the left
                TranslateRight(-distance);
            }

            if (window->KeyHold(GLFW_KEY_S)) 
            {
                // Translate the camera backwards
                MoveForward(-distance);
            }

            if (window->KeyHold(GLFW_KEY_D)) 
            {
                // Translate the camera to the right
                TranslateRight(distance);
            }
        }

        void OnKeyPress(int key, int mods, WindowObject* window)
        {
            if (key == GLFW_KEY_F1)
            {
                window->ShowPointer();
                enabled = false;
            }
            if (key == GLFW_KEY_LEFT_CONTROL)
            {
                thirdPersonView = !thirdPersonView;
            }
        }

        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY, WindowObject* window, float sensitivity)
        {
            if (!enabled) return;

            RotateThirdPerson_OY(-(float)deltaX * sensitivity);
            RotateThirdPerson_OX(-(float)deltaY * sensitivity);
            RotateFirstPerson_OY(-(float)deltaX * sensitivity);
            RotateFirstPerson_OX(-(float)deltaY * sensitivity);
        }

        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods, WindowObject* window)
        {
            if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT))
            {
                thirdPersonView = !thirdPersonView;
            }
            if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT))
            {
                window->DisablePointer();
                enabled = true;
            }
        }

     public:
        float distanceToTarget;
        glm::mat4 projectionMatrix;
        bool thirdPersonView;

    protected:
        glm::vec3 thirdPersonPosition;
        glm::vec3 thirdPersonForward;  // vector normalizat ce contine directia curenta de privire inainte.
        glm::vec3 thirdPersonRight;    // vector normalizat ce contine directia curenta de privire dreapta.
        glm::vec3 thirdPersonUp;       // vector normalizat ce contine directia curenta de privire in sus.

        glm::vec3 firstPersonPosition;
        glm::vec3 firstPersonForward;  // vector normalizat ce contine directia curenta de privire inainte.
        glm::vec3 firstPersonRight;    // vector normalizat ce contine directia curenta de privire dreapta.
        glm::vec3 firstPersonUp;       // vector normalizat ce contine directia curenta de privire in sus.

    private:
        bool enabled;
    };
}
