#include "game_level.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
    // clear old data
    this->Bricks.clear();
    this->Enemies.clear();

    // load from file
    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (fstream)
    {
        while (std::getline(fstream, line)) // read each line from level file
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while (sstream >> tileCode) // read each word separated by spaces
                row.push_back(tileCode);
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
            this->init(tileData, levelWidth, levelHeight);
    }
}


void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Obstacles)
        if (!tile.Destroyed)
            tile.Draw(renderer);

    for (GameObject &tile : this->Coins)
        if (!tile.Destroyed)
            tile.Draw(renderer);

    for (GameObject &tile : this->Enemies)
        tile.Draw(renderer);

    exitGate->Draw(renderer);
}

bool GameLevel::IsCompleted()
{
    for (GameObject &tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed)
            return false;
    return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{

    {
        std::cout<<"Level height : "<<levelHeight<<std::endl;
        std::cout<<"Level width : "<<levelWidth<<std::endl;
    }
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size(); // note we can index vector at [0] since this function is only called if height > 0

    {
        std::cout<<"Height : "<<height<<std::endl;
        std::cout<<"Width : "<<width<<std::endl;
    }

    float unit_width = levelWidth / static_cast<float>(width), unit_height = levelHeight / height; 

    {
        std::cout<<"Unit Height : "<<unit_height<<std::endl;
        std::cout<<"Unit Width : "<<unit_width<<std::endl;
    }

    // initialize level tiles based on tileData		
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            if(tileData[y][x] == 1){
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);

                GameObject obj(pos, size, ResourceManager::GetTexture("obstacle"), glm::vec3(1.0f));
                obj.IsSolid = true;
                this->Obstacles.push_back(obj);

                
            }
            else if(tileData[y][x] == 2){
                glm::vec2 pos1(unit_width * x, unit_height * y);
                glm::vec2 size1(unit_width/3, unit_height/3);

                //EnemyObject obj1(pos1, unit_width/4, glm::vec2(20.0f * (rand()%10 + 2), 20.0f * (rand()%10 + 2)), ResourceManager::GetTexture("obstacle"));
                GameObject obj1(pos1, size1, ResourceManager::GetTexture("enemy"), glm::vec3(1.0f), enemyVelocity * glm::normalize(glm::vec2(1.0f, 1.0f)));
                Enemies.push_back(obj1);
            }
            else if(tileData[y][x] == 3){
                glm::vec2 pos2(unit_width * x, unit_height * y);
                glm::vec2 size2(unit_width/4, unit_height/4);

                //EnemyObject obj1(pos1, unit_width/4, glm::vec2(20.0f * (rand()%10 + 2), 20.0f * (rand()%10 + 2)), ResourceManager::GetTexture("obstacle"));
                GameObject obj2(pos2, size2, ResourceManager::GetTexture("coin"), glm::vec3(1.0f), enemyVelocity * glm::normalize(glm::vec2(1.0f, 1.0f)), 0.0f);
                Coins.push_back(obj2);   
            }

        }
    }

    for(int i = 0; i < fracChasing*Enemies.size() ; i++){
        while(1){
            int index = rand()%(Enemies.size());
            
            if(std::find(listChasing.begin(), listChasing.end(), index) == listChasing.end()){
                listChasing.push_back(index);
                break;
            }
        }
    }

    // std::cout << "Fraction chasing : " << fracChasing << std::endl;
    // std::cout << "Enemy Velocity : " << enemyVelocity << std::endl;
    // std::cout << "List of chasing enemies out of : " << Enemies.size()<< std::endl;
    // std::cout << std::endl;

    for(auto x : listChasing){
        Enemies[x].Color = glm::vec3(1.0f, 0.0f, 0.0f);
        //Enemies[x]
    }

    glm::vec2 pos(unit_width * (width) - unit_width*0.5f, unit_height * (height-1)/2.0f);
    glm::vec2 size(unit_width/2, unit_height);
     
    exitGate = new GameObject(pos, size, ResourceManager::GetTexture("door"), glm::vec3(1.0f));;
}