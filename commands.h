#pragma once

#include <string>
#include "directions.h"

struct Command{};

struct CreateCommand
{
    int storeId;
    int packageId;
};

struct MoveCommand
{
    unsigned routerId;
    VerticalDirection dir;
};

struct GuessCommand
{
    std::string solution;
};
