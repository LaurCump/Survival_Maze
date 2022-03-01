#pragma once

#include "utils/glm_utils.h"

namespace transform2D
{
    // matricea de translatie
    inline glm::mat3 Translate(float translateX, float translateY)
    {
        return glm::transpose( glm::mat3(
                1, 0, translateX,
                0, 1, translateY,
                0, 0, 1
            )
        );
    }

    // matricea de scalare
    inline glm::mat3 Scale(float scaleX, float scaleY)
    {
        return glm::mat3(
            scaleX, 0, 0,
            0, scaleY, 0,
            0, 0, 1
        );
    }

    // matricea de rotatie
    inline glm::mat3 Rotate(float radians)
    {
        float cosU{ glm::cos(radians) };
        float sinU{ glm::sin(radians) };

        return glm::transpose( glm::mat3(
                cosU, -sinU, 0,
                sinU, cosU, 0,
                0, 0, 1
            )
        );
    }
}
