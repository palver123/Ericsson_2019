#include "turnData.h"

using namespace std;

bool Data::is_request() const
{
    return toRouter != fromRouter;
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

void NetworkState::clear()
{
    dataPackets.clear();
}
