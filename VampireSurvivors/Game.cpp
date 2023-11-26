#include "Game.h"

#include <iostream>

#include "CollisionChecker.h"

using namespace std;


Game::Game() {
    init();
}


void Game::run() {
    while (window.isOpen()) {
        update();
    }
}


void Game::init() {
    window.create(sf::VideoMode::getDesktopMode(), "Vampire Survivors", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    generator.seed(time(nullptr));

    loadTextures();
    map.setTexture(mapTexture);
    map.scale(3.84f, 2.16f);
    map.setPosition(-1920, -1080);

    loadFontsAndInitTexts();

    spawnPlayer();
    view.reset(sf::FloatRect(player->cTransform->pos.x, player->cTransform->pos.y,
        1920, 1080));
}


void Game::loadTextures() {
    sf::Texture texture;

    if (!texture.loadFromFile(R"(C:\Users\Daniel\CLionProjects\VampireSurvivors2\Resources\wizzard.png)")) {
        cout << "Error";
    }
    textureMap.emplace("player", std::make_shared<sf::Texture>(texture));

    if (!texture.loadFromFile(R"(C:\Users\Daniel\CLionProjects\VampireSurvivors2\Resources\werewolf.png)")) {
        cout << "Error";
    }
    textureMap.emplace("werewolf", std::make_shared<sf::Texture>(texture));

    if (!texture.loadFromFile(R"(C:\Users\Daniel\CLionProjects\VampireSurvivors2\Resources\barbarian.png)")) {
        cout << "Error";
    }
    textureMap.emplace("barbarian", std::make_shared<sf::Texture>(texture));

    if (!texture.loadFromFile(R"(C:\Users\Daniel\CLionProjects\VampireSurvivors2\Resources\green-monster.png)")) {
        cout << "Error";
    }
    textureMap.emplace("green-monster", std::make_shared<sf::Texture>(texture));

    if (!texture.loadFromFile(R"(C:\Users\Daniel\CLionProjects\VampireSurvivors2\Resources\fireball.png)")) {
        cout << "Error";
    }
    textureMap.emplace("fireball", std::make_shared<sf::Texture>(texture));

    if (!texture.loadFromFile(R"(C:\Users\Daniel\CLionProjects\VampireSurvivors2\Resources\dagger.png)")) {
        cout << "Error";
    }
    textureMap.emplace("dagger", std::make_shared<sf::Texture>(texture));

    if (!mapTexture.loadFromFile(R"(C:\Users\Daniel\CLionProjects\VampireSurvivors2\Resources\map.png)")) {
        cout << "Error";
    }
}

void Game::loadFontsAndInitTexts() {
    font.loadFromFile(R"(C:\Users\Daniel\CLionProjects\VampireSurvivors2\Resources\arial-bold.ttf)");
    timeAliveText.setFont(font);
    timeAliveText.setFillColor(sf::Color::White);
    timeAliveText.setCharacterSize(32);

    remainingHpText.setFont(font);
    remainingHpText.setFillColor(sf::Color::White);
    remainingHpText.setCharacterSize(32);

    miscText.setFont(font);
    miscText.setFillColor(sf::Color::White);
    miscText.setCharacterSize(64);
    miscText.setString("Paused!");
}


void Game::spawnPlayer() {
    const auto entity = entityManager.addEntity("player");

    entity->cTransform = std::make_shared<CTransform>(sf::Vector2f(0, 0), sf::Vector2f(0, 0));

    entity->cSprite = std::make_shared<CSprite>(textureMap["player"], true, 4);
    entity->cSprite->spriteW = 30;
    entity->cSprite->spriteH = 32;
    entity->cSprite->setSpriteRect(0, 0, 30, 32);
    entity->cSprite->sprite.setOrigin(entity->cSprite->sprite.getGlobalBounds().width / 2,
        entity->cSprite->sprite.getGlobalBounds().height / 2);
    entity->cSprite->sprite.scale(3.5, 3.5);

    entity->cInput = std::make_shared<CInput>();

    entity->cHp = std::make_shared<CHp>(100);

    entity->cExp = std::make_shared<CExp>();

    player = entity;
}


void Game::update() {
    // TODO: remove redundancy in future (if any)
    userInputSystem();
    renderSystem();
    switch (gameState) {
        case PLAYING:
            updatePlaying();
            break;
        case PAUSED:
            break;
        case GAME_OVER:
            updateGameOver();
            break;
    }
}


void Game::updatePlaying() {
    currentFrame++;

    if (animationCd > 0)
        animationCd--;
    if (fireBallCd > 0)
        fireBallCd--;
    if (daggerCd > 0)
        daggerCd--;
    if (enemySpawnCd > 0)
        enemySpawnCd--;
    if (levelUpTextUptime > 0)
        levelUpTextUptime--;

    for (const auto &e : entityManager.getEntities())
        if (e->cHp)
            if (e->cHp->damageCd > 0)
                e->cHp->damageCd--;

    if (currentFrame > 60) {
        timeAlive++;
        currentFrame %= 60;
    }

    view.setCenter(calculateNewViewPos());

    entityManager.update();
    movementSystem();
    animationSystem();
    collisionSystem();
    enemySpawnSystem();
    weaponsSystem();
    lifespanSystem();
    updateTexts();
    expSystem();

    if (player->cHp->remainingHp <= 0) {
        gameState = GAME_OVER;
        miscText.setString("Game over!");
        // return;
    }
}


void Game::updateGameOver() {
}


void Game::updateTexts() {
    const sf::Vector2f viewPos = view.getCenter();
    const sf::Vector2f viewSize = view.getSize();

    timeAliveText.setString(to_string(timeAlive));
    timeAliveText.setPosition(viewPos.x - viewSize.x / 2 + 15, viewPos.y - viewSize.y / 2 + 15);

    remainingHpText.setString("HP: " + std::to_string(player->cHp->remainingHp));
    remainingHpText.setPosition(viewPos.x + viewSize.x / 2 - 130, viewPos.y - viewSize.y / 2 + 15);

    miscText.setPosition(viewPos.x - miscText.getGlobalBounds().width / 2,
            viewPos.y - miscText.getGlobalBounds().height / 2);
}


void Game::movementSystem() {
    if (player->cInput) {
        if (player->cInput->up)
            player->cTransform->velocity.y = -speed;
        else if (player->cInput->down)
            player->cTransform->velocity.y = speed;
        else
            player->cTransform->velocity.y = 0;

        if (player->cInput->left)
            player->cTransform->velocity.x = -speed;
        else if (player->cInput->right)
            player->cTransform->velocity.x = speed;
        else
            player->cTransform->velocity.x = 0;

        if (player->cTransform->velocity.x != 0 && player->cTransform->velocity.y != 0)
        {
            player->cTransform->velocity.x *= 0.65;
            player->cTransform->velocity.y *= 0.65;
        }
    }

    const sf::Vector2f playerPos = player->cTransform->pos;
    const sf::FloatRect rect = map.getGlobalBounds();

    for (const auto &e : entityManager.getEntities()) {
        if (e->getTag() == "enemy" && e->cTransform) {
            sf::Vector2f dif =  playerPos - e->cTransform->pos;
            const float length = sqrt(dif.x * dif.x + dif.y * dif.y);

            dif = dif / length * speed * 0.65f;

            if (dif.x >= 0.2f && dif.y >= 0.2f)
                dif *= 0.65f;

            e->cTransform->velocity = dif;
        }

        if (e->cTransform)
            e->cTransform->pos += e->cTransform->velocity;

        if (e->getTag() == "player") {
            sf::Vector2f pos = player->cTransform->pos;
            if (pos.x < rect.left + player->cSprite->sprite.getGlobalBounds().width / 2)
                pos.x = rect.left + player->cSprite->sprite.getGlobalBounds().width / 2;
            else if (pos.x > rect.left + rect.width - player->cSprite->sprite.getGlobalBounds().width / 2)
                pos.x = rect.left + rect.width - player->cSprite->sprite.getGlobalBounds().width / 2;

            if (pos.y < rect.top + player->cSprite->sprite.getGlobalBounds().height / 2)
                pos.y = rect.top + player->cSprite->sprite.getGlobalBounds().height / 2;
            else if (pos.y > rect.top + rect.height - player->cSprite->sprite.getGlobalBounds().height / 2)
                pos.y = rect.top + rect.height - player->cSprite->sprite.getGlobalBounds().height / 2;

            player->cTransform->pos = pos;
        }

        if (e->cSprite && e->cTransform)
            e->cSprite->sprite.setPosition(e->cTransform->pos);
    }
}


void Game::animationSystem() {
    for (const auto &e : entityManager.getEntities())
        if (e->cSprite && e->getTag() != "weapon" && e->cSprite->animated) {
            e->handleFacing();
            if (animationCd == 0)
                if (e->cTransform->pos != sf::Vector2f(0, 0))
                    e->cSprite->cycleSprite();
        }

    if (animationCd == 0)
        animationCd = 30;
}


void Game::collisionSystem() {
    for (const auto &enemy : entityManager.getEntities("enemy")) {
        if (enemy->cDmg && player->cHp && player->cHp->damageCd == 0) {
            if (CollisionChecker::isColliding(*player, *enemy)) {
                player->cHp->remainingHp -= enemy->cDmg->damage;
                player->cHp->damageCd = 60;
            }
    }

        for (const auto &weapon : entityManager.getEntities("weapon")) {
            if (weapon->cDmg && enemy->cHp) {
                if (enemy->cHp->damageCd == 0 && CollisionChecker::isColliding(*enemy, *weapon)) {
                    enemy->cHp->remainingHp -= weapon->cDmg->damage;
                    enemy->cHp->damageCd = 60;

                    if (weapon->getTag() == "fireball")
                        weapon->destroy();

                    if (enemy->cHp->remainingHp <= 0) {
                        if (player->cExp)
                            player->cExp->exp += enemy->cDmg->damage; // a bit scuffed :)
                        enemy->destroy();
                        nrOfEnemies--;
                    }
                }
            }
        }
    }
}


void Game::enemySpawnSystem() {
    if (enemySpawnCd == 0 && nrOfEnemies < maxNrOfEnemies) {
        spawnEnemy();
        enemySpawnCd = 120 - enemySpawnCdReduction;
        nrOfEnemies++;
    }
}


void Game::spawnEnemy() {
    const int chance = intDistribution(generator);
    const auto entity = entityManager.addEntity("enemy");

    const auto playerPos = player->cTransform->pos;

    auto x = static_cast<float>(posXDistribution(generator));
    auto y = static_cast<float>(posYDistribution(generator));
    while ((x- playerPos.x) * (x - playerPos.x) + (y - playerPos.y) * (y - playerPos.y) < 1000) {
        x = static_cast<float>(posXDistribution(generator));
        y = static_cast<float>(posYDistribution(generator));
    }

    entity->cTransform = std::make_shared<CTransform>(sf::Vector2f(x, y), sf::Vector2f(0, 0));

    if (chance <= 55) {
        entity->cSprite = std::make_shared<CSprite>(textureMap["barbarian"], true, 4);
        entity->cSprite->spriteW = 30;
        entity->cSprite->spriteH = 32;
        entity->cSprite->setSpriteRect(0, 0, 30, 32);
        entity->cSprite->sprite.setOrigin(entity->cSprite->sprite.getGlobalBounds().width / 2,
            entity->cSprite->sprite.getGlobalBounds().height / 2);
        entity->cSprite->sprite.scale(3.5, 3.5);

        entity->cHp = std::make_shared<CHp>(70);

        entity->cDmg = std::make_shared<CDamage>(10);
    }
    else if (chance <= 80) {
        entity->cSprite = std::make_shared<CSprite>(textureMap["green-monster"], true, 4);
        entity->cSprite->spriteW = 30;
        entity->cSprite->spriteH = 32;
        entity->cSprite->setSpriteRect(0, 0, 30, 32);
        entity->cSprite->sprite.setOrigin(entity->cSprite->sprite.getGlobalBounds().width / 2,
            entity->cSprite->sprite.getGlobalBounds().height / 2);
        entity->cSprite->sprite.scale(3.5, 3.5);

        entity->cHp = std::make_shared<CHp>(110);

        entity->cDmg = std::make_shared<CDamage>(15);
    }
    else {
        entity->cSprite = std::make_shared<CSprite>(textureMap["werewolf"], true, 4);
        entity->cSprite->spriteW = 31;
        entity->cSprite->spriteH = 32;
        entity->cSprite->setSpriteRect(0, 0, 31, 32);
        entity->cSprite->sprite.setOrigin(entity->cSprite->sprite.getGlobalBounds().width / 2,
            entity->cSprite->sprite.getGlobalBounds().height / 2);
        entity->cSprite->sprite.scale(3.5, 3.5);

        entity->cHp = std::make_shared<CHp>(150);

        entity->cDmg = std::make_shared<CDamage>(20);
    }
}


void Game::weaponsSystem() {
    if (fireBallCd == 0) {
        spawnFireball();
        fireBallCd = 45;
    }
    if (daggerCd == 0) {
        spawnDaggers();
        daggerCd = 120;
    }
}


void Game::spawnFireball() {
    const auto entity = entityManager.addEntity("weapon");

    sf::Vector2f velocity;
    if (player->isFacingLeft())
        velocity = sf::Vector2f(-12, 0);
    else
        velocity = sf::Vector2f(12, 0);
    entity->cTransform = std::make_shared<CTransform>(player->cTransform->pos, velocity);

    entity->cSprite = std::make_shared<CSprite>(textureMap["fireball"]);
    entity->cSprite->sprite.setOrigin(entity->cSprite->sprite.getGlobalBounds().width / 2,
        entity->cSprite->sprite.getGlobalBounds().height / 2);

    if (player->isFacingLeft())
        entity->cSprite->sprite.scale(3, 3);
    else
        entity->cSprite->sprite.scale(-3, 3);

    entity->cLifespan = std::make_shared<CLifespan>(240);

    entity->cDmg = std::make_shared<CDamage>(25 + playerLvl / 2 * 5);
}


void Game::spawnDaggers() {
    const auto entity1 = entityManager.addEntity("weapon");

    entity1->cTransform = make_shared<CTransform>(player->cTransform->pos, sf::Vector2f(-8.5f, 0));

    entity1->cSprite = make_shared<CSprite>(textureMap["dagger"]);
    entity1->cSprite->sprite.setOrigin(entity1->cSprite->sprite.getGlobalBounds().width / 2,
        entity1->cSprite->sprite.getGlobalBounds().height / 2);

    entity1->cSprite->sprite.setScale(1.4f, 1.4f);

    entity1->cLifespan = make_shared<CLifespan>(180);

    entity1->cDmg = make_shared<CDamage>(20 + (playerLvl - 1) / 2 * 5);

    const auto entity2 = entityManager.addEntity("weapon");

    entity2->cTransform = make_shared<CTransform>(player->cTransform->pos, sf::Vector2f(8.5f, 0));

    entity2->cSprite = make_shared<CSprite>(textureMap["dagger"]);
    entity2->cSprite->sprite.setOrigin(entity2->cSprite->sprite.getGlobalBounds().width / 2,
        entity2->cSprite->sprite.getGlobalBounds().height / 2);
    entity2->cSprite->sprite.setScale(-1.4f, 1.4f);

    entity2->cLifespan = make_shared<CLifespan>(180);

    entity2->cDmg = make_shared<CDamage>(20 + playerLvl / 2 * 5);
}


void Game::lifespanSystem() {
    for (const auto &entity : entityManager.getEntities("weapon")) {
        if (entity->cLifespan) {
            entity->cLifespan->lifespan--;

            if (entity->cLifespan->lifespan <= 0)
                entity->destroy();
        }
    }
}


void Game::expSystem() {
    if (player->cExp && player->cExp->exp >= playerLvl * 100) {
        player->cExp->exp = 0;

        if (player->cHp)
            player->cHp->remainingHp += 30;

        playerLvl++;

        if (enemySpawnCdReduction < 60)
            enemySpawnCdReduction += 5;

        if (playerLvl % 2 == 0)
            miscText.setString("Fireball dmg up");
        else
            miscText.setString("Daggers dmg up");

        levelUpTextUptime = 90;
    }
}


void Game::userInputSystem() {
    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed)
            switch (event.key.code)
            {
                case sf::Keyboard::Escape:
                    window.close();
                break;
                case sf::Keyboard::W:
                    player->cInput->up = true;
                break;
                case sf::Keyboard::A:
                    player->cInput->left = true;
                break;
                case sf::Keyboard::S:
                    player->cInput->down = true;
                break;
                case sf::Keyboard::D:
                    player->cInput->right = true;
                break;
                case sf::Keyboard::P: {
                    if (gameState == PLAYING) {
                        gameState = PAUSED;
                        miscText.setString("Paused!");
                    }
                    else if (gameState == PAUSED)
                        gameState = PLAYING;
                    break;
                }
                default:
                    break;
            }

        if (event.type == sf::Event::KeyReleased)
            switch (event.key.code)
            {
                case sf::Keyboard::W:
                    player->cInput->up = false;
                break;
                case sf::Keyboard::A:
                    player->cInput->left = false;
                break;
                case sf::Keyboard::S:
                    player->cInput->down = false;
                break;
                case sf::Keyboard::D:
                    player->cInput->right = false;
                break;
                default:
                    break;
            }
    }
}


