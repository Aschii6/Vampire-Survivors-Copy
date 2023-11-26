#pragma once

#include <SFML/Graphics.hpp>
#include "Entity.h"
#include <random>
#include "EntityManager.h"


enum GameState {
    PLAYING,
    GAME_OVER,
    PAUSED
};


class Game {
    sf::RenderWindow window;
    sf::View view;

    mt19937 generator;
    uniform_int_distribution<> intDistribution{1, 100};
    uniform_int_distribution<> posXDistribution{-1920, 1920};
    uniform_int_distribution<> posYDistribution{-1080, 1080};

    GameState gameState{PLAYING};
    int currentFrame{0};
    int timeAlive{0};
    int nrOfEnemies{0}, maxNrOfEnemies{25};
    int enemySpawnCd{0}, enemySpawnCdReduction{0}, animationCd{30}, fireBallCd{60}, daggerCd{90}, levelUpTextUptime{0};
    int playerLvl{1};
    const float speed{6};

    shared_ptr<Entity> player;
    EntityManager entityManager;

    map<std::string, std::shared_ptr<sf::Texture>> textureMap;
    sf::Texture mapTexture;
    sf::Sprite map;

    sf::Font font;
    sf::Text timeAliveText, remainingHpText, miscText;

    sf::RectangleShape hitbox;

    void init();
    void loadTextures();
    void loadFontsAndInitTexts();

    void spawnPlayer();

    void update();

    void updatePlaying();
    void updateGameOver();

    void updateTexts();
    void movementSystem();
    void animationSystem();
    void collisionSystem();

    void enemySpawnSystem();
    void spawnEnemy();

    void weaponsSystem();
    void spawnFireball();
    void spawnDaggers();

    void lifespanSystem();
    void expSystem();
    void userInputSystem();
    void renderSystem();

    sf::Vector2f calculateNewViewPos() const;
public:
    Game();

    void run();
};
