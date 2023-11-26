#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

using namespace std;


class CTransform {
public:
    sf::Vector2f pos{0, 0};
    sf::Vector2f velocity{0, 0};

    CTransform(const sf::Vector2f pos, const sf::Vector2f velocity): pos(pos), velocity(velocity) {}
};


class CSprite {
    shared_ptr<sf::Texture> textureP;

    int currentFrame = 0;
public:
    sf::Sprite sprite;

    bool animated = false;

    int nrOfFrames = 0;

    int spriteW = 0, spriteH = 0;

    explicit CSprite(const shared_ptr<sf::Texture> &textureP): textureP(textureP) {
        sprite.setTexture(*textureP);
    }

    CSprite(const shared_ptr<sf::Texture> &textureP, bool animated, int nrOfFrames): textureP(textureP), animated(animated),
        nrOfFrames(nrOfFrames)
    {
        sprite.setTexture(*textureP);
    }

    void setSpriteRect(const int x, const int y, const int w, const int h) {
        sprite.setTextureRect(sf::IntRect(x, y, w, h));
    }

    void cycleSprite() {
        if (!animated || spriteW <= 0 || spriteH <= 0 || nrOfFrames < 0)
            return;

        currentFrame++;

        if (currentFrame >= nrOfFrames)
            currentFrame = 0;

        sprite.setTextureRect(sf::IntRect(currentFrame * spriteW, 0, spriteW, spriteH));
    }
};


class CInput {
public:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    CInput() = default;
};


class CHp {
public:
    int remainingHp = 0;

    int damageCd = 0;

    explicit CHp(const int hp): remainingHp(hp) {}
};


class CDamage {
public:
    int damage = 0;

    explicit CDamage(const int damage): damage(damage) {}
};


class CLifespan {
public:
    int lifespan = 0;

    explicit CLifespan(const int lifespan): lifespan(lifespan) {}
};


class CExp {
public:
    int exp = 0;

    CExp() = default;
};
