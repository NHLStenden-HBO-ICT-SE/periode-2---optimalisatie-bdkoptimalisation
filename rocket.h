#pragma once

namespace Tmpl8
{

class Rocket
{
  public:
    Rocket(vec2 position, vec2 direction, uint8_t collision_radius, allignments allignment, Sprite* rocket_sprite);
    ~Rocket();

    void tick();
    void draw(Surface* screen);

    auto intersects(vec2 position_other, uint8_t radius_other) const -> bool;

    vec2 position;
    vec2 speed;

    uint8_t collision_radius;

    bool active;

    allignments allignment;

    int current_frame;
    Sprite* rocket_sprite;
};

} // namespace Tmpl8