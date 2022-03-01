#include "lab_m1/Tema2/Tema2_h/level2.h"
#include <iostream>
#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace m1;

Level2::Level2()
{
    mapSize = 10;

    maze = new int*[mapSize];
    for (int i = 0; i < mapSize; i++) 
    {
        maze[i] = new int[mapSize];
        for (int j = 0; j < mapSize; j++)
        {
            if (i == 0 || i == mapSize - 1 || j == 0 || j == mapSize - 1)
            {  // daca suntem pe marginea hartii matricei, vom pune un perete
                maze[i][j] = MazeState::WALL;
            }
            else    // celula disponibila pentru a determina daca va fi perete sau gol
            {
                maze[i][j] = MazeState::AVAILABLE;
            }
        }
    }

    startCell = glm::vec2(0);
    CreateMaze();
    CreateEnemies();  // harta trebuie sa existe ca aceasta functie sa mearga corect

    timer = solveTime;
}

Level2::~Level2()
{
    for each (Wall2 *w in walls)
    {
        delete w;
    }
    walls.clear();

    for each (Enemy2 *e in enemies)
    {
        delete e;
    }
    enemies.clear();

    for (int i = 0; i < mapSize; ++i)
    {
        delete[] maze[i];
    }
    delete[] maze;
    delete finishHitbox;
}

// creare labirint
void Level2::CreateMaze()
{
    // punctul de start nu poate fi pe marginea labirintului
    startCell.x = 1 + rand() % (mapSize - 2);
    startCell.y = 1 + rand() % (mapSize - 2);

    finishCell = startCell;  // initial ultima celula este celula de start

    GenerateMaze(startCell);    // generam labirintul folosind un algoritm de backtracking

    // trebuie sa lasam sa existe o iesire din labirint
    maze[(int)finishCell.x][(int)finishCell.y] = MazeState::EMPTY;

    // pot exista celule inaccesibile (pot fi observate prin avand starea AVAILABLE)  

    glm::vec3 wallColor{ glm::vec3(0.0f, 0.9f, 0.0f) };
    glm::vec3 wallPos;
    glm::vec3 firstWallPos{ glm::vec3(0, wallSize.y / 2.0f, 0) };

    for (int i = 0; i < mapSize; ++i) 
    {
        for (int j = 0; j < mapSize; ++j)
        {
            if (maze[i][j] == MazeState::WALL) 
            {
                wallPos = firstWallPos;
                wallPos.x += wallSize.x * i;
                wallPos.z += wallSize.z * j;
                walls.push_back(new Wall2(wallPos, wallSize, wallColor));
            }
        }
    }

    finishHitbox = GetFinishHitbox();
}

// cream inamicii
void Level2::CreateEnemies() 
{
    int spawnChance = 22;  // in procente(%)

    for (int i = 0; i < mapSize; ++i) 
    {
        for (int j = 0; j < mapSize; ++j) 
        {
            if (maze[i][j] == MazeState::EMPTY && startCell != glm::vec2(i, j))
            {
                int extractChance = rand() % 101;
                if (extractChance < spawnChance)
                {
                    glm::vec3 ePos{ GetCellWorldPosition(i, j) };
                    Enemy2* e = new Enemy2(ePos);
                    e->pos.y = e->size.y;
                    enemies.push_back(e);
                }
            }
        }
    }
}

// actualizam actiunile inamiciilor in functie daca sunt impuscati sau pana sa fie impuscati
void Level2::UpdateEnemies(float deltaTimeSeconds)
{
    for each (Enemy2 *e in enemies)
    {
        if (!e->isAlive) continue;
        if (e->mustDeform)
        {   // inamicul a fost impuscat, deci acesta va avea o deformare pana va muri.
            if (e->mustDeformTimer <= 0)
            {
                e->mustDeform = false;
                e->isAlive = false;
            }
            e->mustDeformTimer = glm::max(e->mustDeformTimer - deltaTimeSeconds, 0.0f);
        }
        else 
        {  // altfel doar il mutam, deoarece inamicul traieste
           // se misca in linie dreapta
            e->pos.x += e->direction * deltaTimeSeconds * e->moveSpeed;
        }
    }
}

// afisare matrice labirint (am folosit-o doar pentru debug)
// nu am folosit functia in game2.cpp
void Level2::PrintMaze() 
{
    std::cout << '\n'; 
    for (int i = 0; i < mapSize; ++i) 
    {
        for (int j = 0; j < mapSize; ++j) 
        {
            std::cout << maze[i][j] << ' ';
        }
        std::cout << '\n';
    } 
    std::cout << '\n'; 
}

