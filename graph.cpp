#include "graph.h"
#include "simulation.h"

using namespace std;

array<bool, NSLOTS> slotTakenMy{};
array<bool, NSLOTS> slotTakenBot{};
array<bool, NROUTERS> canMoveRouterMe{};
array<bool, NROUTERS> canMoveRouterBot{};

void prepate_global_variables(const vector<Data>& dataPackets)
{
    for (const auto& data : dataPackets)
    {
        if (data.currRouter == GameContext::ourId)
        {
            slotTakenMy[data.currStoreId] = true;
        }
        if (data.fromRouter == GameContext::ourId)
            canMoveRouterMe[data.currRouter] = true;
    }

    if (GameContext::botRouterId < NROUTERS)
    {
        for (const auto& data : dataPackets)
        {
            if (data.currRouter == GameContext::botRouterId)
                slotTakenBot[data.currStoreId] = true;
            if (data.fromRouter == GameContext::botRouterId)
                canMoveRouterBot[data.currRouter] = true;
        }
    }
}

vector<NetworkState> possible_states_after(const NetworkState& initialState)
{
    vector<CreateCommand> createCmds;
    vector<MoveCommand> moveCmds{1};
    vector<NetworkState> states;

    prepate_global_variables(initialState.dataPackets);
    array<VerticalDirection, 2> possibleDirsV = { VerticalDirection::NEGATIVE, VerticalDirection::POSITIVE };
    const auto knowBotRouter = GameContext::botRouterId < NROUTERS;
    auto maxMessageId_mine = -1;
    auto maxMessageId_bot = -1;
    const auto nPacketsMine = initialState.getNumberOfPlayerPackets(GameContext::ourId, maxMessageId_mine);
    const auto nPacketsBot = knowBotRouter ? initialState.getNumberOfPlayerPackets(GameContext::botRouterId, maxMessageId_bot) : MAX_PACKETS_IN_SYSTEM;

    // The system is guaranteed to move a router in an arbitrary direction
    for (moveCmds.back().routerId = 0; moveCmds.back().routerId < NROUTERS; ++moveCmds.back().routerId)
    {
        for (auto dirSys : possibleDirsV)
        {
            moveCmds.back().dir = dirSys;

            // Simulate the case when I pass
            {
                // both me and the BOT pass
                states.push_back(simulate(initialState, createCmds, moveCmds));

                // the BOT creates a packet
                if (nPacketsBot < MAX_PACKETS_IN_SYSTEM)
                {
                    createCmds.push_back(CreateCommand{ static_cast<int>(GameContext::botRouterId), 0, maxMessageId_bot + 1 });
                    for (auto storeIdBot = 0; storeIdBot < NSLOTS; ++storeIdBot)
                    {
                        if (slotTakenBot[storeIdBot])
                            continue;

                        createCmds.back().storeId = storeIdBot;
                        states.push_back(simulate(initialState, createCmds, moveCmds));
                    }
                    createCmds.pop_back();
                }

                // the BOT moves a router
                moveCmds.push_back(MoveCommand{});
                for (auto rBot = 0; rBot < NROUTERS; ++rBot)
                {
                    if (!canMoveRouterBot[rBot])
                        continue;

                    moveCmds.back().routerId = rBot;
                    for (auto dirBot : possibleDirsV)
                    {
                        moveCmds.back().dir = dirBot;
                        states.push_back(simulate(initialState, createCmds, moveCmds));
                    }
                }
                moveCmds.pop_back();
            }

            if (nPacketsMine < MAX_PACKETS_IN_SYSTEM)
            {
                // Simulate the case when I create a packet
                createCmds.push_back(CreateCommand{ static_cast<int>(GameContext::ourId), 0, maxMessageId_mine + 1 });
                for (auto storeId = 0; storeId < NSLOTS; ++storeId)
                {
                    if (slotTakenMy[storeId])
                        continue;

                    createCmds.back().storeId = storeId;

                    // the BOT passes
                    states.push_back(simulate(initialState, createCmds, moveCmds));

                    // the BOT creates a packet too
                    if (nPacketsBot < MAX_PACKETS_IN_SYSTEM)
                    {
                        createCmds.push_back(CreateCommand{ static_cast<int>(GameContext::botRouterId), 0, maxMessageId_bot + 1 });
                        for (auto storeIdBot = 0; storeIdBot < NSLOTS; ++storeIdBot)
                        {
                            if (slotTakenBot[storeIdBot])
                                continue;

                            createCmds.back().storeId = storeIdBot;
                            states.push_back(simulate(initialState, createCmds, moveCmds));
                        }
                        createCmds.pop_back();
                    }

                    // the BOT moves a router
                    moveCmds.push_back(MoveCommand{});
                    for (auto rBot = 0; rBot < NROUTERS; ++rBot)
                    {
                        if (!canMoveRouterBot[rBot])
                            continue;

                        moveCmds.back().routerId = rBot;
                        for (auto dirBot : possibleDirsV)
                        {
                            moveCmds.back().dir = dirBot;
                            states.push_back(simulate(initialState, createCmds, moveCmds));
                        }
                    }
                    moveCmds.pop_back();
                }
                createCmds.pop_back();
            }

            // Simulate the case when I move a router
            {
                moveCmds.push_back(MoveCommand{});
                for (auto rMe = 0; rMe < NROUTERS; ++rMe)
                {
                    if (!canMoveRouterMe[rMe])
                        continue;

                    moveCmds.back().routerId = rMe;
                    for (auto dirMe : possibleDirsV)
                    {
                        moveCmds.back().dir = dirMe;

                        // the BOT passes
                        states.push_back(simulate(initialState, createCmds, moveCmds));

                        // the BOT creates a packet
                        if (nPacketsBot < MAX_PACKETS_IN_SYSTEM)
                        {
                            createCmds.push_back(CreateCommand{ static_cast<int>(GameContext::botRouterId), 0, maxMessageId_bot + 1 });
                            for (auto storeIdBot = 0; storeIdBot < NSLOTS; ++storeIdBot)
                            {
                                if (slotTakenBot[storeIdBot])
                                    continue;

                                createCmds.back().storeId = storeIdBot;
                                states.push_back(simulate(initialState, createCmds, moveCmds));
                            }
                            createCmds.pop_back();
                        }

                        // the BOT moves a router too
                        moveCmds.push_back(MoveCommand{});
                        for (auto rBot = 0; rBot < NROUTERS; ++rBot)
                        {
                            if (!canMoveRouterBot[rBot])
                                continue;

                            moveCmds.back().routerId = rBot;
                            for (auto dirBot : possibleDirsV)
                            {
                                moveCmds.back().dir = dirBot;

                                // the BOT passes
                                states.push_back(simulate(initialState, createCmds, moveCmds));
                            }
                        }
                        moveCmds.pop_back();
                    }
                }
                moveCmds.pop_back();                
            }
        }
    }
    
    return states;
}

void run_graph_tests()
{
}
