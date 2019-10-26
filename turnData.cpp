#include "turnData.h"
#include <iostream>
#include <sstream>

using namespace std;

map<int, MessagePiece> Reader::_allReceivedPieces;
int Reader::_lowestEmptyAnswer = 1;

bool Data::is_request() const
{
    return toRouter != fromRouter;
}

bool Reader::hasReceivedEmptyMessage()
{
    return _lowestEmptyAnswer >= 0;
}

void Reader::OnMessageReceived(const MessagePiece& msg)
{
    _allReceivedPieces.emplace(msg.index, msg);
    if (msg.message.empty() && (!hasReceivedEmptyMessage() || msg.index < _lowestEmptyAnswer))
        _lowestEmptyAnswer = msg.index;
}

bool readData(Reader &to) 
{
    string line;
    to.dataArray.clear();
    to.receivedPieces.clear();
    auto gameOver = false;

    while (getline(cin, line)) {
        if (!line.rfind('.', 0))
            return !gameOver;

        if (!line.rfind("WRONG", 0) ||
            !line.rfind("SCORE", 0) ||
            !line.rfind("TICK", 0))
        {
            gameOver = true;
            to.previous = move(line);
        }
        else if (!line.rfind("REQUEST", 0)) {
            stringstream(move(line).substr(8))
                >> to.commandPrefix.gameId
                >> to.commandPrefix.tickId
                >> to.commandPrefix.routerId;
        }
        else if (!line.rfind("PREVIOUS", 0)) {
            to.previous = move(line).substr(9);
        }
        else if (!line.rfind("ROUTER", 0)) {
            unsigned int routerIndex;
            istringstream(line.substr(7)) >> routerIndex >> line;
            auto it = line.begin();
            for (bool& routers : to.routerBits[routerIndex])
                routers = *it++ == '1';
        }
        else if (!line.rfind("DATA", 0)) {
            auto& curr = to.dataArray.emplace_back();
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
            auto& msg = to.receivedPieces.emplace_back();
            istringstream(move(line).substr(8)) >> msg.index >> msg.message;
            Reader::OnMessageReceived(msg);
        }
        else {
            cerr << "READER ERROR HAPPENED: unrecognized command line: " << line << endl;
            return false;
        }
    }

    cerr << "Unexpected input end." << endl;
    return false;
}
