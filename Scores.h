#pragma once

#include "turnData.h"

class Scores
{
public:
    static double distance_based_scoring_change_handling(const NetworkState& state);
    static double future_seeing(const NetworkState& state);
};

