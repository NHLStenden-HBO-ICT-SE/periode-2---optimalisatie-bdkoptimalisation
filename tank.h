#pragma once

namespace Tmpl8
{
    class Terrain; //forward declare

    enum allignments
    {
        BLUE,
        RED
    };

    class Tank
    {
    public:
        Tank(float pos_x, float pos_y, allignments allignment, Sprite* tank_sprite, Sprite* smoke_sprite, float tar_x,
             float tar_y, int health, float max_speed);

        ~Tank();

        void tick(Terrain& terrain);

        vec2 get_position() const { return position; };
        bool rocket_reloaded() const { return reloaded; };

        void set_route(const std::vector<vec2>& route);
        void reload_rocket();

        void deactivate();
        bool hit(int hit_value);

        void draw(Surface* screen) const;

        int compare_health(const Tank& other) const;

        void push(vec2 direction, float magnitude);

        vec2 position;
        vec2 speed;
        vec2 target;

        vector<vec2> current_route;

        int health;

        vec2 force;

        float max_speed;
        float reload_time;

        bool reloaded;
        bool active;

        allignments allignment;

        int current_frame;
        Sprite* tank_sprite;
        Sprite* smoke_sprite;
    };
} // namespace Tmpl8
