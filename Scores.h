#pragma once

#include "turnData.h"

typedef double (*scoringFuction)(const NetworkState&, int playerId);

class Scores
{
public:
    static double distance_based_scoring_change_handling(const NetworkState& state, int playerId);
    static double future_seeing(const NetworkState& state, int playerId);
};

