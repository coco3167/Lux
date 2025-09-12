#pragma once

#include <vector>
#include <string.h>

#include "lux/annotate.hpp"

#include "Utils/Alias.h"

#include "lux/constants.hpp"
#include "lux/position.hpp"

using namespace lux;
class Annotator
{
public:
    static void TracePath(const Position& startPosition, const Path& path, std::vector<string>& actions)
    {
        Position previousPosition = startPosition;
        Position nextPosition = startPosition;

        int pathSize = path.size() - 1;
        for (int iDir = 0; iDir < pathSize; ++iDir)
        {
            nextPosition = nextPosition.translate(path[iDir], 1);
            actions.push_back(Annotate::line(previousPosition.x, previousPosition.y, nextPosition.x, nextPosition.y));
            previousPosition = nextPosition;
        }
    }
};