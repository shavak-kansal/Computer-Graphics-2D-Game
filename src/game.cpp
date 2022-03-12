#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"
#include "text_renderer.h"

#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>



typedef std::pair<glm::vec2, bool> CollisionInfo;

// Game-related State data
SpriteRenderer  *Renderer;
TextRenderer  *Text;

const float sqrt_2 = std::sqrt(2);

CollisionInfo CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + (1.0f)*one.Size.x >= two.Position.x &&
        two.Position.x + (1.0f)*two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + (1.0f)*one.Size.y >= two.Position.y &&
        two.Position.y + (1.0f)*two.Size.y >= one.Position.y;
    // collision only if on both axes
    glm::vec2 direction[4] = {glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, .0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f)};

    float maxProduct = 0.0f;
    int maxIndex = -1;

    glm::vec2 centreOne = one.Position + one.Size / 2.0f;
    glm::vec2 centreTwo = two.Position + two.Size / 2.0f;

    for(int i = 0 ; i < 4 ; i++){
        if(glm::dot((centreOne - centreTwo), direction[i]) > maxProduct){
            maxProduct = glm::dot(one.Position + (one.Size / 2.0f) - two.Position - (two.Size / 2.0f), direction[i]);
            maxIndex = i;
        }
    }

    glm::vec2 collisionDirect;

    {
        if(glm::dot((centreOne - centreTwo), direction[0]) > glm::dot((centreOne - centreTwo), direction[1]))
            collisionDirect.x = 1.0f;
        else 
            collisionDirect.x = -1.0f;

        if(glm::dot((centreOne - centreTwo), direction[2]) > glm::dot((centreOne - centreTwo), direction[3]))
            collisionDirect.y = 1.0f;
        else 
            collisionDirect.y = -1.0f;


    }
    return std::make_pair(collisionDirect,collisionX && collisionY);
} 



Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height), coinsCollected(0), phaseFuel(100.0f), PhaseFlag(false), playerLives(100), lightOff(false), lightRadius(100.0f){}

Game::~Game(){
}

