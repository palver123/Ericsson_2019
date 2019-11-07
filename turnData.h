#pragma once

#include <array>
#include <string>
#include <vector>
#include "constants.h"
#include <map>

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

    // Is the content of the data package a request or an answer to a request
    bool is_request() const;
};

struct MessagePiece {
    int index;
    std::string message;
};

// Used to prefix our commands like this: <game-id> <tick-id> <router-id> COMMAND...
struct CommandDescription
{
    unsigned gameId;
    unsigned tickId;
    unsigned routerId;
};

struct GameState
{
    // Bitfield for routers: 0 means the slot is closed, 1 means open.
    std::array<std::array<bool, NSLOTS>, NROUTERS> routerBits;
    std::vector<Data> dataArray;

    void clear();
};

struct Context
{
    CommandDescription commandPrefix;

    // The lowest known index of a package whose answer we have received from the target computer and it was empty. (Empty answers signal the end of the message)
    int _lowestEmptyAnswer = -1;

    // ALL the received answers to our requests so far
    std::map<int, MessagePiece> _allReceivedPieces;

    bool have_all_message_pieces() const;
    bool hasReceivedEmptyMessage() const;
    void OnMessageReceived(const MessagePiece&);
};

struct Reader {
    // An optional field that contains a message from the server to our previous command. For example: 'PREVIOUS Wrong solution string.'
    std::string previous;

    // Answers received IN THE CURRENT TURN.
    std::vector<MessagePiece> receivedPieces;

    bool readData(GameState&, Context&);

private:
    void reset();
};