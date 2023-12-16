#pragma once

#include <memory>
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;


class EntityManager {
    EntityVec entities;
    EntityVec toAdd;
    EntityMap entityMap;

    unsigned int totalEnemies = 0;
public:
    explicit EntityManager() = default;

    void update() {
        for (const auto& e : toAdd) {
            entities.push_back(e);
            entityMap[e->getTag()].push_back(e);
        }
        toAdd.clear();

        entities.erase(std::remove_if(entities.begin(), entities.end(),
            [](const std::shared_ptr<Entity>& e) {
                return !e->isAlive();
            }), entities.end());

        for (auto& pair : entityMap)
        {
            pair.second.erase(remove_if(pair.second.begin(), pair.second.end(),
                                        [](const shared_ptr<Entity>& e) {
                return !e->isAlive(); }), pair.second.end());
        }
    }

    std::shared_ptr<Entity> addEntity(const std::string& tag) {
        auto e = std::shared_ptr<Entity>(new Entity(totalEnemies++, tag));
        toAdd.push_back(e);

        return e;
    }

    EntityVec& getEntities() {
        return entities;
    }

    EntityVec& getEntities(const std::string &tag) {
        return entityMap[tag];
    }
};
