#include "IStrategy.h"
#include "Players.h"
#include <random>
#include <array>

using namespace std;

static std::mt19937_64 my_random{std::random_device{}()};

std::string IStrategy::step(NetworkState& turnData, const GameContext& ctx) {
    static auto tc = 0;
    std::cerr << "Turn " << tc++ << std::endl;
    return "PASS";
}

int IStrategy::getRandom(const int from, const int to_exclusive)
{
    const auto d = static_cast<unsigned long long>(to_exclusive - from);
    return static_cast<int>(my_random()  % d) + from;
}

ll IStrategy::getRandomLL(const ll from, const ll to_exclusive)
{
    const auto d = static_cast<unsigned long long>(to_exclusive - from);
    return static_cast<ll>(my_random()  % d) + from;
}

void IStrategy::stepPre(const NetworkState& turnData, const GameContext& ctx)
{
    if (ourId == NROUTERS)
        ourId = static_cast<int>(ctx.commandPrefix.routerId);
    actualData = &turnData;
}

std::vector<Command> IStrategy::getPossibleMoves(const NetworkState& turnData, bool pass, bool create, bool move) const
{
    return Player::getPossibleMoves(turnData, ourId, pass, move, create ? _requestCounter : -1);
}

std::string IStrategy::command_execute(const Command& c)
{
    if (c.t == Command::CREATE && c.i.create.packageId == _requestCounter)
        ++_requestCounter;
    return c.to_exec_string();
}
