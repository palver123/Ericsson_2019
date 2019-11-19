#include "Players.h"
#include <array>
#include <set>
using namespace std;

std::vector<Command> Player::getPossibleMoves(const NetworkState& turnData, int ourId, bool pass,bool move, int create_req)
{
    std::vector<Command> cmds;
    if (pass)
        cmds.push_back(Command::Pass());
    if (create_req != -1 && turnData.getNumberOfPlayerPackets(ourId) < MAX_PACKETS_IN_SYSTEM && !GameContext::receivedEmptyPacket(ourId)) {
        // Try to ask for a new packet
        array<bool, NSLOTS> slotTaken{};
        for (const auto& data : turnData.dataPackets)
        {
            if (data.currRouter == ourId && !data.will_disappear())
                slotTaken[data.currStoreId] = true;
        }
        for (auto slot = 0; slot < NSLOTS; slot++)
            if (!slotTaken[slot] && turnData.routerBits[ourId][slot])
                cmds.push_back(Command::Create(ourId, slot, create_req));
    }
    if (move) {
        set<int> possibleRouters;
        for (const auto& d : turnData.dataPackets)
            if (d.fromRouter == ourId)
                possibleRouters.insert(d.currRouter);

        for (int i : possibleRouters)
        {
            cmds.push_back(Command::Move(i, VerticalDirection::NEGATIVE));
            cmds.push_back(Command::Move(i, VerticalDirection::POSITIVE));
        }
    }
    return cmds;
}
