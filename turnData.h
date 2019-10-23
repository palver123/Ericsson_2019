#pragma once

#include <array>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

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


void readData(Reader& to);
