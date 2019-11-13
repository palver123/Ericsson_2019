#include "ProbabilityScoreStrategy.h"
#include "commands.h"
#include "Scores.h"
#include "simulation.h"

using namespace std;

array<bool, NSLOTS> slotCannotTakeNewPacketBot{};
array<bool, NROUTERS> canMoveRouterMe{};
array<bool, NROUTERS> canMoveRouterBot{};

void prepare_global_variables(const NetworkState& state)
{
    fill(canMoveRouterMe.begin(), canMoveRouterMe.end(), false);
    for (const auto& data : state.dataPackets)
    {
        if (data.fromRouter == GameContext::ourId)
            canMoveRouterMe[data.currRouter] = true;
    }

    for (auto slot = 0; slot < NSLOTS; ++slot)
        slotCannotTakeNewPacketBot[slot] = !state.routerBits[GameContext::botRouterId][slot]; // Closed
    fill(canMoveRouterBot.begin(), canMoveRouterBot.end(), false);
    if (GameContext::botRouterId < NROUTERS)
    {
        for (const auto& data : state.dataPackets)
        {
            if (data.currRouter == GameContext::botRouterId)
                slotCannotTakeNewPacketBot[data.currStoreId] = true; // Or occupied by other data packet
            if (data.fromRouter == GameContext::botRouterId)
                canMoveRouterBot[data.currRouter] = true;
        }
    }
}


string ProbabilityScoreStrategy::step(NetworkState& turnData, const GameContext& ctx)
{
    stepPre(turnData,ctx);
    turnData.nextDir[GameContext::ourId] = (_requestCounter % 2) ? HorizontalDirection::RIGHT : HorizontalDirection::LEFT;
    static bool info_dumped = false;
    if (!info_dumped && GameContext::botRouterId != NROUTERS) {
        std::cerr << fmt::format("!!!!INFO {} {} {}", GameContext::ourId, GameContext::botRouterId, turnData.routers_dump()) << std::endl;
        info_dumped = true;
    }
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

        vector<CreateCommand> ccmds;

        for (auto slot = 0; slot < NSLOTS; slot++)
            if (!slotTaken[slot] && turnData.routerBits[GameContext::ourId][slot])
                ccmds.push_back({ GameContext::ourId, slot, _requestCounter });

        if (ccmds.size())
        {
            CreateCommand bcmd;
            double best = -1e22;
            for (auto& c : ccmds) {
                double score = Scores::future_seeing(simulate(turnData, { c }, {}));
                if (best < score) {
                    bcmd = c;
                    best = score;
                }
            }
            ++_requestCounter;
            return bcmd.to_exec_string();
        }
        else
        {
            std::cerr << "!!! Failed creation !!!" << std::endl;
        }
    }

    // Do the best we can....
    return getBestMoveInNextTurn(turnData, Scores::future_seeing);
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

#define EVAL score = scoringFunc(simulate(initialState, createCmds, moveCmds)); \
    sumScores += score; \
    ++nScores;


string ProbabilityScoreStrategy::getBestMoveInNextTurn(const NetworkState& initialState, double scoringFunc(const NetworkState&))
{
    vector<CreateCommand> createCmds{};
    vector<MoveCommand> moveCmds{};

    prepare_global_variables(initialState);

    array<VerticalDirection, 2> possibleDirsV = { VerticalDirection::NEGATIVE, VerticalDirection::POSITIVE };
    const auto knowBotRouter = GameContext::botRouterId < NROUTERS;
    auto maxMessageId_bot = -1;
    const auto nPacketsBot = knowBotRouter ? initialState.getNumberOfPlayerPackets(GameContext::botRouterId, maxMessageId_bot) : MAX_PACKETS_IN_SYSTEM;
    double score;

    // The system is guaranteed to move a router in an arbitrary direction

    // Simulate the case when I pass
    string bestCommand = "PASS";
    double sumScores = 0;
    unsigned nScores = 0;
    FOR_MOVE(rSys, true)
        EVAL     // both me and the BOT pass

        FOR_CREATE(slotCannotTakeNewPacketBot, nPacketsBot, maxMessageId_bot, GameContext::botRouterId)
            EVAL // the BOT creates a packet
        END_COMMAND_C
        else{
        FOR_MOVE(rBot, canMoveRouterBot[rBot])
            EVAL // the BOT moves a router
        END_COMMAND_M }
    END_COMMAND_M
    auto bestScore = sumScores / nScores; // estimated value of the scores of the possible outcomes of me PASSING

    // Simulate the case when I move a router
    FOR_MOVE(rMe, canMoveRouterMe[rMe])
        auto move_command_str = moveCmds.back().to_exec_string();
        sumScores = 0;
        nScores = 0;
        FOR_MOVE(rSys, true)
            EVAL     // the BOT passes

            FOR_CREATE(slotCannotTakeNewPacketBot, nPacketsBot, maxMessageId_bot, GameContext::botRouterId)
                EVAL // the BOT creates a packet
            END_COMMAND_C

        else{
            FOR_MOVE(rBot, canMoveRouterBot[rBot])
                EVAL // the BOT moves a router too
            END_COMMAND_M}
        END_COMMAND_M
        const auto accumulateCommandScore = sumScores / nScores; // estimated value of the scores of the possible outcomes of me MOVING
        if (accumulateCommandScore > bestScore)
        {
            bestScore = accumulateCommandScore;
            bestCommand = move(move_command_str);
        }
    END_COMMAND_M

    return bestCommand;
}
