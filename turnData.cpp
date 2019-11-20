#include "turnData.h"


#include <algorithm>
#include <set>
#include <iterator>
using namespace std;

bool Data::is_request() const
{
    return toRouter != fromRouter;
}

int Data::distance_from_target() const
{
    if (dir == HorizontalDirection::LEFT)
        return (currRouter - toRouter + NROUTERS) % NROUTERS;
    else
        return (toRouter - currRouter + NROUTERS) % NROUTERS;
}

int Data::will_disappear() const
{
    return fromRouter == toRouter && currRouter == toRouter;
}

bool GameContext::PlayerPackets::receivedEmptyPacket() const
{
    return lowestEmptyAnswer != -1
        && !received.empty()
        && *received.rbegin() >= lowestEmptyAnswer;
}

map<int, GameContext::PlayerPackets> GameContext::playerPackets;

bool GameContext::have_all_message_pieces() const
{
    auto highestReceivedIdx = -1;
    for (const auto& receivedPiece : _allReceivedPieces)
    {
        if (receivedPiece.first - highestReceivedIdx > 1)
            return false; // A packet is missing
        highestReceivedIdx = receivedPiece.first;
    }

    return highestReceivedIdx == playerPackets[ourId()].lowestEmptyAnswer - 1;
}


void GameContext::OnMessageReceived(const MessagePiece& msg)
{
    if (msg.message.empty())
    {
        auto& ourPackets = playerPackets[ourId()];
        if ((ourPackets.lowestEmptyAnswer < 0 || msg.index < ourPackets.lowestEmptyAnswer))
            ourPackets.lowestEmptyAnswer = msg.index;
    }
    else
        _allReceivedPieces.emplace(msg.index, msg);
}

int GameContext::ourId() const
{
    return commandPrefix.routerId;
}

void GameContext::refreshPlayerPackets(const NetworkState& state)
{
    map<int, set<int> > active;
    for(const auto& p: state.dataPackets) {
        auto playerId = p.fromRouter;
        playerPackets.emplace(playerId, PlayerPackets{});
        active[playerId].insert(p.messageId);
    }

    for(auto& it : playerPackets) {
        auto playerId = it.first;
        auto& activeOld = it.second.active;
        auto& activeNew = active[playerId];
        set<int> receivedNow;
        set_difference(activeOld.begin(), activeOld.end(), activeNew.begin(), activeNew.end(),
            inserter(receivedNow, receivedNow.end()));
        it.second.received.insert(receivedNow.begin(), receivedNow.end());
        activeOld = move(activeNew);
    }
}

int NetworkState::getNumberOfPlayerPackets(const int routerOfPlayer, bool skip_arrived) const
{
    auto res = 0;
    for (const auto& packet : dataPackets) {
        if (packet.fromRouter == routerOfPlayer && (!skip_arrived || !packet.will_disappear()))
            ++res;
    }
    return res;
}

void NetworkState::clear()
{
    dataPackets.clear();
}
