#include <iostream>
#include <sstream>
#include "Reader.h"
#include "fmt/format.h"
#undef ERROR // fmt/format.h includes wingdi.h which reserves the word 'ERROR' for a silly macro

using namespace std;

#ifdef LOG_FOR_PYTHON_GUI
#include <fstream>
ofstream Log;

void Reader::openLogFile(int mapSeed)
{
    Log.open(fmt::format("../logs/comm{}.log", to_string(mapSeed)));
}
#endif

bool Reader::readData(NetworkState& state, GameContext& ctx)
{
    state.clear();

    auto gameOver = false;
    string line;

    while (getline(cin, line)) {
#ifdef LOG_FOR_PYTHON_GUI
        Log << line << endl;
#endif
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
            const bool isPri = line.find("PRI") != std::string::npos;
            istringstream(move(line).substr(5))
                >> curr.currRouter
                >> curr.currStoreId
                >> curr.dataIndex
                >> curr.messageId
                >> curr.fromRouter
                >> curr.toRouter
                >> reinterpret_cast<char&>(curr.dir);
            curr.uperPrio = isPri;
            line.clear();
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
