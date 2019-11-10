#include "ProbabilityScoreStrategy.h"
#include "commands.h"
#include "Scores.h"
#include "simulation.h"

std::string ProbabilityScoreStrategy::step(const NetworkState& turnData, const GameContext& ctx)
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
        std::array<bool, NSLOTS> slotTaken{};
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

    std::set<int> possibleRouters;
    for (const auto& d : turnData.dataPackets)
        if (d.fromRouter == GameContext::ourId)
            possibleRouters.insert(d.currRouter);

    std::vector<MoveCommand> cmds;

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
