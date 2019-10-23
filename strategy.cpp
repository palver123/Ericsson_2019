#include "strategy.h"

std::string IStrategy::step(const Reader& turnData) {
    static int tc = 0;
    std::cerr << "Turn " << tc++ << std::endl;
    return "PASS";
}