void Game::Init(){

    this->MapGenerator(std::string("../levels/level1.txt"), 25, 5, 20, 12, 9);
    this->MapGenerator(std::string("../levels/level2.txt"), 30, 10, 20, 12, 9);
    this->MapGenerator(std::string("../levels/level3.txt"), 30, 15, 40, 12, 9);
    // load shaders
    ResourceManager::LoadShader("../src/shaders/sprite.vs", "../src/shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),static_cast<float>(this->Height), 0.0f,  -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load textures
    ResourceManager::LoadTexture("../textures/tileable4g.png", false, "background");

    ResourceManager::LoadTexture("../textures/brick_texture_seamless_-_outdoor_stone_brick_wall_002.jpg", false, "background0");
    ResourceManager::LoadTexture("../textures/distressed_wood_textures_1.jpg", false, "background1");
    ResourceManager::LoadTexture("../textures/wood_textures_hd_2.jpg", false, "background2");

    BackgroundImages.push_back("background0");
    BackgroundImages.push_back("background1");
    BackgroundImages.push_back("background2");


    ResourceManager::LoadTexture("../textures/awesomeface.png", true, "awesome");
    ResourceManager::LoadTexture("../textures/container004-blue.png", false, "obstacle");
    ResourceManager::LoadTexture("../textures/light003.png", true, "enemy");
    ResourceManager::LoadTexture("../textures/PNG/SILVER/BIG/BIG_0000_Capa-1.png", true, "coin");
    ResourceManager::LoadTexture("../textures/sign_browndoor.png", true, "door");  

    Player = new GameObject(glm::vec2(0.0f, Height/2.0f), glm::vec2(50.0f, 50.0f) , ResourceManager::GetTexture("awesome"), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(200.0f, 200.0f));
    ResourceManager::GetShader("sprite").Use().SetVector2f("PlayerPosition", Player->Position);

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("../src/fonts/ManilaSansBld.otf", 24);

    // load levels
    Levels.push_back(GameLevel());
    Levels.push_back(GameLevel());
    Levels.push_back(GameLevel());

    Levels[0].enemyVelocity = 150.0f;
    Levels[0].fracChasing = 0.0f;
    Levels[0].Load("../levels/level1.txt", this->Width, this->Height);
    
    Levels[1].enemyVelocity = 200.0f;
    Levels[1].fracChasing = 0.0f;
    Levels[1].Load("../levels/level2.txt", this->Width, this->Height);

    Levels[2].enemyVelocity = 180.0f;
    Levels[2].fracChasing = 0.5f;
    Levels[2].Load("../levels/level3.txt", this->Width, this->Height);


    
    this->Level = 0;
}

void Game::GenerateObstacles(std::string outputFile, int numFixedObstacles, int wid, int high){
    std::ofstream fileOut;

    fileOut.open(outputFile);

    int width = wid;
    int height = high;
    
    srand(time(NULL));

    for(int i = 0 ; i < height ; i++){
        std::vector<int> pos;

        for (int i = 0; i < numFixedObstacles; ++i)
        {
            pos.push_back(rand()%(width-1));
        }

        for(int j = 0 ; j < width ; j++){
            if(std::find(pos.begin(), pos.end(), j) == pos.end())
                fileOut << 0 << " ";
            else 
                fileOut << 1 << " ";
        }

        fileOut<<std::endl;
    }
}

void Game::MapGenerator(std::string outputFile, int numStatic, int numMoving, int numCoins, int width, int height){
    std::ofstream fileOut;
    fileOut.open(outputFile);

    std::vector<std::vector<int>> grid(height);

    for(auto &x : grid)
        x = std::vector<int>(width);

    int cntStatic = numStatic;
    int cntMoving = numMoving;
    int cntCoins = numCoins;

    srand(time(NULL));

    while(cntStatic > 0){
        int x = rand()%(width-2) + 1;
        int y = rand()%height;

        if(grid[y][x] == 0){
            grid[y][x] = 1;
            cntStatic--;
        }
    }

    while(cntMoving > 0){
        int x = rand()%rand()%(width-2) + 1;
        int y = rand()%height;

        if(grid[y][x] == 0){
            grid[y][x] = 2;
            cntMoving--;
        }
    }

    while(cntCoins){
        int x = rand()%width;
        int y = rand()%height;

        if(grid[y][x] == 0){
            grid[y][x] = 3;
            cntCoins--;
        }
    }

    for(int i = 0 ; i < height ; i++){
        for(int j = 0 ; j < width ; j++){
            fileOut << grid[i][j] << " ";
        }
        fileOut << std::endl;
    }
}

bool Game::CollisionResolution(GameObject &x, GameObject &y){
    auto res = CheckCollision(x, y);

    if(res.second){
        glm::vec2 directionVec = y.Position + (y.Size/(2.0f)) - x.Position - (x.Size/(2.0f));
        float vel = glm::length(x.Velocity);

        x.Velocity = (-1.0f) * vel * glm::normalize(directionVec);
        return true;    
    }
    else {
        return false;
    }
}

void Game::HandleCollisions(){
    for (auto &x : this->Levels[this->Level].Enemies){

        for(auto &y : Levels[Level].Coins){
            CollisionResolution(x, y);
        }

        for(auto &y : Levels[Level].Enemies){
            if(&y == &x)
                continue;
            CollisionResolution(x, y);
        }

        for(auto &y : Levels[Level].Obstacles){
            CollisionResolution(x, y);
        }

        CollisionResolution(x, *Player);
    }

    for (auto &x : this->Levels[this->Level].Coins){

        for(auto &y : Levels[Level].Obstacles){
            CollisionResolution(x, y);
        }

        for(auto &y : Levels[Level].Coins){
            if(&y == &x)
                continue;
            CollisionResolution(x, y);
        }

        for(auto &y : Levels[Level].Enemies){
            if(&y == &x)
                continue;
            CollisionResolution(x, y);
        }
    }

    for(auto &x : this->Levels[this->Level].Coins){
        bool res = CheckCollision(*Player, x).second;

        if(res){
            if(!x.Destroyed){
                if(lightOff)
                    coinsCollected += 3;
                else
                    coinsCollected++;
            }
            x.Destroyed = true;   
        }
    }

    bool death = false;

    for(auto &y : Levels[Level].Enemies)
        if(CheckCollision(*Player, y).second){
            death = true;
            CollisionResolution(y, *Player);
        }

    if(death){
        if(this->playerLives <= 0){
            this->State = GAME_LOSS;
            this->time1 = timeSeconds;
        }
        else 
            this->playerLives--;
    }
}

bool Game::PlayerCollision(){

    if(Keys[80])
        return false;

    if(PhaseFlag)
        return false;

    for(auto &y : Levels[Level].Obstacles){
        if(CheckCollision(*Player, y).second)
            return true;
    }

    for(auto &y : Levels[Level].Enemies){
        if(CheckCollision(*Player, y).second)
            return true;
    }

    return false;
}

void Game::Update(float dt){   

    if(this->State == GAME_WIN || this->State == GAME_LOSS)
        return;

    ResourceManager::GetShader("sprite").Use().SetVector2f("PlayerPosition", Player->Position);

    if(lightOff){
        ResourceManager::GetShader("sprite").Use().SetInteger("flag", 1);
    }
    else {
        ResourceManager::GetShader("sprite").Use().SetInteger("flag", 0);
    }

    if(phaseFuel <= 0){
        Keys[GLFW_KEY_P] = false;
        PhaseFlag = false;
    }

    float decreaseFuel = dt*70;
    float increaseFuel = dt*10;

    if(Keys[GLFW_KEY_P] || PhaseFlag){
        phaseFuel -= decreaseFuel;
        std::cout<<"\r"<<"Decreasing phase fuel by :"<<(decreaseFuel)<<std::flush;
    }
    else {
        if(phaseFuel >= 100.0f)
            phaseFuel = 100.0f;
        else
        {
            std::cout<<"\r"<<"Increasing phase fuel by :"<<(increaseFuel)<<std::flush;
            phaseFuel += increaseFuel;
        }
    }

    for(auto x : Levels[Level].listChasing){
        Levels[Level].Enemies[x].Velocity = Levels[Level].enemyVelocity/1.0f * glm::normalize(Player->Position - Levels[Level].Enemies[x].Position);
    }

    if(CheckCollision(*Player, *(Levels[Level].exitGate)).second){
        if(Level == 2){
            this->State = GAME_WIN;
            this->time1 = timeSeconds;
        }
        else{
            Level++;
            Player->Position = glm::vec2(0.0f, (Height-1)/2.0f);
        }
    }
    HandleCollisions();
    //std::cout << "Coordinates displayed in update function 1 : " << Levels[Level].Enemies[0].Position.x << " " << Levels[Level].Enemies[0].Position.y <<std::endl;
    for ( auto &x : Levels[Level].Enemies){
        x.HandleMovement(dt,Width,Height);
    }

    for ( auto &x : Levels[Level].Coins){
        x.HandleMovement(dt,Width,Height);
    }

    
}


void Game::ProcessInput(float dt){
    if (this->State == GAME_ACTIVE)
    {
        // move playerboard
        if (this->Keys[GLFW_KEY_A])
        {   
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= Player->Velocity.x * dt;
            }

            if(PlayerCollision()){
                Player->Position.x += Player->Velocity.x * dt;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += Player->Velocity.x * dt;
            }

            if(PlayerCollision()){
                Player->Position.x -= Player->Velocity.x * dt;
            }
        }
        if (this->Keys[GLFW_KEY_W])
        {
            if (Player->Position.y >= 0.0f)
            {
                Player->Position.y -= Player->Velocity.y * dt;
            }

            if(PlayerCollision()){
                Player->Position.y += Player->Velocity.y * dt;
            }
        }
        if (this->Keys[GLFW_KEY_S])
        {
            if (Player->Position.y <= this->Height - Player->Size.y)
            {
                Player->Position.y += Player->Velocity.y * dt;
            }

            if(PlayerCollision()){
                Player->Position.y -= Player->Velocity.y * dt;
            }
        }

    }
}

