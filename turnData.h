#pragma once

#include <array>
#include <string>
#include <vector>
#include "constants.h"

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

    bool is_request() const;
};

struct MessagePiece {
    int index;
    std::string message;
};

struct Reader {
    static std::vector<MessagePiece> _allReceivedPieces;

    std::array<unsigned int, 3> data;
    std::string previous;
    std::array<std::array<bool, NSLOTS>, NROUTERS> routerBits;
    std::vector<Data> dataArray;
    std::vector<MessagePiece> receivedPieces;
    bool hasEnd;
    bool gotEmptyMessagePiece;
};


void readData(Reader& to);
