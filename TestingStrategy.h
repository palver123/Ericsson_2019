#pragma once

#include "IStrategy.h"

class TestingStrategy : public IStrategy
{
    int _requestCounter{};

public:
    TestingStrategy() = default;

    // IStrategy interface
    std::string step(const NetworkState&, const GameContext&) override;
};
