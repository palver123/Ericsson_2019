#pragma once

#include <string>
#include "directions.h"

struct Command{};

struct CreateCommand
{
    // TODO maintain
    int routerId = -1; // We need it to simulation (If the bot can, then it will create a new message), we can use this information in the simulation.
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
