#pragma once

#include "IStrategy.h"
#include "commands.h"
#include "Players.h"
#include "Scores.h"
#include <memory>

class ProbabilityScoreStrategy : public IStrategy
{
    static std::vector<std::pair<double, Command> > getMovementScores(const NetworkState& state, const std::vector<Command>& moves, int ourId, const vector<std::shared_ptr<Player> >& players, scoringFuction scoring);
    Command getBestMove(const NetworkState& state, const std::vector<Command>& moves, const vector<std::shared_ptr<Player> >& players, scoringFuction scoring);
public:
    virtual std::string step(NetworkState&, const GameContext&) override;
};

