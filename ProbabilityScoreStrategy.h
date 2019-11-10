#pragma once

#include "IStrategy.h"

class ProbabilityScoreStrategy : public IStrategy
{
    int _requestCounter{};
public:
    virtual std::string step(const NetworkState&, const GameContext&) override;

};

