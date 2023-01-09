#include "precomp.h"
#include "terrain.h"

namespace fs = std::filesystem;
namespace Tmpl8
{

    // Define the pair type for readability
    typedef std::pair<float, std::vector<TerrainTile*>> FloatVectorPair;
    //creating struct for comparison in prioritized queue. this is where the heuristic is defined. min heap sorting
    struct CompareDist {
        bool operator()(const FloatVectorPair& left, const FloatVectorPair& right) const {
            return left.first > right.first;

        }
    };
    
    Terrain::Terrain()
    {
        //Load in terrain sprites
        grass_img = std::make_unique<Surface>("assets/tile_grass.png");
        forest_img = std::make_unique<Surface>("assets/tile_forest.png");
        rocks_img = std::make_unique<Surface>("assets/tile_rocks.png");
        mountains_img = std::make_unique<Surface>("assets/tile_mountains.png");
        water_img = std::make_unique<Surface>("assets/tile_water.png");


        tile_grass = std::make_unique<Sprite>(grass_img.get(), 1);
        tile_forest = std::make_unique<Sprite>(forest_img.get(), 1);
        tile_rocks = std::make_unique<Sprite>(rocks_img.get(), 1);
        tile_water = std::make_unique<Sprite>(water_img.get(), 1);
        tile_mountains = std::make_unique<Sprite>(mountains_img.get(), 1);


        //Load terrain layout file and fill grid based on tiletypes
        fs::path terrain_file_path{ "assets/terrain.txt" };
        std::ifstream terrain_file(terrain_file_path);

        if (terrain_file.is_open())
        {
            std::string terrain_line;

            std::getline(terrain_file, terrain_line);
            std::istringstream lineStream(terrain_line);

            int rows;

            lineStream >> rows;

            for (size_t row = 0; row < rows; row++)
            {
                std::getline(terrain_file, terrain_line);

                for (size_t collumn = 0; collumn < terrain_line.size(); collumn++)
                {
                    switch (std::toupper(terrain_line.at(collumn)))
                    {
                    case 'G':
                        tiles.at(row).at(collumn).tile_type = TileType::GRASS;
                        break;
                    case 'F':
                        tiles.at(row).at(collumn).tile_type = TileType::FORREST;
                        break;
                    case 'R':
                        tiles.at(row).at(collumn).tile_type = TileType::ROCKS;
                        break;
                    case 'M':
                        tiles.at(row).at(collumn).tile_type = TileType::MOUNTAINS;
                        break;
                    case 'W':
                        tiles.at(row).at(collumn).tile_type = TileType::WATER;
                        break;
                    default:
                        tiles.at(row).at(collumn).tile_type = TileType::GRASS;
                        break;
                    }
                }
            }
        }
        else
        {
            std::cout << "Could not open terrain file! Is the path correct? Defaulting to grass.." << std::endl;
            std::cout << "Path was: " << terrain_file_path << std::endl;
        }

        //Instantiate tiles for path planning
        for (size_t y = 0; y < tiles.size(); y++)
        {
            for (size_t x = 0; x < tiles[y].size(); x++)
            {
                tiles[y][x].position_x = x;
                tiles[y][x].position_y = y;

                if (is_accessible(y, x + 1)) { tiles[y][x].exits.push_back(&tiles[y][x + 1]); }
                if (is_accessible(y, x - 1)) { tiles[y][x].exits.push_back(&tiles[y][x - 1]); }
                if (is_accessible(y + 1, x)) { tiles[y][x].exits.push_back(&tiles[y + 1][x]); }
                if (is_accessible(y - 1, x)) { tiles[y][x].exits.push_back(&tiles[y - 1][x]); }
            }
        }
    }

    void Terrain::update()
    {
        //Pretend there is animation code here.. next year :)
    }

    void Terrain::draw(Surface* target) const
    {

        for (size_t y = 0; y < tiles.size(); y++)
        {
            for (size_t x = 0; x < tiles.at(y).size(); x++)
            {
                const int posX = (x * sprite_size) + HEALTHBAR_OFFSET;
                const int posY = y * sprite_size;

                switch (tiles.at(y).at(x).tile_type)
                {
                case TileType::GRASS:
                    tile_grass->draw(target, posX, posY);
                    break;
                case TileType::FORREST:
                    tile_forest->draw(target, posX, posY);
                    break;
                case TileType::ROCKS:
                    tile_rocks->draw(target, posX, posY);
                    break;
                case TileType::MOUNTAINS:
                    tile_mountains->draw(target, posX, posY);
                    break;
                case TileType::WATER:
                    tile_water->draw(target, posX, posY);
                    break;
                default:
                    tile_grass->draw(target, posX, posY);
                    break;
                }
            }
        }
    }

