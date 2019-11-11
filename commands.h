#pragma once

#include <string>
#include "fmt/format.h"
#include "directions.h"

struct Command{};

struct CreateCommand
{
    // TODO maintain
    int routerId = NROUTERS; // We need it to simulation (If the bot can, then it will create a new message), we can use this information in the simulation.
    int storeId;
    int packageId;
    std::string to_exec_string() const {
        return fmt::format("CREATE {} {}", storeId, packageId);
    }
};

struct MoveCommand
{
    int routerId;
    VerticalDirection dir;

    std::string to_exec_string() const {
        return fmt::format("MOVE {} {}", routerId, dir);
    }
};

struct GuessCommand
{
    std::string solution;
};
