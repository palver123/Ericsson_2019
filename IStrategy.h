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
    static int getRandom(int from, int to_exclusive);
    static ll getRandomLL(ll from, ll to_exclusive);

    int ourId = NROUTERS;
    const NetworkState* actualData = nullptr;
    virtual void stepPre(const NetworkState& turnData, const GameContext& ctx);
    virtual std::vector<Command> getPossibleMoves(const NetworkState& turnData, bool pass, bool move, bool create) const;
    int _requestCounter = 0;
    std::string command_execute(const Command& c);
};
