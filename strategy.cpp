#include "strategy.h"

#include <random>

static std::mt19937_64 my_random{std::random_device{}()};

std::string IStrategy::step(const NetworkState& turnData, const GameContext& ctx) {
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

int IStrategy::getNumberOfPlayerPackets(const unsigned routerIdx) const
{
    auto res = 0;
    for (const auto& packet : actualData->dataPackets) {
        if (packet.fromRouter == routerIdx) 
            ++res;
    }
    return res;
}

void IStrategy::stepPre(const NetworkState& turnData)
{
    actualData = &turnData;
}
