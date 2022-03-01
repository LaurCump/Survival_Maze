#pragma once

#include "components/simple_scene.h"
#include "enemy2.h"
#include "wall2.h"

namespace m1
{
    class Level2
    {
    private:
        enum MazeState      // starile din labirint
        {
            AVAILABLE = 0,
            EMPTY = 1,
            WALL = 2
        };
        enum MazeDirections     // directiile din labirint privite de sus
        {  
            UP = 0,
            DOWN = 1,
            LEFT = 2,
            RIGHT = 3
        };

    public:
        Level2();
        ~Level2();
        void CreateMaze();
        void CreateEnemies();   // harta trebuie sa existe pentru ca aceasta sa ruleze corect

        void UpdateEnemies(float deltaTimeSeconds);
        void PrintMaze();
        void GenerateMaze(glm::vec2 currentCell);
        glm::vec3 GetStartPosition();
        glm::vec3 GetCellWorldPosition(int line, int column);

    private:
        Wall2* Level2::GetFinishHitbox();

    public:
        unsigned int mapSize;

        int **maze;
        glm::vec2 startCell;
        glm::vec2 finishCell;
        Wall2* finishHitbox;

        std::vector<Wall2*> walls;
        const glm::vec3 wallSize{ glm::vec3(5.0f, 5.0f, 5.0f) };

        std::vector<Enemy2*> enemies;

        float timer;  // in secunde
        const float solveTime = 60;  // in secunde
    };
}
