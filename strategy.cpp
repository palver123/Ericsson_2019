#include "strategy.h"

#include <random>

static std::mt19937_64 my_random{std::random_device{}()};

std::string IStrategy::step(Reader& turnData) {
    static int tc = 0;
    std::cerr << "Turn " << tc++ << std::endl;
    return "PASS";
}

int IStrategy::getRandom(int from, int to_exclusive)
{
    unsigned long long d = static_cast<unsigned long long>(to_exclusive - from);
    return static_cast<int>(my_random()  % d) + from;
}

ll IStrategy::getRandomLL(ll from, ll to_exclusive)
{
    unsigned long long d = static_cast<unsigned long long>(to_exclusive - from);
    return static_cast<ll>(my_random()  % d) + from;
}

int IStrategy::getNumberOfPlayerPackets(int routerIdx)
{
    int res = 0;
    for (const auto& packet : actualData->dataArray) {
        if (packet.fromRouter == routerIdx) ++res;
    }
    return res;
}

void IStrategy::stepPre(Reader& turnData)
{
    actualData = &turnData;
}