// generare labirint la fiecare incarcare a jocului
void Level2::GenerateMaze(glm::vec2 currentCell)
{
    // returnam daca am ales o celula ce are un perete
    if (maze[(int)currentCell.x][(int)currentCell.y] == MazeState::WALL) 
    {
        // incercam sa facem acest perete sa fie o celula de final
        if (currentCell.x == 0 || currentCell.x == mapSize - 1 || currentCell.y == 0 || currentCell.y == mapSize - 1)   // daca suntem pe un perete care e pe marginea labirintului (poate exista iesirea)
        {  
            float currentDistance = glm::abs(finishCell.x - startCell.x) + glm::abs(finishCell.y - startCell.y);
            float newDistance = glm::abs(currentCell.x - startCell.x) + glm::abs(currentCell.y - startCell.y);
            // sa nu se confunde cu drumul cel mai lung!
            // noua distanta nu este neaparat cel mai lung drum

            if (newDistance > currentDistance)   // daca distanta este mai departe de celula de start decat de cea precedenta de final
            {  
                finishCell = currentCell;
            }
        }

        return;
    }

    // returnam daca am ales o celula goala
    if (maze[(int)currentCell.x][(int)currentCell.y] == MazeState::EMPTY) 
        return;

    if (currentCell.x == 0 || currentCell.x == mapSize - 1 || currentCell.y == 0 || currentCell.y == mapSize - 1)   // daca suntem pe marginea labirintului
    { 
        // setam celula la starea WALL
        maze[(int)currentCell.x][(int)currentCell.y] = MazeState::WALL;
     
        return;
    }


    std::vector<MazeDirections> availableDirectionsPool{
        MazeDirections::UP,
        MazeDirections::DOWN,
        MazeDirections::LEFT,
        MazeDirections::RIGHT
    };

    std::vector<glm::vec2> nextCells;

    for (int i = 0; i < 4; ++i)     // pentru fiecare directie posibila (sus, jos, stanga, dreapta)
    { 
        glm::vec2 nextCell;
        int randomNum;
        if (i == 3)     // daca mai exista doar o directie ramasa in pool
        {  
            randomNum = 0;  // aceasta este directia ramasa 
        }
        else    // alegem un numar random 
        {
            randomNum = rand() % availableDirectionsPool.size();
        }

        // salvam urmatoarea directie
        MazeDirections nextDir = availableDirectionsPool.at(randomNum);

        // generam urmatoarea pozitie bazata pe urmatoarea directie 
        if (nextDir == MazeDirections::UP)  // in sus
        {
            nextCell = glm::vec2(currentCell.x - 1, currentCell.y);
        }
        else if (nextDir == MazeDirections::DOWN)   // in jos
        {
            nextCell = glm::vec2(currentCell.x + 1, currentCell.y);
        }
        else if (nextDir == MazeDirections::LEFT)   // in stanga
        {
            nextCell = glm::vec2(currentCell.x, currentCell.y - 1);
        }
        else   // directia dreapta
        {
            nextCell = glm::vec2(currentCell.x, currentCell.y + 1);
        }

        // adaugam in vector urmatoarea celula
        nextCells.push_back(nextCell);

        // stergem directia din pool-ul de directii 
        availableDirectionsPool.erase(availableDirectionsPool.begin() + randomNum);
    }

    // mai departe verificam care va fi celula curenta in functie de cate celule vecine goale are
    int emptyNeighbours = 0;
    for each (glm::vec2 nextCell in nextCells)
    {
        if (maze[(int)nextCell.x][(int)nextCell.y] == MazeState::EMPTY)
        {
            ++emptyNeighbours;
        }
    }
    if (emptyNeighbours < 2)
    {
        // daca exista doar o celula vecina goala atunci mergem pe aceea si este un drum valid
        // daca nu exista nicio celula vecina goala, incepem una noua goala si va fi un drum valid
        maze[(int)currentCell.x][(int)currentCell.y] = MazeState::EMPTY;
    }
    else  // altfel suntem intr-o celula care este un perete separator intre drumuri
    {
        maze[(int)currentCell.x][(int)currentCell.y] = MazeState::WALL;
        return;  // drum inchis, deci nu e nevoie sa continuam pe acolo
    }
    
    // continuam urmatoarele cai posibile din celula curenta
    for each (glm::vec2 nextCell in nextCells) 
    {
        GenerateMaze(nextCell);  // in cele din urma vom verifica fiecare directie
    }
}

glm::vec3 Level2::GetStartPosition()
{   
   // pozitionarea initiala se face respectand conditia:
   // - celula spre care arata sageata va fi la coordonatele (0, 0, 0), iar celelalte celule vor fi construite pe baza acesteia
   // |
   // v
   // 0  0  0  0
   // 0  0  0  0
   // 0  0  0  0
   // 0  0  0  0
    return GetCellWorldPosition(startCell.x, startCell.y);
}

glm::vec3 Level2::GetCellWorldPosition(int line, int column)
{
    // - celula spre care arata sageata va fi la coordonatele (0, 0, 0), iar celelalte celule vor fi construite pe baza acesteia
    // |
    // v
    // 0  0  0  0
    // 0  0  0  0
    // 0  0  0  0
    // 0  0  0  0
    Wall2* firstWall = walls.at(0);
    glm::vec3 startWorldPosition = firstWall->pos;

    Wall2* w = walls.at(line + column);
    glm::vec3 cellWorldPos{ startWorldPosition };

    cellWorldPos.x += w->size.x * line;
    cellWorldPos.y = 0;
    cellWorldPos.z += w->size.z * column;

    return cellWorldPos;
}

// obtinerea primei celule din afara labirintului la gasirea iesirii
Wall2* Level2::GetFinishHitbox()
{
    // extragem ultima celula din labirint inainte ca jucatorul sa iasa 
    int i = finishCell.x;
    int j = finishCell.y;
    // in functie de unde se afla iesirea, luam prima celula gasita din afara labirintului 
    if (i == 0)
    {
        --i;
    }
    else if (i == (int)mapSize - 1) 
    {
        ++i;
    }
    else if (j == 0) 
    {
        --j;
    }
    else if (j == (int)mapSize - 1)
    {
        ++j;
    }
    
    return new Wall2(GetCellWorldPosition(i, j), wallSize);
}
