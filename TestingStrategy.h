#pragma once

#include "strategy.h"

class TestingStrategy : public IStrategy
{
    int _requestCounter{};

public:
    TestingStrategy() = default;

    // IStrategy interface
    std::string step(const GameState&, const Context&) override;
};
