
#include "../lux/constants.hpp"

using namespace lux;
class Utils
{
    public:
    static float Abs(float value)
    {
        return value < 0.0f ? -value : value;
    }

    static bool IsInMap(const lux::Position& position, const lux::GameMap& map)
    {
        return 
            0 <= position.x && position.x < map.width &&
            0 <= position.y && position.y < map.height;
    }

    static DIRECTIONS GetOppositeDirection(DIRECTIONS dir)
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
};