void Game::renderSystem() {
    window.clear();

    window.setView(view);
    window.draw(map);

    for (const auto &e : entityManager.getEntities())
        if (e->cSprite && e->cTransform) {
            window.draw(e->cSprite->sprite);

            // for debugging purposes
            /*hitbox = sf::RectangleShape(sf::Vector2f(e->cSprite->sprite.getGlobalBounds().width,
                e->cSprite->sprite.getGlobalBounds().height));

            hitbox.setOrigin(e->cSprite->sprite.getGlobalBounds().width / 2, e->cSprite->sprite.getGlobalBounds().height / 2);

            hitbox.setPosition(e->cTransform->pos);

            if (e->cHp && e->cHp->damageCd != 0)
                hitbox.setOutlineColor(sf::Color::Red);
            else
                hitbox.setOutlineColor(sf::Color::White);
            hitbox.setFillColor(sf::Color(0, 0, 0, 0));
            hitbox.setOutlineThickness(3);

            window.draw(hitbox);*/
        }

    window.draw(timeAliveText);
    window.draw(remainingHpText);

    if (gameState == GAME_OVER || gameState == PAUSED || levelUpTextUptime > 0)
        window.draw(miscText);

    window.setView(window.getDefaultView());

    window.display();
}


sf::Vector2f Game::calculateNewViewPos() const {
    sf::Vector2f newPos = sf::Vector2f(player->cTransform->pos.x + player->cSprite->sprite.getGlobalBounds().width / 2,
        player->cTransform->pos.y + player->cSprite->sprite.getGlobalBounds().height / 2);

    const sf::FloatRect rect = map.getGlobalBounds();

    const sf::Vector2f windowSize = sf::Vector2f(window.getSize());

    if (newPos.x < rect.left + windowSize.x / 2)
        newPos.x = rect.left + windowSize.x / 2;
    else if (newPos.x > rect.left + rect.width - windowSize.x / 2)
        newPos.x = rect.left + rect.width - windowSize.x / 2;

    if (newPos.y < rect.top + windowSize.y / 2)
        newPos.y = rect.top + windowSize.y / 2;
    else if (newPos.y > rect.top + rect.height - windowSize.y / 2)
        newPos.y = rect.top + rect.height - windowSize.y / 2;

    return newPos;
}

