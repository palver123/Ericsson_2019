#pragma once

#include "strategy.h"

class TestingStrategy : public IStrategy
{
public:
    TestingStrategy();

    // IStrategy interface
public:
    std::string step(const Reader &turnData) override;
};
