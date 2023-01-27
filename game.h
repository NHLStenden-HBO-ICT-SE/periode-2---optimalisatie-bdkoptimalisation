#pragma once

namespace Tmpl8
{
    //forward declarations
    class Tank;
    class Rocket;
    class Smoke;
    class Particle_beam;

    class Game
    {
    public:
        void set_target(Surface* surface) { screen = surface; }
        void init();
        void update_tanks_multithreaded();
        void update_rockets_multithreaded();
        void update_tanks_partial(int currentloop, int portion);
        void update_rockets_partial(int currentloop, int portion, int remainder);
        static void shutdown();
        void update();
        static void calc_route_singlethread(vector<Tank>& t,const int& position,const int& portion);void draw();
        void tick();void calculate_route_multithreaded(vector<Tank>& t);
        static void insertion_sort_tanks_health(const std::vector<Tank>& original,
                                                std::vector<const Tank*>& sorted_tanks, int begin, int end);
        void draw_health_bars(const std::vector<Tank*>& sorted_tanks, const int team) const;
        void measure_performance();

        Tank& find_closest_enemy(const Tank& current_tank);

        template <typename T, typename Function> //perhaps delete const for predicate later if needed
        static std::vector<T*> merge_sort(std::vector<T>& original, int begin, int end,
                                                       const Function predicate);
        template <typename T, typename Function> //perhaps delete const for predicate later if needed
        static std::vector<T*> merge(std::vector<T*> left, std::vector<T*> right,
                                     const Function predicate);

        void mouse_up(int button)
        {
            /* implement if you want to detect mouse button presses */
        }

        void mouse_down(int button)
        {
            /* implement if you want to detect mouse button presses */
        }

        void mouse_move(int x, int y)
        {
            /* implement if you want to detect mouse movement */
        }

        void key_up(int key)
        {
            /* implement if you want to handle keys */
        }

        void key_down(int key)
        {
            /* implement if you want to handle keys */
        }

    private:
        Surface* screen;

        vector<Tank> tanks;
        vector<Rocket> rockets;
        vector<Smoke> smokes;
        vector<Explosion> explosions;
        vector<Particle_beam> particle_beams;

        Terrain background_terrain;
        std::vector<vec2> forcefield_hull;

        Font* frame_count_font;
        long long frame_count = 0;

        bool lock_update = false;

        //Checks if a point lies on the left of an arbitrary angled line
        static bool left_of_line(vec2 line_start, vec2 line_end, vec2 point);
        void collision();
        void update_tanks();
        void find_first_active_tank(uint16_t& first_active) const;
        void convex_hull();
        void calculate_convex_hull();
        void update_rocket();
        void rocket_hits_convex();
        void update_particle_beams();
        void collision_tanks(vector<Tank>* tankies, uint8_t depth);
    };
}; // namespace Tmpl8
