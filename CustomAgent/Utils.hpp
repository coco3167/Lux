
#include "../lux/constants.hpp"
#include <memory>
#include <string>
#include <stdexcept>

using namespace lux;
class Utils
{
    public:
    static float Abs(float value)
    {
        return value < 0.0f ? -value : value;
    }

    static bool IsInMap(const Position& position, const GameMap& map)
    {
        return 
            0 <= position.x && position.x < map.width &&
            0 <= position.y && position.y < map.height;
    }

    static bool IsInMap(const int x,const int y, const GameMap& map)
    {
        return 
            0 <= x && x < map.width &&
            0 <= y && y < map.height;
    }

    static DIRECTIONS GetOppositeDirection(const DIRECTIONS dir)
    {
        switch (dir)
        {
            case DIRECTIONS::NORTH:
                return DIRECTIONS::SOUTH;
            case DIRECTIONS::EAST:
                return DIRECTIONS::WEST;
            case DIRECTIONS::SOUTH:
                return DIRECTIONS::NORTH;
            case DIRECTIONS::WEST:
                return DIRECTIONS::EAST;
            case DIRECTIONS::CENTER:
                return DIRECTIONS::CENTER;
        }
        return DIRECTIONS::CENTER;
    }


    template<typename ... Args>
    static std::string FormatString( const std::string& format, Args ... args )
    {
        int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
        auto size = static_cast<size_t>( size_s );
        std::unique_ptr<char[]> buf( new char[ size ] );
        std::snprintf( buf.get(), size, format.c_str(), args ... );
        return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    }

    static const CityTile* GetClosestCityTile(const Position& position, const City* city, GameMap& map, Player& player) 
    {
        std::vector<DIRECTIONS> path = {};
        path.reserve(10);

        size_t shortestPathLengh = 999999;
        const CityTile* closestCityTile = nullptr;

        for (const CityTile& tile : city->citytiles) 
        {
            PathFinder::FindPath(map, position, tile.pos, player, path);
            if (path.size() < shortestPathLengh) 
            {
                shortestPathLengh = path.size();
                closestCityTile = &tile;
            }
            path.clear();
        }

        return closestCityTile;
    }
};