#include "ProbabilityScoreStrategy.h"
#include "commands.h"
#include "Scores.h"
#include "simulation.h"

using namespace std;

array<bool, NSLOTS> slotTakenMy{};
array<bool, NSLOTS> slotTakenBot{};
array<bool, NROUTERS> canMoveRouterMe{};
array<bool, NROUTERS> canMoveRouterBot{};

void prepare_global_variables(const vector<Data>& dataPackets)
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


string ProbabilityScoreStrategy::step(const NetworkState& turnData, const GameContext& ctx)
{
    stepPre(turnData,ctx);

    if (ctx.hasReceivedEmptyMessage())
    {
        // Guess the solution
        if (ctx.have_all_message_pieces())
        {
            // Determine solution length
            size_t solutionLength = 0;
            for (const auto& receivedPiece : ctx._allReceivedPieces)
                solutionLength += receivedPiece.second.message.size();

            // Concat the message pieces
            string guess{};
            guess.reserve(solutionLength);
            for (const auto& receivedPiece : ctx._allReceivedPieces)
                guess.append(receivedPiece.second.message);

            return "SOLUTION " + guess;
        }
    }
    // IMPORTANT: Assuming packets cannot get lost.
    // If we received at least 1 empty message then all the missing message pieces are somewhere in the network and will eventually get back to us
    else if (actualData->getNumberOfPlayerPackets(GameContext::ourId) < MAX_PACKETS_IN_SYSTEM)
    {
        // Try to ask for a new packet
        array<bool, NSLOTS> slotTaken{};
        for (const auto& data : turnData.dataPackets)
        {
            if (data.currRouter == GameContext::ourId)
                slotTaken[data.currStoreId] = true;
        }

        for (auto slot = 0; slot < NSLOTS; slot++)
            if (!slotTaken[slot] && turnData.routerBits[GameContext::ourId][slot])
                return fmt::format("CREATE {} {}", slot, _requestCounter++);
    }

    // Try to move

    set<int> possibleRouters;
    for (const auto& d : turnData.dataPackets)
        if (d.fromRouter == GameContext::ourId)
            possibleRouters.insert(d.currRouter);

    vector<MoveCommand> cmds;

    for (int i : possibleRouters)
    {
        cmds.push_back(MoveCommand{ i, VerticalDirection::NEGATIVE });
        cmds.push_back(MoveCommand{ i, VerticalDirection::POSITIVE });
    }

    double best_score = -1e22;
    MoveCommand best_cmd{0, VerticalDirection::NEGATIVE};
    for(const auto& c : cmds) {
        double score = Scores::distance_based_scoring(simulate(turnData, {}, { c }));
        if (best_score < score) {
            best_score = score;
            best_cmd = c;
        }
    }
    return best_cmd.to_exec_string();
}


#define FOR_MOVE(r, canMove) moveCmds.push_back(MoveCommand{});\
    for (auto r = 0; r < NROUTERS; ++r) { if(!(canMove)) continue; moveCmds.back().routerId = r; \
    for (auto d : possibleDirsV) { moveCmds.back().dir = d;

#define FOR_CREATE(slotTaken, nPackets, maxMsgId, playerId)\
    if (nPackets < MAX_PACKETS_IN_SYSTEM) { \
        createCmds.push_back(CreateCommand{ playerId, 0, maxMsgId + 1 }); \
        for (auto s = 0; s < NSLOTS; ++s) { \
            if (slotTaken[s]) continue; \
            createCmds.back().storeId = s;

#define END_COMMAND_M }} moveCmds.pop_back();
#define END_COMMAND_C } createCmds.pop_back(); }

vector<NetworkState> possible_states_after(const NetworkState& initialState)
{
    vector<CreateCommand> createCmds{};
    vector<MoveCommand> moveCmds{};
    vector<NetworkState> states{};

    prepare_global_variables(initialState.dataPackets);

    array<VerticalDirection, 2> possibleDirsV = { VerticalDirection::NEGATIVE, VerticalDirection::POSITIVE };
    const auto knowBotRouter = GameContext::botRouterId < NROUTERS;
    auto maxMessageId_mine = -1;
    auto maxMessageId_bot = -1;
    const auto nPacketsMine = initialState.getNumberOfPlayerPackets(GameContext::ourId, maxMessageId_mine);
    const auto nPacketsBot = knowBotRouter ? initialState.getNumberOfPlayerPackets(GameContext::botRouterId, maxMessageId_bot) : MAX_PACKETS_IN_SYSTEM;

    // The system is guaranteed to move a router in an arbitrary direction
    FOR_MOVE(rSys, true)
        // Simulate the case when I pass
        states.push_back(simulate(initialState, createCmds, moveCmds));     // both me and the BOT pass

        FOR_CREATE(slotTakenBot, nPacketsBot, maxMessageId_bot, GameContext::botRouterId)
            states.push_back(simulate(initialState, createCmds, moveCmds)); // the BOT creates a packet
        END_COMMAND_C

        FOR_MOVE(rBot, canMoveRouterBot[rBot])
            states.push_back(simulate(initialState, createCmds, moveCmds)); // the BOT moves a router
        END_COMMAND_M

        // Simulate the case when I create a packet
        FOR_CREATE(slotTakenMy, nPacketsMine, maxMessageId_mine, GameContext::ourId)
            states.push_back(simulate(initialState, createCmds, moveCmds));     // the BOT passes

            FOR_CREATE(slotTakenBot, nPacketsBot, maxMessageId_bot, GameContext::botRouterId)
                states.push_back(simulate(initialState, createCmds, moveCmds)); // the BOT creates a packet too
            END_COMMAND_C

            FOR_MOVE(rBot, canMoveRouterBot[rBot])
                states.push_back(simulate(initialState, createCmds, moveCmds)); // the BOT moves a router
            END_COMMAND_M
        END_COMMAND_C

        // Simulate the case when I move a router
        FOR_MOVE(rMe, canMoveRouterMe[rMe])
            states.push_back(simulate(initialState, createCmds, moveCmds));     // the BOT passes

            FOR_CREATE(slotTakenBot, nPacketsBot, maxMessageId_bot, GameContext::botRouterId)
                states.push_back(simulate(initialState, createCmds, moveCmds)); // the BOT creates a packet
            END_COMMAND_C

            FOR_MOVE(rBot, canMoveRouterBot[rBot])
                states.push_back(simulate(initialState, createCmds, moveCmds)); // the BOT moves a router too
            END_COMMAND_M
        END_COMMAND_M
    END_COMMAND_M

    return states;
}
