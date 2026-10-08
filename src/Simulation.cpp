#include "Simulation.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <random>

namespace {

std::mt19937 g_rng(0xC0FFEEu);

float randomFloat(float minValue, float maxValue) {
    std::uniform_real_distribution<float> dist(minValue, maxValue);
    return dist(g_rng);
}

float clamp(float value, float minValue, float maxValue) {
    return std::max(minValue, std::min(maxValue, value));
}

} // namespace

Vec2 Vec2::operator+(const Vec2& other) const {
    return {x + other.x, y + other.y};
}

Vec2 Vec2::operator-(const Vec2& other) const {
    return {x - other.x, y - other.y};
}

Vec2 Vec2::operator*(float scalar) const {
    return {x * scalar, y * scalar};
}

Vec2 Vec2::operator/(float scalar) const {
    return {x / scalar, y / scalar};
}

Vec2& Vec2::operator+=(const Vec2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Simulation::Simulation(float width, float height, std::size_t capacity)
    : width_(width), height_(height) {
    entities_.reserve(capacity);
}

void Simulation::seedEntities(std::size_t count) {
    entities_.clear();
    entities_.reserve(count);

    for (std::size_t index = 0; index < count; ++index) {
        Entity entity{};
        entity.id = static_cast<unsigned int>(index);
        entity.radius = randomFloat(4.0f, 10.0f);
        entity.mass = entity.radius * 0.75f + 1.0f;
        entity.position.x = randomFloat(entity.radius, width_ - entity.radius);
        entity.position.y = randomFloat(entity.radius, height_ - entity.radius);
        entity.velocity.x = randomFloat(-120.0f, 120.0f);
        entity.velocity.y = randomFloat(-120.0f, 120.0f);
        entity.flags = static_cast<unsigned char>(EntityFlags::Alive | EntityFlags::Bounce);

        if ((index % 5u) == 0u) {
            entity.flags |= static_cast<unsigned char>(EntityFlags::Seeker);
        }
        if ((index % 7u) == 0u) {
            entity.flags |= static_cast<unsigned char>(EntityFlags::Avoidant);
        }

        entities_.push_back(entity);
    }
}

void Simulation::handleBoundary(Entity& entity) {
    if (entity.position.x < entity.radius) {
        entity.position.x = entity.radius;
        entity.velocity.x = std::abs(entity.velocity.x);
    } else if (entity.position.x > width_ - entity.radius) {
        entity.position.x = width_ - entity.radius;
        entity.velocity.x = -std::abs(entity.velocity.x);
    }

    if (entity.position.y < entity.radius) {
        entity.position.y = entity.radius;
        entity.velocity.y = std::abs(entity.velocity.y);
    } else if (entity.position.y > height_ - entity.radius) {
        entity.position.y = height_ - entity.radius;
        entity.velocity.y = -std::abs(entity.velocity.y);
    }
}

void Simulation::step(float dt) {
    const Vec2 center{width_ * 0.5f, height_ * 0.5f};

    for (Entity& entity : entities_) {
        if (hasFlag(entity, EntityFlags::Seeker)) {
            Vec2 direction = center - entity.position;
            const float length = std::hypot(direction.x, direction.y);
            if (length > 1.0f) {
                direction = direction / length;
                entity.velocity += direction * (20.0f * dt);
            }
        }

        if (hasFlag(entity, EntityFlags::Avoidant)) {
            for (const Entity& other : entities_) {
                if (&entity == &other) {
                    continue;
                }

                const Vec2 delta = entity.position - other.position;
                const float distance = std::hypot(delta.x, delta.y);
                if (distance < 55.0f && distance > 0.001f) {
                    const Vec2 push = delta / distance;
                    entity.velocity += push * (140.0f * dt);
                }
            }
        }

        entity.position += entity.velocity * dt;
        handleBoundary(entity);
        entity.velocity.x = clamp(entity.velocity.x, -180.0f, 180.0f);
        entity.velocity.y = clamp(entity.velocity.y, -180.0f, 180.0f);
    }

    resolveCollisions();
}

void Simulation::resolveCollisions() {
    for (std::size_t i = 0; i < entities_.size(); ++i) {
        for (std::size_t j = i + 1; j < entities_.size(); ++j) {
            Entity& a = entities_[i];
            Entity& b = entities_[j];

            const Vec2 delta = b.position - a.position;
            const float distanceSquared = (delta.x * delta.x) + (delta.y * delta.y);
            const float minDistance = a.radius + b.radius;

            if (distanceSquared >= minDistance * minDistance || distanceSquared <= 0.0001f) {
                continue;
            }

            const float distance = std::sqrt(distanceSquared);
            const Vec2 normal = delta / distance;
            const float overlap = minDistance - distance;

            a.position -= normal * (overlap * 0.5f);
            b.position += normal * (overlap * 0.5f);

            const float relativeVelocity = ((b.velocity - a.velocity).x * normal.x) + ((b.velocity - a.velocity).y * normal.y);
            if (relativeVelocity < 0.0f) {
                const float restitution = 0.75f;
                const float impulse = -(1.0f + restitution) * relativeVelocity / ((1.0f / a.mass) + (1.0f / b.mass));
                const Vec2 impulseVec = normal * impulse;

                a.velocity -= impulseVec / a.mass;
                b.velocity += impulseVec / b.mass;
            }

            handleBoundary(a);
            handleBoundary(b);
        }
    }
}

void Simulation::run(std::size_t frames, float dt, const std::string& csvPath) {
    const std::filesystem::path outputPath(csvPath);
    if (!outputPath.parent_path().empty()) {
        std::filesystem::create_directories(outputPath.parent_path());
    }

    std::ofstream file(csvPath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open output file: " + csvPath);
    }

    file << "frame,id,x,y,vx,vy,radius,mass,flags\n";

    for (std::size_t frame = 0; frame < frames; ++frame) {
        step(dt);

        for (const Entity& entity : entities_) {
            file << frame << ','
                 << entity.id << ','
                 << entity.position.x << ','
                 << entity.position.y << ','
                 << entity.velocity.x << ','
                 << entity.velocity.y << ','
                 << entity.radius << ','
                 << entity.mass << ','
                 << static_cast<int>(entity.flags) << '\n';
        }
    }

    file.close();
}

std::size_t Simulation::entityCount() const noexcept {
    return entities_.size();
}
