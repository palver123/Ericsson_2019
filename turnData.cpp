#include "turnData.h"

using namespace std;

bool Data::is_request() const
{
    return toRouter != fromRouter;
}

unsigned GameContext::my_router() const
{
    return commandPrefix.routerId;
}

bool GameContext::have_all_message_pieces() const
{
    auto highestReceivedIdx = -1;
    for (const auto& receivedPiece : _allReceivedPieces)
    {
        if (receivedPiece.first - highestReceivedIdx > 1)
            return false; // A packet is missing
        highestReceivedIdx = receivedPiece.first;
    }

    return highestReceivedIdx = _lowestEmptyAnswer - 1;
}

bool GameContext::hasReceivedEmptyMessage() const
{
    return _lowestEmptyAnswer >= 0;
}

void GameContext::OnMessageReceived(const MessagePiece& msg)
{
    _allReceivedPieces.emplace(msg.index, msg);
    if (msg.message.empty() && (!hasReceivedEmptyMessage() || msg.index < _lowestEmptyAnswer))
        _lowestEmptyAnswer = msg.index;
}

int NetworkState::getNumberOfPlayerPackets(const unsigned routerOfPlayer) const
{
    auto res = 0;
    for (const auto& packet : dataPackets) {
        if (packet.fromRouter == routerOfPlayer)
            ++res;
    }
    return res;
}

int NetworkState::getNumberOfPlayerPackets(const unsigned routerOfPlayer, int& maxMessageId) const
{
    auto res = 0;
    maxMessageId = -1; // unsigned underflow is well-defined by the standard!
    for (const auto& packet : dataPackets) {
        if (packet.fromRouter == routerOfPlayer)
        {
            maxMessageId = std::max(maxMessageId, static_cast<int>(packet.messageId));
            ++res;
        }
    }
    return res;
}

void NetworkState::clear()
{
    dataPackets.clear();
}
