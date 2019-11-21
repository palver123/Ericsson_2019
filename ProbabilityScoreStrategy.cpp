#include "ProbabilityScoreStrategy.h"
#include "commands.h"
#include "Scores.h"
#include "simulation.h"

using namespace std;

namespace {
    array<bool, NSLOTS> slotCannotTakeNewPacketBot{};
    array<bool, NROUTERS> canMoveRouterMe{};
    array<bool, NROUTERS> canMoveRouterBot{};
}

string ProbabilityScoreStrategy::step(NetworkState& turnData, const GameContext& ctx)
{
    stepPre(turnData,ctx);
    for (const auto& p : GameContext::playerPackets)
        turnData.nextDir[p.first] = ((p.second.active.size() + p.second.received.size()) % 2) ? HorizontalDirection::RIGHT : HorizontalDirection::LEFT;

    vector<std::shared_ptr<Player>> players;
    players.emplace_back(new RandomNetworkMovements{});
    for (const auto& p : GameContext::playerPackets)
        if (p.first != ourId)
            players.emplace_back(new RandomPlayerCreatePref(p.first));

    if (GameContext::receivedEmptyPacket(ourId))
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
    else if (actualData->getNumberOfPlayerPackets(ourId,true) < MAX_PACKETS_IN_SYSTEM)
    {
        vector<Command> ccmds = getPossibleMoves(turnData, false, false, true);

        if (ccmds.size())
        {
            Command bcmd = getBestMove(turnData, ccmds, players, Scores::future_seeing);
            return command_execute(bcmd);
        }
        std::cerr << "!!! Failed creation !!!" << std::endl;
    }
    auto moves = getPossibleMoves(turnData, true, true, false);
    return command_execute(getBestMove(turnData, moves, players, Scores::future_seeing));
}

Command ProbabilityScoreStrategy::getBestMove(const NetworkState& state, const std::vector<Command>& moves, const vector<std::shared_ptr<Player> >& players, scoringFuction scoring)
{
    auto res = Player::getMovementScoresComplex(state, moves, ourId, players, scoring);
//    auto res = Player::getMovementScoresSimple(state, moves, ourId, Scores::distance_based_scoring_change_handling);
    auto [s, c] = bestInVector(res, Command::Pass());
    std::cerr << "Best score: " << s << std::endl;
    return c;
}