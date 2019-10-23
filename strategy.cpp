#include "strategy.h"

std::string IStrategy::step() {
    static int tc = 0;
    std::cerr << "Turn " << tc++ << std::endl;
    return "PASS";
}
