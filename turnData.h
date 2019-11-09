#pragma once

#include <array>
#include <string>
#include <vector>
#include <map>
#include "constants.h"
#include "directions.h"

struct Data {
    unsigned int currRouter;
    unsigned int currStoreId;
    unsigned int dataIndex;
    unsigned int messageId;
    unsigned int fromRouter;
    unsigned int toRouter;
    HorizontalDirection dir;

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

// The state of the network consisting of 10 routers organized in a Daisy Chain.
struct NetworkState
{
    // Bitfield for routers: 0 means the slot is closed, 1 means open.
    std::array<std::array<bool, NSLOTS>, NROUTERS> routerBits;
    std::vector<Data> dataPackets;

    void clear();
};

// Context of the whole game from the first tick to the last (kind of a 'global state')
struct GameContext
{
    CommandDescription commandPrefix;

    // The lowest known index of a package whose answer we have received from the target computer and it was empty. (Empty answers signal the end of the message)
    int _lowestEmptyAnswer = -1;

    // ALL the received answers to our requests so far
    std::map<int, MessagePiece> _allReceivedPieces;

    // The ID of the BOT's router. By default it contains an invalid ID (which will be overwritten when the BOT creates its first data packet)
    unsigned botRouterId = NROUTERS;

    unsigned my_router() const;
    bool have_all_message_pieces() const;
    bool hasReceivedEmptyMessage() const;
    void OnMessageReceived(const MessagePiece&);
};
