#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <game_level.h>

#include <iostream>


enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_LOSS
};

class Game
{
public:
    GameState               State;	
    bool                    Keys[1024];
    unsigned int            Width, Height;

    std::vector<GameLevel> Levels;
    std::vector<std::string> BackgroundImages;
    unsigned int           Level;

    unsigned int timeSeconds;
    GameObject *Player;
    unsigned int coinsCollected;
    unsigned int playerLives;

    float phaseFuel;
    bool PhaseFlag;

    bool lightOff;
    float lightRadius;

    unsigned int time1;

    Game(unsigned int width, unsigned int height);
    ~Game();

    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    void GenerateObstacles(std::string outputFile, int numFixedObstacles, int wid, int high);
    void MapGenerator(std::string outputFile, int numStatic, int numMoving, int numCoins, int width, int height);
    bool CollisionResolution(GameObject &x, GameObject &y);
    void HandleCollisions();

    bool PlayerCollision();
};

#endif