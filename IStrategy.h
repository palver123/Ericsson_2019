#pragma once

#include "turnData.h"
#include "basics.h"
#include <string>
#include "commands.h"

class IStrategy
{
public:
    virtual std::string step(NetworkState&, const GameContext&);
    virtual ~IStrategy() = default;

protected:
    int ourId = NROUTERS;
    const NetworkState* actualData = nullptr;

    virtual void stepPre(const NetworkState& turnData, const GameContext& ctx);
    virtual std::vector<Command> getPossibleMoves(const NetworkState& turnData, bool pass, bool move, bool create) const;
    std::string command_execute(const Command& c);

private:
    int _requestCounter = 0;
};
