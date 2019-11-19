#pragma once

#include "turnData.h"
#include "commands.h"


NetworkState simulate(const NetworkState&, const std::vector<Command>&, int);


void run_sim_tests();