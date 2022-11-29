#include "precomp.h"
#include "particle_beam.h"

namespace Tmpl8
{


Particle_beam::Particle_beam(vec2 min, vec2 max, Sprite* particle_beam_sprite, int damage) : sprite_frame(0),
    damage(damage), particle_beam_sprite(particle_beam_sprite)
{
    min_position = min;
    max_position = min + max;

    rectangle = Rectangle2D(min_position, max_position);
}

void Particle_beam::tick(vector<Tank>& tanks)
{

    if (++sprite_frame == 30)
    {
        sprite_frame = 0;
    }
}

void Particle_beam::draw(Surface* screen) const
{
    const vec2 position = rectangle.min;

    constexpr int offset_x = 23;
    constexpr int offset_y = 137;

    particle_beam_sprite->set_frame(sprite_frame / 10);
    particle_beam_sprite->draw(screen, (int)(position.x - offset_x + HEALTHBAR_OFFSET), (int)(position.y - offset_y));
}

} // namespace Tmpl8