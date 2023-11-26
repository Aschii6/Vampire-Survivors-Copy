#pragma once

#include "Entity.h"

class CollisionChecker {
public:
    static bool isColliding(const Entity& e1, const Entity& e2) {
        if (e1.cSprite && e2.cSprite) {
            return e1.cSprite->sprite.getGlobalBounds().intersects(e2.cSprite->sprite.getGlobalBounds());
        }
        return false;
    }
};
