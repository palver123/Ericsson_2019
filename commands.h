#pragma once

#include <string>
#include "fmt/format.h"
#include "directions.h"


struct Command;

struct CreateCommand
{
    // TODO maintain
    int routerId; // We need it to simulation (If the bot can, then it will create a new message), we can use this information in the simulation.
    int storeId;
    int packageId;
    std::string to_exec_string() const {
        return fmt::format("CREATE {} {}", storeId, packageId);
    }
protected:
    CreateCommand() = default;
    friend struct Command;
};

struct MoveCommand
{
    int routerId;
    VerticalDirection dir;

    std::string to_exec_string() const {
        return fmt::format("MOVE {} {}", routerId, dir);
    }
protected:
    MoveCommand() = default;
    friend struct Command;
};

struct PassCommand
{
    std::string to_exec_string() const {
        return fmt::format("PASS");
    }
protected:
    PassCommand() = default;
    friend struct Command;
};

struct Command {
    enum Type {
        MOVE,
        CREATE,
        PASS,
    };
    Type t;
    union {
        CreateCommand create;
        MoveCommand move;
        PassCommand pass;
    } i;

    static Command Move(int routerId, VerticalDirection dir) { return Command{ MoveCommand{ routerId,dir } }; }
    static Command Create(int routerId, int storeId, int packageId) {
        return Command{ CreateCommand{ routerId,storeId,packageId } };
    }
    static Command Pass() { return Command{ PassCommand{} }; }


    std::string to_exec_string() const {
        if (t == MOVE)
            return i.move.to_exec_string();
        if (t == CREATE) 
            return i.create.to_exec_string();
        return i.pass.to_exec_string();
    }

protected:
    Command(const CreateCommand& p) :t(CREATE), i{ CreateCommand{} } { i.create = p; }
    Command(const MoveCommand& p) :t(MOVE), i{ CreateCommand{} } { i.move = p; }
    Command(const PassCommand& p) :t(PASS), i{ CreateCommand{} } { i.pass = p; }
};


struct GuessCommand
{
    std::string solution;
};
