#include "IStrategy.h"
#include "Players.h"
#include <array>

using namespace std;

std::string IStrategy::step(NetworkState& turnData, const GameContext& ctx) {
    static auto tc = 0;
    std::cerr << "Turn " << tc++ << std::endl;
    return "PASS";
}

void IStrategy::stepPre(const NetworkState& turnData, const GameContext& ctx)
{
    if (ourId == NROUTERS)
        ourId = static_cast<int>(ctx.commandPrefix.routerId);
    actualData = &turnData;
}

std::vector<Command> IStrategy::getPossibleMoves(const NetworkState& turnData, bool pass, bool move, bool create) const
{
    return Player::getPossibleMoves(turnData, ourId, pass, move, create ? _requestCounter : -1);
}

std::string IStrategy::command_execute(const Command& c)
{
    if (c.t == Command::CREATE && c.i.create.packageId == _requestCounter)
        ++_requestCounter;
    return c.to_exec_string();
}
