#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class EntityFlags : unsigned char {
    None = 0,
    Alive = 1u << 0,
    Bounce = 1u << 1,
    Seeker = 1u << 2,
    Avoidant = 1u << 3
};

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;
    Vec2& operator+=(const Vec2& other);
    Vec2& operator-=(const Vec2& other);
};

struct Entity {
    unsigned int id = 0;
    Vec2 position{};
    Vec2 velocity{};
    float radius = 0.0f;
    float mass = 1.0f;
    unsigned char flags = static_cast<unsigned char>(EntityFlags::Alive | EntityFlags::Bounce);
};

inline bool hasFlag(const Entity& entity, EntityFlags flag) {
    return (entity.flags & static_cast<unsigned char>(flag)) != 0u;
}

inline void setFlag(Entity& entity, EntityFlags flag, bool value) {
    const unsigned char mask = static_cast<unsigned char>(flag);
    if (value) {
        entity.flags |= mask;
    } else {
        entity.flags &= static_cast<unsigned char>(~mask);
    }
}

class Simulation {
public:
    Simulation(float width, float height, std::size_t capacity);

    void seedEntities(std::size_t count);
    void step(float dt);
    void run(std::size_t frames, float dt, const std::string& csvPath);

    std::size_t entityCount() const noexcept;

private:
    void handleBoundary(Entity& entity);
    void resolveCollisions();

    std::vector<Entity> entities_;
    float width_ = 0.0f;
    float height_ = 0.0f;
};
