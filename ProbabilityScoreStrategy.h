#pragma once

#include "IStrategy.h"

class ProbabilityScoreStrategy : public IStrategy
{
    int _requestCounter{};
    std::vector<NetworkState> possible_states_after(const NetworkState&);

public:
    virtual std::string step(const NetworkState&, const GameContext&) override;

};