void Game::Render()
{
    //std::cout<<"Grand"<<std::endl;
    if(this->State == GAME_ACTIVE)
    {
        // draw background
        // Renderer->DrawSprite(ResourceManager::GetTexture("background"), 
        //     glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f
        // );

        Renderer->DrawSprite(ResourceManager::GetTexture(BackgroundImages[Level]), 
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f
        );

        this->Levels[this->Level].Draw(*Renderer);

        Player->Draw(*Renderer);

        std::string toPrint = "Score :" + std::to_string(coinsCollected);
        Text->RenderText(toPrint, 5.0f, 5.0f, 1.0f);

        toPrint = "Phase Fuel : " + std::to_string(phaseFuel);
        Text->RenderText(toPrint, 5.0f, 25.0f, 1.0f);

        toPrint = "Lives Left : " + std::to_string(playerLives);
        Text->RenderText(toPrint, 5.0f, 45.0f, 1.0f);

        toPrint = "Level " + std::to_string(Level);
        Text->RenderText(toPrint, 5.0f, 65.0f, 1.0f);

        toPrint = "Time Elapsed " + std::to_string(this->timeSeconds);
        Text->RenderText(toPrint, 5.0f, 85.0f, 1.0f);
        // draw level
        //this->Levels[this->Level].Draw(*Renderer);
    }
    if(this->State == GAME_WIN){
        std::string toPrint = "YOU WIN";
        
        Text->RenderText(toPrint, 5.0f, 5.0f, 2.0f);

        if(timeSeconds > (time1 + 5))
            exit(0);
    }
    if(this->State == GAME_LOSS){
        std::string toPrint = "YOU LOSS";
        
        Text->RenderText(toPrint, 5.0f, 5.0f, 2.0f);

        if(timeSeconds > (time1 + 5))
            exit(0);
    }
}