#pragma once

#include "IStrategy.h"

class ProbabilityScoreStrategy : public IStrategy
{
    int _requestCounter{};
    static std::string getBestMoveInNextTurn(const NetworkState&, double scoringFunc(const NetworkState&));

public:
    virtual std::string step(const NetworkState&, const GameContext&) override;

};

