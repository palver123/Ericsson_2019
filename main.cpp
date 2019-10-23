#include <array>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include "strategy.h"

enum class Direction : char {
    LEFT = 'l',
    RIGHT = 'r'
};

struct Data {
    unsigned int currRouter;
    unsigned int currStoreId;
    unsigned int dataIndex;
    unsigned int messageId;
    unsigned int fromRouter;
    unsigned int toRouter;
    Direction dir;
};

struct MessagePiece {
    int index;
    std::string message;
};

struct Reader {    
    std::array<unsigned int, 3> data;
    std::string previous;
    std::array<std::array<bool, 10>, 14> routerBits;
    std::vector<Data> dataArray;
    std::vector<MessagePiece> receivedPieces;
    bool hasEnd;
};


void readData(Reader& to) {
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

int main() {
    char teamToken[] = "fQFUQhDn4LTMqWgA59Sz";
    // int seed = 0;
    
    std::cout << "START " << teamToken 
        // << " " << seed 
        << std::endl;

    Reader reader = {};

    IStrategy my_stat;
    
    while(true) {
        readData(reader);
        
        if (reader.hasEnd)
            break;
            
        // TODO logika jobb mint a semmitteves
        std::string command = my_stat.step();
        
        // Ha szeretnetek debug uzenetet kuldeni, akkor megtehetitek.
        // Vigyazzatok, mert maximalisan csak 1024 * 1024 bajtot kaptok vissza
        std::cerr << "Send " << command << std::endl;
       
        // standard out-ra meg mehet ki a megoldas! Mas ne irodjon ide ki ;)
        std::cout << reader.data[0] << " " << reader.data[1] << " " << reader.data[2] << " " << command << std::endl;
    }
    std::cerr << "END (latest message): " << reader.previous << std::endl;
}
