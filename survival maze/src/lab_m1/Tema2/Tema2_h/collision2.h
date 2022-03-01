#pragma once

#include "components/simple_scene.h"

namespace Collision
{
    bool AABBAABB(glm::vec3 aMinPos, glm::vec3 aMaxPos, glm::vec3 bMinPos, glm::vec3 bMaxPos)
    {
        return (aMinPos.x <= bMaxPos.x && aMaxPos.x >= bMinPos.x) && (aMinPos.y <= bMaxPos.y && aMaxPos.y >= bMinPos.y) && (aMinPos.z <= bMaxPos.z && aMaxPos.z >= bMinPos.z);
    }

    bool SphereAABBB(glm::vec3 spherePos, float sphereRadius, glm::vec3 boxMinPos, glm::vec3 boxMaxPos)
    {
        // obtinem corpul cel mai apropiat de centrul sferei
        float x = glm::max(boxMinPos.x, glm::min(spherePos.x, boxMaxPos.x));
        float y = glm::max(boxMinPos.y, glm::min(spherePos.y, boxMaxPos.y));
        float z = glm::max(boxMinPos.z, glm::min(spherePos.z, boxMaxPos.z));

        // calculam distanta dintre corpuri
        float distance = glm::sqrt((x - spherePos.x) * (x - spherePos.x) + (y - spherePos.y) * (y - spherePos.y) + (z - spherePos.z) * (z - spherePos.z));

        return distance < sphereRadius;
    }

    bool SphereSphere(glm::vec3 spherePos, float sphereRadius, glm::vec3 otherPos, float otherRadius)
    {
        float distance = glm::sqrt((spherePos.x - otherPos.x) * (spherePos.x - otherPos.x) + (spherePos.y - otherPos.y) * (spherePos.y - otherPos.y) + (spherePos.z - otherPos.z) * (spherePos.z - otherPos.z));
        
        return distance < (sphereRadius + otherRadius);
    }
}
