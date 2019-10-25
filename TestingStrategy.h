#pragma once

#include "strategy.h"

class TestingStrategy : public IStrategy
{
public:
    TestingStrategy() = default;

    // IStrategy interface
    std::string step(Reader &turnData) override;
};
