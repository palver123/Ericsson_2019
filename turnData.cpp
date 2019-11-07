#include "turnData.h"
#include <iostream>
#include <sstream>

using namespace std;

bool Data::is_request() const
{
    return toRouter != fromRouter;
}

bool Context::have_all_message_pieces() const
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


bool Context::hasReceivedEmptyMessage() const
{
    return _lowestEmptyAnswer >= 0;
}

void Context::OnMessageReceived(const MessagePiece& msg)
{
    _allReceivedPieces.emplace(msg.index, msg);
    if (msg.message.empty() && (!hasReceivedEmptyMessage() || msg.index < _lowestEmptyAnswer))
        _lowestEmptyAnswer = msg.index;
}

void GameState::clear()
{
    dataArray.clear();
}

void Reader::reset()
{
    receivedPieces.clear();
}


bool Reader::readData(GameState& state, Context& ctx)
{
    state.clear();
    reset();

    auto gameOver = false;
    string line;

    while (getline(cin, line)) {
        if (!line.rfind('.', 0))
            return !gameOver;

        if (!line.rfind("WRONG", 0) ||
            !line.rfind("SCORE", 0) ||
            !line.rfind("TICK", 0))
        {
            gameOver = true;
            previous = move(line);
        }
        else if (!line.rfind("REQUEST", 0)) {
            stringstream(move(line).substr(8))
                >> ctx.commandPrefix.gameId
                >> ctx.commandPrefix.tickId
                >> ctx.commandPrefix.routerId;
        }
        else if (!line.rfind("PREVIOUS", 0)) {
            previous = move(line).substr(9);
        }
        else if (!line.rfind("ROUTER", 0)) {
            unsigned int routerIndex;
            istringstream(line.substr(7)) >> routerIndex >> line;
            auto it = line.begin();
            for (auto& routers : state.routerBits[routerIndex])
                routers = *it++ == '1';
        }
        else if (!line.rfind("DATA", 0)) {
            auto& curr = state.dataArray.emplace_back();
            istringstream(move(line).substr(5))
                >> curr.currRouter
                >> curr.currStoreId
                >> curr.dataIndex
                >> curr.messageId
                >> curr.fromRouter
                >> curr.toRouter
                >> reinterpret_cast<char&>(curr.dir);
        }
        else if (!line.rfind("MESSAGE")) {
            auto& msg = receivedPieces.emplace_back();
            istringstream(move(line).substr(8)) >> msg.index >> msg.message;
            ctx.OnMessageReceived(msg);
        }
        else {
            cerr << "READER ERROR HAPPENED: unrecognized command line: " << line << endl;
            return false;
        }
    }

    cerr << "Unexpected input end." << endl;
    return false;
}