    //Use Breadth-first search to find shortest route to the destination
    vector<vec2> Terrain::get_route(const Tank& tank, const vec2& target)
    {
        //Find start and target tile
        const size_t pos_x = tank.position.x / sprite_size;
        const size_t pos_y = tank.position.y / sprite_size;

        const size_t target_x = target.x / sprite_size;
        const size_t target_y = target.y / sprite_size;

        //Init queue with start tile
        std::queue<vector<TerrainTile*>> queue;
        queue.emplace();
        queue.back().push_back(&tiles.at(pos_y).at(pos_x));

        std::vector<TerrainTile*> visited;

        bool route_found = false;
        vector<TerrainTile*> current_route;
        while (!queue.empty() && !route_found)
        {
            current_route = queue.front();
            queue.pop();
            const TerrainTile* current_tile = current_route.back();

            //Check all exits, if target then done, else if unvisited push a new partial route
            for (TerrainTile * exit : current_tile->exits)
            {
                if (exit->position_x == target_x && exit->position_y == target_y)
                {
                    current_route.push_back(exit);
                    route_found = true;
                    break;
                }
                if (!exit->visited)
                {
                    exit->visited = true;
                    visited.push_back(exit);
                    queue.push(current_route);
                    queue.back().push_back(exit);
                }
            }
        }

        //Reset tiles
        for (TerrainTile * tile : visited)
        {
            tile->visited = false;
        }

        if (route_found)
        {
            //Convert route to vec2 to prevent dangling pointers
            std::vector<vec2> route;
            route.reserve(current_route.size());
            for (const TerrainTile* tile : current_route)
            {
                route.emplace_back((float)tile->position_x * sprite_size, (float)tile->position_y * sprite_size);
            }

            return route;
        }
        return {};
    }

    //TODO: Function not used, convert BFS to dijkstra and take speed into account next year :)
    float Terrain::get_speed_modifier(const vec2& position) const
    {
        const size_t pos_x = position.x / sprite_size;
        const size_t pos_y = position.y / sprite_size;

        switch (tiles.at(pos_y).at(pos_x).tile_type)
        {
        case TileType::GRASS:
            return 1.0f;
            break;
        case TileType::FORREST:
            return 0.5f;
            break;
        case TileType::ROCKS:
            return 0.75f;
            break;
        case TileType::MOUNTAINS:
            return 0.0f;
            break;
        case TileType::WATER:
            return 0.0f;
            break;
        default:
            return 1.0f;
            break;
        }
    }


    //for calculating the distance between 2 tiles
    float Terrain::get_distance_to_target(const TerrainTile* current_tile, const TerrainTile* destination ) const
    {
        return fabs((((float)destination->position_x) - ((float)current_tile->position_x)) + (((float)destination->position_y) - ((float)current_tile->position_y)));
    }

    //Use A* search to find shortest route to the destination
    vector<vec2> Terrain::a_star(const Tank& tank, const vec2& target)
    {
        //Find start and target tile
        const size_t pos_x = tank.position.x / sprite_size;
        const size_t pos_y = tank.position.y / sprite_size;

        const size_t target_x = target.x / sprite_size;
        const size_t target_y = target.y / sprite_size;

        //Init queue, using a priority que to implement A* heuristics. the heuristic is defined in CompareDist struct on the top of the file
        priority_queue<FloatVectorPair, vector<FloatVectorPair>, CompareDist> queue;
        //setting distance of starter tile
        //adding start tile
        queue.push({0, {&tiles.at(pos_y).at(pos_x)}});
        std::vector<TerrainTile*> visited;

        bool route_found = false;
        vector<TerrainTile*> current_route;
        while (!queue.empty() && !route_found)
        {
            current_route = queue.top().second;
            queue.pop();
            const TerrainTile* current_tile = current_route.back();
           
            //Check all exits, if target then done, else if unvisited push a new partial route
            for (TerrainTile* exit : current_tile->exits)
            {

                if (exit->position_x == target_x && exit->position_y == target_y)
                {
                    current_route.push_back(exit);
                    route_found = true;
                    break;
                }
                else if (std::find(visited.begin(), visited.end(), exit) != visited.end())
                {
                  //  exit->visited = true;
                    visited.push_back(exit);
                    float cost = get_distance_to_target(exit, &tiles.at(target_y).at(target_x));
                    current_route.push_back(exit);
                    queue.push({cost, current_route });
                    current_route.pop_back();
                }
            }
        }


        if (route_found)
        {
            //Convert route to vec2 to prevent dangling pointers
            std::vector<vec2> route;
            for (const TerrainTile* tile : current_route)
            {
                route.push_back(vec2(tile->position_x * sprite_size, tile->position_y * sprite_size));
            }

            return route;
        }
        else
        {
            return  std::vector<vec2>();
        }

    }

    
    bool Terrain::is_accessible(const int y, const int x) const
    {
        //Bounds check
        return (x >= 0 && x < terrain_width && (y >= 0 && y < terrain_height)) && tiles[y][x].tile_type != MOUNTAINS && tiles[y][x].tile_type != WATER;
    }
}