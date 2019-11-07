#pragma once

#include <string>
#include "Direction.h"

struct Command{};

struct CreateCommand
{
    int storeId;
    int packageId;
};

struct MoveCommand
{
    unsigned routerId;
    Direction dir;
};

struct GuessCommand
{
    std::string solution;
};
