#pragma once

#include "turnData.h"
#include "commands.h"


NetworkState simulate(const NetworkState&, const std::vector<CreateCommand>&, const std::vector<MoveCommand>&);
