#include "turnData.h"

void readData(Reader &to) {
    std::string line;
    to.dataArray.clear();
    to.receivedPieces.clear();

    while (std::getline(std::cin, line)) {
        if (!line.rfind(".", 0))
            return;

        if (!line.rfind("WRONG", 0) ||
                !line.rfind("SCORE", 0) ||
                !line.rfind("TICK", 0))
        {
            to.hasEnd = true;
            to.previous = std::move(line);
        } else if (!line.rfind("REQUEST", 0)) {
            std::stringstream(std::move(line).substr(8)) >> to.data[0] >> to.data[1] >> to.data[2];
        } else if (!line.rfind("PREVIOUS", 0)) {
            to.previous = std::move(line).substr(9);
        } else if (!line.rfind("ROUTER", 0)) {
            unsigned int routerIndex;
            std::istringstream(line.substr(7)) >> routerIndex >> line;
            auto it = line.begin();
            for (bool& routers : to.routerBits[routerIndex])
                routers = *it++ == '1';
        } else if (!line.rfind("DATA", 0)) {
            Data& curr = to.dataArray.emplace_back();
            std::istringstream(std::move(line).substr(5))
                    >> curr.currRouter
                    >> curr.currStoreId
                    >> curr.dataIndex
                    >> curr.messageId
                    >> curr.fromRouter
                    >> curr.toRouter
                    >> reinterpret_cast<char&>(curr.dir);
        } else if (!line.rfind("MESSAGE")) {
            MessagePiece& msg = to.receivedPieces.emplace_back();
            std::istringstream(std::move(line).substr(8)) >> msg.index >> msg.message;
        } else {
            std::cerr << "READER ERROR HAPPENED: unrecognized command line: " << line << std::endl;
            to.hasEnd = true;
            return;
        }
    }
    std::cerr << "Unexpected input end." << std::endl;
    to.hasEnd = true;
}
