#pragma once

#include "strategy.h"

class TestingStrategy : public IStrategy
{
public:
    TestingStrategy() = default;

    // IStrategy interface
public:
    std::string step(const Reader &turnData) override;
};
