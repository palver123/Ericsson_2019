#pragma once

#include "turnData.h"
#include "commands.h"

NetworkState simulate(const NetworkState&, const CreateCommand&);
NetworkState simulate(const NetworkState&, const MoveCommand&);