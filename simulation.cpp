#include "simulation.h"

using namespace std;

void remove_arrived_packets(NetworkState& state)
{    
}

void add_new_packets(NetworkState& state, const vector<CreateCommand>& commands)
{    
}

void apply_move_commands(NetworkState& state, const vector<MoveCommand>& commands)
{    
}

bool step_packets(NetworkState& state, vector<bool>& isPacketFrozen)
{
    return false;
}

NetworkState simulate(const NetworkState& initialState, const vector<CreateCommand>& createCmds, const vector<MoveCommand>& moveCmds)
{
    auto retVal{ initialState };

    remove_arrived_packets(retVal);
    add_new_packets(retVal, createCmds);
    apply_move_commands(retVal, moveCmds);

    vector<bool> isPacketFrozen; // If a packet arrives in its destination it is 'frozen' until the next simulate call
    fill_n(back_inserter(isPacketFrozen), retVal.dataPackets.size(), false);
    while (step_packets(retVal, isPacketFrozen)) { }

    return retVal;
}
