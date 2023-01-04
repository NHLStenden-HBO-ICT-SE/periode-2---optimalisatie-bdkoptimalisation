#pragma once
namespace Tmpl8
{

class Smoke
{
  public:
    Smoke(Sprite& smoke_sprite, const vec2 position) : position(position), current_frame(0), smoke_sprite(smoke_sprite) {}

    void tick();
    void draw(Surface* screen) const;

    vec2 position;

    int current_frame;
    Sprite& smoke_sprite;
};
} // namespace Tmpl8