#pragma once

#include <array>
#include <string>
#include <vector>
#include <map>
#include "constants.h"
#include "directions.h"
#include <set>

struct Data {
    int currRouter;
    int currStoreId;
    int dataIndex;
    int messageId;
    int fromRouter;
    int toRouter;
    HorizontalDirection dir;
    bool uperPrio = false;

    // Is the content of the data package a request or an answer to a request
    bool is_request() const;
    int distance_from_target() const;
    int will_disappear() const;
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

// Additional infos for simulation
struct SimuInfo
{
    int additionalArrivedReq = 0;
    int additionalArrivedResp = 0;
    int additionalEnemyArrivedReq = 0;
    int additionalEnemyArrivedResp = 0;
    std::array<std::array<bool, NSLOTS>, NROUTERS> routerBitsOccupied;
};

// The state of the network consisting of 10 routers organized in a Daisy Chain.
struct NetworkState
{
    // Bitfield for routers: 0 means the slot is closed, 1 means open.
    std::array<std::array<bool, NSLOTS>, NROUTERS> routerBits;
    std::vector<Data> dataPackets;
    SimuInfo simuInfo;
    std::array<HorizontalDirection, NROUTERS> nextDir; // TODO maintain

    int getNumberOfPlayerPackets(int routerOfPlayer, bool skip_arrived = false) const;
    void clear();
};

// Context of the whole game from the first tick to the last (kind of a 'global state')
struct GameContext
{
    CommandDescription commandPrefix;

    // The lowest known index of a package whose answer we have received from the target computer and it was empty. (Empty answers signal the end of the message)
    static int lowestEmptyAnswer;

    // Info about the data packets belonging to a player
    struct PlayerPackets {
        // The indices of the message pieces that the player has received since the beginning of the game
        std::set<int> received;

        // The data packets that belong to the player and are present in the current network (on one of the routers).
        std::set<int> active;

    };

    static std::map<int, PlayerPackets> playerPackets;
    static void refreshPlayerPackets(const NetworkState& state);

    // ALL the non-empty received answers to our requests so far
    std::map<int, MessagePiece> _allReceivedPieces;
    static bool receivedEmptyPacket(int playerId);
    bool have_all_message_pieces() const;
    void OnMessageReceived(const MessagePiece&);
};

namespace Router {
    inline int GetPairOfRouter(int router_idx) {
        return (router_idx + NROUTERS / 2) % NROUTERS;
    }
}