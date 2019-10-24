#include "TestingStrategy.h"

std::string TestingStrategy::step(const Reader &turnData)
{
    return fmt::format("MOVE {} {}", getRandom(0,14), getRandom(0,1) ? "^" : "v");
}
