#include <iostream>
#include <sstream>
#include "Reader.h"

using namespace std;

bool Reader::readData(NetworkState& state, GameContext& ctx)
{
    state.clear();

    auto gameOver = false;
    string line;

    while (getline(cin, line)) {
        if (!line.rfind('.', 0))
            return !gameOver;

        if (!line.rfind("WRONG", 0) ||
            !line.rfind("SCORE", 0) ||
            !line.rfind("SLOW", 0) ||
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
            auto& curr = state.dataPackets.emplace_back();
            istringstream(move(line).substr(5))
                >> curr.currRouter
                >> curr.currStoreId
                >> curr.dataIndex
                >> curr.messageId
                >> curr.fromRouter
                >> curr.toRouter
                >> reinterpret_cast<char&>(curr.dir);
            if (curr.fromRouter != static_cast<int>(ctx.commandPrefix.routerId))
            {
                // ASSUMPTION 1: There are only 2 players: the BOT and us
                // ASSUMPTION 2: REQUEST message always precedes every DATA message (so at this point we already know the ID of our router)
                GameContext::botRouterId = curr.fromRouter;
            }
        }
        else if (!line.rfind("MESSAGE")) {
            MessagePiece msg;
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
