#pragma once

#include "Components.h"
#include <memory>
#include <string>

using namespace std;


class Entity {
    friend class EntityManager;

    unsigned int id = 0;
    string tag;
    bool alive = true;
    bool facingLeft = false;

    Entity(const unsigned int i, std::string t) : id(i), tag(std::move(t)) {}
public:
    shared_ptr<CTransform> cTransform;
    shared_ptr<CSprite> cSprite;
    shared_ptr<CInput> cInput;
    std::shared_ptr<CHp> cHp;
    std::shared_ptr<CDamage> cDmg;
    std::shared_ptr<CLifespan> cLifespan;
    std::shared_ptr<CExp> cExp;

    void destroy() {
        alive = false;
    }

    [[nodiscard]] bool isAlive() const {
        return alive;
    }

    [[nodiscard]] unsigned int getId() const {
        return id;
    }

    [[nodiscard]] std::string getTag() const {
        return tag;
    }

    void handleFacing() {
        if (!this->cSprite || !this->cTransform)
            return;

        const bool temp = facingLeft;
        if (cTransform->velocity.x < 0) {
            facingLeft = true;
        } else if (cTransform->velocity.x > 0) {
            facingLeft = false;
        }

        if (temp != facingLeft) {
            this->cSprite->sprite.setScale(-this->cSprite->sprite.getScale().x, this->cSprite->sprite.getScale().y);
        }
    }

    [[nodiscard]] bool isFacingLeft() const {
        return facingLeft;
    }
};
