#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Level.h"

class Entity {
public:
    bool movement = false;
    enum State { left, right, up, down, jump, stay, right_Top };

    std::vector<Object> obj, obj2;
    float dx = 0.0f, dy = 0.0f, x = 0.0f, y = 0.0f, speed = 0.0f, moveTimer = 0.0f;
    int w = 0, h = 0, armour = 100, health = 100;
    bool life = true, isMove = false, onGround = false;

    sf::Texture texture;
    sf::Sprite sprite;
    std::string name;

    Entity(sf::Image& image, const std::string& Name, float X, float Y, int W, int H)
        : x(X), y(Y), w(W), h(H), name(Name) {
        texture.loadFromImage(image);
        sprite.setTexture(texture);
        sprite.setOrigin(static_cast<float>(w) / 2.0f, static_cast<float>(h) / 2.0f);
    }

    sf::FloatRect getRect() const {
        return sf::FloatRect(x, y, static_cast<float>(w), static_cast<float>(h));
    }

    virtual void update(float time) = 0;
};
